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

#ifndef __AGS_PLAY_NOTATION_AUDIO_RUN_H__
#define __AGS_PLAY_NOTATION_AUDIO_RUN_H__

#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_notation.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#define AGS_TYPE_PLAY_NOTATION_AUDIO_RUN                (ags_play_notation_audio_run_get_type())
#define AGS_PLAY_NOTATION_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTATION_AUDIO_RUN, AgsPlayNotationAudioRun))
#define AGS_PLAY_NOTATION_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTATION_AUDIO_RUN, AgsPlayNotationAudioRun))
#define AGS_IS_PLAY_NOTATION_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_NOTATION_AUDIO_RUN))
#define AGS_IS_PLAY_NOTATION_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_NOTATION_AUDIO_RUN))
#define AGS_PLAY_NOTATION_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_NOTATION_AUDIO_RUN, AgsPlayNotationAudioRunClass))

typedef struct _AgsPlayNotationAudioRun AgsPlayNotationAudioRun;
typedef struct _AgsPlayNotationAudioRunClass AgsPlayNotationAudioRunClass;

typedef enum{
  AGS_PLAY_NOTATION_AUDIO_RUN_DEFAULT  =  1,
}AgsPlayNotationAudioRunFlags;

struct _AgsPlayNotationAudioRun
{
  AgsRecallAudioRun recall;

  guint flags;

  AgsDelayAudioRun *delay_audio_run;
  gulong notation_alloc_input_handler;

  AgsCountBeatsAudioRun *count_beats_audio_run;
};

struct _AgsPlayNotationAudioRunClass
{
  AgsRecallAudioRunClass recall;
};

GType ags_play_notation_audio_run_get_type();

AgsPlayNotationAudioRun* ags_play_notation_audio_run_new();

#endif /*__AGS_PLAY_NOTATION_AUDIO_RUN_H__*/
