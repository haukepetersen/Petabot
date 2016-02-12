/*
 * Copyright (C) 2015 Hauke Petersen <mail@haukepetersen.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @ingroup     peta
 * @{
 *
 * @file
 * @brief       Communication module
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>
#include <net/af.h>

#include "kernel.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/conn.h"
#include "net/conn/udp.h"

#include "coap.h"
#include "comm.h"
#include "disco.h"
#include "peta_config.h"

#define SRC_PORT                (46465)

#define SENML_BUFSIZE           (256U)
#define COAP_BUFSIZE            (256U)

/**
 * @brief   Allocate stacks
 * @{
 */
static char pupdate_stack[CONF_STACKSIZE];
static char coap_server_stack[CONF_STACKSIZE];
static msg_t coap_msg_queue[CONF_MSGQUEUE_SIZE];
/** @} */

/**
 * @brief   Hold the prepared SenML statement in this buffer
 * @{
 */
static uint8_t coap_cli_buf[COAP_BUFSIZE];
static uint8_t coap_svr_buf[COAP_BUFSIZE];
static uint8_t coap_scratch_raw[2 * COAP_BUFSIZE];
static coap_rw_buffer_t coap_scratch = { coap_scratch_raw, sizeof(coap_scratch_raw) };
static uint8_t udp_buf[2 * COAP_BUFSIZE];
static char senml_buf[SENML_BUFSIZE];
static size_t senml_base;
/** @} */

/**
 * @brief   Remember the fixed server address for now
 */
static ipv6_addr_t dst_addr;

/**
 * @brief   Access the car's current state
 * @{
 */
extern int behavior;
extern int front_blocked;
extern int back_blocked;
extern int16_t speed;
extern int16_t dir;
/** @} */

/**
 * @brief   Save some space for the disco
 * @{
 */
static disco_t disco;
static const disco_params_t disco_params = {
    .pin   = CONF_DISCO_PIN
};
/** @} */

/**
 * @brief   Preallocate the CoAP request header
 */
static const coap_header_t req_hdr = {
        .version = 1,
        .type    = COAP_TYPE_NONCON,
        .tkllen  = 0,
        .code    = COAP_METHOD_POST,
        .mid     = {5, 57}            // is equivalent to 1337 when converted to uint16_t
};

/**
 * @brief   Configure CoAP Server
 */
static const coap_endpoint_path_t ep_riot = { 1, { "riot" } };
static const coap_endpoint_path_t ep_disco  = { 1, { "disco" } };
static const coap_endpoint_path_t ep_behave = { 1, { "behave" } };


static int handle_get_riot(coap_rw_buffer_t *scratch,
                                 const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                 uint8_t id_hi, uint8_t id_lo)
{
    const char *riot_name = RIOT_BOARD;
    int len = strlen(RIOT_BOARD);

    memcpy(coap_svr_buf, riot_name, len);
    return coap_make_response(scratch, outpkt, coap_svr_buf, len,
                              id_hi, id_lo, &inpkt->token, COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_TEXT_PLAIN, false);
}

static int handle_post_disco(coap_rw_buffer_t *scratch,
                                 const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                 uint8_t id_hi, uint8_t id_lo)
{
    coap_responsecode_t resp = COAP_RSPCODE_CHANGED;
    uint8_t val = inpkt->payload.p[0];

    if (val == '1') {
        disco_party(&disco, CONF_DISCO_PARTYTIME);
        puts("[comm] partytime!");
    }
    else {
        resp = COAP_RSPCODE_NOT_ACCEPTABLE;
    }

    return coap_make_response(scratch, outpkt, NULL, 0,
                              id_hi, id_lo, &inpkt->token, resp,
                              COAP_CONTENTTYPE_TEXT_PLAIN, false);
}

static int handle_post_behave(coap_rw_buffer_t *scratch,
                                 const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                 uint8_t id_hi, uint8_t id_lo)
{
    coap_responsecode_t resp = COAP_RSPCODE_CHANGED;
    uint8_t val = inpkt->payload.p[0];

    if ((val == '0') || (val == '1')) {
        behavior = val - '0';
        printf("[comm] coap: got new behavior: %i\n", behavior);
    }
    else {
        resp = COAP_RSPCODE_NOT_ACCEPTABLE;
    }

    return coap_make_response(scratch, outpkt, NULL, 0,
                              id_hi, id_lo, &inpkt->token, resp,
                              COAP_CONTENTTYPE_TEXT_PLAIN, false);
}

const coap_endpoint_t endpoints[] =
{
    { COAP_METHOD_GET,   handle_get_riot, &ep_riot, "ct=0" },
    { COAP_METHOD_POST,  handle_post_disco, &ep_disco, "ct=0" },
    { COAP_METHOD_POST,  handle_post_behave, &ep_behave, "ct=0" },
    { (coap_method_t)0, NULL, NULL, NULL }
};

static void coap_post(uint8_t *data, size_t len)
{
    coap_packet_t req_pkt = {
            .header  = req_hdr,
            .token   = (coap_buffer_t) { 0 },
            .numopts = 1,
            .opts    = {{{(uint8_t *)"senml", 5}, (uint8_t)COAP_OPTION_URI_PATH}},
            .payload = { data, len }
    };

    size_t pkt_len = sizeof(req_pkt);
    coap_build(coap_cli_buf, &pkt_len, &req_pkt);
    conn_udp_sendto(coap_cli_buf, pkt_len, NULL, 0, &dst_addr, sizeof(dst_addr),
                    AF_INET6, SRC_PORT, CONF_COMM_COAP_PORT);
}

static void send_update(void)
{
    size_t pos = senml_base;
    pos += sprintf(&senml_buf[pos],
                   "{\"n\":\"s:car\", \"u\":\"mixed\", \"v\":[%i, %i, %i, %i]},",
                   (int)speed, (int)dir, (int)front_blocked, (int)back_blocked);
    pos += sprintf(&senml_buf[pos],
                   "{\"n\":\"a:car\", \"u\":\"mixed\", \"v\":\"%i\"}]",
                   (int)behavior);

    coap_post((uint8_t *)senml_buf, pos);
}

void *periodic_update(void *arg)
{
    (void)arg;
    uint32_t lu = xtimer_now();

    /* send periodic updates */
    while (1) {
        send_update();
        xtimer_usleep_until(&lu, CONF_COMM_UPDATE_DELAY);
    }

    return NULL;
}

void *coap_server(void *arg)
{
    (void)arg;
    msg_init_queue(coap_msg_queue, CONF_MSGQUEUE_SIZE);

    puts("[comm]  coap server starting...");

    uint8_t laddr[16] = { 0 };
    uint8_t raddr[16] = { 0 };
    size_t raddr_len;
    uint16_t rport;
    conn_udp_t conn;

    int rc = conn_udp_create(&conn, laddr, sizeof(laddr),
                             AF_INET6, CONF_COMM_COAP_PORT);
    (void)rc;
    while (1) {
        if ((rc = conn_udp_recvfrom(&conn, (char *)udp_buf, sizeof(udp_buf),
                                    raddr, &raddr_len, &rport)) < 0) {
            continue;
        }

        coap_packet_t pkt;
        /* parse UDP packet to CoAP */
        if (0 == (rc = coap_parse(&pkt, udp_buf, rc))) {
            coap_packet_t rsppkt;

            /* handle CoAP request */
            coap_handle_req(&coap_scratch, &pkt, &rsppkt, false, false);

            /* build reply */
            size_t rsplen = sizeof(udp_buf);
            if ((rc = coap_build(udp_buf, &rsplen, &rsppkt)) == 0) {
                /* send reply via UDP */
                rc = conn_udp_sendto(udp_buf, rsplen, NULL, 0, raddr, raddr_len,
                                     AF_INET6, CONF_COMM_COAP_PORT, rport);
            }
        }
    }

    /* never reached */
    return NULL;
}

void comm_run(void)
{
    uint16_t chan = CONF_COMM_CHAN;
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];

    /* set channel and destination address */
    gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, &chan, sizeof(chan));
    ipv6_addr_from_str(&dst_addr, CONF_COMM_HORST_ADDR);

    /* prepare the SenML statement */
    senml_base  = sprintf(senml_buf, "[{\"bn\":\"" CONF_COMM_NODEID "\"},");

    /* initialize the disco light */
    puts("[comm]  initializing disco light...");
    if (disco_init(&disco, &disco_params)) {
        puts("[failed]");
        return;
    }

    printf("stacksize is %i\n", CONF_STACKSIZE);

    /* start the CoAP server */
    puts("[comm]  starting CoAP server");
    thread_create(coap_server_stack, sizeof(coap_server_stack),
                  CONF_PRIO_COAP_SERVER, THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  coap_server, NULL, "coap server");

    /* send out periodic updates */
    puts("[comm]  starting commander tell");
    thread_create(pupdate_stack, sizeof(pupdate_stack),
                  CONF_PRIO_PUPDATE, THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  periodic_update, NULL, "cmd tell");

    puts("[comm]  everything booted up correctly");
}
