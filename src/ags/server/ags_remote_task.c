/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/server/ags_remote_task.h>

#include <ags/object/ags_connectable.h>

void ags_remote_task_class_init(AgsRemoteTaskClass *remote_task);
void ags_remote_task_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remote_task_init(AgsRemoteTask *remote_task);
void ags_remote_task_connect(AgsConnectable *connectable);
void ags_remote_task_disconnect(AgsConnectable *connectable);
void ags_remote_task_finalize(GObject *gobject);

static gpointer ags_remote_task_parent_class = NULL;

GType
ags_remote_task_get_type()
{
  static GType ags_type_remote_task = 0;

  if(!ags_type_remote_task){
    static const GTypeInfo ags_remote_task_info = {
      sizeof (AgsRemoteTaskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remote_task_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoteTask),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remote_task_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remote_task_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remote_task = g_type_register_static(G_TYPE_OBJECT,
					   "AgsRemoteTask\0",
					   &ags_remote_task_info,
					   0);

    g_type_add_interface_static(ags_type_remote_task,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_remote_task);
}

void
ags_remote_task_class_init(AgsRemoteTaskClass *remote_task)
{
  GObjectClass *gobject;

  ags_remote_task_parent_class = g_type_class_peek_parent(remote_task);

  /* GObjectClass */
  gobject = (GObjectClass *) remote_task;

  gobject->finalize = ags_remote_task_finalize;
}

void
ags_remote_task_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_remote_task_connect;
  connectable->disconnect = ags_remote_task_disconnect;
}

void
ags_remote_task_init(AgsRemoteTask *remote_task)
{
}

void
ags_remote_task_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_remote_task_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_remote_task_finalize(GObject *gobject)
{
  AgsRemoteTask *remote_task;

  remote_task = AGS_REMOTE_TASK(gobject);

  G_OBJECT_CLASS(ags_remote_task_parent_class)->finalize(gobject);
}

xmlrpc_value*
ags_remote_task_create(xmlrpc_env *env_p,
		       xmlrpc_value *param_array_p,
		       void *server_info)
{
  //TODO:JK: implement me
}

xmlrpc_value*
ags_remote_task_launch(xmlrpc_env *env_p,
		       xmlrpc_value *param_array_p,
		       void *server_info)
{
  //TODO:JK: implement me
}

xmlrpc_value*
ags_remote_task_create_and_launch(xmlrpc_env *env_p,
				  xmlrpc_value *param_array_p,
				  void *server_info)
{
  //TODO:JK: implement me
}

xmlrpc_value*
ags_remote_task_create_and_launch_timed(xmlrpc_env *env_p,
					xmlrpc_value *param_array_p,
					void *server_info)
{
  //TODO:JK: implement me
}

AgsRemoteTask*
ags_remote_task_new()
{
  AgsRemoteTask *remote_task;

  remote_task = (AgsRemoteTask *) g_object_new(AGS_TYPE_REMOTE_TASK,
					       NULL);

  return(remote_task);
}
