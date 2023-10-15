app_resources = gnome.compile_resources('litos_resources',
  'litos.gresource.xml',
  source_dir: meson.current_source_dir())

app_schemas = gnome.compile_schemas()

executable('litos',
  'main.c',
  'litos.c',
  'litoswin.c',
  'litosprefs.c',
  app_resources,
  app_schemas,
  dependencies: libgtk_dep,
  c_args: common_cflags)
