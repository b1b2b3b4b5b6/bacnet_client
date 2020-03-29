BACNET_PORT=linux
BACNET_PORT_DIR = ports/${BACNET_PORT}
BACNET_CORE = src
BACNET_INCLUDE = include

INCLUDE1 = -I$(BACNET_PORT_DIR)
INCLUDE2 = -I$(BACNET_INCLUDE) 

INCLUDES = $(INCLUDE1) $(INCLUDE2)

# target
LIBRARY = libbacnet.a

CORE_SRC = \
	$(BACNET_CORE)/apdu.c \
	$(BACNET_CORE)/npdu.c \
	$(BACNET_CORE)/bacdcode.c \
	$(BACNET_CORE)/bacint.c \
	$(BACNET_CORE)/bacreal.c \
	$(BACNET_CORE)/bacstr.c \
	$(BACNET_CORE)/bacapp.c \
	$(BACNET_CORE)/bacprop.c \
	$(BACNET_CORE)/bactext.c \
	$(BACNET_CORE)/bactimevalue.c \
	$(BACNET_CORE)/datetime.c \
	$(BACNET_CORE)/indtext.c \
	$(BACNET_CORE)/key.c \
	$(BACNET_CORE)/keylist.c \
	$(BACNET_CORE)/proplist.c \
	$(BACNET_CORE)/debug.c \
	$(BACNET_CORE)/bigend.c \
	$(BACNET_CORE)/arf.c \
	$(BACNET_CORE)/awf.c \
	$(BACNET_CORE)/cov.c \
	$(BACNET_CORE)/dcc.c \
	$(BACNET_CORE)/iam.c \
	$(BACNET_CORE)/ihave.c \
	$(BACNET_CORE)/rd.c \
	$(BACNET_CORE)/rp.c \
	$(BACNET_CORE)/rpm.c \
	$(BACNET_CORE)/timesync.c \
	$(BACNET_CORE)/whohas.c \
	$(BACNET_CORE)/whois.c \
	$(BACNET_CORE)/wp.c \
	$(BACNET_CORE)/wpm.c \
	$(BACNET_CORE)/abort.c \
	$(BACNET_CORE)/reject.c \
	$(BACNET_CORE)/bacerror.c \
	$(BACNET_CORE)/ptransfer.c \
	$(BACNET_CORE)/memcopy.c \
	$(BACNET_CORE)/filename.c \
	$(BACNET_CORE)/tsm.c \
	$(BACNET_CORE)/bacaddr.c \
	$(BACNET_CORE)/address.c \
	$(BACNET_CORE)/bacdevobjpropref.c \
	$(BACNET_CORE)/bacpropstates.c \
	$(BACNET_CORE)/alarm_ack.c \
	$(BACNET_CORE)/event.c \
	$(BACNET_CORE)/getevent.c \
	$(BACNET_CORE)/get_alarm_sum.c \
	$(BACNET_CORE)/readrange.c \
	$(BACNET_CORE)/timestamp.c \
	$(BACNET_CORE)/version.c


PORT_BIP_SRC = \
	$(BACNET_PORT_DIR)/bip-init.c \
	$(BACNET_CORE)/bvlc.c \
	$(BACNET_CORE)/bip.c




PORT_SRC = ${PORT_BIP_SRC}


SRCS = ${CORE_SRC} ${PORT_SRC} 

OBJS = ${SRCS:.c=.o}

# use local includes, but other values from calling Makefile
CFLAGS  = $(INCLUDES) -DDEBUG_ENABLED=1 -Wall -Wmissing-prototypes -DPRINT_ENABLED=1 -DBACAPP_ALL -DBACNET_PROPERTY_LISTS=1 -DBACDL_BIP=1 -DBBMD_ENABLED=1 -DWEAK_FUNC= -O0 -g 

all: $(LIBRARY)

lib: $(LIBRARY)

$(LIBRARY): $(OBJS) Makefile
	$(AR) rcs $@ $(OBJS)

.c.o:
	${CC} -c ${CFLAGS} $*.c -o $@

depend:
	rm -f .depend
	${CC} -MM ${CFLAGS} *.c >> .depend

clean:
	rm -rf $(OBJS) $(LIBRARY)

include: .depend
