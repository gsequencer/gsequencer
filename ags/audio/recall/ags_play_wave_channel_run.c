/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_play_wave_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_wave_audio.h>
#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_channel.h>

#include <ags/i18n.h>

void ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run);
void ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run);
void ags_play_wave_channel_run_dispose(GObject *gobject);
void ags_play_wave_channel_run_finalize(GObject *gobject);

void ags_play_wave_channel_run_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_play_wave_channel_run
 * @short_description: play wave
 * @title: AgsPlayWaveChannelRun
 * @section_id:
 * @include: ags/channel/recall/ags_play_wave_channel_run.h
 *
 * The #AgsPlayWaveChannelRun class play wave.
 */

static gpointer ags_play_wave_channel_run_parent_class = NULL;

GType
ags_play_wave_channel_run_get_type()
{
  static GType ags_type_play_wave_channel_run = 0;

  if(!ags_type_play_wave_channel_run){
    static const GTypeInfo ags_play_wave_channel_run_info = {
      sizeof (AgsPlayWaveChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayWaveChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_channel_run_init,
    };

    ags_type_play_wave_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							    "AgsPlayWaveChannelRun",
							    &ags_play_wave_channel_run_info,
							    0);
  }

  return(ags_type_play_wave_channel_run);
}

void
ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_wave_channel_run_parent_class = g_type_class_peek_parent(play_wave_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_channel_run;

  gobject->dispose = ags_play_wave_channel_run_dispose;
  gobject->finalize = ags_play_wave_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_wave_channel_run;

  recall->run_inter = ags_play_wave_channel_run_run_inter;
}

void
ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run)
{
  AGS_RECALL(play_wave_channel_run)->name = "ags-play-wave";
  AGS_RECALL(play_wave_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_channel_run)->xml_type = "ags-play-wave-channel-run";
  AGS_RECALL(play_wave_channel_run)->port = NULL;

  play_wave_channel_run->timestamp = ags_timestamp_new();

  play_wave_channel_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_wave_channel_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_wave_channel_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_wave_channel_run_finalize(GObject *gobject)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(gobject);

  /* timestamp */
  if(play_wave_channel_run->timestamp != NULL){
    g_object_unref(G_OBJECT(play_wave_channel_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_channel_run_parent_class)->finalize(gobject);
}

void
ags_play_wave_channel_run_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsWave *wave;
  AgsPort *port;
  AgsPlayWaveAudio *play_wave_audio;
  AgsPlayWaveAudioRun *play_wave_audio_run;
  AgsPlayWaveChannel *play_wave_channel;
  AgsPlayWaveChannelRun *play_wave_channel_run;
  
  GList *start_list, *list;

  guint line;
  guint samplerate;
  guint buffer_size;
  guint64 x_offset;
  gboolean do_playback;
  
  GValue do_playback_value = {0,};
  GValue x_offset_value = {0,};

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  play_wave_channel_run = (AgsPlayWaveChannelRun *) recall;
  
  g_object_get(play_wave_channel_run,
	       "recall-audio", &play_wave_audio,
	       "recall-channel", &play_wave_channel,
	       "recall-audio-run", &play_wave_audio_run,
	       NULL);
  
  /* get do playback */
  g_object_get(play_wave_channel,
	       "do-playback", &port,
	       NULL);
  
  g_value_init(&do_playback_value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(port,
		     &do_playback_value);

  do_playback = g_value_get_boolean(&do_playback_value);
  
  g_value_unset(&do_playback_value);
  
  if(!do_playback){
    return;
  }

  /* get x-offset */
  g_object_get(play_wave_channel,
	       "x-offset", &port,
	       NULL);

  g_value_init(&x_offset_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(port,
		     &x_offset_value);

  x_offset = g_value_get_uint64(&x_offset_value);

  g_value_unset(&x_offset_value);

  /* get some fields */
  g_object_get(play_wave_audio_run,
	       "audio", &audio,
	       NULL);

  g_object_get(play_wave_channel_run,
	       "source", &channel,
	       NULL);

  g_object_get(audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "wave", &start_list,
	       NULL);

  g_object_get(channel,
	       "line", &line,
	       NULL);

  /* time stamp offset */
  ags_timestamp_set_ags_offset(play_wave_channel_run->timestamp,
			       (guint64) ((64.0 * (double) samplerate) * floor(x_offset / (64.0 * (double) samplerate))));
  
  /* find wave */
  wave = NULL;
  
  list = ags_wave_find_near_timestamp(start_list, line,
				      play_wave_channel_run->timestamp);

  if(list != NULL){
    wave = list->data;
  }

  //TODO:JK: implement me
}

/**
 * ags_play_wave_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPlayWaveChannelRun
 *
 * Returns: the new #AgsPlayWaveChannelRun
 *
 * Since: 2.0.0
 */
AgsPlayWaveChannelRun*
ags_play_wave_channel_run_new(AgsChannel *source)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  play_wave_channel_run = (AgsPlayWaveChannelRun *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL_RUN,
								 "source", source,
								 NULL);

  return(play_wave_channel_run);
}
