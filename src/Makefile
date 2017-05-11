CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
STRIP = $(CROSS_COMPILE)strip
CFLAGS= -w -Iinclude -Ilib -g
BIN:= httpd

#OBJDIR:=$(PWD)
OBJDIR:=$(CURDIR)
SRCDIR:=$(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.c=.o))

LIBS += $(JSON_LIB)

all: $(BIN)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
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

test:
	echo objdir: $(PWD)
	echo srcdir: $(SRCDIR)
	@echo srcs: $(SRCS)
	@echo objs: $(OBJS)
