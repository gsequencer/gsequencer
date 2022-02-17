/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/GSequencer/ags_dssi_browser_callbacks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <dssi.h>

#include <ags/i18n.h>

void
ags_dssi_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					  AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *filename, *effect;

  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  GList *start_list, *list;

  gchar *str;
  
  GRecMutex *dssi_manager_mutex;

  filename = GTK_COMBO_BOX_TEXT(dssi_browser->filename);
  effect = GTK_COMBO_BOX_TEXT(dssi_browser->effect);

  gtk_list_store_clear(GTK_LIST_STORE(effect));

  dssi_manager = ags_dssi_manager_get_instance();

  /* get dssi manager mutex */
  dssi_manager_mutex = AGS_DSSI_MANAGER_GET_OBJ_MUTEX(dssi_manager);

  /* get dssi plugin */
  g_rec_mutex_lock(dssi_manager_mutex);

  list =
    start_list = g_list_copy_deep(dssi_manager->dssi_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(dssi_manager_mutex);

  str = gtk_combo_box_text_get_active_text(filename);
  
  while((list = ags_base_plugin_find_filename(list, str)) != NULL){
    gchar *str;

    dssi_plugin = list->data;

    /* set effect */
    g_object_get(dssi_plugin,
		 "effect", &str,
		 NULL);
    
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

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_dssi_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *filename, *effect;
  GtkGrid *grid;

  AgsDssiPlugin *dssi_plugin;

  GList *child, *start_child;

  gchar *str;

  guint effect_index;
  guint port_count;
  guint y;
  unsigned long i;

  void *plugin_so;
  DSSI_Descriptor *plugin_descriptor;
  const LADSPA_PortDescriptor *port_descriptor;

  GRecMutex *base_plugin_mutex;

  /* retrieve filename and effect */
  filename = GTK_COMBO_BOX_TEXT(dssi_browser->filename);
  effect = GTK_COMBO_BOX_TEXT(dssi_browser->effect);

  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  gtk_combo_box_text_get_active_text(filename),
						  gtk_combo_box_text_get_active_text(effect));

  g_object_get(dssi_plugin,
	       "plugin-so", &plugin_so,
	       "effect-index", &effect_index,
	       NULL);
  
  /* update description */
  if(plugin_so){
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

    /* plugin and port descriptor */
    g_rec_mutex_lock(base_plugin_mutex);

    plugin_descriptor = AGS_DSSI_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor);

    port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;   

    /* update ui - reading plugin file */
    str = g_strconcat(i18n("Label"),
		      ": ",
		      plugin_descriptor->LADSPA_Plugin->Label,
		      NULL);
    gtk_label_set_text(dssi_browser->label,
		       str);

    g_free(str);

    str = g_strconcat(i18n("Maker"),
		      ": ",
		      plugin_descriptor->LADSPA_Plugin->Label,
		      NULL);
    gtk_label_set_text(dssi_browser->maker,
		       str);

    g_free(str);

    str = g_strconcat(i18n("Copyright"),
		      ": ",
		      plugin_descriptor->LADSPA_Plugin->Label,
		      NULL);
    gtk_label_set_text(dssi_browser->copyright,
		       str);

    g_free(str);

    /* update ui - port information */
    port_count = plugin_descriptor->LADSPA_Plugin->PortCount;

    grid = dssi_browser->port_grid;
    
    child =
      start_child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(start_child);

    for(i = 0, y = 0; i < port_count; i++){
      GtkLabel *label;
      
      if(!(LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	   (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	    LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	continue;
      }

      str = g_strdup(plugin_descriptor->LADSPA_Plugin->PortNames[i]);

      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign", 0.0,
					"label", str,
					NULL);
      gtk_grid_attach(grid,
		      (GtkWidget *) label,
		      0, y,
		      1, 1);

      gtk_grid_attach(grid,
		      (GtkWidget *) ags_dssi_browser_combo_box_controls_new(),
		      1, y,
		      1, 1);

      y++;
    }

    g_rec_mutex_unlock(base_plugin_mutex);

    gtk_widget_show_all((GtkWidget *) grid);
  }else{
    /* update ui - empty */
    str = g_strdup_printf("%s: ",
			  i18n("Label"));
    gtk_label_set_text(dssi_browser->label,
		       str);

    g_free(str);
    
    str = g_strdup_printf("%s: ",
			  i18n("Maker"));
    gtk_label_set_text(dssi_browser->maker,
		       str);

    g_free(str);

    str = g_strdup_printf("%s: ",
			  i18n("Copyright"));
    gtk_label_set_text(dssi_browser->copyright,
		       str);

    g_free(str);

    /* update ui - no ports */
    grid = dssi_browser->port_grid;
    
    child =
      start_child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(start_child);
  }
}
