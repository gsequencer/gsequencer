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

#ifndef __AGS_SCALE_AREA_H__
#define __AGS_SCALE_AREA_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_SCALE_AREA                (ags_scale_area_get_type())
#define AGS_SCALE_AREA(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCALE_AREA, AgsScaleArea))
#define AGS_SCALE_AREA_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCALE_AREA, AgsScaleAreaClass))
#define AGS_IS_SCALE_AREA(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCALE_AREA))
#define AGS_IS_SCALE_AREA_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCALE_AREA))
#define AGS_SCALE_AREA_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCALE_AREA, AgsScaleAreaClass))

#define AGS_SCALE_AREA_DEFAULT_HEIGHT (128)

typedef struct _AgsScaleArea AgsScaleArea;
typedef struct _AgsScaleAreaClass AgsScaleAreaClass;

struct _AgsScaleArea
{
  GObject gobject;

  guint y;
  guint height;
  
  GtkDrawingArea *drawing_area;

  gchar *control_name;

  gdouble lower;
  gdouble upper;

  gdouble steps;  
};

struct _AgsScaleAreaClass
{
  GObjectClass *gobject;
};

GType ags_scale_area_get_type(void);

void ags_scale_area_paint(AgsScaleArea *scale_area);

AgsScaleArea* ags_scale_area_new(GtkDrawingArea *drawing_area);

#endif /*__AGS_SCALE_AREA_H__*/
