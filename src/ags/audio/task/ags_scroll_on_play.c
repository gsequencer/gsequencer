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

#include <ags/audio/task/ags_scroll_on_play.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

void ags_scroll_on_play_class_init(AgsScrollOnPlayClass *scroll_on_play);
void ags_scroll_on_play_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_scroll_on_play_init(AgsScrollOnPlay *scroll_on_play);
void ags_scroll_on_play_connect(AgsConnectable *connectable);
void ags_scroll_on_play_disconnect(AgsConnectable *connectable);
void ags_scroll_on_play_finalize(GObject *gobject);

void ags_scroll_on_play_launch(AgsTask *task);

static gpointer ags_scroll_on_play_parent_class = NULL;
static AgsConnectableInterface *ags_scroll_on_play_parent_connectable_interface;

GType
ags_scroll_on_play_get_type()
{
  static GType ags_type_scroll_on_play = 0;

  if(!ags_type_scroll_on_play){
    static const GTypeInfo ags_scroll_on_play_info = {
      sizeof (AgsScrollOnPlayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scroll_on_play_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrollOnPlay),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scroll_on_play_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_scroll_on_play_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_scroll_on_play = g_type_register_static(AGS_TYPE_TASK,
						     "AgsScrollOnPlay\0",
						     &ags_scroll_on_play_info,
						     0);

    g_type_add_interface_static(ags_type_scroll_on_play,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_scroll_on_play);
}

void
ags_scroll_on_play_class_init(AgsScrollOnPlayClass *scroll_on_play)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_scroll_on_play_parent_class = g_type_class_peek_parent(scroll_on_play);

  /* gobject */
  gobject = (GObjectClass *) scroll_on_play;

  gobject->finalize = ags_scroll_on_play_finalize;

  /* task */
  task = (AgsTaskClass *) scroll_on_play;

  task->launch = ags_scroll_on_play_launch;
}

void
ags_scroll_on_play_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_scroll_on_play_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_scroll_on_play_connect;
  connectable->disconnect = ags_scroll_on_play_disconnect;
}

void
ags_scroll_on_play_init(AgsScrollOnPlay *scroll_on_play)
{
}

void
ags_scroll_on_play_connect(AgsConnectable *connectable)
{
  ags_scroll_on_play_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_scroll_on_play_disconnect(AgsConnectable *connectable)
{
  ags_scroll_on_play_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_scroll_on_play_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_scroll_on_play_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_scroll_on_play_launch(AgsTask *task)
{
}

AgsScrollOnPlay*
ags_scroll_on_play_new(GtkWidget *editor)
{
  AgsScrollOnPlay *scroll_on_play;

  scroll_on_play = (AgsScrollOnPlay *) g_object_new(AGS_TYPE_SCROLL_ON_PLAY,
						    NULL);

  return(scroll_on_play);
}
