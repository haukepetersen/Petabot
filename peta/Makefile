# name of your application
APPLICATION = peta

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../riot/RIOT

# define the modules used for Peta
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += servo
USEMODULE += ng_netif
USEMODULE += auto_init_ng_netif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# include application specific include path
INCLUDES += -I$(CURDIR)/include

include $(RIOTBASE)/Makefile.include
