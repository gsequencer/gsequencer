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
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_play_wave_audio.h>
#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_channel.h>

#include <math.h>

#include <ags/i18n.h>

void ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run);
void ags_play_wave_channel_run_seekable_interface_init(AgsSeekableInterface *seekable);
void ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run);
void ags_play_wave_channel_run_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_play_wave_channel_run_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_play_wave_channel_run_dispose(GObject *gobject);
void ags_play_wave_channel_run_finalize(GObject *gobject);

void ags_play_wave_channel_run_seek(AgsSeekable *seekable,
				    gint64 offset,
				    guint whence);

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

enum{
  PROP_0,
  PROP_X_OFFSET,
};

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

    static const GInterfaceInfo ags_seekable_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_channel_run_seekable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_wave_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							    "AgsPlayWaveChannelRun",
							    &ags_play_wave_channel_run_info,
							    0);

    g_type_add_interface_static(ags_type_play_wave_channel_run,
				AGS_TYPE_SEEKABLE,
				&ags_seekable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_wave_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_play_wave_channel_run_parent_class = g_type_class_peek_parent(play_wave_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_channel_run;

  gobject->set_property = ags_play_wave_channel_run_set_property;
  gobject->get_property = ags_play_wave_channel_run_get_property;

  gobject->dispose = ags_play_wave_channel_run_dispose;
  gobject->finalize = ags_play_wave_channel_run_finalize;

  /* properties */
  /**
   * AgsPlayWaveChannelRun:x-offset:
   *
   * The x offset.
   * 
   * Since: 2.1.24
   */
  param_spec = g_param_spec_uint64("x-offset",
				   i18n_pspec("x offset"),
				   i18n_pspec("The x offset in the wave"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_OFFSET,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_wave_channel_run;

  recall->run_inter = ags_play_wave_channel_run_run_inter;
}

void
ags_play_wave_channel_run_seekable_interface_init(AgsSeekableInterface *seekable)
{
  seekable->seek = ags_play_wave_channel_run_seek;
}

void
ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) play_wave_channel_run, (AGS_SOUND_ABILITY_WAVE));

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
ags_play_wave_channel_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  pthread_mutex_t *recall_mutex;

  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_X_OFFSET:
    {
      guint64 x_offset;

      x_offset = g_value_get_uint64(value);

      pthread_mutex_lock(recall_mutex);

      play_wave_channel_run->x_offset = x_offset;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_wave_channel_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;
  
  pthread_mutex_t *recall_mutex;

  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_X_OFFSET:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint64(value, play_wave_channel_run->x_offset);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
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
ags_play_wave_channel_run_seek(AgsSeekable *seekable,
			       gint64 offset,
			       guint whence)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  GObject *soundcard;

  gdouble absolute_delay;
  guint buffer_size;
  guint64 x_offset;
      
  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(seekable);

  g_object_get(play_wave_channel_run,
	       "output-soundcard", &soundcard,
	       "buffer-size", &buffer_size,
	       NULL);

  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(soundcard));
  
  switch(whence){
  case AGS_SEEK_CUR:
    {
      g_object_get(play_wave_channel_run,
		   "x-offset", &x_offset,
		   NULL);

      if(x_offset + (offset * absolute_delay * buffer_size) < 0){
	x_offset = 0;
      }else{
	x_offset = x_offset + (offset * absolute_delay * buffer_size);
      }

      g_object_set(play_wave_channel_run,
		   "x-offset", x_offset,
		   NULL);
    }
    break;
  case AGS_SEEK_END:
    {
      g_warning("seek from end not implemented");
    }
    break;
  case AGS_SEEK_SET:
    {
      x_offset = offset * absolute_delay * buffer_size;
      
      g_object_set(play_wave_channel_run,
		   "x-offset", x_offset,
		   NULL);
    }
    break;
  }

  g_object_unref(soundcard);
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
  guint64 x_point_offset;
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
    g_object_unref(first_recycling);
    
    play_wave_channel_run->audio_signal = ags_audio_signal_new(output_soundcard,
							       (GObject *) first_recycling,
							       (GObject *) recall_id);
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

  g_object_unref(port);
  
  if(!do_playback){
    g_object_unref(output_soundcard);
    
    g_object_unref(recall_id);

    g_object_unref(play_wave_audio);

    g_object_unref(play_wave_channel);

    g_object_unref(play_wave_audio_run);

    return;
  }

  g_object_get(play_wave_channel_run,
	       "x-offset", &x_offset,
	       NULL);
  
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

  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  attack = (x_offset % relative_offset) % buffer_size;

  frame_count = buffer_size - attack;

  if(x_offset + frame_count > relative_offset * floor(x_offset / relative_offset) + relative_offset){
    frame_count = relative_offset * floor((x_offset + frame_count) / relative_offset) - x_offset;
  }
  
  /* clear */
  if(play_wave_channel_run->audio_signal != NULL){
    ags_audio_buffer_util_clear_buffer(play_wave_channel_run->audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
  }
  
  /* time stamp offset */
  ags_timestamp_set_ags_offset(play_wave_channel_run->timestamp,
			       (guint64) (relative_offset * floor((double) x_offset / (double) relative_offset)));
  
  /* find wave */
  wave = NULL;
  
  list = ags_wave_find_near_timestamp(start_list, line,
				      play_wave_channel_run->timestamp);

  if(list != NULL){
    AgsBuffer *buffer;

    wave = list->data;

    x_point_offset = x_offset - attack;
    buffer = ags_wave_find_point(wave,
				 x_point_offset,
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
			       (guint64) relative_offset * floor((double) (x_offset + frame_count) / (double) relative_offset));
  
  /* play */
  if(attack != 0 ||
     frame_count != buffer_size){
    list = ags_wave_find_near_timestamp(start_list, line,
					play_wave_channel_run->timestamp);

    if(list != NULL){
      AgsBuffer *buffer;

      wave = list->data;

      x_point_offset = x_offset + frame_count;
      buffer = ags_wave_find_point(wave,
				   x_point_offset,
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
	
	ags_audio_buffer_util_copy_buffer_to_buffer(play_wave_channel_run->audio_signal->stream_current->data, 1, frame_count,
						    buffer->data, 1, 0,
						    buffer_size - frame_count, copy_mode);
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
  g_value_unset(&do_loop_value);
  
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
    g_value_unset(&loop_end_value);
    
    if(x_offset / buffer_size / delay >= loop_end){
      g_object_get(play_wave_audio,
		   "wave-loop-start", &port,
		   NULL);

      g_value_init(&loop_start_value,
		   G_TYPE_UINT64);

      ags_port_safe_read(port,
			 &loop_start_value);
      
      loop_start = g_value_get_uint64(&loop_start_value);
      g_value_unset(&loop_start_value);

      x_offset = (relative_offset * floor((delay * buffer_size * loop_start) / relative_offset)) + ((guint64) (delay * buffer_size * loop_start) % relative_offset);
    }
  }
  
  /* new x offset */
  g_object_set(play_wave_channel_run,
	       "x-offset", x_offset,
	       NULL);

  g_object_get(play_wave_channel,
	       "x-offset", &port,
	       NULL);

  g_value_init(&x_offset_value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&x_offset_value,
		     x_offset);

  ags_port_safe_write(port,
		      &x_offset_value);

  g_value_unset(&x_offset_value);
  g_object_unref(port);

  /* unref */
  g_object_unref(output_soundcard);
    
  g_object_unref(recall_id);

  g_object_unref(play_wave_audio);

  g_object_unref(play_wave_channel);

  g_object_unref(play_wave_audio_run);

  g_object_unref(audio);

  g_object_unref(channel);

  g_list_free_full(start_list,
		   g_object_unref);
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
