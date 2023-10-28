app_resources = gnome.compile_resources('litos_resources',
  'litos.gresource.xml',
  source_dir: meson.current_source_dir())

app_schemas = gnome.compile_schemas()

project('org.gtk.litos',
    version: '4.0.0',
    meson_version: '>= 0.59.0',
)

executable('litos',
  'main.c',
  'litosappswin.c',
  'litosfilewin.c',
  'litosaccels.c',
  'sourceview.c',
  'litosappprefs.c',
  app_resources,
  app_schemas,
  dependencies: libgtk_dep,
  c_args: common_cflags)

install_data('org.gtk.litos.gschema.xml',
   install_dir: get_option('datadir') / 'glib-2.0/schemas',
)

gnome = import('gnome')
gnome.post_install(glib_compile_schemas: true)
