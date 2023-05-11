/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_MARKER_H__
#define __AGS_MARKER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MARKER                (ags_marker_get_type())
#define AGS_MARKER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MARKER, AgsMarker))
#define AGS_MARKER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MARKER, AgsMarkerClass))
#define AGS_IS_MARKER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MARKER))
#define AGS_IS_MARKER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MARKER))
#define AGS_MARKER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MARKER, AgsMarkerClass))

#define AGS_MARKER_GET_OBJ_MUTEX(obj) (&(((AgsMarker *) obj)->obj_mutex))

typedef struct _AgsMarker AgsMarker;
typedef struct _AgsMarkerClass AgsMarkerClass;

/**
 * AgsMarkerFlags:
 * @AGS_MARKER_DEFAULT_START: if start is default start point
 * @AGS_MARKER_DEFAULT_END: if end is default end point
 * @AGS_MARKER_GUI: interpret x and y as GUI format
 * @AGS_MARKER_RUNTIME: interpret x and y as runtime formant
 * @AGS_MARKER_HUMAN_READABLE: interpret x and y as human readable
 * @AGS_MARKER_DEFAULT_LENGTH: if default length applies
 * @AGS_MARKER_IS_SELECTED: if the marker is selected
 */
typedef enum{
  AGS_MARKER_DEFAULT_START   = 1,
  AGS_MARKER_DEFAULT_END     = 1 <<  1,
  AGS_MARKER_GUI             = 1 <<  2,
  AGS_MARKER_RUNTIME         = 1 <<  3,
  AGS_MARKER_HUMAN_READABLE  = 1 <<  4,
  AGS_MARKER_DEFAULT_LENGTH  = 1 <<  5,
  AGS_MARKER_IS_SELECTED     = 1 <<  6,
}AgsMarkerFlags;

struct _AgsMarker
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  // gui format, convert easy to visualization
  guint x;
  gdouble y;

  gchar *marker_name;
};

struct _AgsMarkerClass
{
  GObjectClass gobject;
};

GType ags_marker_get_type();
GType ags_marker_flags_get_type();

GRecMutex* ags_marker_get_obj_mutex(AgsMarker *marker);

gboolean ags_marker_test_flags(AgsMarker *marker, AgsMarkerFlags flags);
void ags_marker_set_flags(AgsMarker *marker, AgsMarkerFlags flags);
void ags_marker_unset_flags(AgsMarker *marker, AgsMarkerFlags flags);

gint ags_marker_sort_func(gconstpointer a,
				gconstpointer b);

guint ags_marker_get_x(AgsMarker *marker);
void ags_marker_set_x(AgsMarker *marker, guint x);

gdouble ags_marker_get_y(AgsMarker *marker);
void ags_marker_set_y(AgsMarker *marker, gdouble y);

AgsMarker* ags_marker_duplicate(AgsMarker *marker);

AgsMarker* ags_marker_new();

G_END_DECLS

#endif /*__AGS_MARKER_H__*/
