/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_RULER_H__
#define __AGS_RULER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_RULER                (ags_ruler_get_type())
#define AGS_RULER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_RULER, AgsRuler))
#define AGS_RULER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RULER, AgsRulerClass))
#define AGS_IS_RULER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RULER))
#define AGS_IS_RULER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RULER))
#define AGS_RULER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RULER, AgsRulerClass))

#define AGS_RULER_DEFAULT_STEP (16)
#define AGS_RULER_LARGE_STEP (8.0)
#define AGS_RULER_SMALL_STEP (4.0)

typedef struct _AgsRuler AgsRuler;
typedef struct _AgsRulerClass AgsRulerClass;

struct _AgsRuler
{
  GtkWidget widget;

  guint flags;

  guint font_size;

  GtkAdjustment *adjustment;

  guint step;

  gdouble precision;
  gdouble scale_precision;
};

struct _AgsRulerClass
{
  GtkWidgetClass widget;
};

GType ags_ruler_get_type();

AgsRuler* ags_ruler_new();

#endif /*__AGS_RULER_H__*/
