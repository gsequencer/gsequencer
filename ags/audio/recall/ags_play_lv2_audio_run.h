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

#ifndef __AGS_PLAY_LV2_AUDIO_RUN_H__
#define __AGS_PLAY_LV2_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <lv2.h>

#define AGS_TYPE_PLAY_LV2_AUDIO_RUN                (ags_play_lv2_audio_run_get_type())
#define AGS_PLAY_LV2_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_LV2_AUDIO_RUN, AgsPlayLv2AudioRun))
#define AGS_PLAY_LV2_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_LV2_AUDIO_RUN, AgsPlayLv2AudioRun))
#define AGS_IS_PLAY_LV2_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_LV2_AUDIO_RUN))
#define AGS_IS_PLAY_LV2_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_LV2_AUDIO_RUN))
#define AGS_PLAY_LV2_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_LV2_AUDIO_RUN, AgsPlayLv2AudioRunClass))

#define AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT (8 * 256)

typedef struct _AgsPlayLv2AudioRun AgsPlayLv2AudioRun;
typedef struct _AgsPlayLv2AudioRunClass AgsPlayLv2AudioRunClass;

struct _AgsPlayLv2AudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint flags;

  LV2_Handle *lv2_handle;
  
  float *port_data;
  
  float *input;
  float *output;

  void *event_port;
  void *atom_port;

  guint key_on;
  
  long delta_time;
  
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  AgsDelayAudioRun *delay_audio_run;

  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *destination;
  
  AgsNotation *notation;
  AgsTimestamp *timestamp;

  GObject *worker_handle;
};

struct _AgsPlayLv2AudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_play_lv2_audio_run_get_type();

void ags_play_lv2_audio_run_load_ports(AgsPlayLv2AudioRun *play_lv2_audio_run);

AgsPlayLv2AudioRun* ags_play_lv2_audio_run_new(AgsAudio *audio,
					       AgsDelayAudioRun *delay_audio_run,
					       AgsCountBeatsAudioRun *count_beats_audio_run);

#endif /*__AGS_PLAY_LV2_AUDIO_RUN_H__*/
