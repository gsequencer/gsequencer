# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

i18n = import('i18n')
add_project_arguments('-DPACKAGE="@0@"'.format(meson.project_name()),
                                                      language:'c')
add_project_arguments('-DLOCALEDIR="@0@"'.format(meson.source_root()),
                      language:'c')
i18n.gettext(meson.project_name(),
    args: '--directory=' + meson.source_root()
)
