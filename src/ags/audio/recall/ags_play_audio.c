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

#include <ags/audio/recall/ags_play_audio.h>

#include <ags/main.h>

void ags_play_audio_class_init(AgsPlayAudioClass *play_audio);
void ags_play_audio_init(AgsPlayAudio *play_audio);
void ags_play_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_play_audio
 * @short_description: play audio 
 * @title: AgsPlayAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_audio.h
 *
 * The #AgsPlayAudio class provides ports to the effect processor.
 */

static gpointer ags_play_audio_parent_class = NULL;

GType
ags_play_audio_get_type()
{
  static GType ags_type_play_audio = 0;

  if(!ags_type_play_audio){
    static const GTypeInfo ags_play_audio_info = {
      sizeof (AgsPlayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_init,
    };

    ags_type_play_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						 "AgsPlayAudio\0",
						 &ags_play_audio_info,
						 0);
  }

  return(ags_type_play_audio);
}

void
ags_play_audio_class_init(AgsPlayAudioClass *play_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_audio_parent_class = g_type_class_peek_parent(play_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_audio;

  gobject->finalize = ags_play_audio_finalize;
}

void
ags_play_audio_init(AgsPlayAudio *play_audio)
{
  AGS_RECALL(play_audio)->name = "ags-play\0";
  AGS_RECALL(play_audio)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(play_audio)->build_id = AGS_BUILD_ID;
  AGS_RECALL(play_audio)->xml_type = "ags-play-audio\0";
}

void
ags_play_audio_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_parent_class)->finalize(gobject);
}

/**
 * ags_play_audio_new:
 *
 * Creates an #AgsPlayAudio
 *
 * Returns: a new #AgsPlayAudio
 *
 * Since: 0.4
 */
AgsPlayAudio*
ags_play_audio_new()
{
  AgsPlayAudio *play_audio;

  play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
					     NULL);

  return(play_audio);
}
