/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_RULER_H__
#define __AGS_RULER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_RULER                (ags_ruler_get_type())
#define AGS_RULER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_RULER, AgsRuler))
#define AGS_RULER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RULER, AgsRulerClass))
#define AGS_IS_RULER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RULER))
#define AGS_IS_RULER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RULER))
#define AGS_RULER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RULER, AgsRulerClass))

#define AGS_RULER_FONT_SIZE (12)
#define AGS_RULER_FREE_SPACE (4.0)

#define AGS_RULER_DEFAULT_FACTOR (16.0)
#define AGS_RULER_DEFAULT_PRECISION (1.0)
#define AGS_RULER_DEFAULT_SCALE_PRECISION (1.0)

#define AGS_RULER_DEFAULT_HEIGHT (24)

#define AGS_RULER_DEFAULT_STEP (16)
#define AGS_RULER_DEFAULT_LARGE_STEP (8.0)
#define AGS_RULER_DEFAULT_SMALL_STEP (6.0)

typedef struct _AgsRuler AgsRuler;
typedef struct _AgsRulerClass AgsRulerClass;

struct _AgsRuler
{
  GtkWidget widget;

  GtkOrientation orientation;

  guint font_size;

  GtkAdjustment *adjustment;

  guint step;
  guint large_step;
  guint small_step;
  
  gdouble factor;
  gdouble precision;
  gdouble scale_precision;
};

struct _AgsRulerClass
{
  GtkWidgetClass widget;
};

GType ags_ruler_get_type();

/* properties */
guint ags_ruler_get_font_size(AgsRuler *ruler);
void ags_ruler_set_font_size(AgsRuler *ruler,
			     guint font_size);

guint ags_ruler_get_step(AgsRuler *ruler);
void ags_ruler_set_step(AgsRuler *ruler,
			guint step);

guint ags_ruler_get_large_step(AgsRuler *ruler);
void ags_ruler_set_large_step(AgsRuler *ruler,
			      guint large_step);

guint ags_ruler_get_small_step(AgsRuler *ruler);
void ags_ruler_set_small_step(AgsRuler *ruler,
			      guint small_step);

gdouble ags_ruler_get_factor(AgsRuler *ruler);
void ags_ruler_set_factor(AgsRuler *ruler,
			  gdouble factor);

gdouble ags_ruler_get_precision(AgsRuler *ruler);
void ags_ruler_set_precision(AgsRuler *ruler,
			     gdouble precision);

gdouble ags_ruler_get_scale_precision(AgsRuler *ruler);
void ags_ruler_set_scale_precision(AgsRuler *ruler,
				   gdouble scale_precision);

GtkAdjustment* ags_ruler_get_adjustment(AgsRuler *ruler);
void ags_ruler_set_adjustment(AgsRuler *ruler,
			      GtkAdjustment *adjustment);

/* instantiate */
AgsRuler* ags_ruler_new(GtkOrientation orientation,
			guint step,
			gdouble factor,
			gdouble precision,
			gdouble scale_precision);

G_END_DECLS

#endif /*__AGS_RULER_H__*/
