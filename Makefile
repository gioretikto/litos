CC = gcc
CFLAGS = -std=c99 -W -Wextra -Wall -Wshadow -Wconversion
SYSTEM = `uname -s`

ifeq ($(SYSTEM), SunOS)
	CFLAGS+= -D__EXTENSIONS__
endif

GTK_INCS = `pkg-config --cflags gtk+-3.0 gtksourceview-4`

GTK_LIBS = `pkg-config --libs gtk+-3.0 gtksourceview-4`

CFLAGS += ${GTK_INCS}

SRCS = main.c activate.c litos.c dialogs.c search_replace.c popovers.c sourceview.c acels.c
OBJS = $(SRCS:.c=.o)
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DATADIR ?= /usr/share/applications/
TARGET = litos
TARGETDATA = org.litos.gtk.desktop

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(GTK_INCS) $(GTK_LIBS)
clean:
	rm -f $(OBJS)
	rm -f $(DATADIR)$(TARGETDATA)
install:
	install $(TARGET) $(BINDIR)
	install	$(TARGETDATA) $(DATADIR)
