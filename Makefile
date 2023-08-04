CC = gcc
CFLAGS = -std=c99 -W -Wextra -Wall -Wshadow -Wconversion -O2

LIBS = `pkg-config --cflags --libs gtk+-3.0 gtksourceview-4` -laspell

SRCS = main.c activate.c litos.c dialogs.c search_replace.c popovers.c sourceview.c acels.c spell.c
OBJS = $(SRCS:.c=.o)
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/local/bin/
DESKTOPDIR ?= $(PREFIX)/share/applications/
ICONDIR ?= $(PREFIX)/share/pixmaps
TARGET = litos
ICON = litos.png
TARGETDATA = org.litos.gtk.desktop
#HEADERS = <header files>

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
clean:
	rm -f $(OBJS)
	rm -f $(DATADIR)$(TARGETDATA)
	rm -f $(ICONDIR)$(ICON)
install:
	install $(TARGET) $(BINDIR)
	install $(TARGETDATA) $(DESKTOPDIR) 
	install $(ICON) $(ICONDIR)
