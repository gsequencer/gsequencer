/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/editor/ags_scrolled_wave_edit_box.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_wave_edit.h>

void ags_scrolled_wave_edit_box_class_init(AgsScrolledWaveEditBoxClass *scrolled_wave_edit_box);
void ags_scrolled_wave_edit_box_init(AgsScrolledWaveEditBox *scrolled_wave_edit_box);
void ags_scrolled_wave_edit_box_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_scrolled_wave_edit_box_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_scrolled_wave_edit_box_finalize(GObject *gobject);

void ags_scrolled_wave_edit_box_size_allocate(GtkWidget *widget,
					      GtkAllocation *allocation);
void ags_scrolled_wave_edit_box_get_preferred_width(GtkWidget *widget,
						    gint *minimal_width,
						    gint *natural_width);
void ags_scrolled_wave_edit_box_get_preferred_height(GtkWidget *widget,
						     gint *minimal_height,
						     gint *natural_height);

gboolean ags_scrolled_wave_edit_box_configure_event(GtkWidget *widget, GdkEventConfigure *event,
						    AgsScrolledWaveEditBox *scrolled_wave_edit_box);

/**
 * SECTION:ags_scrolled_wave_edit_box
 * @short_description: scrolled wave_edit box widget
 * @title: AgsScrolledWaveEditBox
 * @section_id:
 * @include: ags/widget/ags_scrolled_wave_edit_box.h
 *
 * The #AgsScrolledWaveEditBox lets you to have a scrolled wave_edit box widget.
 */

enum{
  PROP_0,
  PROP_MARGIN_TOP,
  PROP_MARGIN_BOTTOM,
  PROP_MARGIN_LEFT,
  PROP_MARGIN_RIGHT,
};

static gpointer ags_scrolled_wave_edit_box_parent_class = NULL;

GType
ags_scrolled_wave_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_wave_edit_box = 0;

    static const GTypeInfo ags_scrolled_wave_edit_box_info = {
      sizeof (AgsScrolledWaveEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_wave_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledWaveEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_wave_edit_box_init,
    };

    ags_type_scrolled_wave_edit_box = g_type_register_static(GTK_TYPE_BIN,
							     "AgsScrolledWaveEditBox", &ags_scrolled_wave_edit_box_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_wave_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_wave_edit_box_class_init(AgsScrolledWaveEditBoxClass *scrolled_wave_edit_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_scrolled_wave_edit_box_parent_class = g_type_class_peek_parent(scrolled_wave_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_wave_edit_box;

  gobject->set_property = ags_scrolled_wave_edit_box_set_property;
  gobject->get_property = ags_scrolled_wave_edit_box_get_property;

  gobject->finalize = ags_scrolled_wave_edit_box_finalize;

  /* properties */  
  /**
   * AgsScrolledWaveEditBox:margin-top:
   *
   * The margin top.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("margin-top",
				 "margin top",
				 "The margin top",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_TOP,
				  param_spec);

  /**
   * AgsScrolledWaveEditBox:margin-bottom:
   *
   * The margin bottom.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("margin-bottom",
				 "margin bottom",
				 "The margin bottom",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_BOTTOM,
				  param_spec);

  /**
   * AgsScrolledWaveEditBox:margin-left:
   *
   * The margin left.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("margin-left",
				 "margin left",
				 "The margin left",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_LEFT,
				  param_spec);

  /**
   * AgsScrolledWaveEditBox:margin-right:
   *
   * The margin right.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("margin-right",
				 "margin right",
				 "The margin right",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_RIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) scrolled_wave_edit_box;

  widget->size_allocate = ags_scrolled_wave_edit_box_size_allocate;
  widget->get_preferred_width = ags_scrolled_wave_edit_box_get_preferred_width;
  widget->get_preferred_height = ags_scrolled_wave_edit_box_get_preferred_height;
}

void
ags_scrolled_wave_edit_box_init(AgsScrolledWaveEditBox *scrolled_wave_edit_box)
{
  scrolled_wave_edit_box->margin_top = 0;
  scrolled_wave_edit_box->margin_bottom = 0;
  scrolled_wave_edit_box->margin_left = 0;
  scrolled_wave_edit_box->margin_right = 0;

  /* viewport */
  scrolled_wave_edit_box->viewport = ags_viewport_new();

  gtk_widget_set_vexpand(scrolled_wave_edit_box->viewport, TRUE);
  gtk_widget_set_hexpand(scrolled_wave_edit_box->viewport, TRUE);
   
  gtk_container_add((GtkContainer *) scrolled_wave_edit_box,
		    (GtkWidget *) scrolled_wave_edit_box->viewport);
  
  /* wave_edit box */
  scrolled_wave_edit_box->wave_edit_box = NULL;
  
#if 0
  scrolled_wave_edit_box->wave_edit_box = ags_vwave_edit_box_new();
  gtk_container_add(scrolled_wave_edit_box->viewport,
		    scrolled_wave_edit_box->wave_edit_box);
#endif

  gtk_widget_set_events((GtkWidget *) scrolled_wave_edit_box,
			GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);

  g_signal_connect_after((GObject *) scrolled_wave_edit_box, "configure_event",
			 G_CALLBACK(ags_scrolled_wave_edit_box_configure_event), (gpointer) scrolled_wave_edit_box);
}

void
ags_scrolled_wave_edit_box_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsScrolledWaveEditBox *scrolled_wave_edit_box;

  scrolled_wave_edit_box = AGS_SCROLLED_WAVE_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      scrolled_wave_edit_box->margin_top = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      scrolled_wave_edit_box->margin_bottom = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      scrolled_wave_edit_box->margin_left = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      scrolled_wave_edit_box->margin_right = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_wave_edit_box_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsScrolledWaveEditBox *scrolled_wave_edit_box;

  scrolled_wave_edit_box = AGS_SCROLLED_WAVE_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      g_value_set_uint(value,
		       scrolled_wave_edit_box->margin_top);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      g_value_set_uint(value,
		       scrolled_wave_edit_box->margin_bottom);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      g_value_set_uint(value,
		       scrolled_wave_edit_box->margin_left);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      g_value_set_uint(value,
		       scrolled_wave_edit_box->margin_right);
    }
    break;  
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_wave_edit_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_wave_edit_box_parent_class)->finalize(gobject);
}

void
ags_scrolled_wave_edit_box_size_allocate(GtkWidget *widget,
					 GtkAllocation *allocation)
{
  AgsScrolledWaveEditBox *scrolled_wave_edit_box;

  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  
  scrolled_wave_edit_box = AGS_SCROLLED_WAVE_EDIT_BOX(widget);
  
  //GTK_WIDGET_CLASS(ags_scrolled_wave_edit_box_parent_class)->size_allocate(widget, allocation);
  
  gtk_widget_set_allocation(widget,
			    allocation);
  
  gtk_widget_set_allocation(scrolled_wave_edit_box->viewport,
			    allocation);

  /* viewport allocation */
  gtk_widget_get_child_requisition((GtkWidget *) scrolled_wave_edit_box->viewport,
				   &child_requisition);

  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;

  child_allocation.width = allocation->width;
  child_allocation.height = allocation->height;

  gtk_widget_size_allocate((GtkWidget *) scrolled_wave_edit_box->viewport,
			   &child_allocation);
  
  /* box */
  if(scrolled_wave_edit_box->wave_edit_box != NULL){
    gtk_widget_get_child_requisition((GtkWidget *) scrolled_wave_edit_box->wave_edit_box,
				     &child_requisition);

    child_allocation.x = 0;
    child_allocation.y = 0;

    child_allocation.width = allocation->width;
    child_allocation.height = allocation->height;
  
    gtk_widget_size_allocate((GtkWidget *) scrolled_wave_edit_box->wave_edit_box,
			     &child_allocation);
  }
}

void
ags_scrolled_wave_edit_box_get_preferred_width(GtkWidget *widget,
					       gint *minimal_width,
					       gint *natural_width)
{
  minimal_width[0] =
    natural_width[0] = AGS_LEVEL_DEFAULT_LEVEL_WIDTH;
}

void
ags_scrolled_wave_edit_box_get_preferred_height(GtkWidget *widget,
						gint *minimal_height,
						gint *natural_height)
{
  minimal_height[0] =
    natural_height[0] = 1;
}

gboolean
ags_scrolled_wave_edit_box_configure_event(GtkWidget *widget, GdkEventConfigure *event,
					   AgsScrolledWaveEditBox *scrolled_wave_edit_box)
{
  AgsCompositeEdit *composite_edit;
  GtkAdjustment *vadjustment, *hadjustment;
 
  GList *start_list, *list;
  
  gdouble vscrollbar_value, hscrollbar_value;
  gdouble vnew_upper, vold_upper;
  gdouble hnew_upper, hold_upper;

  composite_edit = gtk_widget_get_ancestor((GtkWidget *) scrolled_wave_edit_box,
					   AGS_TYPE_COMPOSITE_EDIT);
  
  vadjustment = gtk_range_get_adjustment(composite_edit->vscrollbar);
  hadjustment = gtk_range_get_adjustment(composite_edit->hscrollbar);
  
  vscrollbar_value = gtk_adjustment_get_value(vadjustment);
  hscrollbar_value = gtk_adjustment_get_value(hadjustment);
  
  vold_upper = gtk_adjustment_get_upper(vadjustment);
  hold_upper = gtk_adjustment_get_upper(hadjustment);
  
  list = 
    start_list = gtk_container_get_children((GtkContainer *) gtk_bin_get_child(scrolled_wave_edit_box->viewport));

  while(list != NULL){
    ags_wave_edit_reset_vscrollbar(AGS_WAVE_EDIT(list->data));
    ags_wave_edit_reset_hscrollbar(AGS_WAVE_EDIT(list->data));

    list = list->next;
  }
  
  vnew_upper = gtk_adjustment_get_upper(vadjustment);
  hnew_upper = gtk_adjustment_get_upper(hadjustment);
  
  gtk_range_set_value((GtkRange *) composite_edit->vscrollbar,
		      vscrollbar_value * (1.0 / (vold_upper / vnew_upper)));
  
  gtk_range_set_value((GtkRange *) composite_edit->hscrollbar,
		      hscrollbar_value * (1.0 / (hold_upper / hnew_upper)));
  
  g_list_free(start_list);
  
  return(FALSE);
}

/**
 * ags_scrolled_wave_edit_box_new:
 *
 * Create a new #AgsScrolledWaveEditBox.
 *
 * Returns: a new #AgsScrolledWaveEditBox
 *
 * Since: 3.0.0
 */
AgsScrolledWaveEditBox*
ags_scrolled_wave_edit_box_new()
{
  AgsScrolledWaveEditBox *scrolled_wave_edit_box;

  scrolled_wave_edit_box = (AgsScrolledWaveEditBox *) g_object_new(AGS_TYPE_SCROLLED_WAVE_EDIT_BOX,
								   NULL);

  return(scrolled_wave_edit_box);
}