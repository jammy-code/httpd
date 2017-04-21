CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
STRIP = $(CROSS_COMPILE)strip
CFLAGS= -w -Iinclude -Ilib -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
BIN:= httpd

LIBS += $(JSON_LIB)

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) $(GCC_DEFINE) -c -o $@ $<

httpd: $(OBJS)
	$(CC) -o $@ $(OBJS)

sinclude .depfile
dep:
	$(CC) -MM $(CFLAGS) $(SRCS) > .depfile

install: $(BIN)
	$(STRIP) -s $(BIN)
	mkdir -p $(DESTDIR)/sbin
	install -m755 httpd $(DESTDIR)/sbin

clean:
	rm -rf .depfile *.o *.so $(BIN)

