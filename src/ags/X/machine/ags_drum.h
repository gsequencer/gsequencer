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

#ifndef __AGS_DRUM_H__
#define __AGS_DRUM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>

#include <ags/X/machine/ags_drum_input_pad.h>

#define AGS_TYPE_DRUM                (ags_drum_get_type())
#define AGS_DRUM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM, AgsDrum))
#define AGS_DRUM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM, AgsDrumClass))
#define AGS_IS_DRUM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM))
#define AGS_IS_DRUM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM))
#define AGS_DRUM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DRUM, AgsDrumClass))

typedef struct _AgsDrum AgsDrum;
typedef struct _AgsDrumClass AgsDrumClass;

typedef enum{
  AGS_DRUM_BLOCK_PATTERN    =  1,
}AgsDrumFlags;

struct _AgsDrum
{
  AgsMachine machine;

  guint flags;

  /* these recall will be moved to a mapping list in AgsAudio in a future release */
  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsCopyPatternAudio *play_copy_pattern_audio;
  AgsCopyPatternAudioRun *play_copy_pattern_audio_run;

  AgsDelayAudio *recall_delay_audio;
  AgsDelayAudioRun *recall_delay_audio_run;
  AgsCountBeatsAudio *recall_count_beats_audio;
  AgsCountBeatsAudioRun *recall_count_beats_audio_run;
  AgsCopyPatternAudio *recall_copy_pattern_audio;
  AgsCopyPatternAudioRun *recall_copy_pattern_audio_run;

  GtkButton *open;

  GtkCheckButton *loop_button;
  GtkSpinButton *length_spin;
  GtkOptionMenu *tact;

  GtkToggleButton *run;

  GtkVBox *vbox;

  GtkHBox *input_pad;
  GtkVBox *output_pad;

  GtkToggleButton *selected_edit_button;

  AgsDrumInputPad *selected_pad;
  AgsLine *selected_line;

  GtkToggleButton *index0[4];
  GtkToggleButton *selected0;

  GtkToggleButton *index1[12];
  GtkToggleButton *selected1;

  GtkHBox *pattern;

  GtkVBox *offset;
};

struct _AgsDrumClass
{
  AgsMachineClass machine;
};

GType ags_drum_get_type(void);

void ags_drum_set_pattern(AgsDrum *drum);

AgsDrum* ags_drum_new(GObject *devout);

#endif /*__AGS_DRUM_H__*/
