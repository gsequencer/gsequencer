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
  GList *port;
  GList *list, *list_start;

  auto GList* ags_automation_toolbar_enabled_ports(GList *automation);

  GList* ags_automation_toolbar_enabled_ports(GList *automation){
    GList *list;

    list = NULL;

    while(automation != NULL){
      list = g_list_prepend(list,
			    AGS_AUTOMATION(automation->data)->port);

      automation = automation->next;
    }
    
    list = g_list_reverse(list);

    return(list);
  }

  automation_edit = automation_editor->automation_edit;

  /* destroy old and create new drawing area vbox */
  gtk_widget_destroy(automation_edit->drawing_area);

  automation_edit->drawing_area = gtk_vbox_new(FALSE, 0);
  gtk_table_attach(GTK_TABLE(automation_edit), (GtkWidget *) automation_edit->drawing_area,
		   0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* load ports */
  ags_port_selection_load_ports(toolbar->port_selection);

  /* enable ports */
  port = ags_automation_toolbar_enabled_ports(machine->audio->automation);

  ags_port_selection_enable_ports(toolbar->port_selection,
				  port);
}
