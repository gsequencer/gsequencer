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

#include <ags/audio/task/ags_set_devout_play_flags.h>

#include <ags/object/ags_connectable.h>

void ags_set_devout_play_flags_class_init(AgsSetDevoutPlayFlagsClass *set_devout_play_flags);
void ags_set_devout_play_flags_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_devout_play_flags_init(AgsSetDevoutPlayFlags *set_devout_play_flags);
void ags_set_devout_play_flags_connect(AgsConnectable *connectable);
void ags_set_devout_play_flags_disconnect(AgsConnectable *connectable);
void ags_set_devout_play_flags_finalize(GObject *gobject);

void ags_set_devout_play_flags_launch(AgsTask *task);

static gpointer ags_set_devout_play_flags_parent_class = NULL;
static AgsConnectableInterface *ags_set_devout_play_flags_parent_connectable_interface;

GType
ags_set_devout_play_flags_get_type()
{
  static GType ags_type_set_devout_play_flags = 0;

  if(!ags_type_set_devout_play_flags){
    static const GTypeInfo ags_set_devout_play_flags_info = {
      sizeof (AgsSetDevoutPlayFlagsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_devout_play_flags_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetDevoutPlayFlags),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_devout_play_flags_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_devout_play_flags_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_devout_play_flags = g_type_register_static(AGS_TYPE_TASK,
						   "AgsSetDevoutPlayFlags",
						   &ags_set_devout_play_flags_info,
						   0);

    g_type_add_interface_static(ags_type_set_devout_play_flags,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_devout_play_flags);
}

void
ags_set_devout_play_flags_class_init(AgsSetDevoutPlayFlagsClass *set_devout_play_flags)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_set_devout_play_flags_parent_class = g_type_class_peek_parent(set_devout_play_flags);

  /* gobject */
  gobject = (GObjectClass *) set_devout_play_flags;

  gobject->finalize = ags_set_devout_play_flags_finalize;

  /* task */
  task = (AgsTaskClass *) set_devout_play_flags;

  task->launch = ags_set_devout_play_flags_launch;
}

void
ags_set_devout_play_flags_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_devout_play_flags_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_devout_play_flags_connect;
  connectable->disconnect = ags_set_devout_play_flags_disconnect;
}

void
ags_set_devout_play_flags_init(AgsSetDevoutPlayFlags *set_devout_play_flags)
{
  set_devout_play_flags->devout_play = NULL;
  set_devout_play_flags->devout_play_flags = 0;
}

void
ags_set_devout_play_flags_connect(AgsConnectable *connectable)
{
  ags_set_devout_play_flags_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_devout_play_flags_disconnect(AgsConnectable *connectable)
{
  ags_set_devout_play_flags_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_devout_play_flags_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_set_devout_play_flags_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_set_devout_play_flags_launch(AgsTask *task)
{
  AgsSetDevoutPlayFlags *set_devout_play_flags;

  set_devout_play_flags = AGS_SET_DEVOUT_PLAY_FLAGS(task);

  /* append to AgsDevout */
  set_devout_play_flags->devout_play->flags |= (set_devout_play_flags->devout_play_flags);
}

AgsSetDevoutPlayFlags*
ags_set_devout_play_flags_new(AgsDevoutPlay *devout_play,
			      guint flags)
{
  AgsSetDevoutPlayFlags *set_devout_play_flags;

  set_devout_play_flags = (AgsSetDevoutPlayFlags *) g_object_new(AGS_TYPE_SET_DEVOUT_PLAY_FLAGS,
								 NULL);

  set_devout_play_flags->devout_play = devout_play;
  set_devout_play_flags->devout_play_flags = flags;

  return(set_devout_play_flags);
}
