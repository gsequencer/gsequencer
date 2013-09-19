/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_COUNT_BEATS_AUDIO_RUN_H__
#define __AGS_COUNT_BEATS_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

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

  guint first_run_counter;

  guint notation_counter;
  guint sequencer_counter;

  guint recall_ref;

  guint hide_ref;
  guint notation_hide_ref_counter;
  guint sequencer_hide_ref_counter;

  gulong sequencer_alloc_output_handler;
  gulong sequencer_count_handler;

  gulong notation_alloc_output_handler;
  gulong notation_count_handler;

  AgsDelayAudioRun *delay_audio_run;
};

struct _AgsCountBeatsAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
  
  void (*notation_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
  void (*notation_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			guint nth_run);
  void (*notation_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			guint nth_run);
  
  void (*sequencer_start)(AgsCountBeatsAudioRun *count_beats_audio_run,
			  guint nth_run);
  void (*sequencer_loop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
  void (*sequencer_stop)(AgsCountBeatsAudioRun *count_beats_audio_run,
			 guint nth_run);
};

GType ags_count_beats_audio_run_get_type();

void ags_count_beats_audio_run_notation_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);
void ags_count_beats_audio_run_notation_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					     guint nth_run);
void ags_count_beats_audio_run_notation_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					     guint nth_run);

void ags_count_beats_audio_run_sequencer_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					       guint nth_run);
void ags_count_beats_audio_run_sequencer_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);
void ags_count_beats_audio_run_sequencer_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					      guint nth_run);

AgsCountBeatsAudioRun* ags_count_beats_audio_run_new(AgsDelayAudioRun *delay_audio_run);

#endif /*__AGS_COUNT_BEATS_AUDIO_RUN_H__*/
