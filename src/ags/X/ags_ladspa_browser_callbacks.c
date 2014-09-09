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

#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/lib/ags_combo_box_text.h>

#include <ags/object/ags_applicable.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void
ags_ladspa_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					    AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename, *effect;
  AgsLadspaPlugin *ladspa_plugin;
  GList *list;
  gchar *path;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  ags_combo_box_text_remove_all(effect);

  ags_ladspa_manager_load_file(gtk_combo_box_text_get_active_text(filename));
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(gtk_combo_box_text_get_active_text(filename));
  
  plugin_so = ladspa_plugin->plugin_so;

  if(plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      long index;

      /* We've successfully found a ladspa_descriptor function. Pass
	 it to the callback function. */

      for(index = 0; (plugin_descriptor = ladspa_descriptor(index)) != NULL; index++){
	gtk_combo_box_text_append_text(effect,
				       g_strdup_printf("%s:%lu\0", plugin_descriptor->Name, plugin_descriptor->UniqueID));
      }
    }
  }

  gtk_combo_box_set_active(effect,
  			   0);
}

void
ags_ladspa_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					  AgsLadspaBrowser *ladspa_browser)
{
  GtkTable *table;
  GtkComboBoxText *filename, *effect;
  GtkLabel *label;
  AgsLadspaPlugin *ladspa_plugin;
  GList *list, *children;
  gchar *str, *tmp;
  guint port_count;
  guint i;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  long index;

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->description));

  ags_ladspa_manager_load_file(gtk_combo_box_text_get_active_text(filename));
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(gtk_combo_box_text_get_active_text(filename));
  
  plugin_so = ladspa_plugin->plugin_so;

  index = gtk_combo_box_get_active(effect);

  if(plugin_so &&
     index != -1){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      plugin_descriptor = ladspa_descriptor(index);

      port_descriptor = plugin_descriptor->PortDescriptors;   

      /* update ui */
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Label: \0",
				     plugin_descriptor->Label,
				     NULL));

      list = list->next;
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Maker: \0",
				     plugin_descriptor->Maker,
				     NULL));

      list = list->next;
      label = GTK_LABEL(list->data);
      gtk_label_set_text(label,
			 g_strconcat("Copyright: \0",
				     plugin_descriptor->Copyright,
				     NULL));

      port_count = plugin_descriptor->PortCount;

      list = list->next;
      label = GTK_LABEL(list->data);

      str = g_strdup("Ports: \0");
      gtk_label_set_text(label,
			 str);

      list = list->next;
      table = GTK_TABLE(list->data);
    
      children = gtk_container_get_children(GTK_CONTAINER(table));
    
      while(children != NULL){
	gtk_widget_destroy(GTK_WIDGET(children->data));

	children = children->next;
      }

      for(i = 0; i < port_count; i++){
	if(!(LADSPA_IS_PORT_INPUT(port_descriptor[i]) && 
	     LADSPA_IS_PORT_CONTROL(port_descriptor[i]))){
	  continue;
	}

	str = g_strdup(plugin_descriptor->PortNames[i]);

	label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					  "xalign\0", 0.0,
					  "label\0", str,
					  NULL);
	gtk_table_attach_defaults(table,
				  GTK_WIDGET(label),
				  0, 1,
				  i, i + 1);

	gtk_table_attach_defaults(table,
				  GTK_WIDGET(ags_ladspa_browser_combo_box_controls_new()),
				  1, 2,
				  i, i + 1);
      }

      gtk_widget_show_all(table);
    }
  }else{
    /* update ui */
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
    
    children = gtk_container_get_children(GTK_CONTAINER(table));
    
    while(children != NULL){
      gtk_widget_destroy(GTK_WIDGET(children->data));

      children = children->next;
    }
  }
}

void
ags_ladspa_browser_preview_close_callback(GtkWidget *preview,
					  AgsLadspaBrowser *ladspa_browser)
{
  ladspa_browser->preview = NULL;
  gtk_widget_destroy(preview);
}

int
ags_ladspa_browser_ok_callback(GtkWidget *widget, AgsLadspaBrowser *ladspa_browser)
{
  gtk_widget_hide((GtkWidget *) ladspa_browser);

  return(0);
}

int
ags_ladspa_browser_cancel_callback(GtkWidget *widget, AgsLadspaBrowser *ladspa_browser)
{
  gtk_widget_hide((GtkWidget *) ladspa_browser);

  return(0);
}

