if get_option('gtk_doc')
  configure_file(input: 'version.xml.in', output: 'version.xml', configuration: version_conf)

  # gtk-markdown-to-docbook uses pandoc
  pandoc = find_program('pandoc', required: true)
  expand_md = find_program('gtk-markdown-to-docbook')
  expand_md_targets = []
  foreach t : expand_content_md_files
    expand_md_targets += custom_target(t,
                                       input: [ t ],
                                       output: [ fs.replace_suffix(t, '.xml') ],
                                       command: [ expand_md, '@INPUT@', '@OUTPUT@'])
  endforeach

  gnome.gtkdoc('libags-audio',
               mode: 'none',
               main_xml: 'libags.xml',
               src_dir: [
                 includes,
               ],
               dependencies: libgtk_dep,
               gobject_typesfile: configure_file(
                 input: 'libags.types.in',
                 output: 'libags.types',
                 configuration: types_conf,
               ),
               scan_args: [
                 '--ignore-decorators=_GDK_EXTERN|G_GNUC_WARN_UNUSED_RESULT',
                 '--ignore-headers=' + ' '.join(private_headers),
               ],
               mkdb_args: [
                 '--default-includes=gtk/gtk.h',
               ],
               fixxref_args: [
                 '--html-dir=@0@'.format(docpath),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
                 '--extra-dir=@0@'.format(cairo_docpath),
                 '--extra-dir=@0@'.format(gdkpixbuf_docpath),
		 '--extra-dir=../libags',
		 '--extra-dir=../libags-audio',
		 '--extra-dir=../libags-gui',
               ],
               content_files: content_files + expand_md_targets,
               html_assets: images,
               install: true)
endif
