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
#ifndef __AGS_SCALE_H__
#define __AGS_SCALE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_scale_area.h>

#define AGS_TYPE_SCALE                (ags_scale_get_type())
#define AGS_SCALE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCALE, AgsScale))
#define AGS_SCALE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCALE, AgsScaleClass))
#define AGS_IS_SCALE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCALE))
#define AGS_IS_SCALE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCALE))
#define AGS_SCALE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCALE, AgsScaleClass))

#define AGS_SCALE_MARGIN_TOP (24)
#define AGS_SCALE_DEFAULT_HEIGHT (128)
#define AGS_SCALE_DEFAULT_MARGIN (8)

typedef struct _AgsScale AgsScale;
typedef struct _AgsScaleClass AgsScaleClass;

struct _AgsScale
{
  GtkDrawingArea drawing_area;

  GList *scale_area;
};

struct _AgsScaleClass
{
  GtkDrawingAreaClass drawing_area;
};

GType ags_scale_get_type(void);

void ags_scale_paint(AgsScale *scale);

void ags_scale_add_area(AgsScale *scale,
			AgsScaleArea *scale_area);
void ags_scale_remove_area(AgsScale *scale,
			   AgsScaleArea *scale_area);

AgsScale* ags_scale_new();

#endif /*__AGS_SCALE_H__*/
