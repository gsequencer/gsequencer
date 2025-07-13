/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_ACCELERATION_H__
#define __AGS_ACCELERATION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_ACCELERATION                (ags_acceleration_get_type())
#define AGS_ACCELERATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACCELERATION, AgsAcceleration))
#define AGS_ACCELERATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ACCELERATION, AgsAccelerationClass))
#define AGS_IS_ACCELERATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ACCELERATION))
#define AGS_IS_ACCELERATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ACCELERATION))
#define AGS_ACCELERATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ACCELERATION, AgsAccelerationClass))

#define AGS_ACCELERATION_GET_OBJ_MUTEX(obj) (&(((AgsAcceleration *) obj)->obj_mutex))

typedef struct _AgsAcceleration AgsAcceleration;
typedef struct _AgsAccelerationClass AgsAccelerationClass;

/**
 * AgsAccelerationFlags:
 * @AGS_ACCELERATION_DEFAULT_START: if start is default start point
 * @AGS_ACCELERATION_DEFAULT_END: if end is default end point
 * @AGS_ACCELERATION_GUI: interpret x and y as GUI format
 * @AGS_ACCELERATION_RUNTIME: interpret x and y as runtime formant
 * @AGS_ACCELERATION_HUMAN_READABLE: interpret x and y as human readable
 * @AGS_ACCELERATION_DEFAULT_LENGTH: if default length applies
 * @AGS_ACCELERATION_IS_SELECTED: if the acceleration is selected
 */
typedef enum{
  AGS_ACCELERATION_DEFAULT_START   = 1,
  AGS_ACCELERATION_DEFAULT_END     = 1 <<  1,
  AGS_ACCELERATION_GUI             = 1 <<  2,
  AGS_ACCELERATION_RUNTIME         = 1 <<  3,
  AGS_ACCELERATION_HUMAN_READABLE  = 1 <<  4,
  AGS_ACCELERATION_DEFAULT_LENGTH  = 1 <<  5,
  AGS_ACCELERATION_IS_SELECTED     = 1 <<  6,
}AgsAccelerationFlags;

struct _AgsAcceleration
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  // gui format, convert easy to visualization
  guint x;
  gdouble y;

  gchar *acceleration_name;
};

struct _AgsAccelerationClass
{
  GObjectClass gobject;
};

GType ags_acceleration_get_type();
GType ags_acceleration_flags_get_type();

GRecMutex* ags_acceleration_get_obj_mutex(AgsAcceleration *acceleration);

/* flags */
gboolean ags_acceleration_test_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags);
void ags_acceleration_set_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags);
void ags_acceleration_unset_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags);

/* sort */
gint ags_acceleration_sort_func(gconstpointer a,
				gconstpointer b);

/* getter/setter */
guint ags_acceleration_get_x(AgsAcceleration *acceleration);
void ags_acceleration_set_x(AgsAcceleration *acceleration, guint x);

gdouble ags_acceleration_get_y(AgsAcceleration *acceleration);
void ags_acceleration_set_y(AgsAcceleration *acceleration, gdouble y);

/* find */
GList* ags_acceleration_find_range_x(GList *acceleration,
				     guint start_x, guint end_x);

/* duplicate */
AgsAcceleration* ags_acceleration_duplicate(AgsAcceleration *acceleration);

/* insantiate */
AgsAcceleration* ags_acceleration_new();

G_END_DECLS

#endif /*__AGS_ACCELERATION_H__*/
