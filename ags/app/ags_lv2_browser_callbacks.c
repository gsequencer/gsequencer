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

#include <ags/app/ags_lv2_browser_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>

#include <ags/i18n.h>

void
ags_lv2_browser_plugin_filename_callback(GtkTreeView *tree_view,
					 GtkTreePath *path,
					 GtkTreeViewColumn *column,
					 AgsLv2Browser *lv2_browser)
{
  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GtkTreeIter iter;

  GList *start_list, *list;
  
  gchar *filename;
  
  GRecMutex *lv2_manager_mutex;

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(lv2_browser->filename_tree_view));
  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(lv2_browser->effect_tree_view));

  gtk_list_store_clear(effect_list_store);
  
  if(!gtk_tree_model_get_iter(GTK_TREE_MODEL(filename_list_store), &iter, path)){
    return;
  }

  filename = NULL;
  
  gtk_tree_model_get(GTK_TREE_MODEL(filename_list_store),
		     &iter,
		     0, &filename,
		     -1);

  lv2_manager = ags_lv2_manager_get_instance();

  if(filename != NULL){
    AgsTurtle *manifest;
    AgsTurtleManager *turtle_manager;
    
    gchar *path;
    gchar *manifest_filename;

    turtle_manager = ags_turtle_manager_get_instance();
    
    path = g_path_get_dirname(filename);

    manifest_filename = g_strdup_printf("%s%c%s",
					path,
					G_DIR_SEPARATOR,
					"manifest.ttl");

    manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						     manifest_filename);

    if(manifest == NULL){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle **turtle;

      guint n_turtle;

      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	return;
      }

      g_message("new turtle [Manifest] - %s", manifest_filename);
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
      ags_turtle_manager_add(turtle_manager,
			     (GObject *) manifest);

      lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

      n_turtle = 1;
      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

      turtle[0] = manifest;
      turtle[1] = NULL;
	
      ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				  turtle, n_turtle);
    
      g_object_run_dispose((GObject *) lv2_turtle_parser);
      g_object_unref(lv2_turtle_parser);
	
      g_object_unref(manifest);
	
      free(turtle);
    }
    
    g_free(manifest_filename);
  }

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* get lv2 plugin */
  g_rec_mutex_lock(lv2_manager_mutex);

  list =
    start_list = g_list_copy_deep(lv2_manager->lv2_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(lv2_manager_mutex);

  while((list = ags_base_plugin_find_filename(list, filename)) != NULL){
    gchar *effect;

    lv2_plugin = list->data;

    /* set effect */
    g_object_get(lv2_plugin,
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
ags_lv2_browser_plugin_uri_callback(GtkTreeView *tree_view,
				    GtkTreePath *path,
				    GtkTreeViewColumn *column,
				    AgsLv2Browser *lv2_browser)
{
  AgsLv2Plugin *lv2_plugin;

  AgsTurtle *manifest;
  AgsTurtleManager *turtle_manager;
    
  GList *start_port_editor, *port_editor;

  gchar *filename, *effect;
  gchar *lv2_path;
  gchar *manifest_filename;
  gchar *str;

  guint y;

  GRecMutex *base_plugin_mutex;
  GRecMutex *plugin_port_mutex;

  /* retrieve filename and effect */
  filename = ags_lv2_browser_get_plugin_filename(lv2_browser);
  effect = ags_lv2_browser_get_plugin_effect(lv2_browser);

  if(filename == NULL ||
     effect == NULL){
    ags_lv2_browser_clear(lv2_browser);
    
    return;
  }

  /* update description */
  turtle_manager = ags_turtle_manager_get_instance();
    
  lv2_path = g_path_get_dirname(filename);

  manifest_filename = g_strdup_printf("%s%c%s",
				      lv2_path,
				      G_DIR_SEPARATOR,
				      "manifest.ttl");

  manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						   manifest_filename);

  if(manifest == NULL){
    AgsLv2TurtleParser *lv2_turtle_parser;
	
    AgsTurtle **turtle;

    guint n_turtle;

    if(!g_file_test(manifest_filename,
		    G_FILE_TEST_EXISTS)){
      return;
    }

    g_message("new turtle [Manifest] - %s", manifest_filename);
	
    manifest = ags_turtle_new(manifest_filename);
    ags_turtle_load(manifest,
		    NULL);
    ags_turtle_manager_add(turtle_manager,
			   (GObject *) manifest);

    lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

    n_turtle = 1;
    turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

    turtle[0] = manifest;
    turtle[1] = NULL;
	
    ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				turtle, n_turtle);
    
    g_object_run_dispose((GObject *) lv2_turtle_parser);
    g_object_unref(lv2_turtle_parser);
	
    g_object_unref(manifest);
	
    free(turtle);
  }
    
  g_free(manifest_filename);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename,
					       effect);

  if(lv2_plugin != NULL){
    GList *start_plugin_port, *plugin_port;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

    /* update ui - empty */
    g_rec_mutex_lock(base_plugin_mutex);

    str = g_strdup_printf("%s: %s",
			  i18n("Name"),
			  lv2_plugin->foaf_name);
    gtk_label_set_text(lv2_browser->lv2_name,
		       str);

    g_free(str);
    
    str = g_strdup_printf("%s: %s",
			  i18n("Homepage"),
			  lv2_plugin->foaf_homepage);
    gtk_label_set_text(lv2_browser->lv2_homepage,
		       str);

    g_free(str);

    str = g_strdup_printf("%s: %s",
			  i18n("M-Box"),
			  lv2_plugin->foaf_mbox);
    gtk_label_set_text(lv2_browser->lv2_mbox,
		       str);

    g_free(str);
    
    /* update ui - port information */
    port_editor =
      start_port_editor = ags_lv2_browser_get_port_editor(lv2_browser);
    
    
    while(port_editor != NULL){
      ags_lv2_browser_remove_port_editor(lv2_browser,
					 port_editor->data);
      
      g_object_run_dispose(port_editor->data);
      g_object_unref(port_editor->data);

      port_editor = port_editor->next;
    }

    g_list_free(start_port_editor);

    start_plugin_port = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

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

      port_editor = ags_port_editor_new();
      
      /* get base plugin mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get some fields */
      g_rec_mutex_lock(plugin_port_mutex);
      
      str = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

      g_rec_mutex_unlock(plugin_port_mutex);

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
      
      ags_lv2_browser_add_port_editor(lv2_browser,
				      port_editor,
				      0, y,
				      1, 1);
      
      plugin_port = plugin_port->next;
      
      y++;      
    }

    g_list_free(start_plugin_port);
  }else{
    ags_lv2_browser_clear(lv2_browser);
  }
}
