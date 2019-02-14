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

#include <ags/audio/recall/ags_envelope_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_envelope_channel.h>
#include <ags/audio/recall/ags_envelope_channel_run.h>
#include <ags/audio/recall/ags_envelope_recycling.h>

#include <stdlib.h>
#include <complex.h>

void ags_envelope_audio_signal_class_init(AgsEnvelopeAudioSignalClass *envelope_audio_signal);
void ags_envelope_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_audio_signal_init(AgsEnvelopeAudioSignal *envelope_audio_signal);
void ags_envelope_audio_signal_finalize(GObject *gobject);

void ags_envelope_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_envelope_audio_signal
 * @short_description: envelopes audio signal
 * @title: AgsEnvelopeAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_envelope_audio_signal.h
 *
 * The #AgsEnvelopeAudioSignal class envelopes the audio signal.
 */

static gpointer ags_envelope_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_envelope_audio_signal_parent_connectable_interface;

GType
ags_envelope_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_audio_signal = 0;

    static const GTypeInfo ags_envelope_audio_signal_info = {
      sizeof (AgsEnvelopeAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							    "AgsEnvelopeAudioSignal",
							    &ags_envelope_audio_signal_info,
							    0);

    g_type_add_interface_static(ags_type_envelope_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_envelope_audio_signal_class_init(AgsEnvelopeAudioSignalClass *envelope_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_envelope_audio_signal_parent_class = g_type_class_peek_parent(envelope_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_audio_signal;

  gobject->finalize = ags_envelope_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) envelope_audio_signal;

  recall->run_inter = ags_envelope_audio_signal_run_inter;  
}

void
ags_envelope_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_envelope_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_envelope_audio_signal_init(AgsEnvelopeAudioSignal *envelope_audio_signal)
{
  AGS_RECALL(envelope_audio_signal)->name = "ags-envelope";
  AGS_RECALL(envelope_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(envelope_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(envelope_audio_signal)->xml_type = "ags-envelope-audio-signal";
  AGS_RECALL(envelope_audio_signal)->port = NULL;

  AGS_RECALL(envelope_audio_signal)->child_type = G_TYPE_NONE;

  envelope_audio_signal->frame_count = 0;
}

void
ags_envelope_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_envelope_audio_signal_parent_class)->finalize(gobject);
}

void
ags_envelope_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source, *rt_template;
  AgsPort *use_note_length;
  AgsPort *use_fixed_length;
  AgsPort *fixed_length;
  AgsEnvelopeChannel *envelope_channel;
  AgsEnvelopeChannelRun *envelope_channel_run;
  AgsEnvelopeRecycling *envelope_recycling;
  AgsEnvelopeAudioSignal *envelope_audio_signal;

  GObject *output_soundcard;
  
  GList *note_start, *note;
  GList *stream_source;

  guint length;
  guint frame_count;
  guint buffer_size;
  guint format;
  guint note_offset, delay_counter;
  gdouble delay;
  gdouble current_fixed_length;

  guint i, j;

  gboolean do_use_note_length, do_use_fixed_length;

  GValue audio_value = {0,};
  GValue channel_value = {0,};
  GValue value = {0,};

  void (*parent_class_run_inter)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  auto gdouble ags_envelope_audio_signal_run_inter_get_ratio(guint x0, gdouble y0,
							     guint x1, gdouble y1);
  auto gdouble ags_envelope_audio_signal_run_inter_get_volume(gdouble volume, gdouble ratio,
							      guint start_x, guint current_x,
							      guint length);
    
  gdouble ags_envelope_audio_signal_run_inter_get_ratio(guint x0, gdouble y0,
							guint x1, gdouble y1)
  {
    if(x1 - x0 == 0){
      return(0.0);
    }else{
      return((y1 - y0) / (x1 - x0));
    }
  }

  gdouble ags_envelope_audio_signal_run_inter_get_volume(gdouble volume, gdouble ratio,
							 guint start_x, guint current_x,
							 guint length)
  {
    gdouble current_volume;
    
    if(length == 0){
      return(volume);
    }else{
      current_volume = volume + (ratio * (current_x - start_x));

#if 0
      g_message("envelope get volume %f %f -> %f", volume, ratio, current_volume);
#endif
      
      return(current_volume);
    }
  }

  envelope_audio_signal = AGS_ENVELOPE_AUDIO_SIGNAL(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_envelope_audio_signal_parent_class)->run_inter;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_inter(recall);

  /* get some fields */
  g_object_get(envelope_audio_signal,
	       "source", &source,
	       NULL);

  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);

    g_object_unref(source);
    
    return;
  }

  g_object_get(envelope_audio_signal,
	       "parent", &envelope_recycling,
	       NULL);

  g_object_get(envelope_recycling,
	       "parent", &envelope_channel_run,
	       NULL);

  g_object_get(envelope_channel_run,
	       "recall-channel", &envelope_channel,
	       NULL);

  /* get ports */
  g_object_get(envelope_channel,
	       "use-note-length", &use_note_length,
	       "use-fixed-length", &use_fixed_length,
	       "fixed-length", &fixed_length,
	       NULL);
  
  /* get use note length port */
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(use_note_length,
		     &value);
  
  do_use_note_length = g_value_get_boolean(&value);

  g_object_unref(use_note_length);

  /* get use fixed length port */
  g_value_reset(&value);

  ags_port_safe_read(use_fixed_length,
		     &value);
  
  do_use_fixed_length = g_value_get_boolean(&value);
  
  g_value_unset(&value);

  g_object_unref(use_fixed_length);

  /* get fixed length */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  ags_port_safe_read(fixed_length,
		     &value);

  current_fixed_length = g_value_get_double(&value);

  g_value_unset(&value);

  g_object_unref(fixed_length);
  
  /* initialize some control values */
  g_object_get(source,
	       "output-soundcard", &output_soundcard,
	       "rt-template", &rt_template,
	       "note", &note_start,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "length", &length,
	       NULL);

  if(ags_recall_global_get_rt_safe() &&
     rt_template != NULL){
    g_object_get(rt_template,
		 "length", &length,
		 NULL);
  }
  
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
  
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));

  note = note_start;
  
  while(note != NULL){
    if(ags_note_test_flags(AGS_NOTE(note->data), AGS_NOTE_ENVELOPE)){
      AgsNote *current;
      
      AgsComplex attack;
      AgsComplex decay;
      AgsComplex sustain;
      AgsComplex release;  
      AgsComplex ratio;  

      gdouble x0, y0;
      gdouble x1, y1;
      gdouble current_x;
      
      guint key_x0, key_x1;
      guint offset;
      guint current_frame;
      guint start_frame, end_frame;      
      guint current_frame_count, trailing_frame_count;
      
      gdouble current_volume, current_ratio;

      pthread_mutex_t *note_mutex;
      
      current = note->data;
            
      g_object_get(current,
		   "x0", &key_x0,
		   "x1", &key_x1,
		   NULL);
      
      /* get note mutex */
      pthread_mutex_lock(ags_note_get_class_mutex());
  
      note_mutex = current->obj_mutex;
  
      pthread_mutex_unlock(ags_note_get_class_mutex());

      /*  */
      pthread_mutex_lock(note_mutex);

      attack[0] = current->attack[0];
      attack[1] = current->attack[1];

      decay[0] = current->decay[0];
      decay[1] = current->decay[1];

      sustain[0] = current->sustain[0];
      sustain[1] = current->sustain[1];

      release[0] = current->release[0];
      release[1] = current->release[1];
      
      ratio[0] = current->ratio[0];
      ratio[1] = current->ratio[1];

      pthread_mutex_unlock(note_mutex);

      /* set frame count */
      frame_count = (key_x1 - key_x0) * (delay * buffer_size);

      if(do_use_note_length){
	frame_count = (key_x1 - key_x0) * (delay * buffer_size);
      }else if(do_use_fixed_length){
	/* calculuate frame count */
	frame_count = current_fixed_length * (delay * buffer_size);
      }else{
	frame_count = length * buffer_size;
      }

      offset = 0;
      
      current_frame = ((note_offset - key_x0 - 1) * delay + delay_counter) * buffer_size;

#if 0
      g_message("note@%x[%d] -- %d", note->data, frame_count, current_frame);
#endif
      
      /* special case release - #0 key offset bigger than note offset */
      if(key_x1 < note_offset){
	current_x = attack[0] + decay[0];
	
	x0 = sustain[0];
	y0 = sustain[1] + ratio[1];

	x1 = release[0];
	y1 = release[1] + ratio[1];

	start_frame = (current_x + x0) * frame_count;
	end_frame = (current_x + x0 + x1) * frame_count;
    	
	current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								      end_frame - start_frame, y1);
	current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									0, end_frame - start_frame,
									end_frame - start_frame);

	ags_audio_buffer_util_volume(stream_source->data, 1,
				     ags_audio_buffer_util_format_from_soundcard(format),
				     buffer_size,
				     current_volume);
	
	/* iterate */
	note = note->next;

	continue;
      }

      /* special case not applicable - #0 key offset lower than note offset */
      if(key_x0 > note_offset){
	/* iterate */
	note = note->next;

	continue;
      }

      current_x = 0.0;
      
      /* attack */
      x0 = 0.0;
      y0 = ratio[1];

      x1 = attack[0];
      y1 = attack[1] + ratio[1];

      start_frame = (current_x + x0) * frame_count;
      end_frame = (current_x + x0 + x1) * frame_count;

#if 0
      g_message("attack - first-frame: %d -> %d::%d", offset, start_frame, end_frame);
#endif
      
      if(start_frame <= current_frame &&
	 end_frame > current_frame){
	if(end_frame - current_frame < buffer_size){
	  current_frame_count = end_frame - current_frame;
	}else{
	  current_frame_count = buffer_size;
	}
      
	current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								      end_frame - start_frame, y1);
	current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									0, current_frame - start_frame,
									end_frame - start_frame);
	ags_audio_buffer_util_envelope(stream_source->data + offset, 1,
				       ags_audio_buffer_util_format_from_soundcard(format),
				       current_frame_count,
				       current_volume,
				       current_ratio);

	current_frame += current_frame_count;
	offset += current_frame_count;
      }    
      
      if(offset >= buffer_size){
	/* iterate */
	note = note->next;

	continue;
      }

      current_x = 0.0;

      /* decay */
      x0 = attack[0];
      y0 = attack[1] + ratio[1];

      x1 = decay[0];
      y1 = decay[1] + ratio[1];

      start_frame = (current_x + x0) * frame_count;
      end_frame = (current_x + x0 + x1) * frame_count;

#if 0
      g_message("decay - first-frame: %d -> %d::%d", offset, start_frame, end_frame);
#endif
      
      if(start_frame <= current_frame &&
	 end_frame > current_frame){
	if(end_frame - current_frame < buffer_size){
	  current_frame_count = end_frame - current_frame;
	}else{
	  current_frame_count = buffer_size - offset;
	}
      
	current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								      end_frame - start_frame, y1);
	current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									0, current_frame - start_frame,
									end_frame - start_frame);
	ags_audio_buffer_util_envelope(stream_source->data + offset, 1,
				       ags_audio_buffer_util_format_from_soundcard(format),
				       current_frame_count,
				       current_volume,
				       current_ratio);

	current_frame += current_frame_count;
	offset += current_frame_count;
      }    

      if(offset >= buffer_size){
	/* iterate */
	note = note->next;

	continue;
      }

      current_x = attack[0];

      /* sustain */
      x0 = decay[0];
      y0 = decay[1] + ratio[1];

      x1 = sustain[0];
      y1 = sustain[1] + ratio[1];

      start_frame = (current_x + x0) * frame_count;
      end_frame = (current_x + x0 + x1) * frame_count;

#if 0
      g_message("sustain - first-frame: %d -> %d::%d", offset, start_frame, end_frame);
#endif
      
      if(start_frame <= current_frame &&
	 end_frame > current_frame){
	if(end_frame - current_frame < buffer_size){
	  current_frame_count = end_frame - current_frame;
	}else{
	  current_frame_count = buffer_size - offset;
	}
      
	current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								      end_frame - start_frame, y1);
	current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									0, current_frame - start_frame,
									end_frame - start_frame);
	ags_audio_buffer_util_envelope(stream_source->data + offset, 1,
				       ags_audio_buffer_util_format_from_soundcard(format),
				       current_frame_count,
				       current_volume,
				       current_ratio);

	current_frame += current_frame_count;
	offset += current_frame_count;
      }    
    
      if(offset >= buffer_size){
	/* iterate */
	note = note->next;

	continue;
      }

      current_x = decay[0] + sustain[0];

      /* release */
      x0 = sustain[0];
      y0 = sustain[1] + ratio[1];

      x1 = release[0];
      y1 = release[1] + ratio[1];

      start_frame = (current_x + x0) * frame_count;
      end_frame = (current_x + x0 + x1) * frame_count;
    
#if 0
      g_message("release - first-frame: %d -> %d::%d", offset, start_frame, end_frame);
#endif
      
      if(start_frame <= current_frame &&
	 end_frame > current_frame){
	trailing_frame_count = 0;

	if(end_frame - current_frame < buffer_size){
	  current_frame_count = end_frame - current_frame;

	  trailing_frame_count = buffer_size - current_frame_count;
	}else{
	  current_frame_count = buffer_size - offset;
	}
      
	current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								      end_frame - start_frame, y1);
	current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									0, current_frame - start_frame,
									end_frame - start_frame);
	ags_audio_buffer_util_envelope(stream_source->data + offset, 1,
				       ags_audio_buffer_util_format_from_soundcard(format),
				       current_frame_count,
				       current_volume,
				       current_ratio);

	offset += current_frame_count;

	if(trailing_frame_count != 0){
	  current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
									  0, end_frame - start_frame,
									  end_frame);

	  ags_audio_buffer_util_volume(stream_source->data + offset, 1,
				       ags_audio_buffer_util_format_from_soundcard(format),
				       trailing_frame_count,
				       current_volume);
	}
      }
    }

    note = note->next;
  }

  /* unref */
  g_object_unref(source);

  if(rt_template != NULL){
    g_object_unref(rt_template);
  }
  
  g_object_unref(envelope_recycling);

  g_object_unref(envelope_channel_run);

  g_object_unref(envelope_channel);

  g_object_unref(output_soundcard);
  
  g_list_free_full(note_start,
		   g_object_unref);
}

/**
 * ags_envelope_audio_signal_new:
 * @source: the source #AgsAudioSignal
 *
 * Create a new instance of #AgsEnvelopeAudioSignal
 *
 * Returns: the new #AgsEnvelopeAudioSignal
 *
 * Since: 2.0.0
 */
AgsEnvelopeAudioSignal*
ags_envelope_audio_signal_new(AgsAudioSignal *source)
{
  AgsEnvelopeAudioSignal *envelope_audio_signal;

  envelope_audio_signal = (AgsEnvelopeAudioSignal *) g_object_new(AGS_TYPE_ENVELOPE_AUDIO_SIGNAL,
								  "source", source,
								  NULL);

  return(envelope_audio_signal);
}
