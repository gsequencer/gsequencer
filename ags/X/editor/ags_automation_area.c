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

#include <ags/X/editor/ags_automation_area.h>
#include <ags/X/editor/ags_automation_area_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_automation.h>

#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_edit.h>

#include <math.h>

void ags_automation_area_class_init(AgsAutomationAreaClass *automation_area);
void ags_automation_area_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_area_init(AgsAutomationArea *automation_area);
void ags_automation_area_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_automation_area_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_automation_area_connect(AgsConnectable *connectable);
void ags_automation_area_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_area
 * @short_description: area automations
 * @title: AgsAutomationArea
 * @section_id:
 * @include: ags/X/areaor/ags_automation_area.h
 *
 * The #AgsAutomationArea lets you paint automations.
 */

enum{
  PROP_0,
  PROP_AUTOMATION,
};

static gpointer ags_automation_area_parent_class = NULL;

GType
ags_automation_area_get_type(void)
{
  static GType ags_type_automation_area = 0;

  if(!ags_type_automation_area){
    static const GTypeInfo ags_automation_area_info = {
      sizeof (AgsAutomationAreaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_area_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationArea),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_area_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_area_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_area = g_type_register_static(G_TYPE_OBJECT,
						      "AgsAutomationArea\0", &ags_automation_area_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_area,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_area);
}

void
ags_automation_area_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_area_connect;
  connectable->disconnect = ags_automation_area_disconnect;
}

void
ags_automation_area_class_init(AgsAutomationAreaClass *automation_area)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_automation_area_parent_class = g_type_class_peek_parent(automation_area);

  gobject = (GObjectClass *) automation_area;

  gobject->set_property = ags_automation_area_set_property;
  gobject->get_property = ags_automation_area_get_property;

  /* properties */
  /**
   * AgsAutomationArea:automation:
   *
   * The assigned #AgsAutomation
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("automation\0",
				   "automation of automation area\0",
				   "The automation of automation area\0",
				   AGS_TYPE_AUTOMATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);
}

void
ags_automation_area_init(AgsAutomationArea *automation_area)
{
  automation_area->flags = 0;
  
  automation_area->y = 0;
  automation_area->height = AGS_AUTOMATION_AREA_DEFAULT_HEIGHT;

  automation_area->drawing_area = NULL;

  automation_area->audio = NULL;
  automation_area->channel_type = G_TYPE_NONE;
  automation_area->control_name = NULL;
}

void
ags_automation_area_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutomationArea *automation_area;

  automation_area = AGS_AUTOMATION_AREA(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_area_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutomationArea *automation_area;

  automation_area = AGS_AUTOMATION_AREA(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_area_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_automation_area_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

GList*
ags_automation_area_find_specifier(GList *automation_area,
				   gchar *specifier)
{
  while(automation_area != NULL){
    if(!g_ascii_strcasecmp(AGS_AUTOMATION_AREA(automation_area->data)->control_name,
			   specifier)){
      break;
    }
    
    automation_area = automation_area->next;
  }

  return(automation_area);
}

/**
 * ags_automation_area_draw_strip:
 * @automation_area: the #AgsAutomationArea
 * @cr: the #cairo_t surface
 * @x_offset: x position given by #GtkHScrollbar
 * @y_offset: y position given by #GtkVScrollbar
 *
 * Plot data.
 *
 * Since: 0.4.3
 */
void
ags_automation_area_draw_strip(AgsAutomationArea *automation_area,
			       cairo_t *cr,
			       gdouble x_offset, gdouble y_offset)
{
  gdouble y;
  gdouble width, height;

  y = (gdouble) automation_area->y - y_offset;
  
  width = (gdouble) GTK_WIDGET(automation_area->drawing_area)->allocation.width;
  height = (gdouble) automation_area->height;

  /* background */
  cairo_set_source_rgb(cr, 0.5, 0.4, 0.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_stroke(cr);

  /*  */
  //TODO:JK: implement me
}

/**
 * ags_automation_area_draw_segment:
 * @automation_area: the #AgsAutomationArea
 * @cr: the #cairo_t surface
 * @x_offset: x position given by #GtkHScrollbar
 * @y_offset: y position given by #GtkVScrollbar
 *
 * Draws horizontal and vertical lines.
 *
 * Since: 0.4.3
 */
void
ags_automation_area_draw_segment(AgsAutomationArea *automation_area,
				 cairo_t *cr,
				 gdouble x_offset, gdouble y_offset)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;
  GtkWidget *widget;
  
  double tact;
  gdouble y;
  gdouble height;
  guint control_width;
  guint i, j;
  guint j_set;
  
  widget = (GtkWidget *) automation_area->drawing_area;

  automation_edit = (AgsAutomationEdit *) gtk_widget_get_ancestor(GTK_WIDGET(automation_area->drawing_area),
								  AGS_TYPE_AUTOMATION_EDIT);
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  cairo_set_line_width(cr, 1.0);

  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 2.0);

  y = (gdouble) automation_area->y - y_offset;
  
  height = (gdouble) automation_area->height;

  control_width = 64;
  i = control_width - (guint) x_offset % control_width;
  
  if(i < widget->allocation.width &&
     tact > 1.0 ){
    j_set = ((guint) x_offset / control_width + 1) % ((guint) tact);

    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

    if(j_set != 0){
      j = j_set;
      goto ags_automation_area_draw_segment0;
    }
  }

  for(; i < widget->allocation.width; ){
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    
    cairo_move_to(cr, (double) i, y);
    cairo_line_to(cr, (double) i, y + height);
    cairo_stroke(cr);
    
    i += control_width;
    
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    
    for(j = 1; i < widget->allocation.width && j < tact; j++){
    ags_automation_area_draw_segment0:
      cairo_move_to(cr, (double) i, y);
      cairo_line_to(cr, (double) i, y + height);
      cairo_stroke(cr);
      
      i += control_width;
    }
  }
}

/**
 * ags_automation_area_draw_scale:
 * @automation_area: the #AgsAutomationArea
 * @cr: the #cairo_t surface
 * @x_offset: x position given by #GtkHScrollbar
 * @y_offset: y position given by #GtkVScrollbar
 *
 * Draw a scale and its boundaries.
 *
 * Since: 0.4.3
 */
void
ags_automation_area_draw_scale(AgsAutomationArea *automation_area,
			       cairo_t *cr,
			       gdouble x_offset, gdouble y_offset)
{
  gdouble y;
  gdouble width, height;
  gdouble translated_ground;
  
  const static double dashes = {
    0.25,
  };

  y = (gdouble) automation_area->y - y_offset;
  
  width = (gdouble) GTK_WIDGET(automation_area->drawing_area)->allocation.width;
  height = (gdouble) automation_area->height;

  cairo_save(cr);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

  /* middle */
  cairo_move_to(cr,
		0.0, y + height * 0.5);
  cairo_line_to(cr,
		width, y + height * 0.5);
  cairo_stroke(cr);

  /* set dash */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_set_dash(cr,
		 &dashes,
		 1,
		 0.0);

  /* lower quarter */
  cairo_move_to(cr,
		0.0, y + height * 0.25);
  cairo_line_to(cr,
		width, y + height * 0.25);
  cairo_stroke(cr);

  /* upper quarter */
  cairo_move_to(cr,
		0.0, y + height * 0.75);
  cairo_line_to(cr,
		width, y + height * 0.75);
  cairo_stroke(cr);

  cairo_restore(cr);
}

/**
 * ags_automation_area_draw_automation:
 * @automation_area: a #AgsAutomationArea
 * @cr: the #cairo_t surface
 * @x_offset: x position given by #GtkHScrollbar
 * @y_offset: y position given by #GtkVScrollbar
 *
 * Draw the #AgsAutomation of selected #AgsMachine on @automation_edit.
 *
 * Since: 0.4.3
 */
void
ags_automation_area_draw_automation(AgsAutomationArea *automation_area,
				    cairo_t *cr,
				    gdouble x_offset, gdouble y_offset)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;
  AgsNotebook *notebook;    

  AgsAcceleration *current, *prev;

  GList *automation;
  GList *list;

  GType channel_type;

  guint line;
  guint width;
  gdouble x0, x1;
  gdouble val;
  guint n_tab;
  gdouble *tab_x, *tab_y;
  gdouble *prev_tab_x, *prev_tab_y;
  gdouble upper, lower, range;
  gdouble c_upper, c_lower, c_range;
  double tact_factor, zoom_factor;
  double tact;
  
  auto gboolean ags_automation_area_draw_automation_find_tab(gdouble x, gdouble y, gdouble prev_x, gdouble prev_y);

  gboolean ags_automation_area_draw_automation_find_tab(gdouble x, gdouble y, gdouble prev_x, gdouble prev_y){
    guint i;
    
    for(i = 0; i < n_tab; i++){
      if(tab_x[i] == x &&
	 tab_y[i] == y &&
	 prev_tab_x[i] == prev_x &&
	 prev_tab_y[i] == prev_y){
	return(TRUE);
      }
    }
    
    return(FALSE);
  }
  
  automation_editor = gtk_widget_get_ancestor(automation_area->drawing_area,
					      AGS_TYPE_AUTOMATION_EDITOR);
  automation_edit = gtk_widget_get_ancestor(automation_area->drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  if(automation_edit == automation_editor->current_audio_automation_edit){
    notebook = NULL;
    channel_type = G_TYPE_NONE;
  }else if(automation_edit == automation_editor->current_output_automation_edit){
    notebook = automation_editor->current_output_notebook;
    channel_type = AGS_TYPE_OUTPUT;
  }else if(automation_edit == automation_editor->current_input_automation_edit){
    notebook = automation_editor->current_input_notebook;
    channel_type = AGS_TYPE_INPUT;
  }

  zoom_factor = 1.0 / 4.0;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 2.0);

  /*  */
  width = GTK_WIDGET(automation_area->drawing_area)->allocation.width;

  x0 = x_offset;
  x1 = x0 + width;

  n_tab = 0;
  prev_tab_x = NULL;
  prev_tab_y = NULL;
  tab_x = NULL;
  tab_y = NULL;

  /* match specifier */
  if(channel_type == G_TYPE_NONE){
    automation = automation_area->audio->automation;

    while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									 automation_area->control_name,
									 channel_type,
									 0)) != NULL){
      upper = AGS_AUTOMATION(automation->data)->upper;
      lower = AGS_AUTOMATION(automation->data)->lower;
	
      range = upper - lower;

      if(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion != NULL){
	c_upper = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					 upper,
					 FALSE);
	c_lower = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					 lower,
					 FALSE);
	c_range = c_upper - c_lower;
      }else{
	c_upper = upper;
	c_lower = lower;

	c_range = range;
      }

      if(c_range == 0.0){
	automation = automation->next;
	g_warning("ags_automation_area.c - c_range = 0.0\0");
	
	continue;
      }

      /*  */
      list = AGS_AUTOMATION(automation->data)->acceleration;
      prev = NULL;

      while(list != NULL){
	current = AGS_ACCELERATION(list->data);

	if(tact * current->x < x0){
	  prev = current;
	  list = list->next;

	  continue;
	}
    
	if(prev != NULL){
	  gdouble x, y;
	  gdouble prev_x, prev_y;

	  x = current->x;

	  val = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
				       current->y,
				       FALSE);
	  y = (automation_area->height / (1.0 / AGS_AUTOMATION(automation->data)->steps)) * ((val / AGS_AUTOMATION(automation->data)->steps) - (c_lower / AGS_AUTOMATION(automation->data)->steps));
	    
	  prev_x = prev->x;
	    
	  val = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
				       prev->y,
				       FALSE);
	  prev_y = (automation_area->height / (1.0 / AGS_AUTOMATION(automation->data)->steps)) * ((val / AGS_AUTOMATION(automation->data)->steps) - (c_lower / AGS_AUTOMATION(automation->data)->steps));

	  if(!ags_automation_area_draw_automation_find_tab(x, y, prev_x, prev_y)){
	    if(n_tab == 0){
	      tab_x = (gdouble *) malloc(sizeof(gdouble));
	      tab_y = (gdouble *) malloc(sizeof(gdouble));
	      
	      prev_tab_x = (gdouble *) malloc(sizeof(gdouble));
	      prev_tab_y = (gdouble *) malloc(sizeof(gdouble));
	    }else{
	      tab_x = (gdouble *) realloc(tab_x,
					  (n_tab + 1) * sizeof(gdouble));
	      tab_y = (gdouble *) realloc(tab_y,
					  (n_tab + 1) * sizeof(gdouble));
	      
	      prev_tab_x = (gdouble *) realloc(prev_tab_x,
					       (n_tab + 1) * sizeof(gdouble));
	      prev_tab_y = (gdouble *) realloc(prev_tab_y,
					       (n_tab + 1) * sizeof(gdouble));
	    }

	    tab_x[n_tab] = x;
	    tab_y[n_tab] = y;
	    prev_tab_x[n_tab] = prev_x;
	    prev_tab_y[n_tab] = prev_y;
	    
	    ags_automation_area_draw_surface(automation_area, cr,
					     x_offset, y_offset,
					     prev_x, prev_y,
					     x, y,
					     AGS_AUTOMATION(automation->data)->steps);

	    n_tab++;
	  }
	}

	if(current->x >= x1){
	  break;
	}
    
	prev = current;
	list = list->next;
      }
    }
  }else{
    line = 0;
    
    while((line = ags_notebook_next_active_tab(notebook,
					       line)) != -1){
      automation = automation_area->audio->automation;

      while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									   automation_area->control_name,
									   channel_type,
									   line)) != NULL){
	upper = AGS_AUTOMATION(automation->data)->upper;
	lower = AGS_AUTOMATION(automation->data)->lower;
	
	range = upper - lower;

	if(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion != NULL){
	  c_upper = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					   upper,
					   FALSE);
	  c_lower = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					   lower,
					   FALSE);
	  c_range = c_upper - c_lower;
	}else{
	  c_upper = upper;
	  c_lower = lower;

	  c_range = range;
	}
	  
	if(c_range == 0.0){
	  automation = automation->next;
	  g_warning("ags_automation_area.c - c_range = 0.0\0");
	
	  continue;
	}

	/*  */
	list = AGS_AUTOMATION(automation->data)->acceleration;
	prev = NULL;

	while(list != NULL){
	  current = AGS_ACCELERATION(list->data);

	  if(tact * current->x < x0){
	    prev = current;
	    list = list->next;

	    continue;
	  }
    
	  if(prev != NULL){
	    gdouble x, y;
	    gdouble prev_x, prev_y;

	    x = current->x;

	    val = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					 current->y,
					 FALSE);
	    if(val - c_lower != 0.0){
	      y = (automation_area->height) * ((1.0 / c_range) / (1.0 / (val - c_lower)));
	    }else{
	      y = 0.0;
	    }
	    
	    prev_x = prev->x;
	    
	    val = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(automation->data)->port)->conversion,
					 prev->y,
					 FALSE);
	    if(val - c_lower != 0.0){
	      prev_y = (automation_area->height) * ((1.0 / c_range) / (1.0 / (val - c_lower)));
	    }else{
	      prev_y = 0.0;
	    }

	    if(!ags_automation_area_draw_automation_find_tab(x, y, prev_x, prev_y)){
	      if(n_tab == 0){
		tab_x = (gdouble *) malloc(sizeof(gdouble));
		tab_y = (gdouble *) malloc(sizeof(gdouble));
	      
		prev_tab_x = (gdouble *) malloc(sizeof(gdouble));
		prev_tab_y = (gdouble *) malloc(sizeof(gdouble));
	      }else{
		tab_x = (gdouble *) realloc(tab_x,
					    (n_tab + 1) * sizeof(gdouble));
		tab_y = (gdouble *) realloc(tab_y,
					    (n_tab + 1) * sizeof(gdouble));
	      
		prev_tab_x = (gdouble *) realloc(prev_tab_x,
						 (n_tab + 1) * sizeof(gdouble));
		prev_tab_y = (gdouble *) realloc(prev_tab_y,
						 (n_tab + 1) * sizeof(gdouble));
	      }

	      tab_x[n_tab] = x;
	      tab_y[n_tab] = y;
	      prev_tab_x[n_tab] = prev_x;
	      prev_tab_y[n_tab] = prev_y;
	      
	      ags_automation_area_draw_surface(automation_area, cr,
					       x_offset, y_offset,
					       prev_x, prev_y,
					       x, y,
					       AGS_AUTOMATION(automation->data)->steps);

	      n_tab++;
	    }
	  }

	  if(current->x >= x1){
	    break;
	  }
    
	  prev = current;
	  list = list->next;
	}
    
	automation = automation->next;
      }

      line++;
    }
  }

  if(n_tab != 0){
    free(tab_x);
    free(tab_y);
    free(prev_tab_x);
    free(prev_tab_y);
  }
}

/**
 * ags_automation_area_draw_surface:
 * @automation_area: the #AgsAutomationArea
 * @cr: the #cairo_t surface
 * @x_offset: x position given by #GtkHScrollbar
 * @y_offset: y position given by #GtkVScrollbar
 * @x0: x offset
 * @y0: y offset
 * @x1: x offset
 * @y1: y offset
 * @steps: precision range
 *
 * Draw a portion of data.
 *
 * Since: 0.4.3
 */
void
ags_automation_area_draw_surface(AgsAutomationArea *automation_area, cairo_t *cr,
				 gdouble x_offset, gdouble y_offset,
				 gdouble x0, gdouble y0,
				 gdouble x1, gdouble y1,
				 guint steps)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;

  GList *list;

  double tact_factor, zoom_factor;
  double tact;
  gdouble width, height;
  gdouble pos_x, pos_y;
  
  automation_editor = gtk_widget_get_ancestor(automation_area->drawing_area,
					      AGS_TYPE_AUTOMATION_EDITOR);
  automation_edit = gtk_widget_get_ancestor(automation_area->drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);
  
  zoom_factor = 1.0 / 4.0;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 2.0);

  width = (gdouble) GTK_WIDGET(automation_area->drawing_area)->allocation.width;
  height = (gdouble) automation_area->height;

  /* find x */
  pos_x = x_offset;
  
  /* find y */
  pos_y = automation_area->y - y_offset;

  /* clip x */
  if(x0 < 0.0){
    x0 = 0.0;
  }

  if(x1 < 0.0){
    x1 = 0.0;
  }
  
  /* clip y */
  if(y0 < 0.0){
    y0 = 0.0;
  }

  if(y1 < 0.0){
    y1 = 0.0;
  }

  if(y0 > automation_area->height){
    y0 = automation_area->height;
  }

  if(y1 > automation_area->height){
    y1 = automation_area->height;
  }
  
  /*  */
  cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 0.2);

  /* area */
  cairo_rectangle(cr,
		  (tact * x0) - pos_x, pos_y + automation_area->height - y0,
		  (tact * x1) - (tact * x0), y0);
  cairo_arc(cr,
	    (tact * x0) - pos_x, pos_y + automation_area->height - y0,
	    1.2,
	    0.0,
	    2.0 * M_PI);

  cairo_fill(cr);
}

void
ags_automation_area_paint(AgsAutomationArea *automation_area,
			  cairo_t *cr,
			  gdouble x_offset, gdouble y_offset)
{
  if(GTK_WIDGET_VISIBLE(automation_area->drawing_area)){    
    if(x_offset < 0.0){
      x_offset = 0.0;
    }
    
    if(y_offset < 0.0){
      y_offset = 0.0;
    }

    ags_automation_area_draw_strip(automation_area,
				   cr,
				   x_offset, y_offset);
    ags_automation_area_draw_segment(automation_area,
				     cr,
				     x_offset, y_offset);
    ags_automation_area_draw_scale(automation_area,
				   cr,
				   x_offset, y_offset);
    
    ags_automation_area_draw_automation(automation_area,
					cr,
					x_offset, y_offset);
  }
}

/**
 * ags_automation_area_new:
 * @drawing_area: the assigned drawing area
 * @audio: the assigned #AgsAudio
 * @channel_type: the channel type
 * @control_name: the control_name
 *
 * Create a new #AgsAutomationArea.
 *
 * Since: 0.4.3
 */
AgsAutomationArea*
ags_automation_area_new(GtkDrawingArea *drawing_area,
			AgsAudio *audio,
			GType channel_type,
			gchar *control_name)
{
  AgsAutomationArea *automation_area;

  automation_area = (AgsAutomationArea *) g_object_new(AGS_TYPE_AUTOMATION_AREA,
						       NULL);
  
  automation_area->drawing_area = drawing_area;
  automation_area->audio = audio;
  automation_area->channel_type = channel_type;
  automation_area->control_name = control_name;
  
  return(automation_area);
}
