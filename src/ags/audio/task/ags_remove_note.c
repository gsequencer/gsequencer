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

#include <ags/audio/task/ags_remove_note.h>

#include <ags/object/ags_connectable.h>

void ags_remove_note_class_init(AgsRemoveNoteClass *remove_note);
void ags_remove_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_note_init(AgsRemoveNote *remove_note);
void ags_remove_note_connect(AgsConnectable *connectable);
void ags_remove_note_disconnect(AgsConnectable *connectable);
void ags_remove_note_finalize(GObject *gobject);

void ags_remove_note_launch(AgsTask *task);

static gpointer ags_remove_note_parent_class = NULL;
static AgsConnectableInterface *ags_remove_note_parent_connectable_interface;

GType
ags_remove_note_get_type()
{
  static GType ags_type_remove_note = 0;

  if(!ags_type_remove_note){
    static const GTypeInfo ags_remove_note_info = {
      sizeof (AgsRemoveNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_note = g_type_register_static(AGS_TYPE_TASK,
						   "AgsRemoveNote\0",
						   &ags_remove_note_info,
						   0);

    g_type_add_interface_static(ags_type_remove_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_note);
}

void
ags_remove_note_class_init(AgsRemoveNoteClass *remove_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_remove_note_parent_class = g_type_class_peek_parent(remove_note);

  /* gobject */
  gobject = (GObjectClass *) remove_note;

  gobject->finalize = ags_remove_note_finalize;

  /* task */
  task = (AgsTaskClass *) remove_note;

  task->launch = ags_remove_note_launch;
}

void
ags_remove_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_note_connect;
  connectable->disconnect = ags_remove_note_disconnect;
}

void
ags_remove_note_init(AgsRemoveNote *remove_note)
{
  remove_note->notation = NULL;
  remove_note->x = 0;
  remove_note->y = 0;
}

void
ags_remove_note_connect(AgsConnectable *connectable)
{
  ags_remove_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_note_disconnect(AgsConnectable *connectable)
{
  ags_remove_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_note_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_note_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_note_launch(AgsTask *task)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(task);

  /* remove note */
  ags_notation_remove_note_at_position(remove_note->notation,
				       remove_note->x, remove_note->y);
}

AgsRemoveNote*
ags_remove_note_new(AgsNotation *notation,
		    guint x, guint y)
{
  AgsRemoveNote *remove_note;

  remove_note = (AgsRemoveNote *) g_object_new(AGS_TYPE_REMOVE_NOTE,
					       NULL);

  remove_note->notation = notation;
  remove_note->x = x;
  remove_note->y = y;

  return(remove_note);
}
