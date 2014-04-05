/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011, 2014 Joël Krähemann
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

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_note_edit_callbacks.h>

#include <ags/X/ags_editor.h>

void ags_note_edit_class_init(AgsNoteEditClass *note_edit);
void ags_note_edit_init(AgsNoteEdit *note_edit);
void ags_note_edit_connect(AgsNoteEdit *note_edit);
void ags_note_edit_destroy(GtkObject *object);
void ags_note_edit_show(GtkWidget *widget);
void ags_note_edit_paint(AgsNoteEdit *note_edit);

GtkStyle *note_edit_style;

GType
ags_note_edit_get_type(void)
{
  static GType ags_type_note_edit = 0;

  if(!ags_type_note_edit){
    static const GTypeInfo ags_note_edit_info = {
      sizeof (AgsNoteEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_note_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNoteEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_note_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_note_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_note_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsNoteEdit\0", &ags_note_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_note_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_note_edit);
}

void
ags_note_edit_class_init(AgsNoteEditClass *note_edit)
{
}

void
ags_note_edit_init(AgsNoteEdit *note_edit)
{
}

void
ags_note_edit_connect(AgsNoteEdit *note_edit)
{
}

void
ags_note_edit_destroy(GtkObject *object)
{
}

void
ags_note_edit_show(GtkWidget *widget)
{
}

AgsNoteEdit*
ags_note_edit_new()
{
  AgsNoteEdit *note_edit;

  note_edit = (AgsNoteEdit *) g_object_new(AGS_TYPE_NOTE_EDIT, NULL);

  return(note_edit);
}

