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

#ifndef __AGS_AUDIO_LOOP_H__
#define __AGS_AUDIO_LOOP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>

#include <math.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_LOOP                (ags_audio_loop_get_type())
#define AGS_AUDIO_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_LOOP, AgsAudioLoop))
#define AGS_AUDIO_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_LOOP, AgsAudioLoopClass))
#define AGS_IS_AUDIO_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_LOOP))
#define AGS_IS_AUDIO_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_LOOP))
#define AGS_AUDIO_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_LOOP, AgsAudioLoopClass))

#define AGS_AUDIO_LOOP_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsAudioLoop AgsAudioLoop;
typedef struct _AgsAudioLoopClass AgsAudioLoopClass;

/**
 * AgsAudioLoopFlags:
 * @AGS_AUDIO_LOOP_PLAY_CHANNEL: play channel
 * @AGS_AUDIO_LOOP_PLAYING_CHANNEL: playing channnel
 * @AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING: play channe terminating
 * @AGS_AUDIO_LOOP_PLAY_AUDIO: play audio
 * @AGS_AUDIO_LOOP_PLAYING_AUDIO: playing audio
 * @AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING: play audio terminating
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioLoop by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_LOOP_PLAY_CHANNEL                   = 1,
  AGS_AUDIO_LOOP_PLAYING_CHANNEL                = 1 << 1,
  AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING       = 1 << 2,
  AGS_AUDIO_LOOP_PLAY_AUDIO                     = 1 << 3,
  AGS_AUDIO_LOOP_PLAYING_AUDIO                  = 1 << 4,
  AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING         = 1 << 5,
}AgsAudioLoopFlags;

struct _AgsAudioLoop
{
  AgsThread thread;

  guint flags;
      
  GRecMutex tree_lock;

  volatile gboolean is_syncing;

  volatile gboolean is_critical_region;
  volatile guint critical_region_ref;
  
  guint play_channel_ref;
  GList *play_channel; // play AgsChannel

  guint play_audio_ref;
  GList *play_audio; // play AgsAudio

  GList *sync_thread;

  gboolean do_fx_staging;
  
  guint *staging_program;
  guint staging_program_count;
};

struct _AgsAudioLoopClass
{
  AgsThreadClass thread;
};

GType ags_audio_loop_get_type();

/* flags */
gboolean ags_audio_loop_test_flags(AgsAudioLoop *audio_loop, guint flags);
void ags_audio_loop_set_flags(AgsAudioLoop *audio_loop, guint flags);
void ags_audio_loop_unset_flags(AgsAudioLoop *audio_loop, guint flags);

/* runtime */
void ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio);
void ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio);

void ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel);
void ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel);

/* staging */
gboolean ags_audio_loop_get_do_fx_staging(AgsAudioLoop *audio_loop);
void ags_audio_loop_set_do_fx_staging(AgsAudioLoop *audio_loop, gboolean do_fx_staging);

guint* ags_audio_loop_get_staging_program(AgsAudioLoop *audio_loop,
					  guint *staging_program_count);
void ags_audio_loop_set_staging_program(AgsAudioLoop *audio_loop,
					guint *staging_program,
					guint staging_program_count);

/* instantiate */
AgsAudioLoop* ags_audio_loop_new();

G_END_DECLS

#endif /*__AGS_AUDIO_LOOP_H__*/
