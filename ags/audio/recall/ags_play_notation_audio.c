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

#include <ags/audio/recall/ags_play_notation_audio.h>

void ags_play_notation_audio_class_init(AgsPlayNotationAudioClass *play_notation_audio);
void ags_play_notation_audio_init(AgsPlayNotationAudio *play_notation_audio);
void ags_play_notation_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_play_notation_audio
 * @short_description: play audio notation
 * @title: AgsPlayNotationAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_notation_audio.h
 *
 * The #AgsPlayNotationAudio class provides ports to the effect processor.
 */

static gpointer ags_play_notation_audio_parent_class = NULL;

const gchar *ags_play_notation_audio_plugin_name = "ags-play-notation";
const gchar *ags_play_notation_audio_specifier[] = {
};
const gchar *ags_play_notation_audio_control_port[] = {
};

GType
ags_play_notation_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_notation_audio = 0;

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
							  "AgsPlayNotationAudio",
							  &ags_play_notation_audio_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_notation_audio);
  }

  return g_define_type_id__volatile;
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
  GList *port;

  AGS_RECALL(play_notation_audio)->name = "ags-play-notation";
  AGS_RECALL(play_notation_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_notation_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_notation_audio)->xml_type = "ags-play-notation-audio";

  port = NULL;

  /* set port */
  AGS_RECALL(play_notation_audio)->port = port;
}

void
ags_play_notation_audio_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_play_notation_audio_parent_class)->finalize(gobject);
}

/**
 * ags_play_notation_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsPlayNotationAudio
 *
 * Returns: the new #AgsPlayNotationAudio
 *
 * Since: 3.0.0
 */
AgsPlayNotationAudio*
ags_play_notation_audio_new(AgsAudio *audio)
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
							      "audio", audio,
							      NULL);

  return(play_notation_audio);
}
