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

#include <ags/app/ags_lv2_browser.h>
#include <ags/app/ags_lv2_browser_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>

#include <ags/i18n.h>

void ags_lv2_browser_class_init(AgsLv2BrowserClass *lv2_browser);
void ags_lv2_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_lv2_browser_init(AgsLv2Browser *lv2_browser);

void ags_lv2_browser_connect(AgsConnectable *connectable);
void ags_lv2_browser_disconnect(AgsConnectable *connectable);

void ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_lv2_browser_apply(AgsApplicable *applicable);
void ags_lv2_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_lv2_browser
 * @short_description: A composite to select lv2 uri.
 * @title: AgsLv2Browser
 * @section_id:
 * @include: ags/app/ags_lv2_browser.h
 *
 * #AgsLv2Browser is a composite widget to select lv2 plugin and the desired
 * uri.
 */

GType
ags_lv2_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_browser = 0;

    static const GTypeInfo ags_lv2_browser_info = {
      sizeof (AgsLv2BrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Browser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_browser = g_type_register_static(GTK_TYPE_BOX,
						  "AgsLv2Browser", &ags_lv2_browser_info,
						  0);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_browser_class_init(AgsLv2BrowserClass *lv2_browser)
{
  /* empty */
}

void
ags_lv2_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_lv2_browser_connect;
  connectable->disconnect = ags_lv2_browser_disconnect;
}

void
ags_lv2_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_lv2_browser_set_update;
  applicable->apply = ags_lv2_browser_apply;
  applicable->reset = ags_lv2_browser_reset;
}

void
ags_lv2_browser_init(AgsLv2Browser *lv2_browser)
{
  GtkLabel *label;
  GtkTreeViewColumn *filename_column;
  GtkTreeViewColumn *effect_column;
  GtkScrolledWindow *scrolled_window;

  GtkCellRenderer *filename_renderer;
  GtkCellRenderer *effect_renderer;

  GtkListStore *filename_list_store;
  GtkListStore *effect_list_store;

  AgsLv2Manager *lv2_manager;

  gchar *str;
  gchar **filename_strv;

  guint length;
  guint i;

  GRecMutex *lv2_manager_mutex;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(lv2_browser),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(lv2_browser,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  lv2_browser->connectable_flags = 0;
  
  /* plugin */
  lv2_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					       0);
  gtk_box_append((GtkBox *) lv2_browser,
		 (GtkWidget *) lv2_browser->plugin);

  /* filename */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_LV2_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_LV2_BROWSER_FILENAME_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(lv2_browser->plugin,
		 (GtkWidget *) scrolled_window);

  lv2_browser->filename_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(lv2_browser->filename_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) lv2_browser->filename_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) lv2_browser->filename_tree_view,
			      AGS_LV2_BROWSER_FILENAME_WIDTH_REQUEST,
			      AGS_LV2_BROWSER_FILENAME_HEIGHT_REQUEST);

  filename_renderer = gtk_cell_renderer_text_new();

  filename_column = gtk_tree_view_column_new_with_attributes(i18n("filename"),
							     filename_renderer,
							     "text", 0,
							     NULL);
  gtk_tree_view_append_column(lv2_browser->filename_tree_view,
			      filename_column);
  
  filename_list_store = gtk_list_store_new(1,
					   G_TYPE_STRING);

  gtk_tree_view_set_model(lv2_browser->filename_tree_view,
			  GTK_TREE_MODEL(filename_list_store));  

  lv2_browser->path = NULL;

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* get plugin */
  filename_strv = NULL;

  g_rec_mutex_lock(lv2_manager_mutex);

  length = 0;

  if(lv2_manager->quick_scan_plugin_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_plugin_filename);
  }
  
  for(i = 0; i < length; i++){
    gchar *filename, *effect;
    
    /* get filename and effect */
    filename = lv2_manager->quick_scan_plugin_filename[i];
    effect = lv2_manager->quick_scan_plugin_effect[i];
    
    /* create item */
    if(filename != NULL &&
       effect != NULL &&
       (filename_strv == NULL ||
	!g_strv_contains(filename_strv,
			 filename))){
      GtkTreeIter tree_iter;

      guint length;
      
      gtk_list_store_append(filename_list_store,
			    &tree_iter);

      gtk_list_store_set(filename_list_store, &tree_iter,
			 0, filename,
			 -1);

      if(filename_strv == NULL){
	length = 0;
	
	filename_strv = (gchar **) g_malloc(2 * sizeof(gchar *));
      }else{
	length = g_strv_length(filename_strv);

	filename_strv = (gchar **) g_realloc(filename_strv,
					     (length + 2) * sizeof(gchar *));
      }

      filename_strv[length] = filename;
      filename_strv[length + 1] = NULL;
    }
  }

  g_rec_mutex_unlock(lv2_manager_mutex);

  g_free(filename_strv);

  /* effect */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_LV2_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_LV2_BROWSER_EFFECT_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(lv2_browser->plugin,
		 (GtkWidget *) scrolled_window);

  lv2_browser->effect_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(lv2_browser->effect_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) lv2_browser->effect_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) lv2_browser->effect_tree_view,
			      AGS_LV2_BROWSER_EFFECT_WIDTH_REQUEST,
			      AGS_LV2_BROWSER_EFFECT_HEIGHT_REQUEST);

  effect_renderer = gtk_cell_renderer_text_new();

  effect_column = gtk_tree_view_column_new_with_attributes(i18n("effect"),
							   effect_renderer,
							   "text", 0,
							   NULL);
  gtk_tree_view_append_column(lv2_browser->effect_tree_view,
			      effect_column);
  
  effect_list_store = gtk_list_store_new(1,
					 G_TYPE_STRING);

  gtk_tree_view_set_model(lv2_browser->effect_tree_view,
			  GTK_TREE_MODEL(effect_list_store));  

  /* description */
  lv2_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    0);
  gtk_box_append((GtkBox *) lv2_browser,
		 (GtkWidget *) lv2_browser->description);

  /* name */
  str = g_strdup_printf("%s: ",
			i18n("Name"));
  lv2_browser->lv2_name = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						    "xalign", 0.0,
						    "label", str,
						    NULL);
  gtk_box_append(lv2_browser->description,
		 (GtkWidget *) lv2_browser->lv2_name);

  g_free(str);

  /* homepage */
  str = g_strdup_printf("%s: ",
			i18n("Homepage"));
  lv2_browser->lv2_homepage = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							"xalign", 0.0,
							"label", str,
							NULL);
  gtk_box_append(lv2_browser->description,
		 (GtkWidget *) lv2_browser->lv2_homepage);

  g_free(str);

  /* mbox */
  str = g_strdup_printf("%s: ",
			i18n("M-Box"));
  lv2_browser->lv2_mbox = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						    "xalign", 0.0,
						    "label", str,
						    NULL);
  gtk_box_append(lv2_browser->description,
		 (GtkWidget *) lv2_browser->lv2_mbox);

  g_free(str);

  /* ports */
  str = g_strdup_printf("%s: ",
			i18n("Ports"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_append(lv2_browser->description,
		 (GtkWidget *) label);

  g_free(str);

  /* port grid */
  lv2_browser->port_editor_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_append(lv2_browser->description,
		 (GtkWidget *) lv2_browser->port_editor_grid);

  /* preview */
  lv2_browser->preview = NULL;
}

void
ags_lv2_browser_connect(AgsConnectable *connectable)
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = AGS_LV2_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (lv2_browser->connectable_flags)) != 0){
    return;
  }

  lv2_browser->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(G_OBJECT(lv2_browser->filename_tree_view), "row-activated",
			 G_CALLBACK(ags_lv2_browser_plugin_filename_callback), lv2_browser);

  g_signal_connect_after(G_OBJECT(lv2_browser->effect_tree_view), "row-activated",
			 G_CALLBACK(ags_lv2_browser_plugin_uri_callback), lv2_browser);
}

void
ags_lv2_browser_disconnect(AgsConnectable *connectable)
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = AGS_LV2_BROWSER(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (lv2_browser->connectable_flags)) == 0){
    return;
  }

  lv2_browser->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(lv2_browser->filename_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_lv2_browser_plugin_filename_callback),
		      lv2_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(lv2_browser->effect_tree_view),
		      "any_signal::row-activated",
		      G_CALLBACK(ags_lv2_browser_plugin_uri_callback),
		      lv2_browser,
		      NULL);
}

void
ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_lv2_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_lv2_browser_reset(AgsApplicable *applicable)
{
  AgsLv2Browser *lv2_browser;

  GtkTreeModel *model;

  GtkTreeIter tree_iter;

  lv2_browser = AGS_LV2_BROWSER(applicable);

  model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(lv2_browser->filename_tree_view)));

  if(gtk_tree_model_get_iter_first(model, &tree_iter)){
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(lv2_browser->filename_tree_view),
			     gtk_tree_model_get_path(model,
						     &tree_iter),
			     NULL,
			     FALSE);
  }
}

/**
 * ags_lv2_browser_get_plugin_filename:
 * @lv2_browser: the #AgsLv2Browser
 *
 * Retrieve selected lv2 plugin filename.
 *
 * Returns: the active lv2 filename
 *
 * Since: 3.0.0
 */
gchar*
ags_lv2_browser_get_plugin_filename(AgsLv2Browser *lv2_browser)
{
  GtkListStore *filename_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *filename;
  
  if(!AGS_IS_LV2_BROWSER(lv2_browser)){
    return(NULL);
  }

  filename_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(lv2_browser->filename_tree_view));

  gtk_tree_view_get_cursor(lv2_browser->filename_tree_view,
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
 * ags_lv2_browser_get_plugin_effect:
 * @lv2_browser: the #AgsLv2Browser
 *
 * Retrieve selected lv2 effect.
 *
 * Returns: the active lv2 effect
 *
 * Since: 3.0.0
 */
gchar*
ags_lv2_browser_get_plugin_effect(AgsLv2Browser *lv2_browser)
{
  GtkListStore *effect_list_store;
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;
  
  GtkTreeIter iter;

  gchar *effect;
  
  if(!AGS_IS_LV2_BROWSER(lv2_browser)){
    return(NULL);
  }

  effect_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(lv2_browser->effect_tree_view));

  gtk_tree_view_get_cursor(lv2_browser->effect_tree_view,
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
 * ags_lv2_browser_get_port_editor:
 * @lv2_browser: the #AgsLv2Browser
 * 
 * Get bulk member of @lv2_browser.
 * 
 * Returns: the #GList-struct containing #AgsPortEditor
 *
 * Since: 4.0.0
 */
GList*
ags_lv2_browser_get_port_editor(AgsLv2Browser *lv2_browser)
{
  g_return_val_if_fail(AGS_IS_LV2_BROWSER(lv2_browser), NULL);

  return(g_list_reverse(g_list_copy(lv2_browser->port_editor)));
}

/**
 * ags_lv2_browser_add_port_editor:
 * @lv2_browser: the #AgsLv2Browser
 * @port_editor: the #AgsPortEditor
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @port_editor to @lv2_browser.
 * 
 * Since: 4.0.0
 */
void
ags_lv2_browser_add_port_editor(AgsLv2Browser *lv2_browser,
				AgsPortEditor *port_editor,
				guint x, guint y,
				guint width, guint height)
{
  g_return_if_fail(AGS_IS_LV2_BROWSER(lv2_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(lv2_browser->port_editor, port_editor) == NULL){
    lv2_browser->port_editor = g_list_prepend(lv2_browser->port_editor,
					      port_editor);
    
    gtk_grid_attach(lv2_browser->port_editor_grid,
		    port_editor,
		    x, y,
		    width, height);
  }
}

/**
 * ags_lv2_browser_remove_port_editor:
 * @lv2_browser: the #AgsLv2Browser
 * @port_editor: the #AgsPortEditor
 * 
 * Remove @port_editor from @lv2_browser.
 * 
 * Since: 4.0.0
 */
void
ags_lv2_browser_remove_port_editor(AgsLv2Browser *lv2_browser,
				   AgsPortEditor *port_editor)
{
  g_return_if_fail(AGS_IS_LV2_BROWSER(lv2_browser));
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  if(g_list_find(lv2_browser->port_editor, port_editor) != NULL){
    lv2_browser->port_editor = g_list_remove(lv2_browser->port_editor,
					     port_editor);
    
    gtk_grid_remove(lv2_browser->port_editor_grid,
		    port_editor);
  }
}

/**
 * ags_lv2_browser_clear:
 * @lv2_browser: the #AgsLv2Browser
 * 
 * Clear @lv2_browser.
 * 
 * Since: 4.0.0
 */
void
ags_lv2_browser_clear(AgsLv2Browser *lv2_browser)
{
  GList *start_port_editor, *port_editor;

  gchar *str;

  g_return_if_fail(AGS_IS_LV2_BROWSER(lv2_browser));

  /* name */
  str = g_strdup_printf("%s: ",
			i18n("Name"));
  gtk_label_set_text(lv2_browser->lv2_name,
		     str);

  g_free(str);

  /* homepage */
  str = g_strdup_printf("%s: ",
			i18n("Homepage"));
  gtk_label_set_text(lv2_browser->lv2_homepage,
		     str);

  g_free(str);

  /* mbox */
  str = g_strdup_printf("%s: ",
			i18n("M-Box"));
  gtk_label_set_text(lv2_browser->lv2_mbox,
		     str);

  g_free(str);

  /* port editor */
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
}

GtkWidget*
ags_lv2_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_lv2_browser_new:
 *
 * Create a new instance of #AgsLv2Browser
 *
 * Returns: the new #AgsLv2Browser
 *
 * Since: 3.0.0
 */
AgsLv2Browser*
ags_lv2_browser_new()
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = (AgsLv2Browser *) g_object_new(AGS_TYPE_LV2_BROWSER,
					       "homogeneous", FALSE,
					       "spacing", 0,
					       NULL);

  return(lv2_browser);
}
