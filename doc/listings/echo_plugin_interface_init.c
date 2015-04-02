void
ags_echo_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_echo_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_echo_channel_set_ports;
}
