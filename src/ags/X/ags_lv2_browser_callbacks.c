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

#include <ags/lib/ags_combo_box_text.h>

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
  GtkComboBoxText *filename, *uri;

  AgsLv2Plugin *lv2_plugin;

  GList *list;
  GList *node_list;

  gchar *str;
  
  list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  uri = GTK_COMBO_BOX(list->next->next->next->data);

  ags_combo_box_text_remove_all(uri);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(gtk_combo_box_text_get_active_text(filename));

  if(lv2_plugin == NULL){
    return;
  }

  node_list = ags_turtle_find_xpath(lv2_plugin->turtle,
				    "//rdf-triple/rdf-verb[@do=\"doap:name\"]/rdf-list/rdf-value[1]\0");

  while(node_list != NULL){
    str = xmlGetProp(((xmlNode *) node_list->data)->parent->parent->parent,
		     "subject\0");
    str = g_strndup(&(str[1]),
		    strlen(str) - 2);
    
    gtk_combo_box_text_append_text(uri,
				   str);
    
    node_list = node_list->next;
  }
  
  gtk_combo_box_set_active(uri,
  			   0);
}

void
ags_lv2_browser_plugin_uri_callback(GtkComboBoxText *combo_box,
				    AgsLv2Browser *lv2_browser)
{
  GtkTable *table;
  GtkComboBoxText *filename, *uri;
  GtkLabel *label;

  AgsLv2Plugin *lv2_plugin;

  GList *project_node, *name_node, *license_node;
  GList *port_type_node, *port_name_node;
  GList *list, *list_start, *child, *child_start;

  gchar *uri_str;
  gchar *str, *tmp;
  gchar *port_type_0, *port_type_1;
  guint port_count;
  guint y;
  guint i;

  /* retrieve filename and uri */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  uri = GTK_COMBO_BOX(list->next->next->next->data);

  g_list_free(list_start);

  /* update description */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->description));

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(gtk_combo_box_text_get_active_text(filename));
  
  uri_str = gtk_combo_box_text_get_active_text(uri);

  if(uri_str != NULL){
    str = "//rdf-triple/rdf-verb[@do=\"lv2:project\"]/rdf-list/rdf-value[1]\0";
    project_node = ags_turtle_find_xpath(lv2_plugin->turtle,
					 str);

    str = "//rdf-triple/rdf-verb[@do=\"doap:name\"]/rdf-list/rdf-value[1]\0";
    name_node = ags_turtle_find_xpath(lv2_plugin->turtle,
				      str);

    str = "//rdf-triple/rdf-verb[@do=\"doap:license\"]/rdf-list/rdf-value[1]\0";
    license_node = ags_turtle_find_xpath(lv2_plugin->turtle,
					 str);
    
    /* update ui - reading plugin file */
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strconcat("Label: \0",
				   xmlGetProp(name_node->data,
					      "value\0"),
				   NULL));

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strconcat("Project: \0",
				   xmlGetProp(project_node->data,
					      "value\0"),
				   NULL));

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       g_strconcat("License: \0",
				   xmlGetProp(license_node->data,
					      "value\0"),
				   NULL));

    str = "//rdf-triple//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[@has-type=\"true\"]/rdf-list/rdf-value\0";
    port_type_node = ags_turtle_find_xpath(lv2_plugin->turtle,
					   str);
    
    str = "//rdf-triple//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[@do=\"lv2:name\"]/rdf-list/rdf-value[1]\0";
    port_name_node = ags_turtle_find_xpath(lv2_plugin->turtle,
					   str);
    
    port_count = g_list_length(port_name_node);

    list = list->next;
    label = GTK_LABEL(list->data);

    str = g_strdup("Ports: \0");
    gtk_label_set_text(label,
		       str);

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

    for(i = 0, y = 0; i < port_count; i++){
      port_type_0 = xmlGetProp(port_type_node->data,
			       "value\0");
      port_type_1 = xmlGetProp(port_type_node->next->data,
			       "value\0");

      if(g_ascii_strncasecmp(port_type_0,
			     "lv2:ControlPort\0",
			     15) &&
	 g_ascii_strncasecmp(port_type_1,
			     "lv2:ControlPort\0",
			     15)){
	port_type_node = port_type_node->next->next;
	port_name_node = port_name_node->next;
	
	continue;
      }
      
      str = g_strdup(xmlGetProp(port_name_node->data,
				"value\0"));
      
      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign\0", 0.0,
					"label\0", str,
					NULL);
      gtk_table_attach_defaults(table,
				GTK_WIDGET(label),
				0, 1,
				y, y + 1);

      gtk_table_attach_defaults(table,
				GTK_WIDGET(ags_lv2_browser_combo_box_controls_new()),
				1, 2,
				y, y + 1);

      y++;
      port_type_node = port_type_node->next->next;
      port_name_node = port_name_node->next;
    }
      
    gtk_widget_show_all(table);
  }else{
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Label: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Project: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "License: \0");

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
