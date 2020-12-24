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

#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void
ags_ladspa_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					    AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename, *effect;

  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;

  GList *start_list, *list;

  gchar *str;
  
  GRecMutex *ladspa_manager_mutex;

  filename = (GtkComboBoxText *) ladspa_browser->filename;
  effect = (GtkComboBoxText *) ladspa_browser->effect;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model((GtkComboBox *) effect)));

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* get ladspa plugin */
  g_rec_mutex_lock(ladspa_manager_mutex);

  list =
    start_list = g_list_copy_deep(ladspa_manager->ladspa_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(ladspa_manager_mutex);

  str = gtk_combo_box_text_get_active_text(filename);
  
  while((list = ags_base_plugin_find_filename(list, str)) != NULL){
    gchar *str;

    ladspa_plugin = list->data;

    /* set effect */
    g_object_get(ladspa_plugin,
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
ags_ladspa_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					  AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename, *effect;
  GtkGrid *grid;

  AgsLadspaPlugin *ladspa_plugin;

  GList *child, *child_start;

  gchar *str;

  guint effect_index;
  guint port_count;
  guint y;
  unsigned long i;

  void *plugin_so;
  LADSPA_Descriptor *plugin_descriptor;
  const LADSPA_PortDescriptor *port_descriptor;

  GRecMutex *base_plugin_mutex;

  /* retrieve filename and effect */
  filename = (GtkComboBoxText *) ladspa_browser->filename;
  effect = (GtkComboBoxText *) ladspa_browser->effect;

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							gtk_combo_box_text_get_active_text(filename),
							gtk_combo_box_text_get_active_text(effect));

  plugin_so = NULL;
  effect_index = 0;
  
  g_object_get(ladspa_plugin,
	       "plugin-so", &plugin_so,
	       "effect-index", &effect_index,
	       NULL);
  
  /* update description */
  if(plugin_so){
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);

    /* plugin and port descriptor */
    g_rec_mutex_lock(base_plugin_mutex);

    plugin_descriptor = AGS_LADSPA_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(ladspa_plugin)->plugin_descriptor);

    port_descriptor = plugin_descriptor->PortDescriptors;   

    /* update ui - reading plugin file */
    str = g_strconcat(i18n("Label"),
		      ": ",
		      plugin_descriptor->Label,
		      NULL);
    gtk_label_set_text(ladspa_browser->label,
		       str);

    g_free(str);

    str = g_strconcat(i18n("Maker"),
		      ": ",
		      plugin_descriptor->Label,
		      NULL);
    gtk_label_set_text(ladspa_browser->maker,
		       str);

    g_free(str);

    str = g_strconcat(i18n("Copyright"),
		      ": ",
		      plugin_descriptor->Label,
		      NULL);
    gtk_label_set_text(ladspa_browser->copyright,
		       str);

    g_free(str);

    /* update ui - port information */
    port_count = plugin_descriptor->PortCount;

    grid = ladspa_browser->port_grid;
    
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(child_start);

    for(i = 0, y = 0; i < port_count; i++){
      GtkLabel *label;
      
      if(!(LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	   (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	    LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	continue;
      }

      str = g_strdup(plugin_descriptor->PortNames[i]);

      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign", 0.0,
					"label", str,
					NULL);
      gtk_grid_attach(grid,
		      (GtkWidget *) label,
		      0, y,
		      1, 1);

      gtk_grid_attach(grid,
		      (GtkWidget *) ags_ladspa_browser_combo_box_controls_new(),
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
    gtk_label_set_text(ladspa_browser->label,
		       str);

    g_free(str);
    
    str = g_strdup_printf("%s: ",
			  i18n("Maker"));
    gtk_label_set_text(ladspa_browser->maker,
		       str);

    g_free(str);

    str = g_strdup_printf("%s: ",
			  i18n("Copyright"));
    gtk_label_set_text(ladspa_browser->copyright,
		       str);

    g_free(str);

    /* update ui - no ports */
    grid = ladspa_browser->port_grid;
    
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(child_start);
  }
}
