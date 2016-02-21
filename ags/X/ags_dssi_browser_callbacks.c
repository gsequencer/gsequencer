/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_dssi_browser_callbacks.h>

#include <ags/plugin/ags_dssi_manager.h>

#include <ags/object/ags_applicable.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <dssi.h>

void
ags_dssi_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					  AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *filename, *effect;

  AgsDssiPlugin *dssi_plugin;
  GList *list;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;

  list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  gtk_list_store_clear(GTK_LIST_STORE(effect));

  ags_dssi_manager_load_file(gtk_combo_box_text_get_active_text(filename));
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(gtk_combo_box_text_get_active_text(filename));
  
  plugin_so = dssi_plugin->plugin_so;

  if(plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      unsigned long plugin_index;

      /* We've successfully found a dssi_descriptor function. Now load name and uuid member. */

      for(plugin_index = 0; (plugin_descriptor = dssi_descriptor(plugin_index)) != NULL; plugin_index++){
	gtk_combo_box_text_append_text(effect,
				       g_strdup_printf("%s:%lu\0", plugin_descriptor->LADSPA_Plugin->Name, plugin_descriptor->LADSPA_Plugin->UniqueID));
      }
    }
  }

  gtk_combo_box_set_active(effect,
  			   0);
}

void
ags_dssi_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					AgsDssiBrowser *dssi_browser)
{
  GtkTable *table;
  GtkComboBoxText *filename, *effect;
  GtkLabel *label;
  AgsDssiPlugin *dssi_plugin;
  GList *list, *list_start, *child, *child_start;
  gchar *str, *tmp;
  guint port_count;
  guint y;
  unsigned long i;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  unsigned long plugin_index;

  /* retrieve filename and effect */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  g_list_free(list_start);

  /* update description */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->description));

  ags_dssi_manager_load_file(gtk_combo_box_text_get_active_text(filename));
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(gtk_combo_box_text_get_active_text(filename));
  
  plugin_so = dssi_plugin->plugin_so;

  plugin_index = (unsigned long) gtk_combo_box_get_active(effect);

  if(plugin_index != -1 &&
     plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      plugin_descriptor = dssi_descriptor(plugin_index);

      port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;   

      /* update ui - reading plugin file */
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Label: \0",
				     plugin_descriptor->LADSPA_Plugin->Label,
				     NULL));

      list = list->next;
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Maker: \0",
				     plugin_descriptor->LADSPA_Plugin->Maker,
				     NULL));

      list = list->next;
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Copyright: \0",
				     plugin_descriptor->LADSPA_Plugin->Copyright,
				     NULL));

      port_count = plugin_descriptor->LADSPA_Plugin->PortCount;

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
	if(!(LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	     (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	      LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	  continue;
	}

	str = g_strdup(plugin_descriptor->LADSPA_Plugin->PortNames[i]);

	label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					  "xalign\0", 0.0,
					  "label\0", str,
					  NULL);
	gtk_table_attach_defaults(table,
				  GTK_WIDGET(label),
				  0, 1,
				  y, y + 1);

	gtk_table_attach_defaults(table,
				  GTK_WIDGET(ags_dssi_browser_combo_box_controls_new()),
				  1, 2,
				  y, y + 1);

	y++;
      }

      gtk_widget_show_all(table);
    }
  }else{
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Label: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Maker: \0");

    list = list->next;
    label = GTK_LABEL(list->data);
    gtk_label_set_text(label,
		       "Copyright: \0");

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
