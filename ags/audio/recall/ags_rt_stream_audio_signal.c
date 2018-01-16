/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_rt_stream_channel.h>
#include <ags/audio/recall/ags_rt_stream_channel_run.h>
#include <ags/audio/recall/ags_rt_stream_recycling.h>
#include <ags/audio/recall/ags_rt_stream_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_unref_audio_signal.h>

void ags_rt_stream_audio_signal_class_init(AgsRtStreamAudioSignalClass *rt_stream_audio_signal);
void ags_rt_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_rt_stream_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_rt_stream_audio_signal_init(AgsRtStreamAudioSignal *rt_stream_audio_signal);
void ags_rt_stream_audio_signal_connect(AgsConnectable *connectable);
void ags_rt_stream_audio_signal_disconnect(AgsConnectable *connectable);
void ags_rt_stream_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_audio_signal_dispose(GObject *gobject);
void ags_rt_stream_audio_signal_finalize(GObject *gobject);

void ags_rt_stream_audio_signal_run_init_pre(AgsRecall *recall);
void ags_rt_stream_audio_signal_run_pre(AgsRecall *recall);
AgsRecall* ags_rt_stream_audio_signal_duplicate(AgsRecall *recall,
						AgsRecallID *recall_id,
						guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_rt_stream_audio_signal
 * @short_description: rt_streams audio signal
 * @title: AgsRtStreamAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_audio_signal.h
 *
 * The #AgsRtStreamAudioSignal class streams the audio signal template.
 */

static gpointer ags_rt_stream_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_rt_stream_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_rt_stream_audio_signal_parent_dynamic_connectable_interface;

GType
ags_rt_stream_audio_signal_get_type()
{
  static GType ags_type_rt_stream_audio_signal = 0;

  if(!ags_type_rt_stream_audio_signal){
    static const GTypeInfo ags_rt_stream_audio_signal_info = {
      sizeof (AgsRtStreamAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_rt_stream_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRtStreamAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_rt_stream_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_rt_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsRtStreamAudioSignal",
							     &ags_rt_stream_audio_signal_info,
							     0);

    g_type_add_interface_static(ags_type_rt_stream_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_rt_stream_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_rt_stream_audio_signal);
}

void
ags_rt_stream_audio_signal_class_init(AgsRtStreamAudioSignalClass *rt_stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_rt_stream_audio_signal_parent_class = g_type_class_peek_parent(rt_stream_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_audio_signal;

  gobject->dispose = ags_rt_stream_audio_signal_dispose;
  gobject->finalize = ags_rt_stream_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) rt_stream_audio_signal;

  recall->run_init_pre = ags_rt_stream_audio_signal_run_init_pre;
  recall->run_pre = ags_rt_stream_audio_signal_run_pre;
  recall->duplicate = ags_rt_stream_audio_signal_duplicate;
}

void
ags_rt_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_rt_stream_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_rt_stream_audio_signal_connect;
  connectable->disconnect = ags_rt_stream_audio_signal_disconnect;
}

void
ags_rt_stream_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_rt_stream_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_rt_stream_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_rt_stream_audio_signal_disconnect_dynamic;
}

void
ags_rt_stream_audio_signal_init(AgsRtStreamAudioSignal *rt_stream_audio_signal)
{
  AGS_RECALL(rt_stream_audio_signal)->name = "ags-rt_stream";
  AGS_RECALL(rt_stream_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(rt_stream_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(rt_stream_audio_signal)->xml_type = "ags-rt_stream-audio-signal";
  AGS_RECALL(rt_stream_audio_signal)->port = NULL;

  AGS_RECALL(rt_stream_audio_signal)->child_type = G_TYPE_NONE;

  rt_stream_audio_signal->dispose_source = NULL;
}

void
ags_rt_stream_audio_signal_dispose(GObject *gobject)
{
  AGS_RT_STREAM_AUDIO_SIGNAL(gobject)->dispose_source = AGS_RECALL_AUDIO_SIGNAL(gobject)->source;

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_audio_signal_parent_class)->dispose(gobject); 
}

void
ags_rt_stream_audio_signal_finalize(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_RT_STREAM_AUDIO_SIGNAL(gobject)->dispose_source;
  
  if(audio_signal != NULL){
    AgsRecycling *recycling;

    recycling = audio_signal->recycling;
    
    if(recycling != NULL){
      ags_recycling_remove_audio_signal(recycling,
					audio_signal);
    }
    
    g_object_run_dispose(audio_signal);
    g_object_unref(audio_signal);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_rt_stream_audio_signal_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_rt_stream_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_rt_stream_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_rt_stream_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_rt_stream_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_rt_stream_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_rt_stream_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_rt_stream_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_rt_stream_audio_signal_run_init_pre(AgsRecall *recall)
{
  /* call parent */
  AGS_RECALL_CLASS(ags_rt_stream_audio_signal_parent_class)->run_init_pre(recall);

  //  g_message("rt_stream");
}

void
ags_rt_stream_audio_signal_run_pre(AgsRecall *recall)
{
  AgsRtStreamAudioSignal *rt_stream_audio_signal;

  AgsRecycling *recycling;
  AgsAudioSignal *source;
  AgsAudioSignal *template;

  AgsMutexManager *mutex_manager;

  GList *note;

  void *buffer;

  gdouble delay;
  guint buffer_size;
  guint copy_mode;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *recycling_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  AGS_RECALL_CLASS(ags_rt_stream_audio_signal_parent_class)->run_inter(recall);

  rt_stream_audio_signal = AGS_RT_STREAM_AUDIO_SIGNAL(recall);

  source = AGS_RECALL_AUDIO_SIGNAL(rt_stream_audio_signal)->source;

  buffer = source->stream_beginning->data;
  buffer_size = source->buffer_size;

  /* lookup soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) source->soundcard);
	
  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(soundcard_mutex);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(source->soundcard));

  pthread_mutex_unlock(soundcard_mutex);

  /* lookup recycling mutex */
  recycling = source->recycling;

  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* get template */
  pthread_mutex_lock(recycling_mutex);
  
  template = ags_audio_signal_get_template(recycling->audio_signal);
  
  note = source->note;
  
  ags_audio_buffer_util_clear_buffer(buffer, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(source->format));

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(source->format),
						  ags_audio_buffer_util_format_from_soundcard(template->format));

  while(note != NULL){
    AgsNote *current;

    GList *stream, *template_stream;

    guint64 offset;
    
    current = note->data;
    offset = current->rt_offset;
    
    if(offset < delay * current->x[1] ||
       offset < template->length){
      if(template->loop_start < template->loop_end){
	guint frame_count;
	guint loop_length;
	guint loop_frame_count;
	guint n_frames;
	guint copy_n_frames;
	guint nth_loop;
	guint j_start;
	guint i, j, k;

	frame_count = delay * (AGS_NOTE(note->data)->x[1] - AGS_NOTE(note->data)->x[0]) * buffer_size;
	
	loop_length = template->loop_end - template->loop_start;
	loop_frame_count = ((frame_count - template->loop_start) / loop_length) * template->buffer_size;

	if(offset * buffer_size > template->loop_end){
	  if(((guint) offset * buffer_size) + buffer_size > frame_count - (template->frame_count - template->loop_end)){
	    if(offset * buffer_size > frame_count - (template->frame_count - template->loop_end)){
	      j_start = (frame_count - (offset * buffer_size)) % buffer_size;

	      template_stream = g_list_nth(template->stream_beginning,
					   (frame_count - (offset * buffer_size)) / buffer_size);

	      if(template_stream == NULL){
		note = note->next;

		continue;
	      }	      
	    }else{
	      j_start = ((guint) (offset * buffer_size) - template->loop_end) % (template->loop_end - template->loop_start) % buffer_size;

	      template_stream = g_list_nth(template->stream_beginning,
					   (template->loop_start + ((((guint) (offset * buffer_size) - template->loop_end) % (template->loop_end - template->loop_start)) / buffer_size)));

	      if(template_stream == NULL){
		note = note->next;

		continue;
	      }
	    }
	  }else{
	    j_start = ((guint) (offset * buffer_size) - template->loop_end) % (template->loop_end - template->loop_start) % buffer_size;
	    
	    template_stream = g_list_nth(template->stream_beginning,
					 (template->loop_start + ((((guint) (offset * buffer_size) - template->loop_end) % (template->loop_end - template->loop_start)) / buffer_size)));

	    if(template_stream == NULL){
	      note = note->next;

	      continue;
	    }
	  }
	}else{
	  j_start = ((guint) (offset * buffer_size)) % buffer_size;

	  template_stream = g_list_nth(template->stream_beginning,
				       offset);

	  if(template_stream == NULL){
	    note = note->next;

	    continue;
	  }
	}

	if(offset == 0){
	  k = AGS_NOTE(note->data)->rt_attack;
	}else{
	  k = 0;
	}
	
	for(i = 0, j = j_start, nth_loop = offset; i < buffer_size;){
	  /* compute count of frames to copy */
	  copy_n_frames = buffer_size;

	  /* limit nth loop */
	  if(i > template->loop_start &&
	     i + copy_n_frames > template->loop_start + loop_length &&
	     i + copy_n_frames < template->loop_start + loop_frame_count &&
	     i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length){
	    copy_n_frames = (template->loop_start + (nth_loop + 1) * loop_length) - i;
	  }

	  /* check boundaries */
	  if((k % buffer_size) + copy_n_frames > buffer_size){
	    copy_n_frames = buffer_size - (k % buffer_size);
	  }

	  if(j + copy_n_frames > buffer_size){
	    copy_n_frames = buffer_size - j;
	  }

	  if(buffer == NULL ||
	     template_stream == NULL){
	    break;
	  }
    
	  /* copy */
	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, k % buffer_size,
						      template_stream->data, 1, j,
						      copy_n_frames, copy_mode);

	  /* increment and iterate */
	  if((i + copy_n_frames) % buffer_size == 0){
	    break;
	  }

	  if(j + copy_n_frames == template->buffer_size){
	    template_stream = template_stream->next;
	  }
    
	  if(template_stream == NULL ||
	     (i > template->loop_start &&
	      i + copy_n_frames > template->loop_start + loop_length &&
	      i + copy_n_frames < template->loop_start + loop_frame_count &&
	      i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length)){
	    j = template->loop_start % template->buffer_size;
	    template_stream = g_list_nth(template->stream_beginning,
					 floor(template->loop_start / template->buffer_size));

	    nth_loop++;
	  }else{
	    j += copy_n_frames;
	  }
    
	  i += copy_n_frames;
	  k += copy_n_frames;

	  if(j == template->buffer_size){
	    j = 0;
	  }
	}
	
      }else{
	template_stream = g_list_nth(template->stream_beginning,
			    offset);

	if(template_stream == NULL){
	  note = note->next;

	  continue;
	}
	
	if(offset == 0){
	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, AGS_NOTE(note->data)->rt_attack,
						      template_stream->data, 1, 0,
						      buffer_size - AGS_NOTE(note->data)->rt_attack, copy_mode);
	}else{
	  if(AGS_NOTE(note->data)->rt_attack != 0 && template_stream->prev != NULL){
	    ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
							template_stream->prev->data, 1, buffer_size - AGS_NOTE(note->data)->rt_attack,
							AGS_NOTE(note->data)->rt_attack, copy_mode);
	  }

	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, AGS_NOTE(note->data)->rt_attack,
						      template_stream->data, 1, 0,
						      buffer_size - AGS_NOTE(note->data)->rt_attack, copy_mode);	  
	}
      }
    }else{
      ags_audio_signal_remove_note(source,
				   current);
    }

    current->rt_offset += 1;
    
    note = note->next;
  }

  pthread_mutex_unlock(recycling_mutex);
}

AgsRecall*
ags_rt_stream_audio_signal_duplicate(AgsRecall *recall,
				     AgsRecallID *recall_id,
				     guint *n_params, GParameter *parameter)
{
  AgsRtStreamAudioSignal *copy;

  copy = (AgsRtStreamAudioSignal *) AGS_RECALL_CLASS(ags_rt_stream_audio_signal_parent_class)->duplicate(recall,
													 recall_id,
													 n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_rt_stream_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsRtStreamAudioSignal
 *
 * Returns: a new #AgsRtStreamAudioSignal
 *
 * Since: 1.4.0
 */
AgsRtStreamAudioSignal*
ags_rt_stream_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsRtStreamAudioSignal *rt_stream_audio_signal;

  rt_stream_audio_signal = (AgsRtStreamAudioSignal *) g_object_new(AGS_TYPE_RT_STREAM_AUDIO_SIGNAL,
								   "source", audio_signal,
								   NULL);

  return(rt_stream_audio_signal);
}
