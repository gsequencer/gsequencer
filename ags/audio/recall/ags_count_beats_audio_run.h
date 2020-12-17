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

#ifndef __AGS_COUNT_BEATS_AUDIO_RUN_H__
#define __AGS_COUNT_BEATS_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_COUNT_BEATS_AUDIO_RUN                (ags_count_beats_audio_run_get_type())
#define AGS_COUNT_BEATS_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COUNT_BEATS_AUDIO_RUN, AgsCountBeatsAudioRun))
#define AGS_COUNT_BEATS_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COUNT_BEATS_AUDIO_RUN, AgsCountBeatsAudioRun))
#define AGS_IS_COUNT_BEATS_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COUNT_BEATS_AUDIO_RUN))
#define AGS_IS_COUNT_BEATS_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COUNT_BEATS_AUDIO_RUN))
#define AGS_COUNT_BEATS_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COUNT_BEATS_AUDIO_RUN, AgsCountBeatsAudioRunClass))

typedef struct _AgsCountBeatsAudioRun AgsCountBeatsAudioRun;
typedef struct _AgsCountBeatsAudioRunClass AgsCountBeatsAudioRunClass;

struct _AgsCountBeatsAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  gboolean first_run;

  gdouble bpm;
  gdouble tact;
  
  guint64 sequencer_counter;
  guint64 notation_counter;
  guint64 wave_counter;
  guint64 midi_counter;

  guint recall_ref;

  guint hide_ref;
  guint sequencer_hide_ref_counter;
  guint notation_hide_ref_counter;
  guint wave_hide_ref_counter;
  guint midi_hide_ref_counter;

  AgsDelayAudioRun *delay_audio_run;
};

struct _AgsCountBeatsAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
  
  void (*sequencer_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
			  guint nth_run);
  void (*sequencer_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
  void (*sequencer_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
  
  void (*notation_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
  void (*notation_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			guint nth_run);
  void (*notation_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			guint nth_run);

  void (*wave_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
		     guint nth_run);
  void (*wave_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
		    guint nth_run);
  void (*wave_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
		    guint nth_run);

  void (*midi_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
		     guint nth_run);
  void (*midi_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
		    guint nth_run);
  void (*midi_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
		    guint nth_run);
};

G_DEPRECATED
GType ags_count_beats_audio_run_get_type();

G_DEPRECATED
void ags_count_beats_audio_run_sequencer_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					       guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_sequencer_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_sequencer_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);

G_DEPRECATED
void ags_count_beats_audio_run_notation_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_notation_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					     guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_notation_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					     guint nth_run);

G_DEPRECATED
void ags_count_beats_audio_run_wave_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					  guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_wave_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_wave_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run);

G_DEPRECATED
void ags_count_beats_audio_run_midi_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					  guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_midi_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run);
G_DEPRECATED
void ags_count_beats_audio_run_midi_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run);

G_DEPRECATED
AgsCountBeatsAudioRun* ags_count_beats_audio_run_new(AgsAudio *audio,
						     AgsDelayAudioRun *delay_audio_run);

G_END_DECLS

#endif /*__AGS_COUNT_BEATS_AUDIO_RUN_H__*/
