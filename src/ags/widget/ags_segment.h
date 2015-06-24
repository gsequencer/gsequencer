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

#ifndef __AGS_SEGMENT_H__
#define __AGS_SEGMENT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_SEGMENT                (ags_segment_get_type())
#define AGS_SEGMENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEGMENT, AgsSegment))
#define AGS_SEGMENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SEGMENT, AgsSegmentClass))
#define AGS_IS_SEGMENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SEGMENT))
#define AGS_IS_SEGMENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SEGMENT))
#define AGS_SEGMENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SEGMENT, AgsSegmentClass))

typedef struct _AgsSegment AgsSegment;
typedef struct _AgsSegmentClass AgsSegmentClass;

typedef enum{
  AGS_SEGMENT_MONOCHROME      = 1,
  AGS_SEGMENT_COLOR_RED       = 1 <<  1,
  AGS_SEGMENT_COLOR_BLUE      = 1 <<  2,
  AGS_SGEMENT_COLOR_GREEN     = 1 <<  3,
}AGS_SEGMENT_FLAGS;

typedef enum{
  AGS_SEGMENT_UPPER_LEFT                = 1,
  AGS_SEGMENT_UPPER_RIGHT               = 1 <<  1,
  AGS_SEGMENT_LOWER_LEFT                = 1 <<  2,
  AGS_SEGMENT_LOWER_RIGHT               = 1 <<  3,
  AGS_SEGMENT_LEFT_UPPER_1              = 1 <<  4,
  AGS_SEGMENT_LEFT_UPPER_2              = 1 <<  5,
  AGS_SEGMENT_LEFT_LOWER_1              = 1 <<  6,
  AGS_SEGMENT_LEFT_LOWER_2              = 1 <<  7,
  AGS_SEGMENT_RIGHT_UPPER_1             = 1 <<  8,
  AGS_SEGMENT_RIGHT_UPPER_2             = 1 <<  9,
  AGS_SEGMENT_RIGHT_LOWER_1             = 1 << 10,
  AGS_SEGMENT_RIGHT_LOWER_2             = 1 << 11,
  AGS_SEGMENT_DIAGONAL_UPPER_LEFT_1     = 1 << 12,
  AGS_SEGMENT_DIAGONAL_UPPER_LEFT_2     = 1 << 13,
  AGS_SEGMENT_DIAGONAL_UPPER_RIGHT_1    = 1 << 14,
  AGS_SEGMENT_DIAGONAL_UPPER_RIGHT_2    = 1 << 15,
  AGS_SEGMENT_DIAGONAL_LOWER_LEFT_1     = 1 << 16,
  AGS_SEGMENT_DIAGONAL_LOWER_LEFT_2     = 1 << 17,
  AGS_SEGMENT_DIAGONAL_LOWER_RIGHT_1    = 1 << 18,
  AGS_SEGMENT_DIAGONAL_LOWER_RIGHT_2    = 1 << 19,
}AgsSegmentFields;

struct _AgsSegment
{
  GtkWidget widget;

  guint flags;

  guint count;
};

struct _AgsSegmentClass
{
  GtkWidgetClass widget;
};

GType ags_segment_get_type(void);

void ags_segment_field_on(AgsSegment *segment, guint field);
void ags_segment_field_off(AgsSegment *segment, guint field);

void ags_segment_draw_string(AgsSegment *segment, gchar **string);

AgsSegment* ags_segment_new();

#endif /*__AGS_SEGMENT_H__*/
