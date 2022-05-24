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

#include <ags/app/ags_vst3_browser_callbacks.h>

#include <ags/libags-vst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void
ags_vst3_browser_plugin_filename_callback(GtkTreeView *tree_view,
					  GtkTreePath *path,
					  GtkTreeViewColumn *column,
					  AgsVst3Browser *vst3_browser)
{
  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;

  GList *start_list, *list;

  GtkTreeIter tree_iter;

  gchar *filename;
  
  GRecMutex *vst3_manager_mutex;

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(vst3_browser->filename_tree_view));
  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(vst3_browser->effect_tree_view));

  gtk_list_store_clear(effect_list_store);
  
  if(!gtk_tree_model_get_iter(GTK_TREE_MODEL(filename_list_store), &tree_iter, path)){
    return;
  }

  filename = NULL;
  
  gtk_tree_model_get(GTK_TREE_MODEL(filename_list_store),
		     &tree_iter,
		     0, &filename,
		     -1);

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* get vst3 plugin */
  g_rec_mutex_lock(vst3_manager_mutex);

  list =
    start_list = g_list_copy_deep(vst3_manager->vst3_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(vst3_manager_mutex);

  while((list = ags_base_plugin_find_filename(list, filename)) != NULL){
    gchar *effect;

    vst3_plugin = list->data;

    /* set effect */
    g_object_get(vst3_plugin,
		 "effect", &effect,
		 NULL);
    
    if(effect != NULL){
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
ags_vst3_browser_plugin_effect_callback(GtkTreeView *tree_view,
					GtkTreePath *path,
					GtkTreeViewColumn *column,
					AgsVst3Browser *vst3_browser)
{
  GtkComboBoxText *filename_combo, *effect_combo;
  GtkLabel *label;

  AgsVst3Plugin *vst3_plugin;

  GList *start_port_editor, *port_editor;

  gchar *filename, *effect;
  gchar *str;
  guint y;

  GRecMutex *base_plugin_mutex;
  GRecMutex *plugin_port_mutex;

  /* retrieve filename and effect */
  filename = ags_vst3_browser_get_plugin_filename(vst3_browser);
  effect = ags_vst3_browser_get_plugin_effect(vst3_browser);

  if(filename == NULL ||
     effect == NULL){
    ags_vst3_browser_clear(vst3_browser);
    
    return;
  }

  /* update description */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  filename,
						  effect);

  port_editor =
    start_port_editor = ags_vst3_browser_get_port_editor(vst3_browser);
    
  while(port_editor != NULL){
    ags_vst3_browser_remove_port_editor(vst3_browser,
					port_editor->data);

    port_editor = port_editor->next;
  }

  g_list_free(start_port_editor);

  if(vst3_plugin != NULL){
    GList *start_plugin_port, *plugin_port;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

    /* update ui - empty */
    //TODO:JK: implement me
    
    /* update ui - port information */
    start_plugin_port = g_list_copy(AGS_BASE_PLUGIN(vst3_plugin)->plugin_port);

    g_rec_mutex_unlock(base_plugin_mutex);

    plugin_port = start_plugin_port;
    y = 0;
    
    while(plugin_port != NULL){
      AgsPortEditor *port_editor;

      guint flags;

      if(!ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	plugin_port = plugin_port->next;
	
	continue;
      }
      
      /* get base plugin mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get some fields */
      g_rec_mutex_lock(plugin_port_mutex);
      
      str = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

      g_rec_mutex_unlock(plugin_port_mutex);

      port_editor = ags_port_editor_new();

      gtk_label_set_text(port_editor->port_name,
			 str);


      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	flags |= AGS_PORT_EDITOR_IS_OUTPUT;
      }else{
	flags |= AGS_PORT_EDITOR_IS_INPUT;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	flags |= AGS_PORT_EDITOR_IS_BOOLEAN;
      }else{
	flags |= AGS_PORT_EDITOR_IS_ADJUSTMENT;
      }
      
      ags_port_editor_set_flags(port_editor,
				flags);
      
      ags_vst3_browser_add_port_editor(vst3_browser,
					 port_editor,
					 0, y,
					 1, 1);
      
      gtk_widget_show((GtkWidget *) port_editor);
            
      y++;
      
      plugin_port = plugin_port->next;
    }

    g_list_free(start_plugin_port);
  }else{
    ags_vst3_browser_clear(vst3_browser);
  }
}
