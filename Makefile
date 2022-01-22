CC = gcc
CFLAGS = -std=c99 -W -Wextra -Wall -Wshadow -Wconversion
SYSTEM = `uname -s`

ifeq ($(SYSTEM), SunOS)
	CFLAGS+= -D__EXTENSIONS__
endif

GTK_INCS = `pkg-config --cflags gtk+-3.0 gtksourceview-3.0`

GTK_LIBS = `pkg-config --libs gtk+-3.0 gtksourceview-3.0`

CFLAGS += ${GTK_INCS}

SRCS = main.c activate.c litos.c dialogs.c search_replace.c popovers.c sourceview.c acels.c
OBJS = $(SRCS:.c=.o)
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
TARGET = litos

$(TARGET): $(OBJS)
	 $(CC) -o $@ $^ $(CFLAGS) $(GTK_INCS) $(GTK_LIBS)
clean:
	rm -f $(OBJS)
install:
	install $(TARGET) $(BINDIR)
