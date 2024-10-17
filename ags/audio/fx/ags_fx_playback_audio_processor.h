/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_FX_PLAYBACK_AUDIO_PROCESSOR_H__
#define __AGS_FX_PLAYBACK_AUDIO_PROCESSOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_resample_util.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_recall_audio_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR                (ags_fx_playback_audio_processor_get_type())
#define AGS_FX_PLAYBACK_AUDIO_PROCESSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR, AgsFxPlaybackAudioProcessor))
#define AGS_FX_PLAYBACK_AUDIO_PROCESSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR, AgsFxPlaybackAudioProcessor))
#define AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR))
#define AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR))
#define AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR, AgsFxPlaybackAudioProcessorClass))

typedef struct _AgsFxPlaybackAudioProcessor AgsFxPlaybackAudioProcessor;
typedef struct _AgsFxPlaybackAudioProcessorClass AgsFxPlaybackAudioProcessorClass;

/**
 * AgsFxPlaybackAudioProcessorDataMode:
 * @AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY: data mode play sound
 * @AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_RECORD: data mode record sound
 * @AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_FEED: data mode feed audio signal
 * @AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_MASTER: data mode master audio signal
 * 
 * Enum values to enable specific data mode of #AgsFxPlaybackAudioProcessor.
 */
typedef enum{
  AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY,
  AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_RECORD,
  AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_FEED,
  AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_MASTER,
}AgsFxPlaybackAudioProcessorDataMode;

struct _AgsFxPlaybackAudioProcessor
{
  AgsRecallAudioRun recall_audio_run;  

  gdouble delay_completion;

  gdouble delay_counter;
  guint64 offset_counter;
  
  gdouble current_delay_counter;
  guint64 current_offset_counter;

  guint64 x_offset;
  guint64 current_x_offset;
  
  AgsTimestamp *timestamp;
  
  GList *playing_buffer;
  GList *playing_audio_signal;
  
  GList *recording_buffer;
  GList *recording_audio_signal;

  GList *feeding_audio_signal;

  GList *mastering_audio_signal;

  GList *capture_audio_signal;

  AgsAudioBufferUtil audio_buffer_util;
  AgsResampleUtil resample_util;
};

struct _AgsFxPlaybackAudioProcessorClass
{
  AgsRecallAudioRunClass recall_audio_run;

  void (*data_put)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
		   AgsBuffer *buffer,
		   guint data_mode);
  void (*data_get)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
		   AgsBuffer *buffer,
		   guint data_mode);
  
  void (*play)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
  void (*record)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
  void (*feed)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
  void (*master)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

  void (*counter_change)(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
};

GType ags_fx_playback_audio_processor_get_type();

void ags_fx_playback_audio_processor_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      AgsBuffer *buffer,
					      guint data_mode);
void ags_fx_playback_audio_processor_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      AgsBuffer *buffer,
					      guint data_mode);

void ags_fx_playback_audio_processor_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

void ags_fx_playback_audio_processor_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

/*  */
AgsFxPlaybackAudioProcessor* ags_fx_playback_audio_processor_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_PLAYBACK_AUDIO_PROCESSOR_H__*/
