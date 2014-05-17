/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/thread/ags_thread_hangcheck.h>

void ags_thread_hangcheck_class_init(AgsThreadHangcheckClass *thread);
void ags_thread_hangcheck_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_hangcheck_init(AgsThreadHangcheck *thread);
void ags_thread_hangcheck_connect(AgsConnectable *connectable);
void ags_thread_hangcheck_disconnect(AgsConnectable *connectable);
void ags_thread_hangcheck_finalize(GObject *gobject);

static gpointer ags_thread_hangcheck_parent_class = NULL;

GType
ags_thread_hangcheck_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    const GTypeInfo ags_thread_hangcheck_info = {
      sizeof (AgsThreadHangcheckClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_hangcheck_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThreadHangcheck),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_hangcheck_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_hangcheck_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThreadHangcheck\0",
					     &ags_thread_hangcheck_info,
					     0);
        
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_thread);
}

void
ags_thread_hangcheck_class_init(AgsThreadHangcheckClass *hangcheck_thread)
{
  GObjectClass *gobject;

  ags_thread_hangcheck_parent_class = g_type_class_peek_parent(hangcheck_thread);

  /* GObject */
  gobject = (GObjectClass *) hangcheck_thread;

  gobject->finalize = ags_thread_hangcheck_finalize;
}

void
ags_thread_hangcheck_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_thread_hangcheck_connect;
  connectable->disconnect = ags_thread_hangcheck_disconnect;
}

void
ags_thread_hangcheck_init(AgsThreadHangcheck *hangcheck_thread)
{
  thread_hangcheck->thread = NULL;
}

void
ags_thread_hangcheck_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_finalize(GObject *gobject)
{
  AgsThreadHangcheck *thread_hangcheck;

  thread_hangcheck = AGS_THREAD_HANGCHECK(gobject);
  
  ags_list_free_and_unref_data(thread_hangcheck->thread);
}


AgsThreadHangcheck*
ags_thread_hangcheck_new()
{
  AgsThreadHangcheck *thread_hangcheck;

  thread_hangcheck = (AgsThreadHangcheck *) g_object_new(AGS_TYPE_THREAD_HANGCHECK,
							 NULL);

  return(thread_hangcheck);
}
