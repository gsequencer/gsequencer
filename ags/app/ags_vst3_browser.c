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

#include <ags/app/ags_vst3_browser.h>
#include <ags/app/ags_vst3_browser_callbacks.h>

#include <ags/libags-vst.h>

#include <ags/app/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_vst3_browser_class_init(AgsVst3BrowserClass *vst3_browser);
void ags_vst3_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_vst3_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_vst3_browser_init(AgsVst3Browser *vst3_browser);

void ags_vst3_browser_connect(AgsConnectable *connectable);
void ags_vst3_browser_disconnect(AgsConnectable *connectable);

void ags_vst3_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_vst3_browser_apply(AgsApplicable *applicable);
void ags_vst3_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_vst3_browser
 * @short_description: A composite to select vst3 effect.
 * @title: AgsVst3Browser
 * @section_id:
 * @include: ags/app/ags_vst3_browser.h
 *
 * #AgsVst3Browser is a composite widget to select vst3 plugin and the desired
 * effect.
 */

GType
ags_vst3_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_browser = 0;

    static const GTypeInfo ags_vst3_browser_info = {
      sizeof (AgsVst3BrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vst3_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Browser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_vst3_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_vst3_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_vst3_browser = g_type_register_static(GTK_TYPE_BOX,
						   "AgsVst3Browser", &ags_vst3_browser_info,
						   0);

    g_type_add_interface_static(ags_type_vst3_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_vst3_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_browser_class_init(AgsVst3BrowserClass *vst3_browser)
{
  /* empty */
}

void
ags_vst3_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_vst3_browser_connect;
  connectable->disconnect = ags_vst3_browser_disconnect;
}

void
ags_vst3_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_vst3_browser_set_update;
  applicable->apply = ags_vst3_browser_apply;
  applicable->reset = ags_vst3_browser_reset;
}

void
ags_vst3_browser_init(AgsVst3Browser *vst3_browser)
{
  GtkLabel *label;
  GtkTreeViewColumn *filename_column;
  GtkTreeViewColumn *effect_column;
  GtkScrolledWindow *scrolled_window;

  GtkCellRenderer *filename_renderer;
  GtkCellRenderer *effect_renderer;

  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  AgsVst3Manager *vst3_manager;

  GList *start_list, *list;

  gchar *str;
  gchar **filenames, **filenames_start;

  guint length;
  guint i;

  GRecMutex *vst3_manager_mutex;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(vst3_browser),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(vst3_browser,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  vst3_browser->connectable_flags = 0;
  
  /* plugin */
  vst3_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);
  gtk_box_append((GtkBox *) vst3_browser,
		 (GtkWidget *) vst3_browser->plugin);

  /* filename */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_VST3_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_VST3_BROWSER_FILENAME_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) scrolled_window);

  vst3_browser->filename_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(vst3_browser->filename_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) vst3_browser->filename_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) vst3_browser->filename_tree_view,
			      AGS_VST3_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_VST3_BROWSER_FILENAME_HEIGHT_REQUEST);

  filename_renderer = gtk_cell_renderer_text_new();

  filename_column = gtk_tree_view_column_new_with_attributes(i18n("filename"),
							     filename_renderer,
							     "text", 0,
							     NULL);
  gtk_tree_view_append_column(vst3_browser->filename_tree_view,
			      filename_column);
  
  filename_list_store = gtk_list_store_new(1,
					   G_TYPE_STRING);

  gtk_tree_view_set_model(vst3_browser->filename_tree_view,
			  GTK_TREE_MODEL(filename_list_store));  

  vst3_browser->path = NULL;

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  filenames =
    filenames_start = ags_vst3_manager_get_filenames(ags_vst3_manager_get_instance());

  if(filenames_start != NULL){
    list = NULL;
    
    while(filenames[0] != NULL){
      list = g_list_prepend(list,
			    filenames[0]);
     
      filenames++;
    }

    list =
      start_list = g_list_sort(list,
			       g_strcmp0);

    while(list != NULL){
      GtkTreeIter tree_iter;

      gtk_list_store_append(filename_list_store,
			    &tree_iter);

      gtk_list_store_set(filename_list_store, &tree_iter,
			 0, list->data,
			 -1);
 
      list = list->next;
    }
 
    g_list_free(start_list);

    g_strfreev(filenames_start);
  }
  
  /* effect */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_VST3_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_VST3_BROWSER_EFFECT_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) scrolled_window);

  vst3_browser->effect_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(vst3_browser->effect_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) vst3_browser->effect_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) vst3_browser->effect_tree_view,
			      AGS_VST3_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_VST3_BROWSER_EFFECT_HEIGHT_REQUEST);

  effect_renderer = gtk_cell_renderer_text_new();

  effect_column = gtk_tree_view_column_new_with_attributes(i18n("effect"),
							   effect_renderer,
							   "text", 0,
							   NULL);
  gtk_tree_view_append_column(vst3_browser->effect_tree_view,
			      effect_column);
  
  effect_list_store = gtk_list_store_new(1,
					 G_TYPE_STRING);

  gtk_tree_view_set_model(vst3_browser->effect_tree_view,
			  GTK_TREE_MODEL(effect_list_store));  

  /* description */
  vst3_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     0);
  gtk_box_append((GtkBox *) vst3_browser,
		 (GtkWidget *) vst3_browser->description);

  //TODO:JK: implement me
  
  /* ports */
  str = g_strconcat(i18n("Ports"),
		    ": ",
		    NULL);  

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_append((GtkBox *) vst3_browser->description,
		 (GtkWidget *) label);

  g_free(str);
  
  /* port editor */
  vst3_browser->port_editor = NULL;
  
  vst3_browser->port_editor_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(vst3_browser->port_editor_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(vst3_browser->port_editor_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_box_append((GtkBox *) vst3_browser->description,
		 (GtkWidget *) vst3_browser->port_editor_grid);
}

void
ags_vst3_browser_connect(AgsConnectable *connectable)
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = AGS_VST3_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (vst3_browser->connectable_flags)) != 0){
    return;
  }

  vst3_browser->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(vst3_browser->filename_tree_view), "row-activated",
			 G_CALLBACK(ags_vst3_browser_plugin_filename_callback), vst3_browser);

  g_signal_connect_after(G_OBJECT(vst3_browser->effect_tree_view), "row-activated",
			 G_CALLBACK(ags_vst3_browser_plugin_effect_callback), vst3_browser);
}

void
ags_vst3_browser_disconnect(AgsConnectable *connectable)
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = AGS_VST3_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (vst3_browser->connectable_flags)) == 0){
    return;
  }

  vst3_browser->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(vst3_browser->filename_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_vst3_browser_plugin_filename_callback),
		      vst3_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(vst3_browser->effect_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_vst3_browser_plugin_effect_callback),
		      vst3_browser,
		      NULL);
}

void
ags_vst3_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_vst3_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_vst3_browser_reset(AgsApplicable *applicable)
{
  AgsVst3Browser *vst3_browser;

  GtkTreeModel *model;

  GtkTreeIter tree_iter;

  vst3_browser = AGS_VST3_BROWSER(applicable);

  model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(vst3_browser->filename_tree_view)));

  if(gtk_tree_model_get_iter_first(model, &tree_iter)){
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(vst3_browser->filename_tree_view),
			     gtk_tree_model_get_path(model,
						     &tree_iter),
			     NULL,
			     FALSE);
  }
}

/**
 * ags_vst3_browser_get_plugin_filename:
 * @vst3_browser: the #AgsVst3Browser
 *
 * Retrieve selected vst3 plugin filename.
 *
 * Returns: the active vst3 filename
 *
 * Since: 3.10.12
 */
gchar*
ags_vst3_browser_get_plugin_filename(AgsVst3Browser *vst3_browser)
{
  GtkListStore *filename_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *filename;
  
  if(!AGS_IS_VST3_BROWSER(vst3_browser)){
    return(NULL);
  }

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(vst3_browser->filename_tree_view));

  gtk_tree_view_get_cursor(vst3_browser->filename_tree_view,
			   &path,
			   NULL);
  
  gtk_tree_model_get_iter(GTK_TREE_MODEL(filename_list_store), &iter, path);

  gtk_tree_path_free(path);

  filename = NULL;
  
  gtk_tree_model_get(GTK_TREE_MODEL(filename_list_store),
		     &iter,
		     0, &filename,
		     -1);
  
  return(filename);
}

/**
 * ags_vst3_browser_get_plugin_effect:
 * @vst3_browser: the #AgsVst3Browser
 *
 * Retrieve selected vst3 effect.
 *
 * Returns: the active vst3 effect
 *
 * Since: 3.10.12
 */
gchar*
ags_vst3_browser_get_plugin_effect(AgsVst3Browser *vst3_browser)
{
  GtkListStore *effect_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *effect;
  
  if(!AGS_IS_VST3_BROWSER(vst3_browser)){
    return(NULL);
  }

  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(vst3_browser->effect_tree_view));

  gtk_tree_view_get_cursor(vst3_browser->effect_tree_view,
			   &path,
			   NULL);
  
  gtk_tree_model_get_iter(GTK_TREE_MODEL(effect_list_store), &iter, path);

  gtk_tree_path_free(path);

  effect = NULL;
  
  gtk_tree_model_get(GTK_TREE_MODEL(effect_list_store),
		     &iter,
		     0, &effect,
		     -1);
  
  return(effect);
}

/**
 * ags_vst3_browser_get_port_editor:
 * @vst3_browser: the #AgsVst3Browser
 * 
 * Get bulk member of @vst3_browser.
 * 
 * Returns: the #GList-struct containing #AgsPortEditor
 *
 * Since: 4.0.0
 */
GList*
ags_vst3_browser_get_port_editor(AgsVst3Browser *vst3_browser)
{
  g_return_val_if_fail(AGS_IS_VST3_BROWSER(vst3_browser), NULL);

  return(g_list_reverse(g_list_copy(vst3_browser->port_editor)));
}

/**
 * ags_vst3_browser_add_port_editor:
 * @vst3_browser: the #AgsVst3Browser
 * @port_editor: the #AgsPortEditor
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @port_editor to @vst3_browser.
 * 
 * Since: 4.0.0
 */
void
ags_vst3_browser_add_port_editor(AgsVst3Browser *vst3_browser,
				 AgsPortEditor *port_editor,
				 guint x, guint y,
				 guint width, guint height)
{
  g_return_if_fail(AGS_IS_VST3_BROWSER(vst3_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(vst3_browser->port_editor, port_editor) == NULL){
    vst3_browser->port_editor = g_list_prepend(vst3_browser->port_editor,
					       port_editor);
    
    gtk_grid_attach(vst3_browser->port_editor_grid,
		    port_editor,
		    x, y,
		    width, height);
  }
}

/**
 * ags_vst3_browser_remove_port_editor:
 * @vst3_browser: the #AgsVst3Browser
 * @port_editor: the #AgsPortEditor
 * 
 * Remove @port_editor from @vst3_browser.
 * 
 * Since: 4.0.0
 */
void
ags_vst3_browser_remove_port_editor(AgsVst3Browser *vst3_browser,
				    AgsPortEditor *port_editor)
{
  g_return_if_fail(AGS_IS_VST3_BROWSER(vst3_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(vst3_browser->port_editor, port_editor) != NULL){
    vst3_browser->port_editor = g_list_remove(vst3_browser->port_editor,
					      port_editor);
    
    gtk_grid_remove(vst3_browser->port_editor_grid,
		    port_editor);
  }
}

/**
 * ags_vst3_browser_clear:
 * @vst3_browser: the #AgsVst3Browser
 * 
 * Clear @vst3_browser.
 * 
 * Since: 4.0.0
 */
void
ags_vst3_browser_clear(AgsVst3Browser *vst3_browser)
{
  GList *start_port_editor, *port_editor;

  gchar *str;

  g_return_if_fail(AGS_IS_VST3_BROWSER(vst3_browser));

  /* update ui - reading plugin file */
  //TODO:JK: implement me
  
  port_editor =
    start_port_editor = ags_vst3_browser_get_port_editor(vst3_browser);
    
  while(port_editor != NULL){
    ags_vst3_browser_remove_port_editor(vst3_browser,
					port_editor->data);
      
    g_object_run_dispose(port_editor->data);
    g_object_unref(port_editor->data);

    port_editor = port_editor->next;
  }

  g_list_free(start_port_editor);
}

GtkWidget*
ags_vst3_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_vst3_browser_new:
 *
 * Create a new instance of #AgsVst3Browser
 *
 * Returns: the new #AgsVst3Browser
 *
 * Since: 3.10.12
 */
AgsVst3Browser*
ags_vst3_browser_new()
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = (AgsVst3Browser *) g_object_new(AGS_TYPE_VST3_BROWSER,
						 "homogeneous", FALSE,
						 "spacing", 0,
						 NULL);

  return(vst3_browser);
}
