/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/audio/recall/ags_play_notation_audio.h>

#include <ags/main.h>

void ags_play_notation_audio_class_init(AgsPlayNotationAudioClass *play_notation_audio);
void ags_play_notation_audio_init(AgsPlayNotationAudio *play_notation_audio);
void ags_play_notation_audio_finalize(GObject *gobject);

static gpointer ags_play_notation_audio_parent_class = NULL;

GType
ags_play_notation_audio_get_type()
{
  static GType ags_type_play_notation_audio = 0;

  if(!ags_type_play_notation_audio){
    static const GTypeInfo ags_play_notation_audio_info = {
      sizeof (AgsPlayNotationAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_notation_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayNotationAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_notation_audio_init,
    };

    ags_type_play_notation_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							  "AgsPlayNotationAudio\0",
							  &ags_play_notation_audio_info,
							  0);
  }

  return(ags_type_play_notation_audio);
}

void
ags_play_notation_audio_class_init(AgsPlayNotationAudioClass *play_notation_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_notation_audio_parent_class = g_type_class_peek_parent(play_notation_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_notation_audio;

  gobject->finalize = ags_play_notation_audio_finalize;
}

void
ags_play_notation_audio_init(AgsPlayNotationAudio *play_notation_audio)
{
  AGS_RECALL(play_notation_audio)->name = "ags-play-notation\0";
  AGS_RECALL(play_notation_audio)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(play_notation_audio)->build_id = AGS_BUILD_ID;
  AGS_RECALL(play_notation_audio)->xml_type = "ags-play-notation-audio\0";
}

void
ags_play_notation_audio_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_notation_audio_parent_class)->finalize(gobject);
}

AgsPlayNotationAudio*
ags_play_notation_audio_new()
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
							      NULL);

  return(play_notation_audio);
}
