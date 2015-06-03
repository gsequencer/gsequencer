/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/editor/ags_automation_area.h>
#include <ags/X/editor/ags_automation_area_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_automation.h>

#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_edit.h>

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
 * Since: 0.4
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

  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 4.0);

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
    cairo_line_to(cr, (double) i, height);
    cairo_stroke(cr);
    
    i += control_width;
    
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    
    for(j = 1; i < widget->allocation.width && j < tact; j++){
    ags_automation_area_draw_segment0:
      cairo_move_to(cr, (double) i, y);
      cairo_line_to(cr, (double) i, height);
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
 * Since: 0.4
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
 * Since: 0.4
 */
void
ags_automation_area_draw_automation(AgsAutomationArea *automation_area,
				    cairo_t *cr,
				    gdouble x_offset, gdouble y_offset)
{
  AgsAcceleration *current, *prev;

  GList *automation;

  guint width;
  gdouble x0, x1;
  GList *list;
  
  width = GTK_WIDGET(automation_area->drawing_area)->allocation.width;

  x0 = x_offset;
  x1 = x0 + width;

  automation = automation_area->audio->automation;

  while((automation = ags_automation_find_specifier(automation,
						    automation_area->control_name)) != NULL){
    if(AGS_AUTOMATION(automation->data)->channel_type == automation_area->channel_type){
      /*  */
      list = AGS_ACCELERATION(AGS_AUTOMATION(automation->data)->acceleration);
      prev = NULL;

      while(list != NULL){
	current = AGS_ACCELERATION(list->data);

	if(current->x < x0){
	  prev = current;
	  list = list->next;

	  continue;
	}
    
	if(prev != NULL){
	  ags_automation_area_draw_surface(automation_area, cr,
					   x_offset, y_offset,
					   prev->x, prev->y,
					   current->x, current->y);
	}

	if(current->x >= x1){
	  break;
	}
    
	prev = current;
	list = list->next;
      }
    }
    
    automation = automation->next;
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
 *
 * Draw a portion of data.
 *
 * Since: 0.4
 */
void
ags_automation_area_draw_surface(AgsAutomationArea *automation_area, cairo_t *cr,
				 gdouble x_offset, gdouble y_offset,
				 gdouble x0, gdouble y0,
				 gdouble x1, gdouble y1)
{
  gdouble width, height;

  width = (gdouble) GTK_WIDGET(automation_area->drawing_area)->allocation.width;
  height = (gdouble) automation_area->height;

  y0 += automation_area->y - y_offset;
  y1 += automation_area->y - y_offset;
  
  cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);

  /* area */
  cairo_rectangle(cr, x0, 0.0, x1 - x0, ((y0 < y1) ? y0: y1));
  cairo_fill(cr);

  /* acceleration */
  cairo_move_to(cr,
		x0, y0);
  cairo_line_to(cr,
		x1, y1);

  if(y0 > y1){
    cairo_line_to(cr,
		  x0, y0);
  }else{
    cairo_line_to(cr,
		  x1, y1);
  }

  cairo_close_path(cr);
  cairo_fill(cr);
}

void
ags_automation_area_paint(AgsAutomationArea *automation_area,
			  cairo_t *cr,
			  gdouble x_offset, gdouble y_offset)
{
  if(GTK_WIDGET_VISIBLE(automation_area->drawing_area)){    
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
 * @drawing_area: 
 * @audio: 
 * @channel_type:
 * @control_name: 
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
