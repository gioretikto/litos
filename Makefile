CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk4 gtksourceview-5)
LIBS = $(shell $(PKGCONFIG) --libs gtk4 gtksourceview-5)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schemas gio-2.0)

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/local/bin/
DESKTOPDIR ?= $(PREFIX)/share/applications/
ICONDIR ?= $(PREFIX)/share/pixmaps
ICON = litos.png
DESKTOP = org.litos.gtk.desktop
SCHEMA = org.gtk.litos.gschema.xml
SCHEMADIR = /usr/share/glib-2.0/schemas

SRC = litosapp.c litosappwin.c sourceview.c litosappprefs.c litosaccels.c litosfile.c main.c
BUILT_SRC = resources.c
TARGET = litos
OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

all: litos

org.gtk.litos.gschema.valid: org.gtk.litos.gschema.xml
	$(GLIB_COMPILE_SCHEMAS) --strict --dry-run --schema-file=$< && mkdir -p $(@D) && touch $@

gschemas.compiled: org.gtk.litos.gschema.valid
	$(GLIB_COMPILE_SCHEMAS) .

resources.c: litos.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies litos.gresource.xml)
	$(GLIB_COMPILE_RESOURCES) litos.gresource.xml --target=$@ --sourcedir=. --generate-source

%.o: %.c
	$(CC) -c -o $(@F) $(CFLAGS) $<

litos: $(OBJS) gschemas.compiled
	$(CC) -o $(@F) $(OBJS) $(LIBS)

clean:
	rm -f org.gtk.litos.gschema.valid
	rm -f gschemas.compiled
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f litos

uninstall:
	rm -f $(DESKTOPDIR)/$(DESKTOP)
	rm -f $(ICONDIR)/$(ICON)
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(SCHEMADIR)/$(SCHEMA)
	$(GLIB_COMPILE_SCHEMAS) $(SCHEMADIR)
install:
	install $(TARGET) $(BINDIR)
	install $(DESKTOP) $(DESKTOPDIR)
	install $(ICON) $(ICONDIR)
	install $(SCHEMA) $(SCHEMADIR)
	$(GLIB_COMPILE_SCHEMAS) $(SCHEMADIR)
