/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_lv2_browser_callbacks.h>

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/object/ags_applicable.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>

void
ags_lv2_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					 AgsLv2Browser *lv2_browser)
{
  GtkComboBoxText *filename, *effect;

  AgsLv2Manager *lv2_manager;
  
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model((GtkComboBox *) effect)));

  lv2_manager = ags_lv2_manager_get_instance();
  list = lv2_manager->lv2_plugin;

  while((list = ags_base_plugin_find_filename(list, gtk_combo_box_text_get_active_text(filename))) != NULL){
    gtk_combo_box_text_append_text(effect,
				   g_strdup_printf("%s\0", AGS_BASE_PLUGIN(list->data)->effect));

    list = list->next;
  }
  
  gtk_combo_box_set_active((GtkComboBox *) effect,
  			   0);
}

void
ags_lv2_browser_plugin_uri_callback(GtkComboBoxText *combo_box,
				    AgsLv2Browser *lv2_browser)
{
  GtkTable *table;
  GtkComboBoxText *filename, *effect;
  GtkLabel *label;

  AgsLv2Plugin *lv2_plugin;

  GList *list, *list_start;
  GList *child, *child_start;

  gchar *str;
  guint y;

  /* retrieve filename and uri */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  g_list_free(list_start);

  /* update description */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->description));

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       gtk_combo_box_text_get_active_text(filename),
					       gtk_combo_box_text_get_active_text(effect));

  if(lv2_plugin != NULL){
    GList *port_descriptor;
    
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strdup_printf("Name: %s\0",
				       lv2_plugin->foaf_name));

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strdup_printf("Homepage: %s\0",
				       lv2_plugin->foaf_homepage));

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strdup_printf("M-Box: %s\0",
				       lv2_plugin->foaf_mbox));

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Ports: \0");

    list = list->next;
    table = GTK_TABLE(list->data);

    /* update ui - port information */
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(table));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));
      
      child = child->next;
    }
    
    g_list_free(child_start);

    port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

    y = 0;
    
    while(port_descriptor != NULL){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) == 0){
	port_descriptor = port_descriptor->next;
	
	continue;
      }
      
      str = g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name);

      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign\0", 0.0,
					"label\0", str,
					NULL);
      gtk_table_attach_defaults(table,
				GTK_WIDGET(label),
				0, 1,
				y, y + 1);
      
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	gtk_table_attach_defaults(table,
				  GTK_WIDGET(ags_lv2_browser_combo_box_boolean_controls_new()),
				  1, 2,
				  y, y + 1);
      }else{
	gtk_table_attach_defaults(table,
				  GTK_WIDGET(ags_lv2_browser_combo_box_controls_new()),
				  1, 2,
				  y, y + 1);
      }
	
      y++;
      
      port_descriptor = port_descriptor->next;
    }
    
    gtk_widget_show_all((GtkWidget *) table);
  }else{
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Name: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Homepage: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "M-Box: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Ports: \0");

    list = list->next;
    table = GTK_TABLE(list->data);
    
    /* update ui - no ports */
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(table));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(child_start);
  }

  g_list_free(list_start);
}
