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

#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_audio.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/i18n.h>

void ags_play_wave_audio_run_class_init(AgsPlayWaveAudioRunClass *play_wave_audio_run);
void ags_play_wave_audio_run_init(AgsPlayWaveAudioRun *play_wave_audio_run);
void ags_play_wave_audio_run_dispose(GObject *gobject);
void ags_play_wave_audio_run_finalize(GObject *gobject);

/**
 * SECTION:ags_play_wave_audio_run
 * @short_description: play wave
 * @title: AgsPlayWaveAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_wave_audio_run.h
 *
 * The #AgsPlayWaveAudioRun class play wave.
 */

static gpointer ags_play_wave_audio_run_parent_class = NULL;

GType
ags_play_wave_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_wave_audio_run = 0;

    static const GTypeInfo ags_play_wave_audio_run_info = {
      sizeof (AgsPlayWaveAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayWaveAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_audio_run_init,
    };

    ags_type_play_wave_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							  "AgsPlayWaveAudioRun",
							  &ags_play_wave_audio_run_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_wave_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_play_wave_audio_run_class_init(AgsPlayWaveAudioRunClass *play_wave_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_wave_audio_run_parent_class = g_type_class_peek_parent(play_wave_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_audio_run;

  gobject->dispose = ags_play_wave_audio_run_dispose;
  gobject->finalize = ags_play_wave_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_wave_audio_run;
}

void
ags_play_wave_audio_run_init(AgsPlayWaveAudioRun *play_wave_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) play_wave_audio_run, (AGS_SOUND_ABILITY_WAVE));

  AGS_RECALL(play_wave_audio_run)->name = "ags-play-wave";
  AGS_RECALL(play_wave_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_audio_run)->xml_type = "ags-play-wave-audio-run";
  AGS_RECALL(play_wave_audio_run)->port = NULL;
}

void
ags_play_wave_audio_run_dispose(GObject *gobject)
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_wave_audio_run_finalize(GObject *gobject)
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_run_parent_class)->finalize(gobject);
}

/**
 * ags_play_wave_audio_run_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsPlayWaveAudioRun
 *
 * Returns: the new #AgsPlayWaveAudioRun
 *
 * Since: 3.0.0
 */
AgsPlayWaveAudioRun*
ags_play_wave_audio_run_new(AgsAudio *audio)
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = (AgsPlayWaveAudioRun *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO_RUN,
							     "audio", audio,
							     NULL);

  return(play_wave_audio_run);
}
