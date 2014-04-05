/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_notebook_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

void ags_notebook_class_init(AgsNotebookClass *notebook);
void ags_notebook_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notebook_init(AgsNotebook *notebook);
void ags_notebook_connect(AgsConnectable *connectable);
void ags_notebook_disconnect(AgsConnectable *connectable);
void ags_notebook_destroy(GtkObject *object);
void ags_notebook_show(GtkWidget *widget);
void ags_notebook_paint(AgsNotebook *notebook);

AgsNotebookTab* ags_notebook_tab_alloc();

GtkStyle *notebook_style;

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
  gtk_widget_set_style((GtkWidget *) notebook, notebook_style);

  notebook->flags = 0;

  notebook->hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(notebook),
		     GTK_WIDGET(notebook->hbox),
		     FALSE, FALSE,
		     0);

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

  g_signal_connect((GObject *) notebook, "destroy\0",
		   G_CALLBACK(ags_notebook_destroy_callback), (gpointer) notebook);

  g_signal_connect((GObject *) notebook, "show\0",
		   G_CALLBACK(ags_notebook_show_callback), (gpointer) notebook);

  g_signal_connect((GObject *) editor, "change_machine\0",
		   G_CALLBACK(ags_notebook_change_machine_callback), notebook);
}

void
ags_notebook_disconnect(AgsConnectable *connectable)
{
}

void
ags_notebook_destroy(GtkObject *object)
{
}

void
ags_notebook_show(GtkWidget *widget)
{
}

AgsNotebookTab*
ags_notebook_tab_alloc()
{
  AgsNotebookTab *notebook_tab;

  notebook_tab = (AgsNotebookTab *) malloc(sizeof(AgsNotebookTab));

  notebook_tab->flags = 0;

  notebook_tab->toggle = NULL;
  notebook_tab->notation = NULL;

  return(notebook_tab);
}

gint
ags_notebook_tab_index(AgsNotebook *notebook,
		       GObject *notation)
{
  GList *list;
  gint i;

  list = notebook->tabs;

  for(i = 0; list != NULL; i++){
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
  gint index;

  tab = ags_notebook_tab_alloc();

  notebook->tabs = g_list_prepend(notebook->tabs,
				  tab);
  index = g_list_length(notebook->tabs);

  tab->toggle = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("channel %d\0",
										     index + 1));
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);

  return(index);
}

gint
ags_notebook_next_active_tab(AgsNotebook *notebook,
			     gint position)
{
  GList *list;
  gint length;
  gint i;

  list = notebook->tabs;
  length = g_list_length(notebook->tabs);

  for(i = 0; i < length - position && list != NULL; i++){
    if(gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
      return(position + i);
    }

    list = list->next;
  }

  return(-1);
}

void
ags_notebook_insert_tab(AgsNotebook *notebook,
			gint position)
{
  AgsNotebookTab *tab;
  gint length;

  length = g_list_length(notebook->tabs);

  tab = ags_notebook_tab_alloc();
  notebook->tabs = g_list_insert(notebook->tabs,
				 tab,
				 length - position);

  tab->toggle = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("channel %d\0",
										     position + 1));
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(notebook->hbox),
			GTK_WIDGET(tab->toggle),
			position);
}

void
ags_notebook_remove_tab(AgsNotebook *notebook,
			gint nth)
{
  AgsNotebookTab *tab;
  gint length;
  
  length = g_list_length(notebook->tabs);
  tab = g_list_nth_data(notebook->tabs,
			length - nth);

  notebook->tabs = g_list_remove(notebook->tabs,
				 tab);
  gtk_widget_destroy(GTK_WIDGET(tab->toggle));
  free(tab);
}

void
ags_notebook_add_child(AgsNotebook *notebook,
		       GtkWidget *child)
{
  gtk_box_pack_start(GTK_BOX(notebook),
		     child,
		     FALSE, FALSE,
		     0);
}

void
ags_notebook_remove_child(AgsNotebook *notebook,
			  GtkWidget *child)
{
  gtk_widget_destroy(child);
}

AgsNotebook*
ags_notebook_new()
{
  AgsNotebook *notebook;

  notebook = (AgsNotebook *) g_object_new(AGS_TYPE_NOTEBOOK, NULL);

  return(notebook);
}
