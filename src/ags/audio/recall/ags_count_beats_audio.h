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

#ifndef __AGS_COUNT_BEATS_AUDIO_H__
#define __AGS_COUNT_BEATS_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_COUNT_BEATS_AUDIO                (ags_count_beats_audio_get_type())
#define AGS_COUNT_BEATS_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COUNT_BEATS_AUDIO, AgsCountBeatsAudio))
#define AGS_COUNT_BEATS_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COUNT_BEATS_AUDIO, AgsCountBeatsAudio))
#define AGS_IS_COUNT_BEATS_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COUNT_BEATS_AUDIO))
#define AGS_IS_COUNT_BEATS_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COUNT_BEATS_AUDIO))
#define AGS_COUNT_BEATS_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COUNT_BEATS_AUDIO, AgsCountBeatsAudioClass))

typedef struct _AgsCountBeatsAudio AgsCountBeatsAudio;
typedef struct _AgsCountBeatsAudioClass AgsCountBeatsAudioClass;

struct _AgsCountBeatsAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *loop;

  AgsPort *notation_loop_start;
  AgsPort *notation_loop_end;
  AgsPort *sequencer_loop_start;
  AgsPort *sequencer_loop_end;
};

struct _AgsCountBeatsAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_count_beats_audio_get_type();

AgsCountBeatsAudio* ags_count_beats_audio_new(gboolean loop);

#endif /*__AGS_COUNT_BEATS_AUDIO_H__*/
