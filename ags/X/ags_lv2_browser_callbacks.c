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

#include <ags/X/ags_lv2_browser_callbacks.h>

#include <ags/X/ags_ui_provider.h>

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
  GtkComboBoxText *filename_combo, *effect_combo;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *start_list, *list;

  gchar *filename;
  
  GRecMutex *lv2_manager_mutex;

  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename_combo = GTK_COMBO_BOX_TEXT(list->next->data);
  effect_combo = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model((GtkComboBox *) effect_combo)));

  g_list_free(start_list);

  if(gtk_combo_box_get_active(filename_combo) == -1){
    return;
  }

  lv2_manager = ags_lv2_manager_get_instance();

  filename = gtk_combo_box_text_get_active_text(filename_combo);

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

  while((list = ags_base_plugin_find_filename(list, gtk_combo_box_text_get_active_text(filename_combo))) != NULL){
    gchar *effect;

    lv2_plugin = list->data;

    /* set effect */
    g_object_get(lv2_plugin,
		 "effect", &effect,
		 NULL);
    
    if(effect != NULL){
      gtk_combo_box_text_append_text(effect_combo,
				     effect);
    }

    g_free(effect);

    /* iterate */
    list = list->next;
  }
  
  gtk_combo_box_set_active((GtkComboBox *) effect_combo,
  			   -1);

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_lv2_browser_plugin_uri_callback(GtkComboBoxText *combo_box,
				    AgsLv2Browser *lv2_browser)
{
  GtkGrid *grid;
  GtkComboBoxText *filename_combo, *effect_combo;
  GtkLabel *label;

  AgsLv2Plugin *lv2_plugin;

  GList *start_list, *list;
  GList *start_child, *child;

  gchar *filename, *effect;
  gchar *str;
  guint y;

  GRecMutex *base_plugin_mutex;
  GRecMutex *plugin_port_mutex;

  /* retrieve filename and uri */
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename_combo = GTK_COMBO_BOX_TEXT(list->next->data);
  effect_combo = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  g_list_free(start_list);

  if(gtk_combo_box_get_active(effect_combo) == -1){
    list =
      start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->description));
    
    /* update ui - empty */
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s:",
			  i18n("Name"));
    gtk_label_set_text(label,
		       str);

    g_free(str);
    
    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s:",
			  i18n("Homepage"));
    gtk_label_set_text(label,
		       str);

    g_free(str);

    list = list->next;
    label = GTK_LABEL(list->data);
    str = g_strdup_printf("%s:",
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
    
    grid = lv2_browser->port_grid;

    /* update ui - port information */
    child =
      start_child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));
      
      child = child->next;
    }
    
    g_list_free(start_child);

    g_list_free(start_list);
    
    return;
  }

  /* update description */
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->description));

  filename = gtk_combo_box_text_get_active_text(filename_combo);
  effect = gtk_combo_box_text_get_active_text(effect_combo);
  
  if(filename != NULL &&
     effect != NULL){
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

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename,
					       effect);

  if(lv2_plugin != NULL){
    GList *start_plugin_port, *plugin_port;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

    /* update ui - empty */
    g_rec_mutex_lock(base_plugin_mutex);

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
    
    grid = lv2_browser->port_grid;

    /* update ui - port information */
    child =
      start_child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));
      
      child = child->next;
    }
    
    g_list_free(start_child);

    start_plugin_port = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

    g_rec_mutex_unlock(base_plugin_mutex);

    plugin_port = start_plugin_port;
    y = 0;
    
    while(plugin_port != NULL){
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

      label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
					"xalign", 0.0,
					"label", str,
					NULL);

      gtk_widget_set_margin_end((GtkWidget *) label,
				AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

      gtk_grid_attach(grid,
		      (GtkWidget *) label,
		      0, y,
		      1, 1);
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  gtk_grid_attach(grid,
			  GTK_WIDGET(ags_lv2_browser_combo_box_output_boolean_controls_new()),
			  1, y,
			  1, 1);
	}else{
	  gtk_grid_attach(grid,
			  GTK_WIDGET(ags_lv2_browser_combo_box_boolean_controls_new()),
			  1, y,
			  1, 1);
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  gtk_grid_attach(grid,
			  GTK_WIDGET(ags_lv2_browser_combo_box_output_controls_new()),
			  1, y,
			  1, 1);
	}else{
	  gtk_grid_attach(grid,
			  GTK_WIDGET(ags_lv2_browser_combo_box_controls_new()),
			  1, y,
			  1, 1);
	}
      }
      
      y++;
      
      plugin_port = plugin_port->next;
    }

    g_list_free(start_plugin_port);
    
    gtk_widget_show_all((GtkWidget *) grid);
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

    grid = lv2_browser->port_grid;
    
    /* update ui - no ports */
    child =
      start_child = gtk_container_get_children(GTK_CONTAINER(grid));
    
    while(child != NULL){
      gtk_widget_destroy(GTK_WIDGET(child->data));

      child = child->next;
    }

    g_list_free(start_child);
  }

  g_list_free(start_list);
}
