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

#ifndef __AGS_AUDIO_THREAD_H__
#define __AGS_AUDIO_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_THREAD                (ags_audio_thread_get_type())
#define AGS_AUDIO_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_THREAD, AgsAudioThread))
#define AGS_AUDIO_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))
#define AGS_IS_AUDIO_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_THREAD))
#define AGS_IS_AUDIO_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_THREAD))
#define AGS_AUDIO_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))

#define AGS_AUDIO_THREAD_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsAudioThread AgsAudioThread;
typedef struct _AgsAudioThreadScopeData AgsAudioThreadScopeData;
typedef struct _AgsAudioThreadClass AgsAudioThreadClass;

/**
 * AgsAudioThreadFlags:
 * @AGS_AUDIO_THREAD_STATUS_DONE: sync done parent thread, initial wait during #AgsThread::run()
 * @AGS_AUDIO_THREAD_STATUS_WAIT: sync wait parent thread, initial wait during #AgsThread::run()
 * @AGS_AUDIO_THREAD_STATUS_DONE_SYNC: sync done parent thread, signal completed during #AgsThread::run()
 * @AGS_AUDIO_THREAD_STATUS_WAIT_SYNC: sync wait parent thread, signal completed during #AgsThread::run()
 *
 * Enum values to control the behavior or indicate internal state of #AgsAudioThread by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_THREAD_STATUS_DONE            = 1,
  AGS_AUDIO_THREAD_STATUS_WAIT            = 1 <<  1,
  AGS_AUDIO_THREAD_STATUS_DONE_SYNC       = 1 <<  2,
  AGS_AUDIO_THREAD_STATUS_WAIT_SYNC       = 1 <<  3,
}AgsAudioThreadNestedSyncFlags;

struct _AgsAudioThread
{
  AgsThread thread;

  AgsAudioThreadNestedSyncFlags nested_sync_flags;

  GObject *default_output_soundcard;
  
  GMutex wakeup_mutex;
  GCond wakeup_cond;

  GMutex done_mutex;
  GCond done_cond;

  GObject *audio;
  gint sound_scope;
  
  GList *sync_thread;

  gboolean do_fx_staging;
  
  guint *staging_program;
  guint staging_program_count;

  AgsAudioThreadScopeData* scope_data[AGS_SOUND_SCOPE_LAST];

  gboolean processing;

  AgsTaskLauncher *task_launcher;
};

struct _AgsAudioThreadClass
{
  AgsThreadClass thread;
};

struct _AgsAudioThreadScopeData
{
  gboolean fx_done;
  guint fx_wait;
  
  GMutex fx_mutex;
  GCond fx_cond;
};

GType ags_audio_thread_get_type();

/* flags */
gboolean ags_audio_thread_test_nested_sync_flags(AgsAudioThread *audio_thread, AgsAudioThreadNestedSyncFlags nested_sync_flags);
void ags_audio_thread_set_nested_sync_flags(AgsAudioThread *audio_thread, AgsAudioThreadNestedSyncFlags nested_sync_flags);
void ags_audio_thread_unset_nested_sync_flags(AgsAudioThread *audio_thread, AgsAudioThreadNestedSyncFlags nested_sync_flags);

gboolean ags_audio_thread_get_processing(AgsAudioThread *audio_thread);
void ags_audio_thread_set_processing(AgsAudioThread *audio_thread,
				     gboolean processing);

AgsTaskLauncher* ags_audio_thread_get_task_launcher(AgsAudioThread *audio_thread);

void ags_audio_thread_set_sound_scope(AgsAudioThread *audio_thread,
				      gint sound_scope);

AgsAudioThreadScopeData* ags_audio_thread_scope_data_alloc();
void ags_audio_thread_scope_data_free(AgsAudioThreadScopeData *scope_data);

/* staging */
gboolean ags_audio_thread_get_do_fx_staging(AgsAudioThread *audio_thread);
void ags_audio_thread_set_do_fx_staging(AgsAudioThread *audio_thread, gboolean do_fx_staging);

guint* ags_audio_thread_get_staging_program(AgsAudioThread *audio_thread,
					    guint *staging_program_count);
void ags_audio_thread_set_staging_program(AgsAudioThread *audio_thread,
					  guint *staging_program,
					  guint staging_program_count);

/* instantiate */
AgsAudioThread* ags_audio_thread_new(GObject *default_output_soundcard,
				     GObject *audio);

G_END_DECLS

#endif /*__AGS_AUDIO_THREAD_H__*/
