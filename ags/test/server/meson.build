# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

static_tests = [
  'ags_functional_server_test',
  'ags_server_application_context_test',
  'ags_server_test',
  'controller/ags_controller_test',
  'controller/ags_front_controller_test',
  'security/ags_authentication_manager_test',
  'security/ags_business_group_manager_test',
  'security/ags_certificate_manager_test',
  'security/ags_password_store_manager_test',
  'security/ags_security_context_test',
  'security/ags_xml_authentication_test',
  'security/ags_xml_business_group_test',
  'security/ags_xml_certificate_test',
  'security/ags_xml_password_store_test',
]

static_test_dependencies = [
  common_test_dependencies,
  xml2_dependency,
  soup_dependency,
  m_dependency,
]

foreach static_test : static_tests
  source = ['@0@.c'.format(static_test)]
  if static_test.contains('/')
    tmp = static_test.split('/')
    static_test = '_'.join(tmp)
  endif

  static_test_bin = executable(
    static_test,
    source,
    c_args: [compiler_flags, srcdir_def],
    include_directories: [includes],
    dependencies: [static_test_dependencies],
    link_with: [
      liblib,
      libobject,
      libthread,
      libserver,
    ]
  )

  test(static_test, static_test_bin)
endforeach

