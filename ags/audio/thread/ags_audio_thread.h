/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_AUDIO_THREAD_H__
#define __AGS_AUDIO_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_THREAD                (ags_audio_thread_get_type())
#define AGS_AUDIO_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_THREAD, AgsAudioThread))
#define AGS_AUDIO_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))
#define AGS_IS_AUDIO_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_THREAD))
#define AGS_IS_AUDIO_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_THREAD))
#define AGS_AUDIO_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))

#define AGS_AUDIO_THREAD_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsAudioThread AgsAudioThread;
typedef struct _AgsAudioThreadClass AgsAudioThreadClass;

/**
 * AgsAudioThreadFlags:
 * @AGS_AUDIO_THREAD_DONE: sync done parent thread, initial wait during #AgsThread::run()
 * @AGS_AUDIO_THREAD_WAIT: sync wait parent thread, initial wait during #AgsThread::run()
 * @AGS_AUDIO_THREAD_DONE_SYNC: sync done parent thread, signal completed during #AgsThread::run()
 * @AGS_AUDIO_THREAD_WAIT_SYNC: sync wait parent thread, signal completed during #AgsThread::run()
 *
 * Enum values to control the behavior or indicate internal state of #AgsAudioThread by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_THREAD_DONE            = 1,
  AGS_AUDIO_THREAD_WAIT            = 1 <<  1,
  AGS_AUDIO_THREAD_DONE_SYNC       = 1 <<  2,
  AGS_AUDIO_THREAD_WAIT_SYNC       = 1 <<  3,
}AgsAudioThreadFlags;

struct _AgsAudioThread
{
  AgsThread thread;

  volatile guint flags;

  GObject *default_output_soundcard;
  
  GMutex wakeup_mutex;
  GCond wakeup_cond;

  GMutex done_mutex;
  GCond done_cond;

  GObject *audio;
  gint sound_scope;
  
  GList *sync_thread;
};

struct _AgsAudioThreadClass
{
  AgsThreadClass thread;
};

GType ags_audio_thread_get_type();

void ags_audio_thread_set_sound_scope(AgsAudioThread *audio_thread,
				      gint sound_scope);

AgsAudioThread* ags_audio_thread_new(GObject *default_output_soundcard,
				     GObject *audio);

G_END_DECLS

#endif /*__AGS_AUDIO_THREAD_H__*/
