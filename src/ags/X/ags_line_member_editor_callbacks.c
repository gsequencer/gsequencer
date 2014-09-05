/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_line_member_editor_callbacks.h>

void
ags_line_member_editor_add_callback(GtkWidget *button,
				    AgsLineMemberEditor *line_member_editor)
{
  gtk_widget_show_all(line_member_editor->ladspa_browser);
}

void
ags_line_member_editor_ladspa_browser_response_callback(GtkDialog *dialog,
							gint response,
							AgsLineMemberEditor *line_member_editor)
{
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      GtkHBox *hbox;
      GtkCheckButton *check_button;
      GtkLabel *label;
      gchar *filename, *effect;

      filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
      effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

      hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start(GTK_BOX(line_member_editor->line_member),
			 GTK_WIDGET(hbox),
			 FALSE, FALSE,
			 0);
      
      check_button = (GtkCheckButton *) gtk_check_button_new();
      gtk_box_pack_start(GTK_BOX(hbox),
			 GTK_WIDGET(check_button),
			 FALSE, FALSE,
			 0);

      label = (GtkLabel *) gtk_label_new(g_strdup_printf("%s: %s\0",
							 filename,
							 effect));
      gtk_box_pack_start(GTK_BOX(hbox),
			 GTK_WIDGET(label),
			 FALSE, FALSE,
			 0);
      gtk_widget_show_all((GtkWidget *) hbox);
    }
    break;
  }
}

void
ags_line_member_editor_remove_callback(GtkWidget *button,
				       AgsLineMemberEditor *line_member_editor)
{
  GList *line_member;
  GList *children;

  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  line_member = gtk_container_get_children(GTK_CONTAINER(line_member_editor->line_member));

  while(line_member != NULL){
    children = gtk_container_get_children(GTK_CONTAINER(line_member->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
      //TODO:JK: implement me

      gtk_widget_destroy(GTK_WIDGET(line_member->data));
    }

    line_member = line_member->next;
  }
}
