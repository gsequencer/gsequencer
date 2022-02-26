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

void ags_notebook_get_preferred_width(GtkWidget *widget,
				      gint *minimal_width,
				      gint *natural_width);
void ags_notebook_get_preferred_height(GtkWidget *widget,
				       gint *minimal_height,
				       gint *natural_height);
void ags_notebook_size_allocate(AgsNotebook *notebook,
				GtkAllocation *allocation);

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
  PROP_PREFIX,
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
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_notebook_parent_class = g_type_class_peek_parent(notebook);

  /* GObjectClass */
  gobject = (GObjectClass *) notebook;

  gobject->set_property = ags_notebook_set_property;
  gobject->get_property = ags_notebook_get_property;

  gobject->finalize = ags_notebook_finalize;

  /**
   * AgsNotebook:prefix:
   *
   * The prefix used to do enumerated labels.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("prefix",
				   "enumeration prefix",
				   "The label's enumeration prefix",
				   AGS_NOTEBOOK_TAB_DEFAULT_PREFIX,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PREFIX,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) notebook;

  widget->get_preferred_width = ags_notebook_get_preferred_width;
  widget->get_preferred_height = ags_notebook_get_preferred_height;
  widget->size_allocate = ags_notebook_size_allocate;
}

void
ags_notebook_init(AgsNotebook *notebook)
{
  GtkArrow *arrow;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(notebook),
				 GTK_ORIENTATION_VERTICAL);
  
  notebook->flags = 0;

  notebook->tab_width = AGS_NOTEBOOK_TAB_DEFAULT_WIDTH;
  notebook->tab_height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  
  notebook->prefix = g_strdup(AGS_NOTEBOOK_TAB_DEFAULT_PREFIX);

  /* navigation */
  notebook->navigation = (GtkHBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						 0);
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(notebook->navigation),
				 GTK_ORIENTATION_HORIZONTAL);
  
  gtk_box_append(GTK_BOX(notebook),
		 GTK_WIDGET(notebook->navigation));

  /* arrow left */
  arrow = (GtkArrow *) gtk_arrow_new(GTK_ARROW_LEFT,
				     GTK_SHADOW_NONE);
  notebook->scroll_prev = g_object_new(GTK_TYPE_BUTTON,
				       "child", arrow,
				       "relief", GTK_RELIEF_NONE,
				       NULL);
  gtk_box_append(GTK_BOX(notebook->navigation),
		 GTK_WIDGET(notebook->scroll_prev));

  g_signal_connect(G_OBJECT(notebook->scroll_prev), "clicked",
		   G_CALLBACK(ags_notebook_scroll_prev_callback), notebook);

  /* arrow right */
  arrow = (GtkArrow *) gtk_arrow_new(GTK_ARROW_RIGHT,
				     GTK_SHADOW_NONE);
  notebook->scroll_next = g_object_new(GTK_TYPE_BUTTON,
				       "child", arrow,
				       "relief", GTK_RELIEF_NONE,
				       NULL);
  gtk_box_append(GTK_BOX(notebook->navigation),
		 GTK_WIDGET(notebook->scroll_next));

  g_signal_connect(G_OBJECT(notebook->scroll_next), "clicked",
		   G_CALLBACK(ags_notebook_scroll_next_callback), notebook);

  /* viewport with selection */
  notebook->viewport = (GtkViewport *) gtk_viewport_new(NULL,
							NULL);
  gtk_container_add(GTK_CONTAINER(notebook->navigation),
		    GTK_WIDGET(notebook->viewport));
  
  notebook->hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  0);
  gtk_container_add((GtkContainer *) notebook->viewport,
		    (GtkWidget *) notebook->hbox);
  
  notebook->tab = NULL;
  notebook->tab_free_func = ags_notebook_tab_free;
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
  case PROP_PREFIX:
    {
      gchar *prefix;

      prefix = g_value_get_string(value);

      if(notebook->prefix == prefix){
	return;
      }

      g_free(notebook->prefix);
      
      notebook->prefix = g_strdup(prefix);
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
  case PROP_PREFIX:
    {
      g_value_set_string(value, notebook->prefix);
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

  g_free(notebook->prefix);
  
  if(notebook->tab_free_func != NULL){
    g_list_free_full(notebook->tab,
		     (GDestroyNotify) notebook->tab_free_func);
  }else{
    g_list_free_full(notebook->tab,
		     g_free);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_notebook_parent_class)->finalize(gobject);
}

void
ags_notebook_get_preferred_width(GtkWidget *widget,
				 gint *minimal_width,
				 gint *natural_width)
{
  minimal_width =
    natural_width = NULL;
}

void
ags_notebook_get_preferred_height(GtkWidget *widget,
				  gint *minimal_height,
				  gint *natural_height)
{
  minimal_height[0] =
    natural_height[0] = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
}

void
ags_notebook_size_allocate(AgsNotebook *notebook,
			   GtkAllocation *allocation)
{
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;

  GList *list, *list_start;

  guint x;

  if(allocation->width < (2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT) + (5 * AGS_NOTEBOOK_TAB_DEFAULT_WIDTH)){
    allocation->width = (2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT) + (5 * AGS_NOTEBOOK_TAB_DEFAULT_WIDTH);
  }

  allocation->height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  GTK_WIDGET_CLASS(ags_notebook_parent_class)->size_allocate(notebook,
							     allocation);

  /*  */
  gtk_widget_get_child_requisition((GtkWidget *) notebook->navigation,
				   &child_requisition);

  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;

  child_allocation.width = 2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  gtk_widget_size_allocate((GtkWidget *) notebook->navigation,
			   &child_allocation);
  
  /*  */
  gtk_widget_get_child_requisition((GtkWidget *) notebook->scroll_prev,
				   &child_requisition);

  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;

  child_allocation.width = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  gtk_widget_size_allocate((GtkWidget *) notebook->scroll_prev,
			   &child_allocation);

  gtk_widget_get_child_requisition(gtk_bin_get_child((GtkBin *) notebook->scroll_prev),
				   &child_requisition);

  child_allocation.x = allocation->x + 4;
  child_allocation.y = allocation->y + 4;

  child_allocation.width = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT - 8;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT - 8;

  gtk_widget_size_allocate(gtk_bin_get_child((GtkBin *) notebook->scroll_prev),
			   &child_allocation);
  
  /*  */
  gtk_widget_get_child_requisition((GtkWidget *) notebook->scroll_next,
				   &child_requisition);

  child_allocation.x = allocation->x + AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.y = allocation->y;

  child_allocation.width = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  gtk_widget_size_allocate((GtkWidget *) notebook->scroll_next,
			   &child_allocation);

  gtk_widget_get_child_requisition(gtk_bin_get_child((GtkBin *) notebook->scroll_next),
				   &child_requisition);

  child_allocation.x = allocation->x + AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT + 4;
  child_allocation.y = allocation->y + 4;

  child_allocation.width = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT - 8;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT - 8;

  gtk_widget_size_allocate(gtk_bin_get_child((GtkBin *) notebook->scroll_next),
			   &child_allocation);

  
  /*  */
  gtk_widget_get_child_requisition((GtkWidget *) notebook->viewport,
				   &child_requisition);

  child_allocation.x = allocation->x + 2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.y = allocation->y;

  child_allocation.width = allocation->width - 2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  gtk_widget_size_allocate((GtkWidget *) notebook->viewport,
			   &child_allocation);

  /*  */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) notebook->hbox);

  /*  */
  gtk_widget_get_child_requisition((GtkWidget *) notebook->hbox,
				   &child_requisition);

  child_allocation.x = 0;
  child_allocation.y = 0;

  child_allocation.width = g_list_length(list) * AGS_NOTEBOOK_TAB_DEFAULT_WIDTH;

  if(child_allocation.width < allocation->width - 2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT){
    child_allocation.width = allocation->width - 2 * AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;
  }
  
  child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

  gtk_widget_size_allocate((GtkWidget *) notebook->hbox,
			   &child_allocation);

  x = 0;
  
  while(list != NULL){
    gtk_widget_get_child_requisition((GtkWidget *) list->data,
				     &child_requisition);

    child_allocation.x = x;
    child_allocation.y = 0;

    child_allocation.width = AGS_NOTEBOOK_TAB_DEFAULT_WIDTH;
    child_allocation.height = AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT;

    gtk_widget_size_allocate(list->data,
			     &child_allocation);

    x += AGS_NOTEBOOK_TAB_DEFAULT_WIDTH;
    list = list->next;
  }

  g_list_free(list_start);
  //  gtk_widget_size_allocate(notebook->hbox);
}

void
ags_notebook_scroll_prev_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;
    
  adjustment = gtk_viewport_get_hadjustment(notebook->viewport);

  if(gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_step_increment(adjustment) > 0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_step_increment(adjustment));
  }else{
    gtk_adjustment_set_value(adjustment,
			     0.0);
  }

  gtk_widget_show_all((GtkWidget *) notebook->hbox);
}

void
ags_notebook_scroll_next_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;
  
  adjustment = gtk_viewport_get_hadjustment(notebook->viewport);
  
  if(gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_step_increment(adjustment) < gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment)){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_step_increment(adjustment));
  }else{
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
  }

  gtk_widget_show_all((GtkWidget *) notebook->hbox);
}

/**
 * ags_notebook_tab_alloc:
 * 
 * Allocate #AgsNotebookTab-struct.
 * 
 * Returns: (type gpointer) (transfer full): the newly allocated #AgsNotebookTab-struct
 * 
 * Since: 3.0.0
 */
AgsNotebookTab*
ags_notebook_tab_alloc()
{
  AgsNotebookTab *notebook_tab;

  notebook_tab = (AgsNotebookTab *) g_malloc(sizeof(AgsNotebookTab));

  notebook_tab->data = NULL;
  notebook_tab->toggle = NULL;

  return(notebook_tab);
}

/**
 * ags_notebook_tab_free:
 * @tab: (type gpointer) (transfer full): the #AgsNotebookTab-struct
 * 
 * Free @tab's memory.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_tab_free(AgsNotebookTab *tab)
{
  if(tab == NULL){
    return;
  }

  g_free(tab->data);

  g_free(tab);
}

/**
 * ags_notebook_tab_set_data:
 * @notebook: the #AgsNotebook
 * @position: the tab's position
 * @data: the data to set
 * 
 * Set the data field of #AgsNotebookTab-struct at @position.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_tab_set_data(AgsNotebook *notebook,
			  gint position,
			  gpointer data)
{
  GList *tab;
  
  if(!AGS_IS_NOTEBOOK(notebook) ||
     position < 0){
    return;
  }

  tab = g_list_nth(notebook->tab,
		   position);

  if(tab != NULL &&
     tab->data != NULL){
    AGS_NOTEBOOK_TAB(tab->data)->data = data;
  }
}

/**
 * ags_notebook_tab_index:
 * @notebook: the #AgsNotebook
 * @data: the assigned data
 * 
 * Retrieve tab index assigned with @data.
 * 
 * Returns: the position as integer
 * 
 * Since: 3.0.0
 */
gint
ags_notebook_tab_index(AgsNotebook *notebook,
		       gpointer data)
{
  GList *list;
  gint i;

  if(!AGS_IS_NOTEBOOK(notebook)){
    return(-1);
  }

  list = notebook->tab;

  for(i = g_list_length(notebook->tab) - 1; list != NULL; i--){
    if(AGS_NOTEBOOK_TAB(list->data)->data == data){
      return(i);
    }

    list = list->next;
  }
  
  return(-1);
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
  GList *list, *list_start;
  gint i;

  if(!AGS_IS_NOTEBOOK(notebook)){
    return(-1);
  }
  
  list_start = g_list_copy(notebook->tab);
  list_start = 
    list = g_list_reverse(list_start);

  list = g_list_nth(list,
		    position);

  for(i = 0; list != NULL;){
    if(gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
      g_list_free(list_start);
      return(position + i);
    }

    /* iterate */
    list = list->next;

    i++;
  }

  g_list_free(list_start);
  
  return(-1);
}

/**
 * ags_notebook_add_tab:
 * @notebook: the #AgsNotebook
 * 
 * Add a new #AgsNotebookTab-struct to @notebook.
 * 
 * Returns: the position as integer
 * 
 * Since: 3.0.0
 */
gint
ags_notebook_add_tab(AgsNotebook *notebook)
{
  AgsNotebookTab *tab;
  GtkViewport *viewport;

  GtkAdjustment *adjustment;

  gchar *str;
  
  gint tab_index;
  gint length;
  guint width;

  if(!AGS_IS_NOTEBOOK(notebook)){
    return(-1);
  }

  /* new tab */
  tab = ags_notebook_tab_alloc();

  notebook->tab = g_list_prepend(notebook->tab,
				 tab);
  tab_index = g_list_length(notebook->tab);

  str = NULL;

  if((AGS_NOTEBOOK_TAB_PREFIXED_LABEL & (notebook->flags)) != 0 &&
     (AGS_NOTEBOOK_TAB_ENUMERATE & (notebook->flags)) != 0){
    str = g_strdup_printf("%s %d",
			  notebook->prefix,
			  tab_index);
  }else if((AGS_NOTEBOOK_TAB_PREFIXED_LABEL & (notebook->flags)) != 0){
    str = g_strdup_printf("%s",
			  notebook->prefix);
  }else if((AGS_NOTEBOOK_TAB_ENUMERATE & (notebook->flags)) != 0){
    str = g_strdup_printf("%d",
			  tab_index);
  }
  
  tab->toggle = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						 "label", str,
						 "xalign", 0.0,
						 "yalign", 0.0,
						 "active", TRUE,
						 NULL);
  gtk_widget_set_size_request((GtkWidget *) tab->toggle,
			      AGS_NOTEBOOK_TAB_DEFAULT_WIDTH, AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT);  
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all((GtkWidget *) notebook->hbox);

  g_free(str);
  
  return(tab_index);
}

/**
 * ags_notebook_add_tab_with_label:
 * @notebook: the #AgsNotebook
 * @label: the label
 * 
 * Add a new #AgsNotebookTab-struct to @notebook and set specified @label.
 * 
 * Returns: the position as integer
 * 
 * Since: 3.0.0
 */
gint
ags_notebook_add_tab_with_label(AgsNotebook *notebook,
				gchar *label)
{
  GList *tab;
  
  gint position;
  
  if(!AGS_IS_NOTEBOOK(notebook)){
    return(-1);
  }

  position = ags_notebook_add_tab(notebook);

  tab = g_list_nth(notebook->tab,
		   position);

  if(tab != NULL){
    g_object_set(AGS_NOTEBOOK_TAB(tab->data)->toggle,
		 "label", label,
		 NULL);
  }
  
  return(position);
}

/**
 * ags_notebook_insert_tab:
 * @notebook: the #AgsNotebook
 * @position: the position as integer
 * 
 * Insert a new #AgsNotebookTab-struct to @notebook.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_insert_tab(AgsNotebook *notebook,
			gint position)
{
  AgsNotebookTab *tab;
  
  GtkAdjustment *adjustment;
  
  gint length;
  guint width;

  if(!AGS_IS_NOTEBOOK(notebook)){
    return;
  }

  /* insert tab */
  length = g_list_length(notebook->tab);

  tab = ags_notebook_tab_alloc();
  notebook->tab = g_list_insert(notebook->tab,
				tab,
				length - position);

  tab->toggle = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%s %d",
										     notebook->prefix,
										     position + 1));
  g_object_set(tab->toggle,
	       "xalign", 0.0,
	       "yalign", 0.0,
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) tab->toggle,
			      AGS_NOTEBOOK_TAB_DEFAULT_WIDTH, AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT);
  gtk_box_pack_start(GTK_BOX(notebook->hbox),
		     GTK_WIDGET(tab->toggle),
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(notebook->hbox),
			GTK_WIDGET(tab->toggle),
			position);

  gtk_widget_show_all((GtkWidget *) notebook->hbox);
}

/**
 * ags_notebook_insert_tab_with_label:
 * @notebook: the #AgsNotebook
 * @label: the label as string
 * @position: the position as integer
 * 
 * Insert a new #AgsNotebookTab-struct to @notebook at @position and set specified @label.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_insert_tab_with_label(AgsNotebook *notebook,
				   gchar *label,
				   gint position)
{
  GList *tab;

  if(!AGS_IS_NOTEBOOK(notebook)){
    return;
  }

  ags_notebook_insert_tab(notebook,
			  position);
  
  tab = g_list_nth(notebook->tab,
		   position);

  if(tab != NULL){
    g_object_set(AGS_NOTEBOOK_TAB(tab->data)->toggle,
		 "label", label,
		 NULL);
  }
}

/**
 * ags_notebook_remove_tab:
 * @notebook: the #AgsNotebook
 * @position: the position of the tab
 * 
 * Remove #AgsNotebookTab at @position.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_remove_tab(AgsNotebook *notebook,
			gint position)
{
  AgsNotebookTab *tab;

  gint length;

  if(!AGS_IS_NOTEBOOK(notebook) ||
     notebook->tab == NULL ||
     position < 0){
    return;
  }
  
  length = g_list_length(notebook->tab);

  tab = g_list_nth_data(notebook->tab,
			length - position - 1);

  if(tab != NULL){
    notebook->tab = g_list_remove(notebook->tab,
				  tab);
    gtk_widget_destroy(GTK_WIDGET(tab->toggle));

    if(notebook->tab_free_func != NULL){
      notebook->tab_free_func(tab);
    }
  }
}

/**
 * ags_notebook_remove_tab_with_data:
 * @notebook: the #AgsNotebook
 * @data: the data to lookup
 * 
 * Remove #AgsNotebookTab assigned with @data.
 * 
 * Since: 3.0.0
 */
void
ags_notebook_remove_tab_with_data(AgsNotebook *notebook,
				  gpointer data)
{
  gint position;
  
  if(!AGS_IS_NOTEBOOK(notebook)){
    return;
  }

  position = ags_notebook_tab_index(notebook,
				    data);
  ags_notebook_remove_tab(notebook,
			  position);
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
