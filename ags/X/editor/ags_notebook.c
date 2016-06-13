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

#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_notebook_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

void ags_notebook_class_init(AgsNotebookClass *notebook);
void ags_notebook_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notebook_init(AgsNotebook *notebook);
void ags_notebook_connect(AgsConnectable *connectable);
void ags_notebook_disconnect(AgsConnectable *connectable);

void ags_notebook_paint(AgsNotebook *notebook);

AgsNotebookTab* ags_notebook_tab_alloc();

/**
 * SECTION:ags_notebook
 * @short_description: select channel
 * @title: AgsNotebook
 * @section_id:
 * @include: ags/X/editor/ags_notebook.h
 *
 * The #AgsNotebook lets select/deselect channels to edit.
 */

GType
ags_notebook_get_type(void)
{
  static GType ags_type_notebook = 0;

  if(!ags_type_notebook){
    static const GTypeInfo ags_notebook_info = {
      sizeof (AgsNotebookClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notebook_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotebook),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notebook_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notebook_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notebook = g_type_register_static(GTK_TYPE_VBOX,
					       "AgsNotebook\0", &ags_notebook_info,
					       0);
    
    g_type_add_interface_static(ags_type_notebook,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_notebook);
}

void
ags_notebook_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notebook_connect;
  connectable->disconnect = ags_notebook_disconnect;
}

void
ags_notebook_class_init(AgsNotebookClass *notebook)
{
}

void
ags_notebook_init(AgsNotebook *notebook)
{
  GtkHBox *hbox;
  GtkArrow *arrow;
    
  notebook->flags = 0;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(notebook),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  arrow = (GtkArrow *) gtk_arrow_new(GTK_ARROW_LEFT,
				     GTK_SHADOW_NONE);
  notebook->scroll_prev = g_object_new(GTK_TYPE_BUTTON,
				       "child\0", arrow,
				       "relief\0", GTK_RELIEF_NONE,
				       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(notebook->scroll_prev),
		     FALSE, FALSE,
		     0);

  arrow = (GtkArrow *) gtk_arrow_new(GTK_ARROW_RIGHT,
				     GTK_SHADOW_NONE);
  notebook->scroll_next = g_object_new(GTK_TYPE_BUTTON,
				       "child\0", arrow,
				       "relief\0", GTK_RELIEF_NONE,
				       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(notebook->scroll_next),
		     FALSE, FALSE,
		     0);

  //TODO:JK: improve the code
  notebook->viewport = (GtkScrolledWindow *) gtk_viewport_new(NULL,
							      NULL);
  gtk_widget_set_size_request(notebook->viewport,
			      6 * AGS_NOTEBOOK_TAB_DEFAULT_WIDTH, AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(notebook->viewport),
		     //		     TRUE, TRUE,
		     FALSE, FALSE,
		     0);
  
  notebook->hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add(notebook->viewport,
		    GTK_WIDGET(notebook->hbox));
  
  notebook->tabs = NULL;
  notebook->child = NULL;
}

void
ags_notebook_connect(AgsConnectable *connectable)
{
  AgsNotebook *notebook;
  AgsEditor *editor;

  notebook = AGS_NOTEBOOK(connectable);

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) notebook, AGS_TYPE_EDITOR);

  g_message("connect\0");
  
  notebook->scroll_prev_handler = g_signal_connect(G_OBJECT(notebook->scroll_prev), "clicked\0",
						   G_CALLBACK(ags_notebook_scroll_prev_callback), notebook);
  notebook->scroll_next_handler = g_signal_connect(G_OBJECT(notebook->scroll_next), "clicked\0",
						   G_CALLBACK(ags_notebook_scroll_next_callback), notebook);
}

void
ags_notebook_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

AgsNotebookTab*
ags_notebook_tab_alloc()
{
  AgsNotebookTab *notebook_tab;

  notebook_tab = (AgsNotebookTab *) malloc(sizeof(AgsNotebookTab));

  notebook_tab->flags = 0;

  notebook_tab->notation = NULL;
  notebook_tab->automation = NULL;
  
  notebook_tab->toggle = NULL;

  return(notebook_tab);
}

gint
ags_notebook_tab_index(AgsNotebook *notebook,
		       GObject *notation)
{
  GList *list;
  gint i;

  if(notebook == NULL){
    return(-1);
  }

  list = notebook->tabs;

  for(i = g_list_length(notebook->tabs) - 1; list != NULL; i--){
    if(AGS_NOTEBOOK_TAB(list->data)->notation == notation){
      return(i);
    }

    list = list->next;
  }
  
  return(-1);
}

gint
ags_notebook_add_tab(AgsNotebook *notebook)
{
  AgsNotebookTab *tab;
  GtkViewport *viewport;

  GtkAdjustment *adjustment;
  
  gint tab_index;
  gint length;
  guint width;

  if(notebook == NULL){
    return(-1);
  }

  /* new tab */
  tab = ags_notebook_tab_alloc();

  notebook->tabs = g_list_prepend(notebook->tabs,
				  tab);
  tab_index = g_list_length(notebook->tabs);

  tab->toggle = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("channel %d\0",
										     tab_index));
  g_object_set(tab->toggle,
	       "xalign\0", 0.0,
	       "yalign\0", 0.0,
	       NULL);
  gtk_toggle_button_set_active(tab->toggle, TRUE);
  gtk_widget_set_size_request(tab->toggle,
			      AGS_NOTEBOOK_TAB_DEFAULT_WIDTH, AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT);  
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);

  gdk_window_invalidate_rect(gtk_viewport_get_view_window(notebook->viewport),
			     &(GTK_WIDGET(notebook->hbox)->allocation),
			     TRUE);

  return(tab_index);
}

gint
ags_notebook_next_active_tab(AgsNotebook *notebook,
			     gint position)
{
  GList *list, *list_start;
  gint i;

  if(notebook == NULL){
    return(-1);
  }
  
  list_start = g_list_copy(notebook->tabs);
  list_start = 
    list = g_list_reverse(list_start);

  list = g_list_nth(list,
		    position);

  for(i = 0; list != NULL; i++){
    if(gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
      g_list_free(list_start);
      return(position + i);
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  return(-1);
}

void
ags_notebook_insert_tab(AgsNotebook *notebook,
			gint position)
{
  AgsNotebookTab *tab;
  
  GtkAdjustment *adjustment;
  
  gint length;
  guint width;

  if(notebook == NULL){
    return;
  }

  /* insert tab */
  length = g_list_length(notebook->tabs);

  tab = ags_notebook_tab_alloc();
  notebook->tabs = g_list_insert(notebook->tabs,
				 tab,
				 length - position);

  tab->toggle = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("channel %d\0",
										     position + 1));
  g_object_set(tab->toggle,
	       "xalign\0", 0.0,
	       "yalign\0", 0.0,
	       NULL);
  gtk_widget_set_size_request(tab->toggle,
			      AGS_NOTEBOOK_TAB_DEFAULT_WIDTH, AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT);
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(notebook->hbox),
			GTK_WIDGET(tab->toggle),
			position);

  gdk_window_invalidate_rect(gtk_viewport_get_view_window(notebook->viewport),
			     &(GTK_WIDGET(notebook->hbox)->allocation),
			     TRUE);
}

void
ags_notebook_remove_tab(AgsNotebook *notebook,
			gint nth)
{
  AgsNotebookTab *tab;
  gint length;

  if(notebook->tabs == NULL){
    return;
  }
  
  length = g_list_length(notebook->tabs);

  tab = g_list_nth_data(notebook->tabs,
			length - nth - 1);

  notebook->tabs = g_list_remove(notebook->tabs,
				 tab);
  gtk_widget_destroy(GTK_WIDGET(tab->toggle));
  free(tab);
}

void
ags_notebook_add_child(AgsNotebook *notebook,
		       GtkWidget *child)
{
  if(notebook == NULL){
    return;
  }

  gtk_box_pack_start(GTK_BOX(notebook),
		     child,
		     FALSE, FALSE,
		     0);
}

void
ags_notebook_remove_child(AgsNotebook *notebook,
			  GtkWidget *child)
{
  if(notebook == NULL){
    return;
  }

  gtk_widget_destroy(child);
}

/**
 * ags_notebook_new:
 *
 * Create a new #AgsNotebook.
 *
 * Since: 0.4
 */
AgsNotebook*
ags_notebook_new()
{
  AgsNotebook *notebook;

  notebook = (AgsNotebook *) g_object_new(AGS_TYPE_NOTEBOOK, NULL);

  return(notebook);
}
