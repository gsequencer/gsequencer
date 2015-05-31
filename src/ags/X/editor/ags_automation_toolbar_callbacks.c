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

#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

void
ags_automation_toolbar_machine_changed_callback(AgsAutomationEditor *automation_editor,
						AgsMachine *machine,
						AgsAutomationToolbar *toolbar)
{
  AgsAutomationEdit *automation_edit;
  AgsAutomation *automation;

  automation_edit = automation_editor->automation_edit;
  
  /* load ports */
  ags_automation_toolbar_load_port(automation_editor->automation_toolbar);
}


void
ags_automation_toolbar_port_changed_callback(GtkComboBox *combo_box,
					     AgsAutomationToolbar *automation_toolbar)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GValue value = {0,};
  
  model = gtk_combo_box_get_model(combo_box);
  gtk_combo_box_get_active_iter(combo_box, &iter);

  gtk_tree_model_get_value(model,
			   &iter,
			   0,
			   &value);

  g_value_set_boolean(&value, !g_value_get_boolean(&value));
  gtk_list_store_set_value(GTK_LIST_STORE(model),
			   &iter,
			   0,
			   &value);
}
