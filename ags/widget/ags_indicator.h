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

#ifndef __AGS_INDICATOR_H__
#define __AGS_INDICATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_INDICATOR                (ags_indicator_get_type())
#define AGS_INDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INDICATOR, AgsIndicator))
#define AGS_INDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INDICATOR, AgsIndicatorClass))
#define AGS_IS_INDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INDICATOR))
#define AGS_IS_INDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INDICATOR))
#define AGS_INDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INDICATOR, AgsIndicatorClass))

#define AGS_INDICATOR_DEFAULT_SEGMENT_WIDTH (7)
#define AGS_INDICATOR_DEFAULT_SEGMENT_HEIGHT (7)
#define AGS_INDICATOR_DEFAULT_SEGMENT_PADDING (3)
#define AGS_INDICATOR_DEFAULT_SEGMENT_COUNT (10)

typedef struct _AgsIndicator AgsIndicator;
typedef struct _AgsIndicatorClass AgsIndicatorClass;

struct _AgsIndicator
{
  GtkWidget widget;

  GtkOrientation orientation;
  
  guint segment_width;
  guint segment_height;

  guint segment_padding;
  
  guint segment_count;
  
  GtkAdjustment *adjustment;
};

struct _AgsIndicatorClass
{
  GtkWidgetClass widget;
};

GType ags_indicator_get_type(void);

/* properties */
guint ags_indicator_get_segment_width(AgsIndicator *indicator);
void ags_indicator_set_segment_width(AgsIndicator *indicator,
				     guint segment_width);

guint ags_indicator_get_segment_height(AgsIndicator *indicator);
void ags_indicator_set_segment_height(AgsIndicator *indicator,
				      guint segment_height);

guint ags_indicator_get_segment_padding(AgsIndicator *indicator);
void ags_indicator_set_segment_padding(AgsIndicator *indicator,
				       guint segment_padding);

guint ags_indicator_get_segment_count(AgsIndicator *indicator);
void ags_indicator_set_segment_count(AgsIndicator *indicator,
				     guint segment_count);

GtkAdjustment* ags_indicator_get_adjustment(AgsIndicator *indicator);
void ags_indicator_set_adjustment(AgsIndicator *indicator,
				  GtkAdjustment *adjustment);

/* instantiate */
AgsIndicator* ags_indicator_new(GtkOrientation orientation,
				guint segment_width,
				guint segment_height);

G_END_DECLS

#endif /*__AGS_INDICATOR_H__*/
