/*
 ============================================================================
 Name        : ImageProcessor.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>

//#define ADDRESS "127.0.0.1"
#define ADDRESS "192.168.1.39"
#define PORT 8333

struct sockaddr_in server;
int sock;

CvCapture *capture;
IplImage *image;

void dieWithError(char *errorMessage) {
	perror(errorMessage);
	close(sock);
	exit(1);
}

void initialize() {
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ADDRESS);
	server.sin_port = htons(PORT);
	capture = cvCaptureFromCAM(0);
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		dieWithError("connect() failed.");
	}

	int retry = 5;
	int i = 0;
	while (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
		i++;
		if(i == retry)
		{
			dieWithError("the motherfucker is still not reachable and I give up!\n");
		}
		sleep(1);
		printf("the motherfucker is not reachable!\n");
	}
}

void processImage() {
	image = cvQueryFrame(capture);

	if(send(sock, image->imageData, image->imageSize, MSG_DONTROUTE) < 0)
	{
		dieWithError("something bad happened");
	}
	cvWaitKey(100);
}

int main(void) {
	initialize();
	while (1) {
		processImage(capture);
	}
	return 0;
}
