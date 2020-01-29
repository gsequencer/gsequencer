/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_TRACK_H__
#define __AGS_TRACK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_TRACK                (ags_track_get_type())
#define AGS_TRACK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TRACK, AgsTrack))
#define AGS_TRACK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TRACK, AgsTrackClass))
#define AGS_IS_TRACK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TRACK))
#define AGS_IS_TRACK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_TRACK))
#define AGS_TRACK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TRACK, AgsTrackClass))

#define AGS_TRACK_GET_OBJ_MUTEX(obj) (&(((AgsTrack *) obj)->obj_mutex))

#define AGS_TRACK_DEFAULT_TICKS_PER_QUARTER_TRACK (16.0)

typedef struct _AgsTrack AgsTrack;
typedef struct _AgsTrackClass AgsTrackClass;

/**
 * AgsTrackFlags:
 * @AGS_TRACK_IS_SELECTED: is selected
 *
 * Enum values to control the behavior or indicate internal state of #AgsTrack by
 * enable/disable as flags.
 */
typedef enum{
  AGS_TRACK_IS_SELECTED     = 1,
}AgsTrackFlags;

struct _AgsTrack
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  guint64 x;

  unsigned char *smf_buffer;
  guint smf_buffer_length;
};

struct _AgsTrackClass
{
  GObjectClass gobject;
};

GType ags_track_get_type();

GRecMutex* ags_track_get_obj_mutex(AgsTrack *track);

void ags_track_lock(AgsTrack *track);
void ags_track_unlock(AgsTrack *track);

gboolean ags_track_test_flags(AgsTrack *track, guint flags);
void ags_track_set_flags(AgsTrack *track, guint flags);
void ags_track_unset_flags(AgsTrack *track, guint flags);

gint ags_track_sort_func(gconstpointer a,
			 gconstpointer b);

guint64 ags_track_get_x(AgsTrack *track);
void ags_track_set_x(AgsTrack *track, guint64 x);

gpointer ags_track_get_smf_buffer(AgsTrack *track,
				  guint *smf_buffer_length);

AgsTrack* ags_track_duplicate(AgsTrack *track);

AgsTrack* ags_track_new();

G_END_DECLS

#endif /*__AGS_TRACK_H__*/
