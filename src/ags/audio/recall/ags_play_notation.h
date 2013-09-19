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

#ifndef __AGS_PLAY_NOTATION_H__
#define __AGS_PLAY_NOTATION_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_notation.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#define AGS_TYPE_PLAY_NOTATION                (ags_play_notation_get_type())
#define AGS_PLAY_NOTATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTATION, AgsPlayNotation))
#define AGS_PLAY_NOTATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTATION, AgsPlayNotation))
#define AGS_IS_PLAY_NOTATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_NOTATION))
#define AGS_IS_PLAY_NOTATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_NOTATION))
#define AGS_PLAY_NOTATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_NOTATION, AgsPlayNotationClass))

typedef struct _AgsPlayNotation AgsPlayNotation;
typedef struct _AgsPlayNotationClass AgsPlayNotationClass;

typedef enum{
  AGS_PLAY_NOTATION_DEFAULT  =  1,
}AgsPlayNotationFlags;

struct _AgsPlayNotation
{
  AgsRecallAudioRun recall;

  guint flags;

  GList *notation;

  AgsDelayAudioRun *delay_audio_run;
  gulong tic_alloc_input_handler;

  AgsCountBeatsAudioRun *count_beats_audio_run;
};

struct _AgsPlayNotationClass
{
  AgsRecallAudioRunClass recall;
};

GType ags_play_notation_get_type();

AgsPlayNotation* ags_play_notation_new();

#endif /*__AGS_PLAY_NOTATION_H__*/
