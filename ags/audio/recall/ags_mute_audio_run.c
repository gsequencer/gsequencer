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

#include <ags/audio/recall/ags_mute_audio_run.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_mute_audio.h>

void ags_mute_audio_run_class_init(AgsMuteAudioRunClass *mute_audio_run);
void ags_mute_audio_run_init(AgsMuteAudioRun *mute_audio_run);
void ags_mute_audio_run_finalize(GObject *gobject);

/**
 * SECTION:ags_mute_audio_run
 * @short_description: copy pattern
 * @title: AgsMuteAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_mute_audio_run.h
 *
 * The #AgsMuteAudioRun class copy pattern.
 */

static gpointer ags_mute_audio_run_parent_class = NULL;

GType
ags_mute_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mute_audio_run = 0;

    static const GTypeInfo ags_mute_audio_run_info = {
      sizeof (AgsMuteAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_audio_run_init,
    };

    ags_type_mute_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						     "AgsMuteAudioRun",
						     &ags_mute_audio_run_info,
						     0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mute_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_mute_audio_run_class_init(AgsMuteAudioRunClass *mute_audio_run)
{
  GObjectClass *gobject;

  ags_mute_audio_run_parent_class = g_type_class_peek_parent(mute_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_audio_run;

  gobject->finalize = ags_mute_audio_run_finalize;
}

void
ags_mute_audio_run_init(AgsMuteAudioRun *mute_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) mute_audio_run, (AGS_SOUND_ABILITY_SEQUENCER |
							      AGS_SOUND_ABILITY_NOTATION |
							      AGS_SOUND_ABILITY_WAVE |
							      AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(mute_audio_run)->name = "ags-mute";
  AGS_RECALL(mute_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_audio_run)->xml_type = "ags-mute-audio-run";
  AGS_RECALL(mute_audio_run)->port = NULL;
}

void
ags_mute_audio_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_run_parent_class)->finalize(gobject);
}

/**
 * ags_mute_audio_run_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsMuteAudioRun
 *
 * Returns: the new #AgsMuteAudioRun
 *
 * Since: 2.0.0
 */
AgsMuteAudioRun*
ags_mute_audio_run_new(AgsAudio *audio)
{
  AgsMuteAudioRun *mute_audio_run;

  mute_audio_run = (AgsMuteAudioRun *) g_object_new(AGS_TYPE_MUTE_AUDIO_RUN,
						    "audio", audio,
						    NULL);

  return(mute_audio_run);
}
