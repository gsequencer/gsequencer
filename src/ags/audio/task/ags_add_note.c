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

#include <ags/audio/task/ags_add_note.h>

#include <ags-lib/object/ags_connectable.h>

void ags_add_note_class_init(AgsAddNoteClass *add_note);
void ags_add_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_note_init(AgsAddNote *add_note);
void ags_add_note_connect(AgsConnectable *connectable);
void ags_add_note_disconnect(AgsConnectable *connectable);
void ags_add_note_finalize(GObject *gobject);

void ags_add_note_launch(AgsTask *task);

/**
 * SECTION:ags_add_note
 * @short_description: add note object to notation
 * @title: AgsAddNote
 * @section_id:
 * @include: ags/audio/task/ags_add_note.h
 *
 * The #AgsAddNote task adds #AgsNote to #AgsNotation.
 */

static gpointer ags_add_note_parent_class = NULL;
static AgsConnectableInterface *ags_add_note_parent_connectable_interface;

GType
ags_add_note_get_type()
{
  static GType ags_type_add_note = 0;

  if(!ags_type_add_note){
    static const GTypeInfo ags_add_note_info = {
      sizeof (AgsAddNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_note = g_type_register_static(AGS_TYPE_TASK,
					       "AgsAddNote\0",
					       &ags_add_note_info,
					       0);

    g_type_add_interface_static(ags_type_add_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_note);
}

void
ags_add_note_class_init(AgsAddNoteClass *add_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_note_parent_class = g_type_class_peek_parent(add_note);

  /* gobject */
  gobject = (GObjectClass *) add_note;

  gobject->finalize = ags_add_note_finalize;

  /* task */
  task = (AgsTaskClass *) add_note;

  task->launch = ags_add_note_launch;
}

void
ags_add_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_note_connect;
  connectable->disconnect = ags_add_note_disconnect;
}

void
ags_add_note_init(AgsAddNote *add_note)
{
  add_note->notation = NULL;
  add_note->note = NULL;
  add_note->use_selection_list = FALSE;
}

void
ags_add_note_connect(AgsConnectable *connectable)
{
  ags_add_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_note_disconnect(AgsConnectable *connectable)
{
  ags_add_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_note_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_note_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_note_launch(AgsTask *task)
{
  AgsAddNote *add_note;

  add_note = AGS_ADD_NOTE(task);

  /* add note */
  ags_notation_add_note(add_note->notation,
			add_note->note,
			add_note->use_selection_list);
}

/**
 * ags_add_note_new:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to add
 * @use_selection_list: if %TRUE added to selection, otherwise to notation
 *
 * Creates an #AgsAddNote.
 *
 * Returns: an new #AgsAddNote.
 *
 * Since: 0.4
 */
AgsAddNote*
ags_add_note_new(AgsNotation *notation,
		 AgsNote *note,
		 gboolean use_selection_list)
{
  AgsAddNote *add_note;

  add_note = (AgsAddNote *) g_object_new(AGS_TYPE_ADD_NOTE,
					 NULL);

  add_note->notation = notation;
  add_note->note = note;
  add_note->use_selection_list = use_selection_list;

  return(add_note);
}
