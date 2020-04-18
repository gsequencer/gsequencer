/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_FX_NOTATION_AUDIO_H__
#define __AGS_FX_NOTATION_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_NOTATION_AUDIO                (ags_fx_notation_audio_get_type())
#define AGS_FX_NOTATION_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_NOTATION_AUDIO, AgsFxNotationAudio))
#define AGS_FX_NOTATION_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_NOTATION_AUDIO, AgsFxNotationAudio))
#define AGS_IS_FX_NOTATION_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_NOTATION_AUDIO))
#define AGS_IS_FX_NOTATION_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_NOTATION_AUDIO))
#define AGS_FX_NOTATION_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_NOTATION_AUDIO, AgsFxNotationAudioClass))

#define AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_START (0)
#define AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_END (64)
  
typedef struct _AgsFxNotationAudio AgsFxNotationAudio;
typedef struct _AgsFxNotationAudioClass AgsFxNotationAudioClass;

typedef enum{
  AGS_FX_NOTATION_AUDIO_PLAY     = 1,
  AGS_FX_NOTATION_AUDIO_RECORD   = 1 <<  1,
  AGS_FX_NOTATION_AUDIO_FEED     = 1 <<  2,
}AgsFxNotationAudioFlags;

struct _AgsFxNotationAudio
{
  AgsRecallAudio recall_audio;

  guint flags;

  GList *feed_note;
  
  AgsPort *bpm;
  AgsPort *tact;

  AgsPort *delay;
  AgsPort *duration;

  AgsPort *loop;
  AgsPort *loop_start;
  AgsPort *loop_end;
};

struct _AgsFxNotationAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_fx_notation_audio_get_type();

gboolean ags_fx_notation_audio_test_flags(AgsFxNotationAudio *fx_notation_audio, guint flags);
void ags_fx_notation_audio_set_flags(AgsFxNotationAudio *fx_notation_audio, guint flags);
void ags_fx_notation_audio_unset_flags(AgsFxNotationAudio *fx_notation_audio, guint flags);

/*  */
GList* ags_fx_notation_audio_get_feed_note(AgsFxNotationAudio *fx_notation_audio);

void ags_fx_notation_audio_add_feed_note(AgsFxNotationAudio *fx_notation_audio,
					 AgsNote *feed_note);
void ags_fx_notation_audio_remove_feed_note(AgsFxNotationAudio *fx_notation_audio,
					    AgsNote *feed_note);

/*  */
AgsFxNotationAudio* ags_fx_notation_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_NOTATION_AUDIO_H__*/
