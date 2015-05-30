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

void
ags_scale_area_paint(AgsScaleArea *scale_area)
{
  //TODO:JK: implement me
}

/**
 * ags_scale_area_new:
 *
 * Create a new #AgsScaleArea.
 *
 * Since: 0.4.3
 */
AgsScaleArea*
ags_scale_area_new(GtkDrawingArea *drawing_area)
{
  AgsScaleArea *scale_area;

  scale_area = (AgsScaleArea *) g_object_new(AGS_TYPE_SCALE_AREA, NULL);
  scale_area->drawing_area = drawing_area;
  
  return(scale_area);
}
