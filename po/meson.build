i18n = import('i18n')
add_project_arguments('-DPACKAGE="@0@"'.format(meson.project_name()),
                                                      language:'c')
add_project_arguments('-DLOCALEDIR="@0@"'.format(meson.source_root()),
                      language:'c')
i18n.gettext(meson.project_name(),
    args: '--directory=' + meson.source_root()
)
