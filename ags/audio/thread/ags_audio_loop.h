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

#ifndef __AGS_AUDIO_LOOP_H__
#define __AGS_AUDIO_LOOP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_soundcard.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif

#include <math.h>

#define AGS_TYPE_AUDIO_LOOP                (ags_audio_loop_get_type())
#define AGS_AUDIO_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_LOOP, AgsAudioLoop))
#define AGS_AUDIO_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_LOOP, AgsAudioLoopClass))
#define AGS_IS_AUDIO_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_LOOP))
#define AGS_IS_AUDIO_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_LOOP))
#define AGS_AUDIO_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_LOOP, AgsAudioLoopClass))

#define AGS_AUDIO_LOOP_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsAudioLoop AgsAudioLoop;
typedef struct _AgsAudioLoopClass AgsAudioLoopClass;

typedef enum{
  AGS_AUDIO_LOOP_PLAY_RECALL                    = 1,
  AGS_AUDIO_LOOP_PLAYING_RECALL                 = 1 << 1,
  AGS_AUDIO_LOOP_PLAY_RECALL_TERMINATING        = 1 << 2,
  AGS_AUDIO_LOOP_PLAY_CHANNEL                   = 1 << 3,
  AGS_AUDIO_LOOP_PLAYING_CHANNEL                = 1 << 4,
  AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING       = 1 << 5,
  AGS_AUDIO_LOOP_PLAY_AUDIO                     = 1 << 6,
  AGS_AUDIO_LOOP_PLAYING_AUDIO                  = 1 << 7,
  AGS_AUDIO_LOOP_PLAY_NOTATION                  = 1 << 8,
  AGS_AUDIO_LOOP_PLAYING_NOTATION               = 1 << 9,
  AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING         = 1 << 10,
}AgsAudioLoopFlags;

typedef enum{
  AGS_AUDIO_LOOP_TIMING_WAITING                 = 1,
  AGS_AUDIO_LOOP_TIMING_WAKEUP                  = 1 <<  1,
}AgsAudioLoopTimingFlags;

struct _AgsAudioLoop
{
  AgsThread thread;

  guint flags;
  volatile guint timing_flags;
  
  volatile guint tic;
  volatile guint last_sync;

  guint time_cycle;
  volatile guint time_spent;
  
  GObject *application_context;
  GObject *soundcard;
  
  GObject *async_queue;
    
  pthread_mutexattr_t tree_lock_mutexattr;
  pthread_mutex_t *tree_lock;
  pthread_mutex_t *recall_mutex;

  GMutex mutex;
  GCond cond;
  
  GMainContext *main_context;
  gint cached_poll_array_size;
  GPollFD *cached_poll_array;

  pthread_mutex_t *timing_mutex;
  pthread_cond_t *timing_cond;
  
  pthread_t *timing_thread;
  
  guint play_recall_ref;
  GList *play_recall; // play AgsRecall

  guint play_channel_ref;
  GList *play_channel; // play AgsChannel

  guint play_audio_ref;
  GList *play_audio; // play AgsAudio

  guint play_notation_ref;
  GList *play_notation;

  GList *tree_sanity;
};

struct _AgsAudioLoopClass
{
  AgsThreadClass thread;
};

GType ags_audio_loop_get_type();

void ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio);
void ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio);

void ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel);
void ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel);

void ags_audio_loop_add_recall(AgsAudioLoop *audio_loop, gpointer playback);
void ags_audio_loop_remove_recall(AgsAudioLoop *audio_loop, gpointer playback);

AgsAudioLoop* ags_audio_loop_new(GObject *soundcard, GObject *application_context);

#endif /*__AGS_AUDIO_LOOP_H__*/
