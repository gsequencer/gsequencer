configure_file(output: 'config.h', configuration: ags_config)

server_sources = files(
  'ags_registry.c',
  'ags_server_application_context.c',
  'ags_server.c',
  'ags_server_status.c',
  'ags_service_provider.c',
  'controller/ags_controller.c',
  'controller/ags_front_controller.c',
  'controller/ags_plugin_controller.c',
  'security/ags_authentication.c',
  'security/ags_authentication_manager.c',
  'security/ags_auth_security_context.c',
  'security/ags_business_group.c',
  'security/ags_business_group_manager.c',
  'security/ags_certificate.c',
  'security/ags_certificate_manager.c',
  'security/ags_password_store.c',
  'security/ags_password_store_manager.c',
  'security/ags_security_context.c',
  'security/ags_xml_authentication.c',
  'security/ags_xml_business_group.c',
  'security/ags_xml_certificate.c',
  'security/ags_xml_password_store.c',
)

server_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
  dependency('libsoup-2.4'),
  crypt_dependency,
]

libserver = static_library('server',
                        server_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: server_dependencies,
                       )
