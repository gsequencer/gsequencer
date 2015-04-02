ags_echo_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
                "./delay[0]\0",
                10)){
      g_object_set(G_OBJECT(plugin),
                   "delay\0", AGS_PORT(port->data),
                   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
                      "./repeat[0]\0",
                      11)){
      g_object_set(G_OBJECT(plugin),
                   "repeat\0", AGS_PORT(port->data),
                   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
                      "./fade[0]\0",
                      9)){
      g_object_set(G_OBJECT(plugin),
                   "fade\0", AGS_PORT(port->data),
                   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
                      "./dry[0]\0",
                      8)){
      g_object_set(G_OBJECT(plugin),
                   "dry\0", AGS_PORT(port->data),
                   NULL);
    }

    port = port->next;
  }
}
