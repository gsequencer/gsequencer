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

#include <ags/audio/ags_recycling.h>
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_wave_channel_run = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_wave_channel_run);
  }

  return g_define_type_id__volatile;
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
  ags_recall_set_ability_flags(play_wave_channel_run, (AGS_SOUND_ABILITY_WAVE));

  AGS_RECALL(play_wave_channel_run)->name = "ags-play-wave";
  AGS_RECALL(play_wave_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_channel_run)->xml_type = "ags-play-wave-channel-run";
  AGS_RECALL(play_wave_channel_run)->port = NULL;

  play_wave_channel_run->timestamp = ags_timestamp_new();
  g_object_ref(play_wave_channel_run->timestamp);
  
  play_wave_channel_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_wave_channel_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_wave_channel_run->timestamp->timer.ags_offset.offset = 0;

  play_wave_channel_run->audio_signal = NULL;

  play_wave_channel_run->x_offset = 0;
}

void
ags_play_wave_channel_run_dispose(GObject *gobject)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_channel_run_parent_class)->dispose(gobject);
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
  AgsRecallID *recall_id;

  AgsPlayWaveAudio *play_wave_audio;
  AgsPlayWaveAudioRun *play_wave_audio_run;
  AgsPlayWaveChannel *play_wave_channel;
  AgsPlayWaveChannelRun *play_wave_channel_run;
  
  GObject *output_soundcard;
  
  GList *start_list, *list;
  
  guint line;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint64 x_offset;
  guint64 relative_offset;
  gdouble delay;
  guint frame_count;
  guint attack;
  gboolean do_playback;
  gboolean do_loop;
  
  GValue do_playback_value = {0,};
  GValue do_loop_value = {0,};
  GValue x_offset_value = {0,};

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  auto void ags_play_wave_channel_run_run_inter_add_audio_signal();

  void ags_play_wave_channel_run_run_inter_add_audio_signal(){
    AgsChannel *output;
    AgsRecycling *first_recycling;

    g_object_get(channel,
 		 "first-recycling", &first_recycling,
		 NULL);
	  
    play_wave_channel_run->audio_signal = ags_audio_signal_new(output_soundcard,
							       first_recycling,
							       recall_id);
    g_object_set(play_wave_channel_run->audio_signal,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);
    ags_audio_signal_stream_resize(play_wave_channel_run->audio_signal,
				   3);

    play_wave_channel_run->audio_signal->stream_current = play_wave_channel_run->audio_signal->stream;

    ags_recycling_add_audio_signal(first_recycling,
				   play_wave_channel_run->audio_signal);	  

    ags_connectable_connect(AGS_CONNECTABLE(play_wave_channel_run->audio_signal));
  }
    
  play_wave_channel_run = (AgsPlayWaveChannelRun *) recall;
  
  g_object_get(play_wave_channel_run,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       "recall-audio", &play_wave_audio,
	       "recall-channel", &play_wave_channel,
	       "recall-audio-run", &play_wave_audio_run,
	       NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));
  
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

  x_offset = play_wave_channel_run->x_offset;
  
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
	       "format", &format,
	       "wave", &start_list,
	       NULL);

  g_object_get(channel,
	       "line", &line,
	       NULL);

  frame_count = buffer_size;
  
  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  if(x_offset + frame_count > relative_offset * floor(x_offset / relative_offset) + relative_offset){
    frame_count = relative_offset * floor(x_offset / relative_offset) + relative_offset - x_offset;
  }else{
    frame_count = (x_offset % relative_offset) % buffer_size;
  }

  attack = buffer_size - frame_count;

  /* clear */
  if(play_wave_channel_run->audio_signal != NULL){
    ags_audio_buffer_util_clear_buffer(play_wave_channel_run->audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
  }
  
  /* time stamp offset */
  ags_timestamp_set_ags_offset(play_wave_channel_run->timestamp,
			       (guint64) (relative_offset * floor(x_offset / (relative_offset))));
  
  /* find wave */
  wave = NULL;
  
  list = ags_wave_find_near_timestamp(start_list, line,
				      play_wave_channel_run->timestamp);

  if(list != NULL){
    AgsBuffer *buffer;

    wave = list->data;

    buffer = ags_wave_find_point(wave,
				 x_offset,
				 FALSE);

    if(buffer != NULL){
      guint copy_mode;
      guint current_format;
    
      g_object_get(buffer,
		   "format", &current_format,
		   NULL);
	
      if(play_wave_channel_run->audio_signal == NULL){
	ags_play_wave_channel_run_run_inter_add_audio_signal();
      }

      copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      ags_audio_buffer_util_format_from_soundcard(current_format));
	
      ags_audio_buffer_util_copy_buffer_to_buffer(play_wave_channel_run->audio_signal->stream_current->data, 1, 0,
						  buffer->data, 1, attack,
						  frame_count, copy_mode);
    }
  }

  /* 2nd attempt */
  ags_timestamp_set_ags_offset(play_wave_channel_run->timestamp,
			       relative_offset * floor((x_offset + frame_count) / relative_offset));
  
  /* play */
  if(attack != 0){
    list = ags_wave_find_near_timestamp(start_list, line,
					play_wave_channel_run->timestamp);

    if(list != NULL){
      AgsBuffer *buffer;

      wave = list->data;

      buffer = ags_wave_find_point(wave,
				   x_offset,
				   FALSE);

      if(buffer != NULL){
	guint copy_mode;
	guint current_format;
    
	g_object_get(buffer,
		     "format", &current_format,
		     NULL);
	
	if(play_wave_channel_run->audio_signal == NULL){
	  ags_play_wave_channel_run_run_inter_add_audio_signal();
	}

	copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							ags_audio_buffer_util_format_from_soundcard(current_format));
	
	ags_audio_buffer_util_copy_buffer_to_buffer(play_wave_channel_run->audio_signal->stream_current->data, 1, 0,
						    buffer->data, 1, 0,
						    attack, copy_mode);
      }
    }
  }
  
  /* check loop */
  x_offset += buffer_size;
  
  g_object_get(play_wave_audio,
	       "wave-loop", &port,
	       NULL);

  g_value_init(&do_loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(port,
		     &do_loop_value);

  do_loop = g_value_get_boolean(&do_loop_value);

  if(do_loop){
    guint64 loop_start, loop_end;
    
    GValue loop_start_value = {0,};
    GValue loop_end_value = {0,};
    
    g_object_get(play_wave_audio,
		 "wave-loop-end", &port,
		 NULL);

    g_value_init(&loop_end_value,
		 G_TYPE_UINT64);

    ags_port_safe_read(port,
		       &loop_end_value);

    loop_end = g_value_get_uint64(&loop_end_value);
    
    if(x_offset / buffer_size / delay >= loop_end){
      g_object_get(play_wave_audio,
		   "wave-loop-start", &port,
		   NULL);

      g_value_init(&loop_start_value,
		   G_TYPE_UINT64);

      ags_port_safe_read(port,
			 &loop_start_value);
      
      loop_start = g_value_get_uint64(&loop_start_value);

      x_offset = (relative_offset * floor((delay * buffer_size * loop_start) / relative_offset)) + ((guint64) (delay * buffer_size * loop_start) % relative_offset);
    }
  }
  
  /* new x offset */
  play_wave_channel_run->x_offset = x_offset;

  g_object_get(play_wave_channel,
	       "x-offset", &port,
	       NULL);

  g_value_init(&x_offset_value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&x_offset_value,
		     x_offset);

  ags_port_safe_write(port,
		      &x_offset_value);
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
