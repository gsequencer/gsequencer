/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_PLAYBACK_H__
#define __AGS_PLAYBACK_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_PLAYBACK                (ags_playback_get_type())
#define AGS_PLAYBACK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYBACK, AgsPlayback))
#define AGS_PLAYBACK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAYBACK, AgsPlayback))
#define AGS_IS_PLAYBACK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAYBACK))
#define AGS_IS_PLAYBACK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAYBACK))
#define AGS_PLAYBACK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PLAYBACK, AgsPlaybackClass))

typedef struct _AgsPlayback AgsPlayback;
typedef struct _AgsPlaybackClass AgsPlaybackClass;

/**
 * AgsPlaybackFlags:
 * @AGS_PLAYBACK_CONNECTED: indicates the playback was connected by calling #AgsConnectable::connect()
 * @AGS_PLAYBACK_SINGLE_THREADED: single threaded
 * @AGS_PLAYBACK_SUPER_THREADED_CHANNEL: super threaded channel
 *
 * Enum values to control the behavior or indicate internal state of #AgsPlayback by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PLAYBACK_CONNECTED                    = 1,
  AGS_PLAYBACK_SINGLE_THREADED              = 1 <<  1,
  AGS_PLAYBACK_SUPER_THREADED_CHANNEL       = 1 <<  2,
}AgsPlaybackFlags;

struct _AgsPlayback
{
  GObject gobject;
  
  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *playback_domain;
  
  GObject *channel;
  guint audio_channel;

  GObject *play_note;
  
  AgsThread **channel_thread;
  
  AgsRecallID **recall_id;
};

struct _AgsPlaybackClass
{
  GObjectClass gobject;
};

GType ags_playback_get_type();

pthread_mutex_t* ags_playback_get_class_mutex();

gboolean ags_playback_test_flags(AgsPlayback *playback, guint flags);
void ags_playback_set_flags(AgsPlayback *playback, guint flags);
void ags_playback_unset_flags(AgsPlayback *playback, guint flags);

/* get and set */
void ags_playback_set_channel_thread(AgsPlayback *playback,
				     AgsThread *thread,
				     gint sound_scope);
AgsThread* ags_playback_get_channel_thread(AgsPlayback *playback,
					   gint sound_scope);

void ags_playback_set_recall_id(AgsPlayback *playback,
				AgsRecallID *recall_id,
				gint sound_scope);
AgsRecallID* ags_playback_get_recall_id(AgsPlayback *playback,
					gint sound_scope);

/* find */
AgsPlayback* ags_playback_find_channel(GList *playback,
				       GObject *channel);

/* instance */
AgsPlayback* ags_playback_new(GObject *channel);

#endif /*__AGS_PLAYBACK_H__*/
