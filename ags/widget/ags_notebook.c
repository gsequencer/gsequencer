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

#include <ags/widget/ags_notebook.h>

#include <stdlib.h>

void ags_notebook_class_init(AgsNotebookClass *notebook);
void ags_notebook_init(AgsNotebook *notebook);
void ags_notebook_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_notebook_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_notebook_finalize(GObject *gobject);

void ags_notebook_scroll_prev_callback(GtkWidget *button,
				       AgsNotebook *notebook);
void ags_notebook_scroll_next_callback(GtkWidget *button,
				       AgsNotebook *notebook);

/**
 * SECTION:ags_notebook
 * @short_description: selection widget
 * @title: AgsNotebook
 * @section_id:
 * @include: ags/widget/ags_notebook.h
 *
 * The #AgsNotebook lets you select/deselect tabs and assign data to them.
 */

enum{
  PROP_0,
  PROP_TAB_WIDTH,
  PROP_TAB_HEIGHT,
};

static gpointer ags_notebook_parent_class = NULL;

GType
ags_notebook_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notebook = 0;

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

    ags_type_notebook = g_type_register_static(GTK_TYPE_BOX,
					       "AgsNotebook", &ags_notebook_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notebook);
  }

  return g_define_type_id__volatile;
}

void
ags_notebook_class_init(AgsNotebookClass *notebook)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_notebook_parent_class = g_type_class_peek_parent(notebook);

  /* GObjectClass */
  gobject = (GObjectClass *) notebook;

  gobject->set_property = ags_notebook_set_property;
  gobject->get_property = ags_notebook_get_property;

  gobject->finalize = ags_notebook_finalize;

  /* properties */
  /**
   * AgsNotebook:tab-width:
   *
   * The tab's width.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("tab-width",
				 "tab width",
				 "The tab's width",
				 0,
				 G_MAXUINT,
				 AGS_NOTEBOOK_TAB_DEFAULT_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TAB_WIDTH,
				  param_spec);

  /**
   * AgsNotebook:tab-height:
   *
   * The tab's height.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("tab-height",
				 "tab height",
				 "The tab's height",
				 0,
				 G_MAXUINT,
				 AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TAB_HEIGHT,
				  param_spec);
}

void
ags_notebook_init(AgsNotebook *notebook)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(notebook),
				 GTK_ORIENTATION_VERTICAL);
  
  notebook->tab_width = AGS_NOTEBOOK_TAB_DEFAULT_WIDTH;
  notebook->tab_height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  /* navigation */
  notebook->navigation = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(notebook->navigation),
				 GTK_ORIENTATION_HORIZONTAL);
  
  gtk_box_append(GTK_BOX(notebook),
		 GTK_WIDGET(notebook->navigation));

  /* arrow left */
  notebook->scroll_prev = g_object_new(GTK_TYPE_BUTTON,
				       "icon-name", "pan-start",
				       NULL);
  gtk_box_append(notebook->navigation,
		 GTK_WIDGET(notebook->scroll_prev));

  g_signal_connect(G_OBJECT(notebook->scroll_prev), "clicked",
		   G_CALLBACK(ags_notebook_scroll_prev_callback), notebook);

  /* arrow right */
  notebook->scroll_next = g_object_new(GTK_TYPE_BUTTON,
				       "icon-name", "pan-end",
				       NULL);
  gtk_box_append(notebook->navigation,
		 GTK_WIDGET(notebook->scroll_next));

  g_signal_connect(G_OBJECT(notebook->scroll_next), "clicked",
		   G_CALLBACK(ags_notebook_scroll_next_callback), notebook);

  /* scrolled window with selection */
  notebook->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_halign((GtkWidget *) notebook->scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) notebook->scrolled_window,
			GTK_ALIGN_FILL);
  
  gtk_widget_set_hexpand((GtkWidget *) notebook->scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) notebook->scrolled_window,
			 TRUE);
  
  gtk_scrolled_window_set_policy(notebook->scrolled_window,
				 GTK_POLICY_EXTERNAL,
				 GTK_POLICY_EXTERNAL);
  
  gtk_box_append(notebook->navigation,
		 GTK_WIDGET(notebook->scrolled_window));
  
  notebook->tab_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					     0);
  gtk_scrolled_window_set_child(notebook->scrolled_window,
				(GtkWidget *) notebook->tab_box);
  
  notebook->tab = NULL;
}

void
ags_notebook_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsNotebook *notebook;

  notebook = AGS_NOTEBOOK(gobject);

  switch(prop_id){
  case PROP_TAB_WIDTH:
    {
      GList *tab;
      
      guint tab_width;

      tab_width = g_value_get_uint(value);

      if(notebook->tab_width == tab_width){
	return;
      }

      tab = notebook->tab;

      while(tab != NULL){
	gtk_widget_set_size_request(tab->data,
				    tab_width, notebook->tab_height);
	
	tab = tab->next;
      }
      
      notebook->tab_width = tab_width;
    }
    break;
  case PROP_TAB_HEIGHT:
    {
      GList *tab;
      
      guint tab_height;

      tab_height = g_value_get_uint(value);

      if(notebook->tab_height == tab_height){
	return;
      }

      tab = notebook->tab;

      while(tab != NULL){
	gtk_widget_set_size_request(tab->data,
				    notebook->tab_width, tab_height);
	
	tab = tab->next;
      }
      
      notebook->tab_height = tab_height;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notebook_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsNotebook *notebook;

  notebook = AGS_NOTEBOOK(gobject);

  switch(prop_id){
  case PROP_TAB_WIDTH:
    {
      g_value_set_uint(value, notebook->tab_width);
    }
    break;
  case PROP_TAB_HEIGHT:
    {
      g_value_set_uint(value, notebook->tab_height);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notebook_finalize(GObject *gobject)
{
  AgsNotebook *notebook;
  
  notebook = AGS_NOTEBOOK(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_notebook_parent_class)->finalize(gobject);
}

void
ags_notebook_scroll_prev_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;
    
  adjustment = gtk_scrolled_window_get_hadjustment(notebook->scrolled_window);

  if(gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_step_increment(adjustment) > 0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_step_increment(adjustment));
  }else{
    gtk_adjustment_set_value(adjustment,
			     0.0);
  }

  gtk_widget_show((GtkWidget *) notebook->tab_box);
}

void
ags_notebook_scroll_next_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;
  
  adjustment = gtk_scrolled_window_get_hadjustment(notebook->scrolled_window);
  
  if(gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_step_increment(adjustment) < gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment)){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_step_increment(adjustment));
  }else{
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
  }

  gtk_widget_show((GtkWidget *) notebook->tab_box);
}

/**
 * ags_notebook_next_active_tab:
 * @notebook: the #AgsNotebook
 * @position: the position as integer
 * 
 * Get next active tab following @position.
 * 
 * Returns: the position of next active tab as integer
 * 
 * Since: 3.0.0
 */
gint
ags_notebook_next_active_tab(AgsNotebook *notebook,
			     gint position)
{
  GList *start_list, *list;

  gint retval;
  gint i;

  g_return_val_if_fail(AGS_IS_NOTEBOOK(notebook), -1);

  start_list = ags_notebook_get_tab(notebook);

  list = g_list_nth(start_list,
		    position);

  for(i = 0; list != NULL; i++){
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
      break;
    }

    /* iterate */
    list = list->next;
  }

  retval = -1;
  
  if(list != NULL){
    retval = position + i;  
  }
  
  g_list_free(start_list);
  
  return(retval);
}

/**
 * ags_notebook_get_tab:
 * @notebook: the #AgsNotebook
 * 
 * Get tab.
 * 
 * Returns: (element-type Gtk.ToggleButton) (transfer container): the #GList-struct containing #GtkToggleButton
 * 
 * Since: 4.0.0
 */
GList*
ags_notebook_get_tab(AgsNotebook *notebook)
{
  g_return_val_if_fail(AGS_IS_NOTEBOOK(notebook), NULL);

  return(g_list_reverse(g_list_copy(notebook->tab)));
}

/**
 * ags_notebook_add_tab:
 * @notebook: the #AgsNotebook
 * @tab: the #GtkToggleButton
 * 
 * Add @tab to @notebook.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_add_tab(AgsNotebook *notebook,
		     GtkToggleButton *tab)
{
  g_return_if_fail(AGS_IS_NOTEBOOK(notebook));
  g_return_if_fail(GTK_IS_TOGGLE_BUTTON(tab));

  /* new tab */
  notebook->tab = g_list_prepend(notebook->tab,
				 tab);
  
  gtk_widget_set_size_request((GtkWidget *) tab,
			      notebook->tab_width, notebook->tab_height);  

  gtk_box_append(notebook->tab_box,
		 GTK_WIDGET(tab));

  gtk_toggle_button_set_active(tab,
			       TRUE);

  gtk_widget_show((GtkWidget *) tab);
}

/**
 * ags_notebook_insert_tab:
 * @notebook: the #AgsNotebook
 * @tab: the #GtkToggleButton
 * @position: the position as integer
 * 
 * Insert a new #AgsNotebookTab-struct to @notebook.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_insert_tab(AgsNotebook *notebook,
			GtkToggleButton *tab,
			gint position)
{
  GtkToggleButton *sibling_tab;
  
  GList *start_list, *list;

  g_return_if_fail(AGS_IS_NOTEBOOK(notebook));
  g_return_if_fail(GTK_IS_TOGGLE_BUTTON(tab));

  /* insert tab */
  list = 
    start_list = ags_notebook_get_tab(notebook);

  sibling_tab = g_list_nth_data(start_list,
				position - 1);
  
  gtk_widget_set_size_request((GtkWidget *) tab,
			      notebook->tab_width, notebook->tab_height);  

  gtk_box_insert_child_after(notebook->tab_box,
			     GTK_WIDGET(tab),
			     GTK_WIDGET(sibling_tab));

  start_list = g_list_insert(start_list,
			     tab,
			     position);

  g_list_free(notebook->tab);

  notebook->tab = g_list_reverse(start_list);

  gtk_toggle_button_set_active(tab,
			       TRUE);
  
  gtk_widget_show((GtkWidget *) tab);
}

/**
 * ags_notebook_remove_tab:
 * @notebook: the #AgsNotebook
 * @tab: the #GtkToggleButton
 * 
 * Remove @tab from @notebook.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_remove_tab(AgsNotebook *notebook,
			GtkToggleButton *tab)
{
  g_return_if_fail(AGS_IS_NOTEBOOK(notebook));
  g_return_if_fail(GTK_IS_TOGGLE_BUTTON(tab));
  
  if(g_list_find(notebook->tab, tab) != NULL){    
    notebook->tab = g_list_remove(notebook->tab,
				  tab);
    
    gtk_box_remove(notebook->tab_box,
		   tab);
  }
}

/**
 * ags_notebook_new:
 *
 * Create a new #AgsNotebook.
 *
 * Returns: a new #AgsNotebook
 *
 * Since: 3.0.0
 */
AgsNotebook*
ags_notebook_new()
{
  AgsNotebook *notebook;

  notebook = (AgsNotebook *) g_object_new(AGS_TYPE_NOTEBOOK, NULL);

  return(notebook);
}
