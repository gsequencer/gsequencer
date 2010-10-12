#include "ags_connectable.h"

#include <stdio.h>

void ags_connectable_base_init(AgsConnectableInterface *interface);

GType
ags_connectable_get_type()
{
  static GType ags_type_connectable = 0;

  if(!ags_type_connectable){
    static const GTypeInfo ags_connectable_info = {
      sizeof(AgsConnectableInterface),
      (GBaseInitFunc) ags_connectable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_connectable = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsConnectable\0", &ags_connectable_info,
						  0);
  }

  return(ags_type_connectable);
}

void
ags_connectable_base_init(AgsConnectableInterface *interface)
{
  //  interface->connect = NULL;
  //  interface->disconnect = NULL;
}

void
ags_connectable_connect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect);
  connectable_interface->connect(connectable);
}

void
ags_connectable_disconnect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect);
  connectable_interface->connect(connectable);
}
