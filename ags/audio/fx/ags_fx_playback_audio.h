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

#ifndef __AGS_FX_PLAYBACK_AUDIO_H__
#define __AGS_FX_PLAYBACK_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

#include <ags/audio/file/ags_audio_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_PLAYBACK_AUDIO                (ags_fx_playback_audio_get_type())
#define AGS_FX_PLAYBACK_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_PLAYBACK_AUDIO, AgsFxPlaybackAudio))
#define AGS_FX_PLAYBACK_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_PLAYBACK_AUDIO, AgsFxPlaybackAudioClass))
#define AGS_IS_FX_PLAYBACK_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_PLAYBACK_AUDIO))
#define AGS_IS_FX_PLAYBACK_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_PLAYBACK_AUDIO))
#define AGS_FX_PLAYBACK_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_PLAYBACK_AUDIO, AgsFxPlaybackAudioClass))

#define AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_START (0)
#define AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_END (64)

typedef struct _AgsFxPlaybackAudio AgsFxPlaybackAudio;
typedef struct _AgsFxPlaybackAudioClass AgsFxPlaybackAudioClass;

/**
 * AgsFxPlaybackAudioFlags:
 * @AGS_FX_PLAYBACK_AUDIO_PLAY: do play sound
 * @AGS_FX_PLAYBACK_AUDIO_RECORD: do record sound
 * @AGS_FX_PLAYBACK_AUDIO_FEED: do feed audio signal
 * @AGS_FX_PLAYBACK_AUDIO_MASTER: do master audio signal
 * 
 * Enum values to enable specific feature of #AgsFxPlaybackAudio.
 */
typedef enum{
  AGS_FX_PLAYBACK_AUDIO_PLAY     = 1,
  AGS_FX_PLAYBACK_AUDIO_RECORD   = 1 <<  1,
  AGS_FX_PLAYBACK_AUDIO_FEED     = 1 <<  2,
  AGS_FX_PLAYBACK_AUDIO_MASTER   = 1 <<  3,
}AgsFxPlaybackAudioFlags;

/**
 * AgsFxPlaybackAudioCaptureMode:
 * @AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_DISCARD: discard capture
 * @AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE: capture none and preserve data
 * @AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE: capture replace any sound
 * @AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_MIX: capture mix with existing sound
 * 
 * Enum values to specify capture mode of #AgsFxPlaybackAudio.
 */
typedef enum{
  AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_DISCARD,
  AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE,
  AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE,
  AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_MIX,
}AgsFxPlaybackAudioCaptureMode;

struct _AgsFxPlaybackAudio
{
  AgsRecallAudio recall_audio;

  guint flags;
  
  GList *feed_audio_signal;  

  GList *master_audio_signal;  
  
  AgsAudioFile *audio_file;
  
  AgsPort *bpm;
  AgsPort *tact;

  AgsPort *delay;
  AgsPort *duration;

  AgsPort *loop;
  AgsPort *loop_start;
  AgsPort *loop_end;

  AgsPort *capture_mode;

  AgsPort *do_export;
  AgsPort *filename;

  AgsPort *file_audio_channels;
  AgsPort *file_samplerate;
  AgsPort *file_buffer_size;
  AgsPort *file_format;
};

struct _AgsFxPlaybackAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_fx_playback_audio_get_type();

gboolean ags_fx_playback_audio_test_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags);
void ags_fx_playback_audio_set_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags);
void ags_fx_playback_audio_unset_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags);

/* feed audio signal */
GList* ags_fx_playback_audio_get_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio);

void ags_fx_playback_audio_add_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
						 AgsAudioSignal *audio_signal);
void ags_fx_playback_audio_remove_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
						    AgsAudioSignal *audio_signal);

/* master audio signal */
GList* ags_fx_playback_audio_get_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio);

void ags_fx_playback_audio_add_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
						   AgsAudioSignal *audio_signal);
void ags_fx_playback_audio_remove_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
						      AgsAudioSignal *audio_signal);

/* open audio file */
void ags_fx_playback_audio_open_audio_file(AgsFxPlaybackAudio *fx_playback_audio);

/* instantiate */
AgsFxPlaybackAudio* ags_fx_playback_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_PLAYBACK_AUDIO_H__*/
