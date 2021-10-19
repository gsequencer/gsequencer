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

#include <ags/widget/ags_viewport.h>

#include <math.h>

void ags_viewport_class_init(AgsViewportClass *viewport);
void ags_viewport_init(AgsViewport *viewport);
void ags_viewport_finalize(GObject *gobject);

void ags_viewport_map(GtkWidget *widget);
void ags_viewport_realize(GtkWidget *widget);
void ags_viewport_size_allocate(GtkWidget *widget,
				GtkAllocation *allocation);
void ags_viewport_send_configure(AgsViewport *viewport);
void ags_viewport_get_preferred_width(GtkWidget *widget,
				      gint *minimal_width,
				      gint *natural_width);
void ags_viewport_get_preferred_height(GtkWidget *widget,
				       gint *minimal_height,
				       gint *natural_height);
void ags_viewport_show(GtkWidget *widget);
void ags_viewport_show_all(GtkWidget *widget);

void ags_viewport_configure_event(GtkWidget *widget,
				  GdkEventConfigure *event,
				  AgsViewport *viewport);
void ags_viewport_vadjustment_value_changed(GtkAdjustment *adjustment,
					    AgsViewport *viewport);

static gpointer ags_viewport_parent_class = NULL;

GType
ags_viewport_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_viewport;

    static const GTypeInfo ags_viewport_info = {
      sizeof(AgsViewportClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_viewport_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsViewport),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_viewport_init,
    };

    ags_type_viewport = g_type_register_static(GTK_TYPE_BIN,
					       "AgsViewport", &ags_viewport_info,
					       0);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_viewport);
  }

  return g_define_type_id__volatile;
}

void
ags_viewport_class_init(AgsViewportClass *viewport)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_viewport_parent_class = g_type_class_peek_parent(viewport);

  /* GObjectClass */
  gobject = (GObjectClass *) viewport;

  gobject->finalize = ags_viewport_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) viewport;

  widget->map = ags_viewport_map;
  widget->realize = ags_viewport_realize;
  widget->size_allocate = ags_viewport_size_allocate;
  widget->get_preferred_width = ags_viewport_get_preferred_width;
  widget->get_preferred_height = ags_viewport_get_preferred_height;
  widget->show = ags_viewport_show;
  widget->show_all = ags_viewport_show_all;
}

void
ags_viewport_init(AgsViewport *viewport)
{
  viewport->hadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.25, 0.25);
  viewport->vadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.25, 0.25);

  viewport->remaining_width = 0;
  viewport->remaining_height = 0;

  g_signal_connect(viewport, "configure_event",
		   G_CALLBACK(ags_viewport_configure_event), viewport);

  g_signal_connect(viewport->vadjustment, "value-changed",
		   G_CALLBACK(ags_viewport_vadjustment_value_changed), viewport);
}

void
ags_viewport_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_viewport_parent_class)->finalize(gobject);
}

void
ags_viewport_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized(widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_viewport_parent_class)->map(widget);
    
    gdk_window_show(gtk_widget_get_window(widget));
  }
}

void
ags_viewport_realize(GtkWidget *widget)
{
  AgsViewport *viewport;
  GtkWidget *child;
  
  GdkWindow *window;
  
  GtkAllocation allocation;
  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_VIEWPORT(widget));

  viewport = AGS_VIEWPORT(widget);
  
  child = gtk_bin_get_child((GtkBin *) widget);
  
  gtk_widget_set_realized(widget, TRUE);

  gtk_widget_get_allocation(widget,
			    &allocation);

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual(widget);
  attributes.event_mask = gtk_widget_get_events(widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
			    GDK_KEY_PRESS_MASK | 
			    GDK_KEY_RELEASE_MASK |
                            GDK_BUTTON1_MOTION_MASK |
                            GDK_BUTTON3_MOTION_MASK |
                            GDK_POINTER_MOTION_HINT_MASK |
                            GDK_POINTER_MOTION_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_LEAVE_NOTIFY_MASK);

  window = gdk_window_new(gtk_widget_get_parent_window(widget),
			  &attributes, attributes_mask);

  gtk_widget_register_window(widget, window);
  gtk_widget_set_window(widget, window);

  ags_viewport_send_configure(viewport);
}

void
ags_viewport_size_allocate(GtkWidget *widget,
			   GtkAllocation *allocation)
{
  AgsViewport *viewport;
  GtkWidget *parent;
  GtkWidget *child;

  GtkAllocation parent_allocation;
  GtkAllocation child_allocation;
  GtkAllocation clip_allocation;

  gint position_x, position_y;
  gint margin_top, margin_left;

  g_return_if_fail(AGS_IS_VIEWPORT(widget));
  g_return_if_fail(allocation != NULL);

  viewport = AGS_VIEWPORT(widget);

  parent = gtk_widget_get_parent(widget);

  gtk_widget_get_allocation(parent,
			    &parent_allocation);

  allocation->width = parent_allocation.width - (allocation->x - parent_allocation.x) - viewport->remaining_width;
  
  if(allocation->width < AGS_VIEWPORT_MINIMAL_WIDTH){
    allocation->width = AGS_VIEWPORT_MINIMAL_WIDTH;
  }
  
  allocation->height = parent_allocation.height - (allocation->y - parent_allocation.y) - viewport->remaining_height;
  
  if(allocation->height < AGS_VIEWPORT_MINIMAL_HEIGHT){
    allocation->height = AGS_VIEWPORT_MINIMAL_HEIGHT;
  }

  gtk_widget_set_allocation(widget, allocation);
  
  child = gtk_bin_get_child((GtkBin *) widget);

  position_x = 0.0;
  position_y = 0.0;
  
  margin_top = gtk_widget_get_margin_top(child);
  margin_left = gtk_widget_get_margin_left(child);
  
  child_allocation.x = 0;
  child_allocation.y = 0;
  child_allocation.width = allocation->width;
  child_allocation.height = allocation->height;
  
  if(child != NULL){
    gdouble value_x;
    gdouble value_y;
    gint width, height;
    gint min_width, min_height;

    gtk_widget_get_preferred_width(child,
				   &min_width,
				   &width);
    
    gtk_widget_get_preferred_height(child,
				    &min_height,
				    &height);

    if(width > 0 && width > min_width){
      child_allocation.width = width;
    }else{
      child_allocation.width = min_width;
    }

    if(height > 0 && height > min_height){
      child_allocation.height = height;
    }else{
      child_allocation.height = min_height;
    }

    if(child_allocation.width < allocation->width){
      child_allocation.width = allocation->width;
    }

    if(child_allocation.height < allocation->height){
      child_allocation.height = allocation->height;
    }

    position_x = 0;
    position_y = 0;

    if(child_allocation.width > allocation->width){
      value_x = gtk_adjustment_get_value(viewport->hadjustment);

      position_x = (gint) floor(value_x);
    }

    if(child_allocation.height > allocation->height){
      value_y = gtk_adjustment_get_value(viewport->vadjustment);

      position_y = (gint) floor(value_y);
    }
    
    //g_message("++> %d %d", child_allocation.width, child_allocation.height);
    
    gtk_widget_size_allocate(child,
			     &child_allocation);
    
    //    gtk_widget_get_allocation(child,
    //			      &child_allocation);
    
    // g_message("--> %d %d", child_allocation.x, child_allocation.y);
  }

  clip_allocation.x = position_x;
  clip_allocation.y = position_y;
  clip_allocation.width = allocation->width;
  clip_allocation.height = allocation->height;
  
  gtk_widget_set_clip(widget, &clip_allocation);
  
  if(gtk_widget_get_realized(widget)){
    gdk_window_move_resize(gtk_widget_get_window(widget),
			   allocation->x - position_x, allocation->y - position_y,
			   child_allocation.width, child_allocation.height);
    
    ags_viewport_send_configure(viewport);
  }
}

void
ags_viewport_send_configure(AgsViewport *viewport)
{
  GtkAllocation allocation;
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new(GDK_CONFIGURE);

  widget = GTK_WIDGET(viewport);
  gtk_widget_get_allocation(widget, &allocation);

  event->configure.window = g_object_ref(gtk_widget_get_window (widget));
  event->configure.send_event = TRUE;
  event->configure.x = allocation.x;
  event->configure.y = allocation.y;
  event->configure.width = allocation.width;
  event->configure.height = allocation.height;

  gtk_widget_event(widget, event);
  gdk_event_free(event);
}

void
ags_viewport_get_preferred_width(GtkWidget *widget,
				 gint *minimal_width,
				 gint *natural_width)
{
  GtkAllocation allocation;
  
  gtk_widget_get_allocation(widget, &allocation);
  
  if(minimal_width != NULL){
    minimal_width[0] = AGS_VIEWPORT_MINIMAL_WIDTH;
  }

  if(natural_width != NULL){
    if(allocation.width > AGS_VIEWPORT_MINIMAL_WIDTH){
      natural_width[0] = allocation.width;
    }else{
      natural_width[0] = AGS_VIEWPORT_MINIMAL_WIDTH;
    }
  }
}

void
ags_viewport_get_preferred_height(GtkWidget *widget,
				  gint *minimal_height,
				  gint *natural_height)
{
  GtkWidget *parent;
  
  GtkAllocation allocation;

  parent = gtk_widget_get_parent(widget);
  
  gtk_widget_get_allocation(widget, &allocation);
  
  if(minimal_height != NULL){
    minimal_height[0] = AGS_VIEWPORT_MINIMAL_HEIGHT;
  }

  if(natural_height != NULL){
    if(allocation.height > AGS_VIEWPORT_MINIMAL_HEIGHT){
      natural_height[0] = allocation.height;
    }else{
      natural_height[0] = AGS_VIEWPORT_MINIMAL_HEIGHT;
    }
  }
}

void
ags_viewport_show(GtkWidget *widget)
{
  AgsViewport *viewport;
  GtkWidget *child;

  viewport = (AgsViewport *) widget;
  
  GTK_WIDGET_CLASS(ags_viewport_parent_class)->show(widget);

  child = gtk_bin_get_child(widget);
  
  gtk_widget_queue_draw(widget);
  gtk_widget_queue_resize(widget);

  gtk_widget_show(child);
  
  gtk_widget_queue_draw(child);
  gtk_widget_queue_resize(child);
}

void
ags_viewport_show_all(GtkWidget *widget)
{
  AgsViewport *viewport;
  GtkWidget *child;
  
  viewport = (AgsViewport *) widget;
  
  GTK_WIDGET_CLASS(ags_viewport_parent_class)->show_all(widget);
  
  child = gtk_bin_get_child(widget);
  
  gtk_widget_queue_draw(widget);
  gtk_widget_queue_resize(widget);

  gtk_widget_show_all(child);
  
  gtk_widget_queue_draw(child);
  gtk_widget_queue_resize(child);
}

void
ags_viewport_configure_event(GtkWidget *widget,
			     GdkEventConfigure *event,
			     AgsViewport *viewport)
{
  GtkWidget *child;
  
  GtkAllocation allocation;   
  GtkAllocation child_allocation;

  gint width, height;
  gint min_width, min_height;
  
  child = gtk_bin_get_child(widget);

  gtk_widget_get_preferred_width(child,
				 &min_width,
				 &width);
    
  gtk_widget_get_preferred_height(child,
				  &min_height,
				  &height);
  
  gtk_widget_get_allocation(widget,
			    &allocation);

  /* vadjustment */
  gtk_adjustment_set_step_increment(viewport->vadjustment,
				    1.0);
  gtk_adjustment_set_page_increment(viewport->vadjustment,
				    1.0);

  if(min_height > allocation.height){
    gtk_adjustment_set_upper(viewport->vadjustment,
			     (gdouble) min_height - (gdouble) allocation.height);
  }else{
    gtk_adjustment_set_upper(viewport->vadjustment,
			     1.0);
  }
  
  gtk_adjustment_changed(viewport->vadjustment);
  
  /* hadjustment */
  gtk_adjustment_set_step_increment(viewport->hadjustment,
				    1.0);
  gtk_adjustment_set_page_increment(viewport->hadjustment,
				    1.0);

  if(min_width > allocation.width){
    gtk_adjustment_set_upper(viewport->hadjustment,
			     (gdouble) min_width - (gdouble) allocation.width);
  }else{
    gtk_adjustment_set_upper(viewport->hadjustment,
			     1.0);
  }
  
  gtk_adjustment_changed(viewport->hadjustment);
}

void
ags_viewport_vadjustment_value_changed(GtkAdjustment *adjustment,
				       AgsViewport *viewport)
{
  GtkAllocation allocation;
  
  gtk_widget_get_allocation(viewport, &allocation);
  
  ags_viewport_size_allocate(viewport,
			     &allocation);
}

/**
 * ags_viewport_new:
 * 
 * Create a new instance of #AgsViewport
 *
 * Returns: the new #AgsViewport
 * 
 * Since: 3.12.1
 */
AgsViewport*
ags_viewport_new()
{
  AgsViewport *viewport;

  viewport = (AgsViewport *) g_object_new(AGS_TYPE_VIEWPORT,
					  NULL);
  
  return(viewport);
}
