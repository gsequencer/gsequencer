/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/editor/ags_scale_area.h>
#include <ags/X/editor/ags_scale_area_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <math.h>

void ags_scale_area_class_init(AgsScaleAreaClass *scale_area);
void ags_scale_area_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_scale_area_init(AgsScaleArea *scale_area);
void ags_scale_area_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_scale_area_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_scale_area_connect(AgsConnectable *connectable);
void ags_scale_area_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_scale_area
 * @short_description: area scales
 * @title: AgsScaleArea
 * @section_id:
 * @include: ags/X/areaor/ags_scale_area.h
 *
 * The #AgsScaleArea lets you paint scales.
 */

enum{
  PROP_0,
};

static gpointer ags_scale_area_parent_class = NULL;

GType
ags_scale_area_get_type(void)
{
  static GType ags_type_scale_area = 0;

  if(!ags_type_scale_area){
    static const GTypeInfo ags_scale_area_info = {
      sizeof (AgsScaleAreaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scale_area_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScaleArea),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scale_area_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_scale_area_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_scale_area = g_type_register_static(G_TYPE_OBJECT,
						      "AgsScaleArea\0", &ags_scale_area_info,
						      0);
    
    g_type_add_interface_static(ags_type_scale_area,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_scale_area);
}

void
ags_scale_area_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_scale_area_connect;
  connectable->disconnect = ags_scale_area_disconnect;
}

void
ags_scale_area_class_init(AgsScaleAreaClass *scale_area)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_scale_area_parent_class = g_type_class_peek_parent(scale_area);

  gobject = (GObjectClass *) scale_area;

  gobject->set_property = ags_scale_area_set_property;
  gobject->get_property = ags_scale_area_get_property;

  /* properties */
}

void
ags_scale_area_init(AgsScaleArea *scale_area)
{
  scale_area->y = 0;
  scale_area->height = AGS_SCALE_AREA_DEFAULT_HEIGHT;

  scale_area->font_size = 11;
  
  scale_area->drawing_area = NULL;

  scale_area->control_name = NULL;

  scale_area->lower = 0.0;
  scale_area->upper = 1.0;

  scale_area->steps = 1;
}

void
ags_scale_area_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsScaleArea *scale_area;

  scale_area = AGS_SCALE_AREA(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_area_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsScaleArea *scale_area;

  scale_area = AGS_SCALE_AREA(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_area_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_scale_area_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

GList*
ags_scale_area_find_specifier(GList *scale_area,
			      gchar *specifier)
{
  while(scale_area != NULL){
    if(!g_ascii_strcasecmp(AGS_SCALE_AREA(scale_area->data)->control_name,
			   specifier)){
      break;
    }
    
    scale_area = scale_area->next;
  }

  return(scale_area);
}

void
ags_scale_area_paint(AgsScaleArea *scale_area,
		     cairo_t *cr)
{
  gdouble y;
  gdouble width, height;

  y = (gdouble) scale_area->y;
  
  width = (gdouble) GTK_WIDGET(scale_area->drawing_area)->allocation.width;
  height = (gdouble) scale_area->height;

  cairo_save(cr);
  
  cairo_select_font_face(cr, "Georgia\0",
			 CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, (gdouble) scale_area->font_size);

  /* background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.5, 0.4, 0.0);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_stroke(cr);

  /* draw scale */
  //TODO:JK: implement me

  /* show control name */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_move_to(cr, scale_area->font_size, y + height - 1.0);
  cairo_rotate(cr,
	       2 * M_PI * 0.75);
  cairo_show_text(cr, scale_area->control_name);

  cairo_restore(cr);
}

/**
 * ags_scale_area_new:
 * @drawing_area: 
 * @control_name: 
 * @lower: 
 * @upper: 
 * @steps: 
 *
 * Create a new #AgsScaleArea.
 *
 * Since: 0.4.3
 */
AgsScaleArea*
ags_scale_area_new(GtkDrawingArea *drawing_area,
		   gchar *control_name,
		   gdouble lower,
		   gdouble upper,
		   gdouble steps)
{
  AgsScaleArea *scale_area;

  scale_area = (AgsScaleArea *) g_object_new(AGS_TYPE_SCALE_AREA,
					     NULL);
  
  scale_area->drawing_area = drawing_area;
  scale_area->control_name = control_name;
  scale_area->lower = lower;
  scale_area->upper = upper;
  scale_area->steps = steps;
  
  return(scale_area);
}
