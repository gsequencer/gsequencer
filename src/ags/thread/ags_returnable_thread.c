/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/thread/ags_returnable_thread.h>

#include <ags-lib/object/ags_connectable.h>

void ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread);
void ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_returnable_thread_init(AgsReturnableThread *returnable_thread);
void ags_returnable_thread_connect(AgsConnectable *connectable);
void ags_returnable_thread_disconnect(AgsConnectable *connectable);
void ags_returnable_thread_finalize(GObject *gobject);

static gpointer ags_returnable_thread_parent_class = NULL;

GType
ags_returnable_thread_get_type()
{
  static GType ags_type_returnable_thread = 0;

  if(!ags_type_returnable_thread){
    static const GTypeInfo ags_returnable_thread_info = {
      sizeof (AgsReturnableThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_returnable_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsReturnableThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_returnable_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_returnable_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_returnable_thread = g_type_register_static(G_TYPE_OBJECT,
							"AgsReturnableThread\0",
							&ags_returnable_thread_info,
							0);

    g_type_add_interface_static(ags_type_returnable_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_returnable_thread);
}

void
ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread)
{
  GObjectClass *gobject;

  ags_returnable_thread_parent_class = g_type_class_peek_parent(returnable_thread);

  /* GObjectClass */
  gobject = (GObjectClass *) returnable_thread;

  gobject->finalize = ags_returnable_thread_finalize;
}

void
ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_returnable_thread_connect;
  connectable->disconnect = ags_returnable_thread_disconnect;
}

void
ags_returnable_thread_init(AgsReturnableThread *returnable_thread)
{
}

void
ags_returnable_thread_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_returnable_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_returnable_thread_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_returnable_thread_parent_class)->finalize(gobject);
}

AgsReturnableThread*
ags_returnable_thread_new()
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = (AgsReturnableThread *) g_object_new(AGS_TYPE_RETURNABLE_THREAD,
							   NULL);

  return(returnable_thread);
}
