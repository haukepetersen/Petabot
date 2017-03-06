# name of your application
APPLICATION = remote

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Define modules that are needed
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += shell
USEMODULE += shell_commands

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Include application specific headers
INCLUDES += -I$(CURDIR)/include

include $(RIOTBASE)/Makefile.include
