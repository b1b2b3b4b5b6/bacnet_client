# tools - only if you need them.
# Most platforms have this already defined
# CC = gcc
# AR = ar
# MAKE = make
# SIZE = size
#
# Assumes rm and cp are available

# Directories
BACNET_PORT = linux
BACNET_PORT_DIR = ../ports/${BACNET_PORT}
BACNET_INCLUDE = ../include
BACNET_OBJECT = object
BACNET_HANDLER = handler
# BACnet Library
BACNET_LIB_DIR = ../
BACNET_LIB_NAME = bacnet
BACNET_LIB_TARGET = $(BACNET_LIB_DIR)/lib$(BACNET_LIB_NAME).a
# Compiler Setup
INCLUDE1 = -I$(BACNET_PORT_DIR) -I$(BACNET_OBJECT) -I$(BACNET_HANDLER)
INCLUDE2 = -I$(BACNET_INCLUDE)
INCLUDES = $(INCLUDE1) $(INCLUDE2)
BACNET_LIB=-L$(BACNET_LIB_DIR),-l$(BACNET_LIB_NAME)
ifeq (${BACNET_PORT},linux)
PFLAGS = -pthread
TARGET_EXT =
SYSTEM_LIB=-lc,-lgcc,-lrt,-lm
endif


MY_BACNET_DEFINES = -DPRINT_ENABLED=1
MY_BACNET_DEFINES += -DBACAPP_ALL
MY_BACNET_DEFINES += -DINTRINSIC_REPORTING
MY_BACNET_DEFINES += -DBACNET_PROPERTY_LISTS=1

BACNET_DEFINES ?= $(MY_BACNET_DEFINES)

BACDL_DEFINE?=-DBACDL_BIP=1

BBMD_DEFINE ?= -DBBMD_ENABLED=1

MAKE_DEFINE ?=

# Define WEAK_FUNC for [...somebody help here; I can't find any uses of it]
DEFINES = $(BACNET_DEFINES) $(BACDL_DEFINE) $(BBMD_DEFINE) -DWEAK_FUNC=
DEFINES += $(MAKE_DEFINE)

#build for release (default) or debug
DEBUGGING =
OPTIMIZATION = -Os
BUILD = debug
ifeq (${BUILD},debug)
OPTIMIZATION = -O0
DEBUGGING = -g -DDEBUG_ENABLED=1
ifeq (${BACDL_DEFINE},-DBACDL_BIP=1)
DEFINES += -DBIP_DEBUG
endif
endif

# put all the flags together
CFLAGS := -Wall $(DEBUGGING) $(OPTIMIZATION) $(INCLUDES) $(DEFINES)
LFLAGS := -Wl,$(BACNET_LIB),$(SYSTEM_LIB)


.PHONY : all clean

TARGET = client
BUILD_DIR = ./build
TARGET_BIN = ${TARGET}$(TARGET_EXT)

SRC = $(wildcard ./*.c)

OBJECT_SRC = \
	$(BACNET_OBJECT)/device.c \
	$(BACNET_OBJECT)/ai.c \
	$(BACNET_OBJECT)/ao.c \
	$(BACNET_OBJECT)/av.c \
	$(BACNET_OBJECT)/bi.c \
	$(BACNET_OBJECT)/bo.c \
	$(BACNET_OBJECT)/bv.c \
	$(BACNET_OBJECT)/nc.c \
	
HANDLER_SRC = \
	$(BACNET_HANDLER)/dlenv.c \
	$(BACNET_HANDLER)/txbuf.c \
	$(BACNET_HANDLER)/noserv.c \
	$(BACNET_HANDLER)/h_npdu.c \
	$(BACNET_HANDLER)/h_whois.c \
	$(BACNET_HANDLER)/h_iam.c  \
	$(BACNET_HANDLER)/h_rp.c \
	$(BACNET_HANDLER)/h_rp_a.c \
	$(BACNET_HANDLER)/h_rpm.c \
	$(BACNET_HANDLER)/h_rpm_a.c \
	$(BACNET_HANDLER)/h_rr.c \
	$(BACNET_HANDLER)/h_rr_a.c  \
	$(BACNET_HANDLER)/h_wp.c  \
	$(BACNET_HANDLER)/h_wpm.c \
	$(BACNET_HANDLER)/h_alarm_ack.c  \
	$(BACNET_HANDLER)/h_arf.c  \
	$(BACNET_HANDLER)/h_arf_a.c  \
	$(BACNET_HANDLER)/h_awf.c  \
	$(BACNET_HANDLER)/h_rd.c  \
	$(BACNET_HANDLER)/h_dcc.c  \
	$(BACNET_HANDLER)/h_ts.c  \
	$(BACNET_HANDLER)/h_whohas.c  \
	$(BACNET_HANDLER)/h_ihave.c  \
	$(BACNET_HANDLER)/h_cov.c  \
	$(BACNET_HANDLER)/h_ccov.c  \
	$(BACNET_HANDLER)/h_ucov.c  \
	$(BACNET_HANDLER)/h_getevent.c  \
	$(BACNET_HANDLER)/h_gas_a.c  \
	$(BACNET_HANDLER)/h_get_alarm_sum.c  \
	$(BACNET_HANDLER)/h_pt.c  \
	$(BACNET_HANDLER)/h_pt_a.c  \
	$(BACNET_HANDLER)/h_upt.c  \
	$(BACNET_HANDLER)/s_arfs.c \
	$(BACNET_HANDLER)/s_awfs.c \
	$(BACNET_HANDLER)/s_dcc.c \
	$(BACNET_HANDLER)/s_ihave.c \
	$(BACNET_HANDLER)/s_iam.c  \
	$(BACNET_HANDLER)/s_cov.c  \
	$(BACNET_HANDLER)/s_ptransfer.c \
	$(BACNET_HANDLER)/s_rd.c \
	$(BACNET_HANDLER)/s_rp.c  \
	$(BACNET_HANDLER)/s_rpm.c  \
	$(BACNET_HANDLER)/s_readrange.c  \
	$(BACNET_HANDLER)/s_ts.c \
	$(BACNET_HANDLER)/s_cevent.c  \
	$(BACNET_HANDLER)/s_router.c  \
	$(BACNET_HANDLER)/s_uevent.c  \
	$(BACNET_HANDLER)/s_whohas.c \
	$(BACNET_HANDLER)/s_whois.c  \
	$(BACNET_HANDLER)/s_wpm.c  \
	$(BACNET_HANDLER)/s_upt.c \
	$(BACNET_HANDLER)/s_wp.c
SRCS = ${SRC} ${OBJECT_SRC} ${HANDLER_SRC}

OBJS = ${SRCS:%.c=${BUILD_DIR}/%.o}

all: depend ${TARGET_BIN}

${TARGET_BIN}: ${OBJS} ${BACNET_LIB_TARGET} Makefile
	${CC} ${PFLAGS} ${OBJS} ${LFLAGS} -o $@
	size $@


${BACNET_LIB_TARGET}:
	${MAKE} all -C ../ 

${BUILD_DIR}/%.o:%.c
	${CC} -c ${CFLAGS} $*.c -o $@

depend: 
	-mkdir build 
	-mkdir build/object
	-mkdir build/handler
	rm -f .depend
	${CC} -MM ${CFLAGS} *.c >> .depend

include .depend

clean:
	rm -f core ${TARGET_BIN} ${OBJS} $(TARGET).map
	rm -rf build -r