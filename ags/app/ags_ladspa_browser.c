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

#include <ags/app/ags_ladspa_browser.h>
#include <ags/app/ags_ladspa_browser_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_ladspa_browser_class_init(AgsLadspaBrowserClass *ladspa_browser);
void ags_ladspa_browser_init(AgsLadspaBrowser *ladspa_browser);
void ags_ladspa_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ladspa_browser_applicable_interface_init(AgsApplicableInterface *applicable);

void ags_ladspa_browser_connect(AgsConnectable *connectable);
void ags_ladspa_browser_disconnect(AgsConnectable *connectable);

void ags_ladspa_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_ladspa_browser_apply(AgsApplicable *applicable);
void ags_ladspa_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_ladspa_browser
 * @short_description: A composite to select ladspa effect.
 * @title: AgsLadspaBrowser
 * @section_id:
 * @include: ags/app/ags_ladspa_browser.h
 *
 * #AgsLadspaBrowser is a composite widget to select ladspa plugin and the desired
 * effect.
 */

GType
ags_ladspa_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ladspa_browser = 0;

    static const GTypeInfo ags_ladspa_browser_info = {
      sizeof (AgsLadspaBrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaBrowser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ladspa_browser = g_type_register_static(GTK_TYPE_BOX,
						     "AgsLadspaBrowser", &ags_ladspa_browser_info,
						     0);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ladspa_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_ladspa_browser_class_init(AgsLadspaBrowserClass *ladspa_browser)
{
  /* empty */
}

void
ags_ladspa_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ladspa_browser_connect;
  connectable->disconnect = ags_ladspa_browser_disconnect;
}

void
ags_ladspa_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ladspa_browser_set_update;
  applicable->apply = ags_ladspa_browser_apply;
  applicable->reset = ags_ladspa_browser_reset;
}

void
ags_ladspa_browser_init(AgsLadspaBrowser *ladspa_browser)
{
  GtkLabel *label;
  GtkTreeViewColumn *filename_column;
  GtkTreeViewColumn *effect_column;
  GtkScrolledWindow *scrolled_window;

  GtkCellRenderer *filename_renderer;
  GtkCellRenderer *effect_renderer;

  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  GList *start_list, *list;

  gchar **filenames, **filenames_start;
  gchar *str;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(ladspa_browser),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(ladspa_browser,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  ladspa_browser->connectable_flags = 0;
  
  /* plugin */
  ladspa_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						  AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) ladspa_browser,
		 (GtkWidget *) ladspa_browser->plugin);

  /* filename */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_LADSPA_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_LADSPA_BROWSER_FILENAME_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(ladspa_browser->plugin,
		 (GtkWidget *) scrolled_window);

  ladspa_browser->filename_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(ladspa_browser->filename_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) ladspa_browser->filename_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) ladspa_browser->filename_tree_view,
			      AGS_LADSPA_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_LADSPA_BROWSER_FILENAME_HEIGHT_REQUEST);

  filename_renderer = gtk_cell_renderer_text_new();

  filename_column = gtk_tree_view_column_new_with_attributes(i18n("filename"),
							     filename_renderer,
							     "text", 0,
							     NULL);
  gtk_tree_view_append_column(ladspa_browser->filename_tree_view,
			      filename_column);
  
  filename_list_store = gtk_list_store_new(1,
					   G_TYPE_STRING);

  gtk_tree_view_set_model(ladspa_browser->filename_tree_view,
			  GTK_TREE_MODEL(filename_list_store));  

  ladspa_browser->path = NULL;

  filenames =
    filenames_start = ags_ladspa_manager_get_filenames(ags_ladspa_manager_get_instance());

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
			      AGS_LADSPA_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_LADSPA_BROWSER_EFFECT_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(ladspa_browser->plugin,
		 (GtkWidget *) scrolled_window);

  ladspa_browser->effect_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(ladspa_browser->effect_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) ladspa_browser->effect_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) ladspa_browser->effect_tree_view,
			      AGS_LADSPA_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_LADSPA_BROWSER_EFFECT_HEIGHT_REQUEST);

  effect_renderer = gtk_cell_renderer_text_new();

  effect_column = gtk_tree_view_column_new_with_attributes(i18n("effect"),
							   effect_renderer,
							   "text", 0,
							   NULL);
  gtk_tree_view_append_column(ladspa_browser->effect_tree_view,
			      effect_column);
  
  effect_list_store = gtk_list_store_new(1,
					 G_TYPE_STRING);

  gtk_tree_view_set_model(ladspa_browser->effect_tree_view,
			  GTK_TREE_MODEL(effect_list_store));  

  /* description */
  ladspa_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						       AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) ladspa_browser,
		 (GtkWidget *) ladspa_browser->description);

  /* label */
  str = g_strconcat(i18n("Label"),
		    ": ",
		    NULL);
  
  ladspa_browser->label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						    "xalign", 0.0,
						    "label", str,
						    NULL);
  gtk_box_append((GtkBox *) ladspa_browser->description,
		 (GtkWidget *) ladspa_browser->label);

  g_free(str);

  /* maker */
  str = g_strconcat(i18n("Maker"),
		    ": ",
		    NULL);

  ladspa_browser->maker = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						    "xalign", 0.0,
						    "label", str,
						    NULL);
  gtk_box_append((GtkBox *) ladspa_browser->description,
		 (GtkWidget *) ladspa_browser->maker);

  g_free(str);

  /* copyright */
  str = g_strconcat(i18n("Copyright"),
		    ": ",
		    NULL);

  ladspa_browser->copyright = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							"xalign", 0.0,
							"label", str,
							NULL);
  gtk_box_append((GtkBox *) ladspa_browser->description,
		 (GtkWidget *) ladspa_browser->copyright);

  g_free(str);

  /* ports */
  str = g_strconcat(i18n("Ports"),
		    ": ",
		    NULL);  

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_append((GtkBox *) ladspa_browser->description,
		 (GtkWidget *) label);

  g_free(str);

  /* port editor */
  ladspa_browser->port_editor = NULL;
  
  ladspa_browser->port_editor_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(ladspa_browser->port_editor_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(ladspa_browser->port_editor_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_box_append((GtkBox *) ladspa_browser->description,
		 (GtkWidget *) ladspa_browser->port_editor_grid);

  /* preview */
  ladspa_browser->preview = NULL;
}

void
ags_ladspa_browser_connect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (ladspa_browser->connectable_flags)) != 0){
    return;
  }

  ladspa_browser->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(G_OBJECT(ladspa_browser->filename_tree_view), "row-activated",
			 G_CALLBACK(ags_ladspa_browser_plugin_filename_callback), ladspa_browser);

  g_signal_connect_after(G_OBJECT(ladspa_browser->effect_tree_view), "row-activated",
			 G_CALLBACK(ags_ladspa_browser_plugin_effect_callback), ladspa_browser);
}

void
ags_ladspa_browser_disconnect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (ladspa_browser->connectable_flags)) == 0){
    return;
  }

  ladspa_browser->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(ladspa_browser->filename_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_ladspa_browser_plugin_filename_callback),
		      ladspa_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(ladspa_browser->effect_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_ladspa_browser_plugin_effect_callback),
		      ladspa_browser,
		      NULL);
}

void
ags_ladspa_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ladspa_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_ladspa_browser_reset(AgsApplicable *applicable)
{
  AgsLadspaBrowser *ladspa_browser;

  GtkTreeModel *model;

  GtkTreeIter tree_iter;

  ladspa_browser = AGS_LADSPA_BROWSER(applicable);
	  
  model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(ladspa_browser->filename_tree_view)));

  if(gtk_tree_model_get_iter_first(model, &tree_iter)){
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(ladspa_browser->filename_tree_view),
			     gtk_tree_model_get_path(model,
						     &tree_iter),
			     NULL,
			     FALSE);
  }
}

/**
 * ags_ladspa_browser_get_plugin_filename:
 * @ladspa_browser: the #AgsLadspaBrowser
 *
 * Retrieve selected ladspa plugin filename.
 *
 * Returns: the active ladspa filename
 *
 * Since: 3.0.0
 */
gchar*
ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser)
{
  GtkListStore *filename_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *filename;
  
  if(!AGS_IS_LADSPA_BROWSER(ladspa_browser)){
    return(NULL);
  }

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(ladspa_browser->filename_tree_view));

  path = NULL;
  gtk_tree_view_get_cursor(ladspa_browser->filename_tree_view,
			   &path,
			   NULL);  

  filename = NULL;

  if(gtk_tree_model_get_iter(GTK_TREE_MODEL(filename_list_store), &iter, path)){
    gtk_tree_model_get(GTK_TREE_MODEL(filename_list_store),
		       &iter,
		       0, &filename,
		       -1);
  }

  gtk_tree_path_free(path);
    
  return(filename);
}

/**
 * ags_ladspa_browser_get_plugin_effect:
 * @ladspa_browser: the #AgsLadspaBrowser
 *
 * Retrieve selected ladspa effect.
 *
 * Returns: the active ladspa effect
 *
 * Since: 3.0.0
 */
gchar*
ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser)
{
  GtkListStore *effect_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *effect;
  
  if(!AGS_IS_LADSPA_BROWSER(ladspa_browser)){
    return(NULL);
  }

  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(ladspa_browser->effect_tree_view));

  gtk_tree_view_get_cursor(ladspa_browser->effect_tree_view,
			   &path,
			   NULL);

  effect = NULL;
  
  if(gtk_tree_model_get_iter(GTK_TREE_MODEL(effect_list_store), &iter, path)){
    gtk_tree_model_get(GTK_TREE_MODEL(effect_list_store),
		       &iter,
		       0, &effect,
		       -1);
  }
  
  gtk_tree_path_free(path);
  
  return(effect);
}

/**
 * ags_ladspa_browser_get_port_editor:
 * @ladspa_browser: the #AgsLadspaBrowser
 * 
 * Get bulk member of @ladspa_browser.
 * 
 * Returns: the #GList-struct containing #AgsPortEditor
 *
 * Since: 4.0.0
 */
GList*
ags_ladspa_browser_get_port_editor(AgsLadspaBrowser *ladspa_browser)
{
  g_return_val_if_fail(AGS_IS_LADSPA_BROWSER(ladspa_browser), NULL);

  return(g_list_reverse(g_list_copy(ladspa_browser->port_editor)));
}

/**
 * ags_ladspa_browser_add_port_editor:
 * @ladspa_browser: the #AgsLadspaBrowser
 * @port_editor: the #AgsPortEditor
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @port_editor to @ladspa_browser.
 * 
 * Since: 4.0.0
 */
void
ags_ladspa_browser_add_port_editor(AgsLadspaBrowser *ladspa_browser,
				   AgsPortEditor *port_editor,
				   guint x, guint y,
				   guint width, guint height)
{
  g_return_if_fail(AGS_IS_LADSPA_BROWSER(ladspa_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(ladspa_browser->port_editor, port_editor) == NULL){
    ladspa_browser->port_editor = g_list_prepend(ladspa_browser->port_editor,
						 port_editor);
    
    gtk_grid_attach(ladspa_browser->port_editor_grid,
		    port_editor,
		    x, y,
		    width, height);
  }
}

/**
 * ags_ladspa_browser_remove_port_editor:
 * @ladspa_browser: the #AgsLadspaBrowser
 * @port_editor: the #AgsPortEditor
 * 
 * Remove @port_editor from @ladspa_browser.
 * 
 * Since: 4.0.0
 */
void
ags_ladspa_browser_remove_port_editor(AgsLadspaBrowser *ladspa_browser,
				      AgsPortEditor *port_editor)
{
  g_return_if_fail(AGS_IS_LADSPA_BROWSER(ladspa_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(ladspa_browser->port_editor, port_editor) != NULL){
    ladspa_browser->port_editor = g_list_remove(ladspa_browser->port_editor,
						port_editor);
    
    gtk_grid_remove(ladspa_browser->port_editor_grid,
		    port_editor);
  }
}

/**
 * ags_ladspa_browser_clear:
 * @ladspa_browser: the #AgsLadspaBrowser
 * 
 * Clear @ladspa_browser.
 * 
 * Since: 4.0.0
 */
void
ags_ladspa_browser_clear(AgsLadspaBrowser *ladspa_browser)
{
  GList *start_port_editor, *port_editor;

  gchar *str;

  g_return_if_fail(AGS_IS_LADSPA_BROWSER(ladspa_browser));

  /* update ui - reading plugin file */
  str = g_strconcat(i18n("Label"),
		    ": ",
		    NULL);
  gtk_label_set_text(ladspa_browser->label,
		     str);

  g_free(str);

  str = g_strconcat(i18n("Maker"),
		    ": ",
		    NULL);
  gtk_label_set_text(ladspa_browser->maker,
		     str);

  g_free(str);

  str = g_strconcat(i18n("Copyright"),
		    ": ",
		    NULL);
  gtk_label_set_text(ladspa_browser->copyright,
		     str);

  g_free(str);

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
}

GtkWidget*
ags_ladspa_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_ladspa_browser_new:
 *
 * Create a new instance of #AgsLadspaBrowser
 *
 * Returns: the new #AgsLadspaBrowser
 *
 * Since: 3.0.0
 */
AgsLadspaBrowser*
ags_ladspa_browser_new()
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = (AgsLadspaBrowser *) g_object_new(AGS_TYPE_LADSPA_BROWSER,
						     "homogeneous", FALSE,
						     "spacing", 0,
						     NULL);

  return(ladspa_browser);
}
