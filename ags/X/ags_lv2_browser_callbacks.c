/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_lv2_browser_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>

#include <ags/i18n.h>

void
ags_lv2_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					 AgsLv2Browser *lv2_browser)
{
  GtkComboBoxText *filename, *effect;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *start_list, *list;

  pthread_mutex_t *lv2_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model((GtkComboBox *) effect)));

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  pthread_mutex_lock(ags_lv2_manager_get_class_mutex());
  
  lv2_manager_mutex = lv2_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_manager_get_class_mutex());

  /* get lv2 plugin */
  pthread_mutex_lock(lv2_manager_mutex);

  list =
    start_list = g_list_copy(lv2_manager->lv2_plugin);

  pthread_mutex_unlock(lv2_manager_mutex);

  while((list = ags_base_plugin_find_filename(list, gtk_combo_box_text_get_active_text(filename))) != NULL){
    gchar *str;

    lv2_plugin = list->data;

    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* set effect */
    pthread_mutex_lock(base_plugin_mutex);

    str = g_strdup(AGS_BASE_PLUGIN(lv2_plugin)->effect);

    pthread_mutex_unlock(base_plugin_mutex);
    
    if(str != NULL){
      gtk_combo_box_text_append_text(effect,
				     str);
    }

    g_free(str);

    /* iterate */
    list = list->next;
  }
  
  gtk_combo_box_set_active((GtkComboBox *) effect,
  			   0);


  g_list_free(start_list);
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

  pthread_mutex_t *base_plugin_mutex;
  pthread_mutex_t *plugin_port_mutex;

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
    GList *start_plugin_port, *plugin_port;
    
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* update ui - empty */
    pthread_mutex_lock(base_plugin_mutex);

    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: %s",
			  i18n("Name"),
			  lv2_plugin->foaf_name);
    gtk_label_set_text(label,
		       str);

    g_free(str);
    
    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: %s",
			  i18n("Homepage"),
			  lv2_plugin->foaf_homepage);
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: %s",
			  i18n("M-Box"),
			  lv2_plugin->foaf_mbox);
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);

    str = g_strdup_printf("%s: ",
			  i18n("Ports"));
    gtk_label_set_text(label,
		       str);

    g_free(str);
    
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

    start_plugin_port = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

    pthread_mutex_unlock(base_plugin_mutex);

    plugin_port = start_plugin_port;
    y = 0;
    
    while(plugin_port != NULL){
      if(!ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	plugin_port = plugin_port->next;
	
	continue;
      }
      
      /* get base plugin mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
  
      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
    
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get some fields */
      pthread_mutex_lock(plugin_port_mutex);
      
      str = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

      pthread_mutex_unlock(plugin_port_mutex);

      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign", 0.0,
					"label", str,
					NULL);
      gtk_table_attach_defaults(table,
				GTK_WIDGET(label),
				0, 1,
				y, y + 1);
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  gtk_table_attach_defaults(table,
				    GTK_WIDGET(ags_lv2_browser_combo_box_output_boolean_controls_new()),
				    1, 2,
				    y, y + 1);
	}else{
	  gtk_table_attach_defaults(table,
				    GTK_WIDGET(ags_lv2_browser_combo_box_boolean_controls_new()),
				    1, 2,
				    y, y + 1);
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  gtk_table_attach_defaults(table,
				    GTK_WIDGET(ags_lv2_browser_combo_box_output_controls_new()),
				    1, 2,
				    y, y + 1);
	}else{
	  gtk_table_attach_defaults(table,
				    GTK_WIDGET(ags_lv2_browser_combo_box_controls_new()),
				    1, 2,
				    y, y + 1);
	}
      }
      
      y++;
      
      plugin_port = plugin_port->next;
    }

    g_list_free(start_plugin_port);
    
    gtk_widget_show_all((GtkWidget *) table);
  }else{
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: ",
			  i18n("Name"));
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: ",
			  i18n("Homepage"));
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: ",
			  i18n("M-Box"));
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s: ",
			  i18n("Ports"));
    gtk_label_set_text(label,
		       str);

    g_free(str);

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
