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

#include <ags/audio/task/ags_insert_note.h>

#include <ags/object/ags_connectable.h>

void ags_insert_note_class_init(AgsInsertNoteClass *insert_note);
void ags_insert_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_insert_note_init(AgsInsertNote *insert_note);
void ags_insert_note_connect(AgsConnectable *connectable);
void ags_insert_note_disconnect(AgsConnectable *connectable);
void ags_insert_note_finalize(GObject *gobject);

void ags_insert_note_launch(AgsTask *task);

static gpointer ags_insert_note_parent_class = NULL;
static AgsConnectableInterface *ags_insert_note_parent_connectable_interface;

GType
ags_insert_note_get_type()
{
  static GType ags_type_insert_note = 0;

  if(!ags_type_insert_note){
    static const GTypeInfo ags_insert_note_info = {
      sizeof (AgsInsertNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_insert_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInsertNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_insert_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_insert_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_insert_note = g_type_register_static(AGS_TYPE_TASK,
						   "AgsInsertNote\0",
						   &ags_insert_note_info,
						   0);

    g_type_add_interface_static(ags_type_insert_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_insert_note);
}

void
ags_insert_note_class_init(AgsInsertNoteClass *insert_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_insert_note_parent_class = g_type_class_peek_parent(insert_note);

  /* gobject */
  gobject = (GObjectClass *) insert_note;

  gobject->finalize = ags_insert_note_finalize;

  /* task */
  task = (AgsTaskClass *) insert_note;

  task->launch = ags_insert_note_launch;
}

void
ags_insert_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_insert_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_insert_note_connect;
  connectable->disconnect = ags_insert_note_disconnect;
}

void
ags_insert_note_init(AgsInsertNote *insert_note)
{
  insert_note->notation = NULL;
  insert_note->note = NULL;
}

void
ags_insert_note_connect(AgsConnectable *connectable)
{
  ags_insert_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_insert_note_disconnect(AgsConnectable *connectable)
{
  ags_insert_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_insert_note_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_insert_note_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_insert_note_launch(AgsTask *task)
{
  AgsInsertNote *insert_note;

  insert_note = AGS_INSERT_NOTE(task);

  /* add note */
  ags_notation_add_note(insert_note->notation,
			insert_note->note,
			FALSE);
}

AgsInsertNote*
ags_insert_note_new(AgsNotation *notation,
		    AgsNote *note)
{
  AgsInsertNote *insert_note;

  insert_note = (AgsInsertNote *) g_object_new(AGS_TYPE_INSERT_NOTE,
					       NULL);

  insert_note->notation = notation;
  insert_note->note = note;

  return(insert_note);
}
