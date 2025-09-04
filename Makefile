# Compilazione
CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = -Wall -Wextra $(shell $(PKGCONFIG) --cflags gtk4 gtksourceview-5)
LIBS = $(shell $(PKGCONFIG) --libs gtk4 gtksourceview-5)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schemas gio-2.0)

# Percorsi di installazione
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/local/bin/
DESKTOPDIR ?= $(PREFIX)/share/applications/
ICONDIR ?= $(PREFIX)/share/pixmaps
SCHEMADIR = /usr/share/glib-2.0/schemas

# File e target
ICON = litos.png
DESKTOP = org.litos.gtk.desktop
SCHEMA = org.gtk.litos.gschema.xml
SRC = litosapp.c litosappwin.c sourceview.c litosappprefs.c litosaccels.c litosfile.c main.c
BUILT_SRC = resources.c
TARGET = litos
OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

# Ottieni l'utente reale che ha invocato sudo (o fallback su USER)
REAL_USER := $(if $(SUDO_USER),$(SUDO_USER),$(USER))
USER_HOME := $(shell getent passwd $(REAL_USER) | cut -d: -f6)

# Percorso della cartella Templates (fallback se xdg-user-dir fallisce)
TEMPLATE_DIR := $(shell sudo -u $(REAL_USER) xdg-user-dir TEMPLATES 2>/dev/null || echo $(USER_HOME)/Templates)
TEMPLATESRC = Templates/*

# Regole principali
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

# Installazione
install:
	install -m 755 $(TARGET) $(BINDIR)
	install -m 644 $(DESKTOP) $(DESKTOPDIR)
	install -m 644 $(ICON) $(ICONDIR)
	install -m 644 $(SCHEMA) $(SCHEMADIR)
	$(GLIB_COMPILE_SCHEMAS) $(SCHEMADIR)

	@echo "Installazione dei template in $(TEMPLATE_DIR)"
	@sudo -u $(REAL_USER) mkdir -p "$(TEMPLATE_DIR)"
	@sudo -u $(REAL_USER) cp -u $(TEMPLATESRC) "$(TEMPLATE_DIR)/"
	@echo "Templates correctly copied."


# Disinstallazione
uninstall:
	rm -f $(DESKTOPDIR)/$(DESKTOP)
	rm -f $(ICONDIR)/$(ICON)
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(SCHEMADIR)/$(SCHEMA)
	$(GLIB_COMPILE_SCHEMAS) $(SCHEMADIR)

# Pulizia
clean:
	rm -f org.gtk.litos.gschema.valid
	rm -f gschemas.compiled
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f litos
