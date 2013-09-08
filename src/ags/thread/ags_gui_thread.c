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

#include <ags/thread/ags_gui_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <math.h>

void ags_gui_thread_class_init(AgsGuiThreadClass *gui_thread);
void ags_gui_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gui_thread_init(AgsGuiThread *gui_thread);
void ags_gui_thread_connect(AgsConnectable *connectable);
void ags_gui_thread_disconnect(AgsConnectable *connectable);
void ags_gui_thread_finalize(GObject *gobject);

void ags_gui_thread_start(AgsThread *thread);
void ags_gui_thread_run(AgsThread *thread);

static gpointer ags_gui_thread_parent_class = NULL;
static AgsConnectableInterface *ags_gui_thread_parent_connectable_interface;

GType
ags_gui_thread_get_type()
{
  static GType ags_type_gui_thread = 0;

  if(!ags_type_gui_thread){
    static const GTypeInfo ags_gui_thread_info = {
      sizeof (AgsGuiThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gui_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGuiThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gui_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gui_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gui_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsGuiThread\0",
						    &ags_gui_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_gui_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_gui_thread);
}

void
ags_gui_thread_class_init(AgsGuiThreadClass *gui_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_gui_thread_parent_class = g_type_class_peek_parent(gui_thread);

  /* GObject */
  gobject = (GObjectClass *) gui_thread;

  gobject->finalize = ags_gui_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) gui_thread;

  thread->start = ags_gui_thread_start;
  thread->run = ags_gui_thread_run;
}

void
ags_gui_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_gui_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_gui_thread_connect;
  connectable->disconnect = ags_gui_thread_disconnect;
}

void
ags_gui_thread_init(AgsGuiThread *gui_thread)
{
  gui_thread->frequency = 200.0 / 70.0;
}

void
ags_gui_thread_connect(AgsConnectable *connectable)
{
  ags_gui_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_gui_thread_disconnect(AgsConnectable *connectable)
{
  ags_gui_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_gui_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_gui_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_gui_thread_start(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  gui_thread = AGS_GUI_THREAD(thread);

  gui_thread->iter = 0.0;

  AGS_THREAD_CLASS(ags_gui_thread_parent_class)->start(thread);
}

void
ags_gui_thread_run(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  gui_thread = AGS_GUI_THREAD(thread);

  if(gui_thread->iter > gui_thread->frequency){
    gtk_main_iteration();

    gui_thread->iter = 0.0;
  }else{
    gui_thread->iter += (gdouble) ceil(1.0 / gui_thread->frequency);
  }
}

AgsGuiThread*
ags_gui_thread_new()
{
  AgsGuiThread *gui_thread;

  gui_thread = (AgsGuiThread *) g_object_new(AGS_TYPE_GUI_THREAD,
						   NULL);


  return(gui_thread);
}
