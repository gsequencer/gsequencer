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

#ifndef __AGS_PLAY_DSSI_AUDIO_RUN_H__
#define __AGS_PLAY_DSSI_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ladspa.h>
#include <dssi.h>

#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_notation.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#define AGS_TYPE_PLAY_DSSI_AUDIO_RUN                (ags_play_dssi_audio_run_get_type())
#define AGS_PLAY_DSSI_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_DSSI_AUDIO_RUN, AgsPlayDssiAudioRun))
#define AGS_PLAY_DSSI_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_DSSI_AUDIO_RUN, AgsPlayDssiAudioRun))
#define AGS_IS_PLAY_DSSI_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_DSSI_AUDIO_RUN))
#define AGS_IS_PLAY_DSSI_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_DSSI_AUDIO_RUN))
#define AGS_PLAY_DSSI_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_DSSI_AUDIO_RUN, AgsPlayDssiAudioRunClass))

#define AGS_PLAY_DSSI_AUDIO_DEFAULT_MIDI_LENGHT (8 * 256)

typedef struct _AgsPlayDssiAudioRun AgsPlayDssiAudioRun;
typedef struct _AgsPlayDssiAudioRunClass AgsPlayDssiAudioRunClass;

typedef enum{
  AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT  =  1,
}AgsPlayDssiAudioRunFlags;

struct _AgsPlayDssiAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint flags;

  LADSPA_Handle *ladspa_handle;

  unsigned long audio_channels;
  
  LADSPA_Data *input;
  LADSPA_Data *output;

  long delta_time;
  
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  guint key_on;

  AgsDelayAudioRun *delay_audio_run;

  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *destination;
  
  AgsNotation *notation;
  GList *offset;
};

struct _AgsPlayDssiAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_play_dssi_audio_run_get_type();

void ags_play_dssi_audio_run_load_ports(AgsPlayDssiAudioRun *play_dssi_audio_run);

AgsPlayDssiAudioRun* ags_play_dssi_audio_run_new();

#endif /*__AGS_PLAY_DSSI_AUDIO_RUN_H__*/
