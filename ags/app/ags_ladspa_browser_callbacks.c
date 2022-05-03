/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_ladspa_browser_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void
ags_ladspa_browser_plugin_filename_callback(GtkTreeView *tree_view,
					    GtkTreePath *path,
					    GtkTreeViewColumn *column,
					    AgsLadspaBrowser *ladspa_browser)
{
  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;

  GtkTreeIter iter;

  GList *start_list, *list;

  gchar *filename;
  
  GRecMutex *ladspa_manager_mutex;

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(ladspa_browser->filename_tree_view));
  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(ladspa_browser->effect_tree_view));

  gtk_list_store_clear(effect_list_store);
  
  if(!gtk_tree_model_get_iter(GTK_TREE_MODEL(filename_list_store), &iter, path)){
    return;
  }

  filename = NULL;
  
  gtk_tree_model_get(GTK_TREE_MODEL(filename_list_store),
		     &iter,
		     0, &filename,
		     -1);
  
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
  
  while((list = ags_base_plugin_find_filename(list, filename)) != NULL){
    gchar *effect;

    ladspa_plugin = list->data;

    /* set effect */
    g_object_get(ladspa_plugin,
		 "effect", &effect,
		 NULL);
    
    if(effect != NULL){
      GtkTreeIter tree_iter;

      gtk_list_store_append(effect_list_store,
			    &tree_iter);

      gtk_list_store_set(effect_list_store, &tree_iter,
			 0, effect,
			 -1);
    }

    g_free(effect);

    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_ladspa_browser_plugin_effect_callback(GtkTreeView *tree_view,
					  GtkTreePath *path,
					  GtkTreeViewColumn *column,
					  AgsLadspaBrowser *ladspa_browser)
{
  AgsLadspaPlugin *ladspa_plugin;

  GList *start_port_editor, *port_editor;

  gchar *filename, *effect;
  gchar *str;

  guint port_count;
  guint y;
  unsigned long i;

  void *plugin_so;
  LADSPA_Descriptor *plugin_descriptor;
  const LADSPA_PortDescriptor *port_descriptor;

  GRecMutex *base_plugin_mutex;

  /* retrieve filename and effect */
  filename = ags_ladspa_browser_get_plugin_filename(ladspa_browser);
  effect = ags_ladspa_browser_get_plugin_effect(ladspa_browser);

  if(filename == NULL ||
     effect == NULL){
    ags_ladspa_browser_clear(ladspa_browser);
    
    return;
  }

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename,
							effect);

  plugin_so = NULL;
  
  g_object_get(ladspa_plugin,
	       "plugin-so", &plugin_so,
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

    port_editor =
      start_port_editor = ags_ladspa_browser_get_port_editor(ladspa_browser);
    
    while(port_editor != NULL){
      ags_ladspa_browser_remove_port_editor(ladspa_browser,
					    port_editor->data);
      
      g_object_run_dispose(port_editor->data);
      g_object_unref(port_editor->data);

      port_editor = port_editor->next;
    }

    g_list_free(start_port_editor);

    for(i = 0, y = 0; i < port_count; i++){
      AgsPortEditor *port_editor;

      const LADSPA_PortRangeHint *range_hint;
      LADSPA_PortRangeHintDescriptor hint_descriptor;

      guint flags;
      
      if(!(LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	   (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	    LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	continue;
      }

      port_editor = ags_port_editor_new();

      gtk_label_set_text(port_editor->port_name,
			 plugin_descriptor->PortNames[i]);

      flags = 0;

      if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	flags |= AGS_PORT_EDITOR_IS_OUTPUT;
      }else{
	flags |= AGS_PORT_EDITOR_IS_INPUT;
      }

      range_hint = &(AGS_LADSPA_PLUGIN_DESCRIPTOR(plugin_descriptor)->PortRangeHints[i]);
      hint_descriptor = range_hint->HintDescriptor;

      if(LADSPA_IS_HINT_TOGGLED(hint_descriptor)){
	flags |= AGS_PORT_EDITOR_IS_BOOLEAN;
      }else{
	flags |= AGS_PORT_EDITOR_IS_ADJUSTMENT;
      }
      
      ags_port_editor_set_flags(port_editor,
				flags);

      ags_ladspa_browser_add_port_editor(ladspa_browser,
					 port_editor,
					 0, y,
					 1, 1);
      
      gtk_widget_show((GtkWidget *) port_editor);
      
      y++;
    }

    g_rec_mutex_unlock(base_plugin_mutex);
  }else{
    ags_ladspa_browser_clear(ladspa_browser);
  }
}
