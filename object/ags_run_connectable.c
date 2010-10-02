#include "ags_run_connectable.h"

#include <stdio.h>

GType ags_run_connectable_get_type();
void ags_run_connectable_base_init(AgsRunConnectableInterface *interface);

GType
ags_run_connectable_get_type()
{
  static GType ags_type_run_connectable = 0;

  if(!ags_type_run_connectable){
    static const GTypeInfo ags_run_connectable_info = {
      sizeof(AgsRunConnectableInterface),
      (GBaseInitFunc) ags_run_connectable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_run_connectable = g_type_register_static(G_TYPE_INTERFACE,
						      "AgsRunConnectable\0", &ags_run_connectable_info,
						      0);
  }

  return(ags_type_run_connectable);
}

void
ags_run_connectable_base_init(AgsRunConnectableInterface *interface)
{
  //  interface->connect = NULL;
  //  interface->disconnect = NULL;
}

void
ags_run_connectable_connect(AgsRunConnectable *connectable)
{
  AgsRunConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_RUN_CONNECTABLE(connectable));
  connectable_interface = AGS_RUN_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect);
  connectable_interface->connect(connectable);
}

void
ags_run_connectable_disconnect(AgsRunConnectable *connectable)
{
  AgsRunConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_RUN_CONNECTABLE(connectable));
  connectable_interface = AGS_RUN_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect);
  connectable_interface->connect(connectable);
}
