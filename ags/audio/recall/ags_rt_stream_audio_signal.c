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

#include <ags/audio/recall/ags_rt_stream_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_rt_stream_channel.h>
#include <ags/audio/recall/ags_rt_stream_channel_run.h>
#include <ags/audio/recall/ags_rt_stream_recycling.h>

void ags_rt_stream_audio_signal_class_init(AgsRtStreamAudioSignalClass *rt_stream_audio_signal);
void ags_rt_stream_audio_signal_init(AgsRtStreamAudioSignal *rt_stream_audio_signal);
void ags_rt_stream_audio_signal_dispose(GObject *gobject);
void ags_rt_stream_audio_signal_finalize(GObject *gobject);

void ags_rt_stream_audio_signal_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_rt_stream_audio_signal
 * @short_description: rt streams audio signal
 * @title: AgsRtStreamAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_audio_signal.h
 *
 * The #AgsRtStreamAudioSignal class streams the audio signal template.
 */

static gpointer ags_rt_stream_audio_signal_parent_class = NULL;

GType
ags_rt_stream_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_rt_stream_audio_signal = 0;

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

    ags_type_rt_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsRtStreamAudioSignal",
							     &ags_rt_stream_audio_signal_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_rt_stream_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_rt_stream_audio_signal_class_init(AgsRtStreamAudioSignalClass *rt_stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_rt_stream_audio_signal_parent_class = g_type_class_peek_parent(rt_stream_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_audio_signal;

  gobject->dispose = ags_rt_stream_audio_signal_dispose;
  gobject->finalize = ags_rt_stream_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) rt_stream_audio_signal;

  recall->run_pre = ags_rt_stream_audio_signal_run_pre;
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
  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_audio_signal_parent_class)->dispose(gobject); 
}

void
ags_rt_stream_audio_signal_finalize(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = (AgsAudioSignal *) AGS_RT_STREAM_AUDIO_SIGNAL(gobject)->dispose_source;
  
  if(audio_signal != NULL){
    AgsRecycling *recycling;

    recycling = (AgsRecycling *) audio_signal->recycling;
    
    if(recycling != NULL){
      ags_recycling_remove_audio_signal(recycling,
					audio_signal);
    }
    
    g_object_run_dispose((GObject *) audio_signal);
    g_object_unref((GObject *) audio_signal);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_rt_stream_audio_signal_run_pre(AgsRecall *recall)
{
  AgsRecycling *recycling;
  AgsAudioSignal *source;
  AgsAudioSignal *rt_template;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context, *parent_recycling_context;
  AgsRtStreamAudioSignal *rt_stream_audio_signal;

  GList *start_note, *note;

  void *buffer;

  gdouble delay;
  guint rt_template_length;
  guint rt_template_frame_count;
  guint loop_start, loop_end;
  guint buffer_size;
  guint rt_template_buffer_size;
  guint source_format, rt_template_format;
  guint copy_mode;

  void (*parent_class_run_pre)(AgsRecall *recall);

  /* get parent class */
  AGS_RECALL_LOCK_CLASS();
  
  parent_class_run_pre = AGS_RECALL_CLASS(ags_rt_stream_audio_signal_parent_class)->run_pre;

  AGS_RECALL_UNLOCK_CLASS();
  
  /* call parent */
  parent_class_run_pre(recall);

  /* get some fields */
  rt_stream_audio_signal = AGS_RT_STREAM_AUDIO_SIGNAL(recall);

  g_object_get(rt_stream_audio_signal,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);
  
  buffer = source->stream->data;

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &source_format,
	       "delay", &delay,
	       "recycling", &recycling,
	       "rt-template", &rt_template,
	       "note", &start_note,
	       NULL);

  /* get template */
  if(rt_template == NULL){
    g_object_unref(source);

    g_object_unref(recall_id);

    g_list_free_full(start_note,
		     g_object_unref);
    
    return;
  }

  g_object_get(rt_template,
	       "buffer-size", &rt_template_buffer_size,
	       "format", &rt_template_format,
	       "length", &rt_template_length,
	       "frame-count", &rt_template_frame_count,
	       "loop-start", &loop_start,
	       "loop-end", &loop_end,
	       NULL);
    
  ags_audio_buffer_util_clear_buffer(buffer, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(source_format));

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(source_format),
						  ags_audio_buffer_util_format_from_soundcard(rt_template_format));

  /* check note */
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  note = start_note;
  
  while(note != NULL){
    AgsNote *current;

    GList *stream, *template_stream;

    guint note_x0, note_x1;
    guint rt_attack;
    guint64 rt_offset;
    
    current = note->data;

    g_object_get(current,
		 "rt-offset", &rt_offset,
		 "x0", &note_x0,
		 "x1", &note_x1,
		 NULL);
    rt_attack = 0;
    
    if(rt_offset < rt_template_length ||
       rt_offset < delay * note_x1){
      if(loop_start < loop_end){
	guint frame_count;
	guint loop_length;
	guint loop_frame_count;
	guint n_frames;
	guint copy_n_frames;
	guint nth_loop;
	guint j_start;
	guint i, j, k;

	frame_count = delay * (note_x1 - note_x0) * buffer_size;
	
	loop_length = loop_end - loop_start;
	loop_frame_count = ((frame_count - loop_start) / loop_length) * rt_template_buffer_size;

	if(rt_offset * buffer_size > loop_end){
	  if(((guint) rt_offset * buffer_size) + buffer_size > frame_count - (rt_template_frame_count - loop_end)){
	    if(rt_offset * buffer_size > frame_count - (rt_template_frame_count - loop_end)){
	      j_start = (frame_count - (rt_offset * buffer_size)) % buffer_size;

	      template_stream = g_list_nth(rt_template->stream,
					   (frame_count - (rt_offset * buffer_size)) / buffer_size);

	      if(template_stream == NULL){
		note = note->next;

		continue;
	      }	      
	    }else{
	      j_start = ((guint) (rt_offset * buffer_size) - loop_end) % (loop_end - loop_start) % buffer_size;

	      template_stream = g_list_nth(rt_template->stream,
					   (loop_start + ((((guint) (rt_offset * buffer_size) - loop_end) % (loop_end - loop_start)) / buffer_size)));

	      if(template_stream == NULL){
		note = note->next;

		continue;
	      }
	    }
	  }else{
	    j_start = ((guint) (rt_offset * buffer_size) - loop_end) % (loop_end - loop_start) % buffer_size;
	    
	    template_stream = g_list_nth(rt_template->stream,
					 (loop_start + ((((guint) (rt_offset * buffer_size) - loop_end) % (loop_end - loop_start)) / buffer_size)));

	    if(template_stream == NULL){
	      note = note->next;

	      continue;
	    }
	  }
	}else{
	  j_start = ((guint) (rt_offset * buffer_size)) % buffer_size;

	  template_stream = g_list_nth(rt_template->stream,
				       rt_offset);

	  if(template_stream == NULL){
	    note = note->next;

	    continue;
	  }
	}

	if(rt_offset == 0){
	  k = rt_attack;
	}else{
	  k = 0;
	}
	
	for(i = 0, j = j_start, nth_loop = rt_offset; i < buffer_size;){
	  /* compute count of frames to copy */
	  copy_n_frames = buffer_size;

	  /* limit nth loop */
	  if(i > loop_start &&
	     i + copy_n_frames > loop_start + loop_length &&
	     i + copy_n_frames < loop_start + loop_frame_count &&
	     i + copy_n_frames >= loop_start + (nth_loop + 1) * loop_length){
	    copy_n_frames = (loop_start + (nth_loop + 1) * loop_length) - i;
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

	  if(j + copy_n_frames == rt_template_buffer_size){
	    template_stream = template_stream->next;
	  }
    
	  if(template_stream == NULL ||
	     (i > loop_start &&
	      i + copy_n_frames > loop_start + loop_length &&
	      i + copy_n_frames < loop_start + loop_frame_count &&
	      i + copy_n_frames >= loop_start + (nth_loop + 1) * loop_length)){
	    j = loop_start % rt_template_buffer_size;
	    template_stream = g_list_nth(rt_template->stream,
					 floor(loop_start / rt_template_buffer_size));

	    nth_loop++;
	  }else{
	    j += copy_n_frames;
	  }
    
	  i += copy_n_frames;
	  k += copy_n_frames;

	  if(j == rt_template_buffer_size){
	    j = 0;
	  }
	}
	
      }else{
	template_stream = g_list_nth(rt_template->stream,
				     rt_offset);

	if(template_stream == NULL){
	  note = note->next;

	  continue;
	}
	
	if(rt_offset == 0){
	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, rt_attack,
						      template_stream->data, 1, 0,
						      buffer_size - rt_attack, copy_mode);
	}else{
	  if(rt_attack != 0 && template_stream->prev != NULL){
	    ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
							template_stream->prev->data, 1, buffer_size - rt_attack,
							rt_attack, copy_mode);
	  }

	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, rt_attack,
						      template_stream->data, 1, 0,
						      buffer_size - rt_attack, copy_mode);	  
	}
      }
    }else{
      GList *start_list;

      ags_audio_signal_remove_note(source,
				   (GObject *) current);

      g_object_get(source,
		   "note", &start_list,
		   NULL);

      if(start_list == NULL &&
	 parent_recycling_context == NULL){
	ags_recall_done(recall);
      }

      g_list_free(start_list);
    }

    g_object_set(current,
		 "rt-offset", rt_offset + 1,
		 NULL);
    
    note = note->next;
  }

  /* unref */
  g_object_unref(source);

  g_object_unref(recall_id);

  g_list_free_full(start_note,
		   g_object_unref);

  g_object_unref(recycling_context);
  g_object_unref(parent_recycling_context);
}

/**
 * ags_rt_stream_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsRtStreamAudioSignal
 *
 * Returns: the new #AgsRtStreamAudioSignal
 *
 * Since: 2.0.0
 */
AgsRtStreamAudioSignal*
ags_rt_stream_audio_signal_new(AgsAudioSignal *source)
{
  AgsRtStreamAudioSignal *rt_stream_audio_signal;

  rt_stream_audio_signal = (AgsRtStreamAudioSignal *) g_object_new(AGS_TYPE_RT_STREAM_AUDIO_SIGNAL,
								   "source", source,
								   NULL);

  return(rt_stream_audio_signal);
}
