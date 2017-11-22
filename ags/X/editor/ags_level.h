/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_LEVEL_H__
#define __AGS_LEVEL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <cairo.h>

#define AGS_TYPE_LEVEL                (ags_level_get_type())
#define AGS_LEVEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LEVEL, AgsLevel))
#define AGS_LEVEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LEVEL, AgsLevelClass))
#define AGS_IS_LEVEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LEVEL))
#define AGS_IS_LEVEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LEVEL))
#define AGS_LEVEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_LEVEL, AgsLevelClass))

typedef struct _AgsLevel AgsLevel;
typedef struct _AgsLevelClass AgsLevelClass;

struct _AgsLevel
{
  GtkDrawingArea drawing_area;
};

struct _AgsLevelClass
{
  GtkDrawingAreaClass drawing_area;
};

GType ags_level_get_type(void);

void ags_level_paint(AgsLevel *level,
		     cairo_t *cr);

AgsLevel* ags_level_new();

#endif /*__AGS_LEVEL_H__*/
