GType
ags_echo_channel_get_type()
{
  static GType ags_type_echo_channel = 0;

  if(!ags_type_echo_channel){
    static const GTypeInfo ags_echo_channel_info = {
      sizeof (AgsEchoChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_echo_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsEchoChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_echo_channel_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_echo_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_echo_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsEchoChannel\0",
						   &ags_echo_channel_info,
						   0);

    g_type_add_interface_static(ags_type_echo_channel,
                                AGS_TYPE_PLUGIN,
                                &ags_plugin_interface_info);
  }

  return(ags_type_echo_channel);
}
