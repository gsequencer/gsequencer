/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/app/ags_line_member_editor_entry.h>

void ags_line_member_editor_entry_class_init(AgsLineMemberEditorEntryClass *line_member_editor_entry);
void ags_line_member_editor_entry_init(AgsLineMemberEditorEntry *line_member_editor_entry);

/**
 * SECTION:ags_line_member_editor_entry
 * @short_description: line member editor entry
 * @title: AgsLineMemberEditorEntry
 * @section_id:
 * @include: ags/app/ags_line_member_editor_entry.h
 *
 * #AgsLineMemberEditorEntry is a composite widget to show line member
 * information.
 */

GType
ags_line_member_editor_entry_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line_member_editor_entry = 0;

    static const GTypeInfo ags_line_member_editor_entry_info = {
      sizeof (AgsLineMemberEditorEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_member_editor_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLineMemberEditorEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_member_editor_entry_init,
    };

    ags_type_line_member_editor_entry = g_type_register_static(GTK_TYPE_BOX,
							       "AgsLineMemberEditorEntry", &ags_line_member_editor_entry_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line_member_editor_entry);
  }

  return g_define_type_id__volatile;
}

void
ags_line_member_editor_entry_class_init(AgsLineMemberEditorEntryClass *line_member_editor_entry)
{
  /* empty */
}

void
ags_line_member_editor_entry_init(AgsLineMemberEditorEntry *line_member_editor_entry)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(line_member_editor_entry),
				 GTK_ORIENTATION_HORIZONTAL);  

  line_member_editor_entry->check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_append(line_member_editor_entry,
		 (GtkWidget *) line_member_editor_entry->check_button);

  line_member_editor_entry->label = (GtkLabel *) gtk_label_new(str);
  gtk_box_append(line_member_editor_entry,
		 (GtkWidget *) line_member_editor_entry->label);
}

/**
 * ags_line_member_editor_entry_new:
 *
 * Create a new instance of #AgsLineMemberEditorEntry
 *
 * Returns: the new #AgsLineMemberEditorEntry
 *
 * Since: 3.0.0
 */
AgsLineMemberEditorEntry*
ags_line_member_editor_entry_new()
{
  AgsLineMemberEditorEntry *line_member_editor_entry;

  line_member_editor_entry = (AgsLineMemberEditorEntry *) g_object_new(AGS_TYPE_LINE_MEMBER_EDITOR_ENTRY,
								       NULL);

  return(line_member_editor_entry);
}
