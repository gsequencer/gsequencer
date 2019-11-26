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

#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

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
  
  pthread_mutex_t *ladspa_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  filename = (GtkComboBoxText *) ladspa_browser->filename;
  effect = (GtkComboBoxText *) ladspa_browser->effect;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model((GtkComboBox *) effect)));

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get ladspa manager mutex */
  pthread_mutex_lock(ags_ladspa_manager_get_class_mutex());
  
  ladspa_manager_mutex = ladspa_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_ladspa_manager_get_class_mutex());

  /* get ladspa plugin */
  pthread_mutex_lock(ladspa_manager_mutex);

  list =
    start_list = g_list_copy(ladspa_manager->ladspa_plugin);

  pthread_mutex_unlock(ladspa_manager_mutex);

  str = gtk_combo_box_text_get_active_text(filename);
  
  while((list = ags_base_plugin_find_filename(list, str)) != NULL){
    gchar *str;

    ladspa_plugin = list->data;

    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(ladspa_plugin)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* set effect */
    pthread_mutex_lock(base_plugin_mutex);

    str = g_strdup(AGS_BASE_PLUGIN(ladspa_plugin)->effect);

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
ags_ladspa_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					  AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename, *effect;
  GtkTable *table;

  AgsLadspaPlugin *ladspa_plugin;

  GList *child, *child_start;

  gchar *str, *tmp;

  guint effect_index;
  guint port_count;
  guint y;
  unsigned long i;

  void *plugin_so;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;

  pthread_mutex_t *base_plugin_mutex;

  /* retrieve filename and effect */
  filename = (GtkComboBoxText *) ladspa_browser->filename;
  effect = (GtkComboBoxText *) ladspa_browser->effect;

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							gtk_combo_box_text_get_active_text(filename),
							gtk_combo_box_text_get_active_text(effect));

  g_object_get(ladspa_plugin,
	       "plugin-so", &plugin_so,
	       "effect-index", &effect_index,
	       NULL);
  
  /* update description */
  if(plugin_so){
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(ladspa_plugin)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* plugin and port descriptor */
    pthread_mutex_lock(base_plugin_mutex);

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

    table = ladspa_browser->port_table;
    
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(table));
    
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
      gtk_table_attach_defaults(table,
				GTK_WIDGET(label),
				0, 1,
				y, y + 1);

      gtk_table_attach_defaults(table,
				GTK_WIDGET(ags_ladspa_browser_combo_box_controls_new()),
				1, 2,
				y, y + 1);

      y++;
    }

    pthread_mutex_unlock(base_plugin_mutex);

    gtk_widget_show_all((GtkWidget *) table);
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
    table = ladspa_browser->port_table;
    
    child_start = 
      child = gtk_container_get_children(GTK_CONTAINER(table));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(child_start);
  }
}
