/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
#include <ags/audio/recall/ags_envelope_channel.h>

#include <ags/lib/ags_complex.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <stdlib.h>
#include <complex.h>

void ags_envelope_audio_signal_class_init(AgsEnvelopeAudioSignalClass *envelope_audio_signal);
void ags_envelope_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_envelope_audio_signal_init(AgsEnvelopeAudioSignal *envelope_audio_signal);
void ags_envelope_audio_signal_connect(AgsConnectable *connectable);
void ags_envelope_audio_signal_disconnect(AgsConnectable *connectable);
void ags_envelope_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_envelope_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_envelope_audio_signal_finalize(GObject *gobject);

void ags_envelope_audio_signal_run_init_pre(AgsRecall *recall);
void ags_envelope_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_envelope_audio_signal_duplicate(AgsRecall *recall,
					       AgsRecallID *recall_id,
					       guint *n_params, GParameter *parameter);

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
static AgsDynamicConnectableInterface *ags_envelope_audio_signal_parent_dynamic_connectable_interface;

GType
ags_envelope_audio_signal_get_type()
{
  static GType ags_type_envelope_audio_signal = 0;

  if(!ags_type_envelope_audio_signal){
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

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_audio_signal_dynamic_connectable_interface_init,
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

    g_type_add_interface_static(ags_type_envelope_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_envelope_audio_signal);
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

  recall->run_init_pre = ags_envelope_audio_signal_run_init_pre;
  recall->run_inter = ags_envelope_audio_signal_run_inter;  
  recall->duplicate = ags_envelope_audio_signal_duplicate;
}

void
ags_envelope_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_envelope_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_envelope_audio_signal_connect;
  connectable->disconnect = ags_envelope_audio_signal_disconnect;
}

void
ags_envelope_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_envelope_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_envelope_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_envelope_audio_signal_disconnect_dynamic;
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
ags_envelope_audio_signal_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_envelope_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_envelope_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_envelope_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_envelope_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_envelope_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_envelope_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_envelope_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_envelope_audio_signal_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_envelope_audio_signal_parent_class)->finalize(gobject);
}

AgsRecall*
ags_envelope_audio_signal_duplicate(AgsRecall *recall,
				    AgsRecallID *recall_id,
				    guint *n_params, GParameter *parameter)
{
  AgsEnvelopeAudioSignal *envelope;

  envelope = (AgsEnvelopeAudioSignal *) AGS_RECALL_CLASS(ags_envelope_audio_signal_parent_class)->duplicate(recall,
													    recall_id,
													    n_params, parameter);

  return((AgsRecall *) envelope);
}

void
ags_envelope_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsEnvelopeChannel *envelope_channel;
  AgsEnvelopeAudioSignal *envelope_audio_signal;

  AgsAudioSignal *source;
  AgsNote *note;

  gdouble delay;
  gdouble fixed_length;
  guint buffer_size;
  gboolean use_note_length, use_fixed_length;
  
  GValue value = {0,};
  
  /* call parent */
  AGS_RECALL_CLASS(ags_envelope_audio_signal_parent_class)->run_init_pre(recall);

  /*  */
  envelope_channel = AGS_ENVELOPE_CHANNEL(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  envelope_audio_signal = AGS_ENVELOPE_AUDIO_SIGNAL(recall);
  
  source = AGS_RECALL_AUDIO_SIGNAL(envelope_audio_signal)->source;
  note = source->note;

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(source->soundcard));
  buffer_size = source->buffer_size;

  /* get use note length port */
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(envelope_channel->use_note_length,
		     &value);
  
  use_note_length = g_value_get_boolean(&value);
  g_value_unset(&value);

  /* get use fixed length port */
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(envelope_channel->use_fixed_length,
		     &value);
  
  use_fixed_length = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  /* set frame count */
  if(use_note_length){
    envelope_audio_signal->frame_count = (note->x[1] - note->x[0]) * (delay * buffer_size);
  }else if(use_fixed_length){
    g_value_init(&value,
		 G_TYPE_DOUBLE);

    ags_port_safe_read(envelope_channel->fixed_length,
		       &value);

    fixed_length = g_value_get_double(&value);
    g_value_unset(&value);

    envelope_audio_signal->frame_count = fixed_length * (delay * buffer_size);
  }else{
    envelope_audio_signal->frame_count = AGS_RECALL_AUDIO_SIGNAL(recall)->source->frame_count;
  }
}

void
ags_envelope_audio_signal_run_inter(AgsRecall *recall)
{
  AgsEnvelopeAudioSignal *envelope_audio_signal;

  AgsAudioSignal *source;
  AgsNote *note;
  
  AgsComplex *attack;
  AgsComplex *decay;
  AgsComplex *sustain;
  AgsComplex *release;  
  AgsComplex *ratio;  

  GList *stream_source;

  gdouble current_volume, current_ratio;

  guint frame_count;
  guint buffer_size;

  guint i, j;

  GValue audio_value = {0,};
  GValue channel_value = {0,};

  auto gdouble ags_envelope_audio_signal_run_inter_get_ratio(guint x0, gdouble y0,
							     guint x1, gdouble y1);
  auto gdouble ags_envelope_audio_signal_run_inter_get_volume(gdouble volume, gdouble ratio,
							      guint start_x, guint current_x,
							      guint length);
    
  gdouble ags_envelope_audio_signal_run_inter_get_ratio(guint x0, gdouble y0,
							guint x1, gdouble y1){
    if(x1 - x0 == 0){
      return(1.0);
    }else{
      return((y1 - y0) / (x1 - x0));
    }
  }

  gdouble ags_envelope_audio_signal_run_inter_get_volume(gdouble volume, gdouble ratio,
							 guint start_x, guint current_x,
							 guint length){
    if(current_x - start_x == 0){
      return(volume);
    }else{
      return(volume + ratio * (length / (current_x - start_x)));
    }
  }

  AGS_RECALL_CLASS(ags_envelope_audio_signal_parent_class)->run_inter(recall);

  envelope_audio_signal = AGS_ENVELOPE_AUDIO_SIGNAL(recall);

  source = AGS_RECALL_AUDIO_SIGNAL(envelope_audio_signal)->source;

  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);
    return;
  }

  /* initialize some control values */
  frame_count = envelope_audio_signal->frame_count;

  if(source->note != NULL &&
     (AGS_NOTE_ENVELOPE & (AGS_NOTE(source->note)->flags)) != 0){
    gdouble x0, y0;
    gdouble x1, y1;
    guint start_position;
    guint start_frame, first_frame, buffer_size;
    guint current_frame, current_buffer_size;
    guint offset, prev_offset;
    
    note = source->note;

    attack = &(note->attack);
    decay = &(note->decay);
    sustain = &(note->sustain);
    release = &(note->release);

    ratio = &(note->ratio);

    /* get offsets */
    start_position = g_list_position(source->stream_beginning,
				     source->stream_current);

    if(start_position == 0){
      start_frame = 0;
      first_frame = source->first_frame;
      buffer_size = source->buffer_size - source->first_frame;
    }else{
      start_frame = start_position * source->buffer_size - source->first_frame;
      first_frame = 0;
      buffer_size = source->buffer_size;
    }

    /* attack */
    x0 = 0.0;
    y0 = ratio[0][1];

    x1 = attack[0][0];
    y1 = attack[0][1] + ratio[0][1];

    offset = x1 * frame_count;
    current_frame = first_frame;
    
    if(offset >= start_frame){
      if(current_frame + x1 * frame_count > buffer_size){
	current_buffer_size = buffer_size - current_frame;
      }else{
	if(x1 * frame_count > buffer_size){
	  current_buffer_size = buffer_size;
	}else{
	  current_buffer_size = x1 * frame_count;
	}
      }
      
      current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								    x1 * frame_count, y1);
      current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
								      0, start_frame,
								      x1 * frame_count);
      ags_audio_buffer_util_envelope(stream_source->data + current_frame, 1,
				     ags_audio_buffer_util_format_from_soundcard(source->format),
				     current_buffer_size,
				     current_volume,
				     current_ratio);

      current_frame += current_buffer_size;
    }
    
    /* decay */
    x0 = *attack[0];
    y0 = *attack[1] + ratio[0][1];

    x1 = *decay[0];
    y1 = *decay[1] + ratio[0][1];

    prev_offset = offset;
    offset += (x1 * frame_count);

    if(offset >= start_frame &&
       prev_offset < start_frame + buffer_size){
      if(current_frame + x1 * frame_count > buffer_size){
	current_buffer_size = buffer_size - current_frame;
      }else{
	if(x1 * frame_count > buffer_size){
	  current_buffer_size = buffer_size;
	}else{
	  current_buffer_size = x1 * frame_count;
	}
      }
      
      current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								    x1 * frame_count, y1);
      current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
								      0, prev_offset + current_frame,
								      x1 * frame_count);
      ags_audio_buffer_util_envelope(stream_source->data + current_frame, 1,
				     ags_audio_buffer_util_format_from_soundcard(source->format),
				     current_buffer_size,
				     current_volume,
				     current_ratio);

      current_frame += current_buffer_size;
    }

    /* sustain */
    x0 = *decay[0];
    y0 = *decay[1] + ratio[0][1];

    x1 = *sustain[0];
    y1 = *sustain[1] + ratio[0][1];

    prev_offset = offset;
    offset += (x1 * frame_count);

    if(offset >= start_frame &&
       prev_offset < start_frame + buffer_size){
      if(current_frame + x1 * frame_count > buffer_size){
	current_buffer_size = buffer_size - current_frame;
      }else{
	if(x1 * frame_count > buffer_size){
	  current_buffer_size = buffer_size;
	}else{
	  current_buffer_size = x1 * frame_count;
	}
      }
      
      current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								    x1 * frame_count, y1);
      current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
								      0, prev_offset + current_frame,
								      x1 * frame_count);
      ags_audio_buffer_util_envelope(stream_source->data + current_frame, 1,
				     ags_audio_buffer_util_format_from_soundcard(source->format),
				     current_buffer_size,
				     current_volume,
				     current_ratio);

      current_frame += current_buffer_size;
    }
    
    /* release */
    x0 = *sustain[0];
    y0 = *sustain[1] + ratio[0][1];

    x1 = *release[0];
    y1 = *release[1] + ratio[0][1];

    prev_offset = offset;
    offset += (x1 * frame_count);
    
    if(offset >= start_frame &&
       prev_offset < start_frame + buffer_size){
      if(current_frame + x1 * frame_count > buffer_size){
	current_buffer_size = buffer_size - current_frame;
      }else{
	if(x1 * frame_count > buffer_size){
	  current_buffer_size = buffer_size;
	}else{
	  current_buffer_size = x1 * frame_count;
	}
      }
      
      current_ratio = ags_envelope_audio_signal_run_inter_get_ratio(0, y0,
								    x1 * frame_count, y1);
      current_volume = ags_envelope_audio_signal_run_inter_get_volume(y0, current_ratio,
								      0, prev_offset + current_frame,
								      x1 * frame_count);
      ags_audio_buffer_util_envelope(stream_source->data + current_frame, 1,
				     ags_audio_buffer_util_format_from_soundcard(source->format),
				     current_buffer_size,
				     current_volume,
				     current_ratio);

      current_frame += current_buffer_size;
    }
  }
}

/**
 * ags_envelope_audio_signal_new:
 * @source: the source #AgsAudioSignal
 *
 * Creates an #AgsEnvelopeAudioSignal
 *
 * Returns: a new #AgsEnvelopeAudioSignal
 *
 * Since: 1.0.0
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
