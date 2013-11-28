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

#ifndef __AGS_DELAY_AUDIO_RUN_H__
#define __AGS_DELAY_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_run.h>

#define AGS_TYPE_DELAY_AUDIO_RUN                (ags_delay_audio_run_get_type())
#define AGS_DELAY_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_DELAY_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_IS_DELAY_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_IS_DELAY_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_DELAY_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRunClass))

typedef struct _AgsDelayAudioRun AgsDelayAudioRun;
typedef struct _AgsDelayAudioRunClass AgsDelayAudioRunClass;

struct _AgsDelayAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint dependency_ref;

  guint hide_ref;
  guint hide_ref_counter;
  
  guint notation_counter;
  guint sequencer_counter;
};

struct _AgsDelayAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;

  void (*notation_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint run_order,
				guint delay, guint attack);
  void (*notation_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint run_order,
			       guint delay, guint attack);
  void (*notation_count)(AgsDelayAudioRun *delay_audio_run, guint run_order,
			 guint delay, guint attack);

  void (*sequencer_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint run_order,
				 guint delay, guint attack);
  void (*sequencer_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint run_order,
				guint delay, guint attack);
  void (*sequencer_count)(AgsDelayAudioRun *delay_audio_run, guint run_order,
			  guint delay, guint attack);
};

GType ags_delay_audio_run_get_type();

void ags_delay_audio_run_notation_alloc_output(AgsDelayAudioRun *delay_audio_run, guint run_order,
					       guint delay, guint attack);
void ags_delay_audio_run_notation_alloc_input(AgsDelayAudioRun *delay_audio_run, guint run_order,
					      guint delay, guint attack);
void ags_delay_audio_run_notation_count(AgsDelayAudioRun *delay_audio_run, guint run_order,
					guint delay, guint attack);

void ags_delay_audio_run_sequencer_alloc_output(AgsDelayAudioRun *delay_audio_run, guint run_order,
						guint delay, guint attack);
void ags_delay_audio_run_sequencer_alloc_input(AgsDelayAudioRun *delay_audio_run, guint run_order,
					       guint delay, guint attack);
void ags_delay_audio_run_sequencer_count(AgsDelayAudioRun *delay_audio_run, guint run_order,
					 guint delay, guint attack);

AgsDelayAudioRun* ags_delay_audio_run_new();

#endif /*__AGS_DELAY_AUDIO_RUN_H__*/
