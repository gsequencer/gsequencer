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

#include <ags/audio/ags_audio_signal.h>

#include <ags/object/ags_marshal.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_config.h>
#include <ags/audio/ags_devout.h>

#include <stdint.h>
#include <emmintrin.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * SECTION:ags_audio_signal
 * @short_description: Contains the audio data and its alignment
 * @title: AgsAudioSignal
 * @section_id:
 * @include: ags/audio/ags_audio_signal.h
 *
 * #AgsAudioSignal organizes audio data within a #GList whereby data
 * pointing to the buffer.
 */

void ags_audio_signal_class_init(AgsAudioSignalClass *audio_signal_class);
void ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_signal_init(AgsAudioSignal *audio_signal);
void ags_audio_signal_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_audio_signal_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_audio_signal_finalize(GObject *gobject);
void ags_audio_signal_connect(AgsConnectable *connectable);
void ags_audio_signal_disconnect(AgsConnectable *connectable);

void ags_audio_signal_real_realloc_buffer_size(AgsAudioSignal *audio_signal, guint buffer_size);
void ags_audio_signal_real_morph_samplerate(AgsAudioSignal *audio_signal, guint samplerate, double k_morph);

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_RECYCLING,
  PROP_RECALL_ID,
};

enum{
  REALLOC_BUFFER_SIZE,
  MORPH_SAMPLERATE,
  LAST_SIGNAL,
};

extern AgsConfig *config;

static gpointer ags_audio_signal_parent_class = NULL;
static guint audio_signal_signals[LAST_SIGNAL];

GType
ags_audio_signal_get_type(void)
{
  static GType ags_type_audio_signal = 0;

  if(!ags_type_audio_signal){
    static const GTypeInfo ags_audio_signal_info = {
      sizeof (AgsAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_signal = g_type_register_static(G_TYPE_OBJECT,
						   "AgsAudioSignal\0",
						   &ags_audio_signal_info,
						   0);

    g_type_add_interface_static(ags_type_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_audio_signal);
}

void
ags_audio_signal_class_init(AgsAudioSignalClass *audio_signal)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_signal_parent_class = g_type_class_peek_parent(audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_signal;

  gobject->set_property = ags_audio_signal_set_property;
  gobject->get_property = ags_audio_signal_get_property;

  gobject->finalize = ags_audio_signal_finalize;

  /* properties */
  /**
   * AgsAudioSignal:devout:
   *
   * The assigned #AgsDevout providing default settings.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /**
   * AgsAudioSignal:recycling:
   *
   * The assigned #AgsRecycling linking tree.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recycling\0",
				   "assigned recycling\0",
				   "The recycling it is assigned with\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  /**
   * AgsAudioSignal:recall-id:
   *
   * The assigned #AgsRecallID providing context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall_id\0",
				   "assigned recall id\0",
				   "The recall id it is assigned with\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /* AgsAudioSignalClass */
  audio_signal->realloc_buffer_size = ags_audio_signal_real_realloc_buffer_size;
  audio_signal->morph_samplerate = ags_audio_signal_real_morph_samplerate;

  /* signals */
  /**
   * AgsAudioSignal::realloc-buffer-size:
   * @audio_signal: the object to realloc buffer size
   * @buffer_size: new buffer size
   *
   * The ::reallloc-buffer-size signal is invoked to notify modified buffer size.
   */
  audio_signal_signals[REALLOC_BUFFER_SIZE] =
    g_signal_new("realloc-buffer-size\0",
		 G_TYPE_FROM_CLASS (audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsAudioSignalClass, realloc_buffer_size),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsAudioSignal::morph-samplerate:
   * @audio_signal: the object to morph samplerate
   * @samplerate: new samplerate
   *
   * The ::morph-samplerate signal is invoked to notify modified samplerate.
   */
  audio_signal_signals[MORPH_SAMPLERATE] =
    g_signal_new("morph-samplerate\0",
		 G_TYPE_FROM_CLASS (audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsAudioSignalClass, morph_samplerate),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_DOUBLE,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_DOUBLE);
}

void
ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_audio_signal_connect;
  connectable->disconnect = ags_audio_signal_disconnect;
}

void
ags_audio_signal_init(AgsAudioSignal *audio_signal)
{
  audio_signal->flags = 0;

  audio_signal->devout = NULL;

  audio_signal->recycling = NULL;
  audio_signal->recall_id = NULL;

  audio_signal->samplerate = g_ascii_strtoull(ags_config_get(config,
							     AGS_CONFIG_DEVOUT,
							     "samplerate\0"),
					      NULL,
					      10);
  audio_signal->buffer_size = g_ascii_strtoull(ags_config_get(config,
							      AGS_CONFIG_DEVOUT,
							      "buffer-size\0"),
					       NULL,
					       10);
  audio_signal->resolution = AGS_DEVOUT_RESOLUTION_16_BIT;

  audio_signal->length = 0;
  audio_signal->last_frame = 0;
  audio_signal->loop_start = 0;
  audio_signal->loop_end = 0;

  audio_signal->delay = 0.0;
  audio_signal->attack = 0;

  audio_signal->lock_attack = 0;

  audio_signal->stream_beginning = NULL;
  audio_signal->stream_current = NULL;
  audio_signal->stream_end = NULL;
}

void
ags_audio_signal_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      GObject *devout;

      devout = g_value_get_object(value);

      if(audio_signal->devout == devout)
	return;

      if(audio_signal->devout != NULL)
	g_object_unref(audio_signal->devout);

      if(devout != NULL)
	g_object_ref(devout);

      audio_signal->devout = devout;
    }
    break;
  case PROP_RECYCLING:
    {
      GObject *recycling;

      recycling = g_value_get_object(value);

      if(audio_signal->recycling == recycling)
	return;

      if(audio_signal->recycling != NULL)
	g_object_unref(audio_signal->recycling);

      if(recycling != NULL)
	g_object_ref(recycling);

      audio_signal->recycling = recycling;
    }
    break;
  case PROP_RECALL_ID:
    {
      GObject *recall_id;

      recall_id = g_value_get_object(value);
      
      if(audio_signal->recall_id == recall_id)
	return;
      
      if(audio_signal->recall_id != NULL)
	g_object_unref(audio_signal->recall_id);

      if(recall_id != NULL)
	g_object_ref(recall_id);

      audio_signal->recall_id = recall_id;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_signal_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, audio_signal->devout);
    break;
  case PROP_RECYCLING:
    g_value_set_object(value, audio_signal->recycling);
    break;
  case PROP_RECALL_ID:
    g_value_set_object(value, audio_signal->recall_id);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_signal_finalize(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

#ifdef AGS_DEBUG
  g_message("finalize AgsAudioSignal\0");
#endif

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0)
    g_warning("AGS_AUDIO_SIGNAL_TEMPLATE: destroying\n\0");

  if(audio_signal->devout != NULL)
    g_object_unref(audio_signal->devout);

  if(audio_signal->recycling != NULL)
    g_object_unref(audio_signal->recycling);

  if(audio_signal->recall_id != NULL)
    g_object_unref(audio_signal->recall_id);

  if(audio_signal->stream_beginning != NULL){
    g_list_free_full(audio_signal->stream_beginning,
		     g_free);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->finalize(gobject);
}

void
ags_audio_signal_connect(AgsConnectable *connectable)
{
  // empty
}

void
ags_audio_signal_disconnect(AgsConnectable *connectable)
{
  // empty
}

/**
 * ags_stream_alloc:
 * @buffer_size: the buffer size
 *
 * Allocs an audio buffer.
 *
 * Returns: the audio data array
 *
 * Since: 0.3
 */
signed short*
ags_stream_alloc(guint buffer_size)
{
  signed short *buffer;

  buffer = (signed short *) malloc(buffer_size * sizeof(signed short));
  memset(buffer, 0, buffer_size * sizeof(signed short));

  return(buffer);
}

/**
 * ags_audio_signal_get_length_till_current:
 * @audio_signal: an #AgsAudioSignal
 *
 * Counts the buffers from stream_beginning upto stream_current.
 *
 * Returns: the counted length.
 *
 * Since: 0.4
 */
guint
ags_audio_signal_get_length_till_current(AgsAudioSignal *audio_signal)
{
  GList *list, *stop;
  guint length;

  if(audio_signal == NULL){
    return(0);
  }

  list = audio_signal->stream_beginning;
  length = 0;

  if(audio_signal->stream_current != NULL){
    stop = audio_signal->stream_current->next;
  }else{
    return(0);
  }

  while(list != stop){
    length++;
    list = list->next;
  }

  return(length);
}

/**
 * ags_audio_signal_add_stream:
 * @audio_signal: an #AgsAudioSignal
 *
 * Adds a buffer at the end of the stream.
 *
 * Since: 0.3
 */
void
ags_audio_signal_add_stream(AgsAudioSignal *audio_signal)
{
  GList *stream, *end_old;
  signed short *buffer;

  stream = g_list_alloc();
  buffer = ags_stream_alloc(audio_signal->buffer_size);
  stream->data = buffer;

  if(audio_signal->stream_end != NULL){
    end_old = audio_signal->stream_end;

    stream->prev = end_old;
    end_old->next = stream;
  }else{
    audio_signal->stream_beginning = stream;
    audio_signal->stream_current = stream;
  }

  audio_signal->stream_end = stream;

  audio_signal->length += 1;
}

/**
 * ags_audio_signal_stream_resize:
 * @audio_signal: an #AgsAudioSignal to resize.
 * @length: a guint as the new length.
 *
 * Resizes an #AgsAudioSignal's stream but be carefull with shrinking.
 * This function may crash the application.
 *
 * Since: 0.3
 */
void
ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint i;

  if(audio_signal->length < length){
    GList *stream, *end_old;
    signed short *buffer;

    stream = NULL;

    for(i = audio_signal->length; i < length; i++){
      buffer = ags_stream_alloc(audio_signal->buffer_size);

      stream = g_list_prepend(stream,
			      buffer);
    }

    stream = g_list_reverse(stream);

    if(audio_signal->stream_end != NULL){
      end_old = audio_signal->stream_end;
      audio_signal->stream_end = g_list_last(stream);

      stream->prev = end_old;
      end_old->next = stream;
    }else{
      audio_signal->stream_beginning = stream;
      audio_signal->stream_end = g_list_last(stream);
      audio_signal->stream_current = stream;
    }
  }else if(audio_signal->length > length){
    GList *stream, *stream_end, *stream_next;
    gboolean check_current;

    stream = audio_signal->stream_beginning;
    check_current = TRUE;

    for(i = 0; i < length; i++){
      if(check_current && stream == audio_signal->stream_current){
	audio_signal->stream_current = NULL;
	check_current = FALSE;
      }

      stream = stream->next;
    }

    if(length != 0){
      stream_end = stream->prev;
      stream_end->next = NULL;
      audio_signal->stream_end = stream_end;
    }else{
      audio_signal->stream_beginning = NULL;
      audio_signal->stream_current = NULL;
      audio_signal->stream_end = NULL;
    }

    stream->prev = NULL;
    g_list_free_full(stream,
		     g_free);
  }

  audio_signal->length = length;
}

/**
 * ags_audio_signal_stream_safe_resize:
 * @audio_signal: an #AgsAudioSignal
 * @length: a guint
 *
 * Resizes an #AgsAudioSignal's stream but doesn't shrink more than the
 * current stream position.
 *
 * Since: 0.4
 */
void
ags_audio_signal_stream_safe_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint length_till_current;

  length_till_current = ags_audio_signal_get_length_till_current(audio_signal);

  if(length_till_current < length){
    ags_audio_signal_stream_resize(audio_signal,
				   length);
  }else{
    ags_audio_signal_stream_resize(audio_signal,
				   length_till_current);
  }
}

void
ags_audio_signal_real_realloc_buffer_size(AgsAudioSignal *audio_signal, guint buffer_size)
{
  GList *current, *old; 
  guint old_buffer_size;
  guint counter;
  guint i_old, i_current;

  old = audio_signal->stream_beginning;
  old_buffer_size = audio_signal->buffer_size;

  current = NULL;
  current = g_list_prepend(current,
			   ags_stream_alloc(buffer_size));
  counter = 0;

  i_old = 0;
  i_current = 0;
  
  while(old != NULL){
    ((signed short *) current->data)[i_current] = ((signed short *) old->data)[i_old];
    
    if(i_current == buffer_size){
      current = g_list_prepend(current,
			       ags_stream_alloc(buffer_size));
      
      i_current = 0;
      
      counter++;
    }
    
    if(i_old == old_buffer_size){
      old = old->next;
      
      i_old = 0;
    }
    
    i_current++;
    i_old++;
  }
  
  memset(((signed short *) current->data), 0, (buffer_size - i_current) * sizeof(signed short));
  
  /*  */
  audio_signal->length = counter;
  audio_signal->last_frame = i_current;
  
  audio_signal->stream_end = current;
  audio_signal->stream_beginning = g_list_reverse(current);
}

/**
 * ags_audio_signal_realloc_buffer_size:
 * @audio_signal: an #AgsAudioSignal
 * @buffer_size: the buffer size
 *
 * Realloc the stream to the new buffer size. 
 *
 * Since: 0.4
 */
void
ags_audio_signal_realloc_buffer_size(AgsAudioSignal *audio_signal, guint buffer_size)
{
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(audio_signal));
  g_object_ref(G_OBJECT(audio_signal));
  g_signal_emit(G_OBJECT(audio_signal),
		audio_signal_signals[REALLOC_BUFFER_SIZE], 0,
		buffer_size);
  g_object_unref(G_OBJECT(audio_signal));
}

void
ags_audio_signal_real_morph_samplerate(AgsAudioSignal *audio_signal, guint samplerate, double k_morph)
{
  GList *current, *old, *tmp; 
  guint old_samplerate;
  guint counter;
  guint i_old, i_current;
  guint j_old, j_current;
  double k_old, k_current;
  guint frame;
  double factor, value, preview;

  old = audio_signal->stream_beginning;
  old_samplerate = audio_signal->samplerate;

  current = NULL;
  current = g_list_prepend(current,
			   ags_stream_alloc(audio_signal->buffer_size));
  counter = 0;

  i_old = 0;
  i_current = 0;

  factor = samplerate / old_samplerate;

  if(k_morph == 0.0){
    k_morph = (samplerate < old_samplerate) ? 2.0 * factor: 0.5 * factor;
  }

  while(old != NULL){
    value = (double) (((signed short *) old->data)[i_old]);

    for(j_old = 0, j_current = 0; (((samplerate < old_samplerate) && (j_old < ceil(1.0 / factor))) ||
				   (j_current < ceil(factor))); j_old++, j_current++){
      preview = 0.0;

      for(frame = 0; frame < ceil(factor); frame++){
	//	preview += ;
      }

      for(frame = 0; frame < ceil(factor); frame++){
	//	preview += ;
      }

      for(k_old = 0.0, k_current = 0.0; (((k_morph < 1.0) && (k_old < ceil(1.0 / k_morph))) ||
					 (k_current < ceil(k_morph))); k_old++, k_current++){
	//	value = value * ( / );
      }
    }

    ((signed short *) current->data)[i_current] = value;

    if(i_current == samplerate){
      current = g_list_prepend(current,
			       ags_stream_alloc(audio_signal->buffer_size));

      i_current = 0;

      counter++;
    }

    if(i_old == old_samplerate){
      old = old->next;

      i_old = 0;
    }

    i_current++;
    i_old++;
  }

  /*  */
  audio_signal->length = counter;
  audio_signal->last_frame = i_current;

  audio_signal->stream_end = current;
  audio_signal->stream_beginning = g_list_reverse(current);
}

/**
 * ags_audio_signal_morph_samplerate:
 * @audio_signal: an #AgsAudioSignal
 * @samplerate: the new samplerate
 * @k_morph: reserved for future usage
 *
 * Morph audio quality to new samplerate.
 *
 * Since: 0.4
 */
void
ags_audio_signal_morph_samplerate(AgsAudioSignal *audio_signal, guint samplerate, double k_morph)
{
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(audio_signal));
  g_object_ref(G_OBJECT(audio_signal));
  g_signal_emit(G_OBJECT(audio_signal),
		audio_signal_signals[MORPH_SAMPLERATE], 0,
		samplerate, k_morph);
  g_object_unref(G_OBJECT(audio_signal));
}

/**
 * ags_audio_signal_copy_buffer_to_buffer:
 * @destination: destination buffer
 * @dchannels: destination audio channels
 * @source: source buffer
 * @schannels: source audio channels
 * @size: frame count to copy
 *
 * Copy a buffer to an other buffer.
 *
 * Since: 0.3
 */
void
ags_audio_signal_copy_buffer_to_buffer(signed short *destination, guint dchannels, signed short *source, guint schannels, guint size)
{
  signed long value;

  for(; 0<size; --size){
    //    *destination += *source;
    value = 0xffff & ((*destination) + (*source));
    *destination = (signed short) value;
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_signal_copy_buffer_to_double_buffer:
 * @destination: destination buffer
 * @dchannels: destination audio channels
 * @source: source buffer
 * @schannels: source audio channels
 * @size: frame count to copy
 *
 * Copy a buffer to an other buffer.
 *
 * Since: 0.4
 */
void
ags_audio_signal_copy_buffer_to_double_buffer(double *destination, guint dchannels,
					      signed short *source, guint schannels,
					      guint size)
{
  for(; 0<size; --size){
    *destination += *source;

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_signal_copy_double_buffer_to_buffer:
 * @destination: destination buffer
 * @dchannels: destination audio channels
 * @source: source buffer
 * @schannels: source audio channels
 * @size: frame count to copy
 *
 * Copy a buffer to an other buffer.
 *
 * Since: 0.4
 */
void
ags_audio_signal_copy_double_buffer_to_buffer(signed short *destination, guint dchannels,
					      double *source, guint schannels,
					      guint size)
{
  for(; 0<size; --size){
    *destination += *source;

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_signal_duplicate_stream:
 * @audio_signal: destination #AgsAudioSignal
 * @template: source #AgsAudioSignal
 * 
 * Copy stream of one audio signal to another
 *
 * Since: 0.3
 */
void
ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				  AgsAudioSignal *template)
{
  if(template == NULL){
    if(audio_signal == NULL){
      return;
    }else{
      ags_audio_signal_stream_resize(audio_signal,
				     0);
      return;
    }
  }

  if(template->stream_beginning == NULL){
    ags_audio_signal_stream_resize(audio_signal,
				   0);
  }else{
    //    AgsDevout *devout;
    GList *template_stream, *stream;
    //    guint i, j, j_offcut;
    //    guint k, template_k;

    audio_signal->buffer_size = template->buffer_size;
    ags_audio_signal_stream_resize(audio_signal,
				   template->length);

    stream = audio_signal->stream_beginning;
    template_stream = template->stream_beginning;

    //    k = 0;
    //    template_k = 0;

    //TODO:JK: enhance me
    while(template_stream != NULL){
      ags_audio_signal_copy_buffer_to_buffer(stream->data, 1,
					     template_stream->data, 1,
					     template->buffer_size);

      stream = stream->next;
      template_stream = template_stream->next;
    }
  }
}

/**
 * ags_audio_signal_get_template:
 * @audio_signal: a #GList containing #AgsAudioSignal
 *
 * Retrieve the template audio signal.
 *
 * Returns: the template #AgsAudioSignal
 *
 * Since: 0.3
 */
AgsAudioSignal*
ags_audio_signal_get_template(GList *audio_signal)
{
  GList *list;

  list = audio_signal;

  while(list != NULL){
    if((AGS_AUDIO_SIGNAL_TEMPLATE & (AGS_AUDIO_SIGNAL(list->data)->flags)) != 0)
      return((AgsAudioSignal *) list->data);

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_get_stream_current:
 * @audio_signal: a #GList containing #AgsAudioSignal
 * @recall_id: the matching #AgsRecallID
 * 
 * Retrieve next current stream of #AgsAudioSignal list.
 *
 * Returns: next #GList matching #AgsRecallID
 *
 * Since: 0.4
 */
GList*
ags_audio_signal_get_stream_current(GList *list_audio_signal,
				    GObject *recall_id)
{
  AgsAudioSignal *audio_signal;
  GList *list;

  list = list_audio_signal;

  while(list != NULL){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);

    if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
      list = list->next;
      continue;
    }

    if(audio_signal->stream_current != NULL && audio_signal->recall_id == recall_id)
      return(list);

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_get_by_recall_id:
 * @audio_signal: a #GList containing #AgsAudioSignal
 * @recall_id: matching #AgsRecallID
 *
 * Retrieve next audio signal refering to @recall_id
 *
 * Returns: matching #AgsAudioSignal
 *
 * Since: 0.3
 */
GList*
ags_audio_signal_get_by_recall_id(GList *list_audio_signal,
				  GObject *recall_id)
{
  AgsAudioSignal *audio_signal;
  GList *list;

  list = list_audio_signal;

  while(list != NULL){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);

    if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
      list = list->next;
      continue;
    }

    if(audio_signal->recall_id == recall_id)
      return(list);

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_tile:
 * @audio_signal: an #AgsAudioSignal
 * @template: the source #AgsAudioSignal
 * @frame_count: new frame count
 *
 * Tile audio signal data.
 *
 * Since: 0.4
 */
void
ags_audio_signal_tile(AgsAudioSignal *audio_signal,
		      AgsAudioSignal *template,
		      guint frame_count)
{
  AgsDevout *devout;
  GList *template_stream, *audio_signal_stream, *audio_signal_stream_end;
  signed short *template_buffer, *audio_signal_buffer;
  guint template_size;
  guint remaining_size;
  guint i, j, j_offcut;
  guint k, k_end;
  gboolean alloc_buffer;
  
  devout = AGS_DEVOUT(audio_signal->devout);

  audio_signal_stream = NULL;
  template_stream = template->stream_beginning;

  if(template_stream == NULL){
    return;
  }

  template_size = (guint) (template->delay * template->buffer_size) +
    template->length * template->buffer_size +
    template->last_frame;

  j = 0;
  k = 0;

  if(template->buffer_size > audio_signal->buffer_size){
    k_end = audio_signal->buffer_size;
  }else if(template->buffer_size == audio_signal->buffer_size){
    k_end = audio_signal->buffer_size;
  }else{
    k_end = template->buffer_size;
  }

  j_offcut = 0;
  alloc_buffer = TRUE;

  /* write buffers */
  for(i = 0; i < frame_count - template->buffer_size; i += audio_signal->buffer_size){
    /* alloc buffer and prepend */
    if(alloc_buffer){
      audio_signal_buffer = ags_stream_alloc(audio_signal->buffer_size);
      audio_signal_stream = g_list_prepend(audio_signal_stream,
					   audio_signal_buffer);
    }

    /* get template buffer */
    template_buffer = (signed short *) template_stream->data;

    /* allocate and copy buffer */
    if(template_size < audio_signal->buffer_size){
      /* copy buffer */
      ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					     &(template_buffer[j_offcut]), 1, template_size - j_offcut);

      for(j = template_size - j_offcut; j < audio_signal->buffer_size - template_size; j += template_size){
	ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					       template_buffer, 1, template_size);
      }

      j_offcut = audio_signal->buffer_size - j;
      ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					     template_buffer, 1, j_offcut);
    }else{
      /* deep copy */
      ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					     &(template_buffer[j_offcut]), 1, k_end - j_offcut);
      k += (k_end - j_offcut);

      /* iterate template stream */
      if(k_end + j_offcut == template->buffer_size){
	if(template_stream->next != NULL){
	  template_stream = template_stream->next;
	}else{
	  template_stream = template->stream_beginning;
	}
      }

      /* copy parameters */
      j = k % audio_signal->buffer_size;
	
      if(template->buffer_size < audio_signal->buffer_size){
	j_offcut = k % template->buffer_size;
      }else if(template->buffer_size == audio_signal->buffer_size){
	j_offcut = k % audio_signal->buffer_size;
      }else{
	j_offcut = k % audio_signal->buffer_size;
      }

      k_end = template->buffer_size - j_offcut;

      /* alloc audio signal */
      if(j == 0){
	alloc_buffer = TRUE;
      }else{
	alloc_buffer = FALSE;
      }
    }
  }
  
  /* write remaining buffer */
  remaining_size = frame_count - (i * audio_signal->buffer_size);

  if(remaining_size > k_end - j_offcut){
    ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					   &(template_buffer[j_offcut]), 1, k_end - j_offcut);

    if(k_end + j_offcut == template->buffer_size){
      if(template_stream->next != NULL){
	template_stream = template_stream->next;
      }else{
	template_stream = template->stream_beginning;
      }
    }

    ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					   &(template_buffer[j_offcut]), 1, remaining_size - (k_end - j_offcut));
  }else{
    ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					   &(template_buffer[j_offcut]), 1, remaining_size);
  }

  /* reverse list */
  audio_signal_stream_end = audio_signal_stream;
  audio_signal_stream = g_list_reverse(audio_signal_stream);

  if(audio_signal->stream_beginning != NULL){
    ags_list_free_and_free_link(audio_signal->stream_beginning);
  }

  audio_signal->stream_beginning = audio_signal_stream;
  //  audio_signal->stream_current = audio_signal_stream;
  audio_signal->stream_end = audio_signal_stream_end;
}

/**
 * ags_audio_signal_scale:
 * @audio_signal: an #AgsAudioSignal
 * @template: the source #AgsAudioSignal
 * @length: new frame count
 *
 * Scale audio signal data.
 *
 * Since: 0.4
 */
void
ags_audio_signal_scale(AgsAudioSignal *audio_signal,
		       AgsAudioSignal *template,
		       guint length)
{
  GList *source, *destination, *stream_template;
  gpointer data;
  double scale_factor, morph_factor;
  guint offset;
  double step;
  guint i, j, j_stop;
  guint k, template_k;
  gboolean expand;

  auto void ags_audio_signal_scale_copy_8_bit(GList *source, GList *destination,
					      guint soffset, guint doffset,
					      guint dresolution);
  auto void ags_audio_signal_scale_copy_16_bit(GList *source, GList *destination,
					       guint soffset, guint doffset,
					       guint dresolution);
  auto void ags_audio_signal_scale_copy_24_bit(GList *source, GList *destination,
					       guint soffset, guint doffset,
					       guint dresolution);
  auto void ags_audio_signal_scale_copy_32_bit(GList *source, GList *destination,
					       guint soffset, guint doffset,
					       guint dresolution);
  auto void ags_audio_signal_scale_copy_64_bit(GList *source, GList *destination,
					       guint soffset, guint doffset,
					       guint dresolution);

  void ags_audio_signal_scale_copy_8_bit(GList *source, GList *destination,
					 guint soffset, guint doffset,
					 guint dresolution){
    gint8 *sbuffer;

    sbuffer = (gint8 *) source->data;

    switch(dresolution){
    case AGS_DEVOUT_RESOLUTION_8_BIT:
      {
	gint8 *dbuffer;
	gdouble scale;

	dbuffer = (gint8 *) destination->data;

	scale = 1.0;

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_16_BIT:
      {
	gint16 *dbuffer;
	gdouble scale;

	dbuffer = (gint16 *) destination->data;

	scale = exp2(8.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_24_BIT:
      {
	unsigned char *dbuffer;
	gint16 value;
	gdouble scale;
	gint16 mask;

	dbuffer = (unsigned char *) destination->data;

	scale = exp2(16.0);

	value = scale * sbuffer[soffset];
	mask = 0xff;

	dbuffer[doffset * 3] = mask & value;
	dbuffer[doffset * 3 + 1] = (mask << 8) & value;
	
	if(sbuffer[soffset] < 0){
	  dbuffer[doffset * 3 + 1] &= (~0x80);
	  dbuffer[doffset * 3 + 2] = 0x80;
	}else{
	  dbuffer[doffset * 3 + 2] = 0;
	}
      }
      break;
    case AGS_DEVOUT_RESOLUTION_32_BIT:
      {
	gint32 *dbuffer;
	gdouble scale;

	dbuffer = (gint32 *) destination->data;

	scale = exp2(24.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_64_BIT:
      {
	gint64 *dbuffer;
	gdouble scale;

	dbuffer = (gint64 *) destination->data;

	scale = exp2(56.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    }
  }
  void ags_audio_signal_scale_copy_16_bit(GList *source, GList *destination,
					  guint soffset, guint doffset,
					  guint dresolution){
    gint16 *sbuffer;

    sbuffer = (gint16 *) source->data;

    switch(dresolution){
    case AGS_DEVOUT_RESOLUTION_8_BIT:
      {
	gint8 *dbuffer;
	gdouble scale;

	dbuffer = (gint8 *) destination->data;

	scale = exp2(1.0 / 8.0);

	dbuffer[doffset] = (gint8) floor(scale * sbuffer[soffset]);
      }
      break;
    case AGS_DEVOUT_RESOLUTION_16_BIT:
      {
	gint16 *dbuffer;

	dbuffer = (gint16 *) destination->data;

	dbuffer[doffset] = sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_24_BIT:
      {
	unsigned char *dbuffer;
	gint32 value;
	gdouble scale;
	gint32 mask;

	dbuffer = (unsigned char *) destination->data;

	scale = exp2(8.0);
	mask = 0xff;

	value = scale * sbuffer[soffset];
      
	dbuffer[doffset * 3] = mask & value;
	dbuffer[doffset * 3 + 1] = (mask << 8) & value;
	dbuffer[doffset * 3 + 2] = (mask << 16) & value;
	
	if(sbuffer[soffset] < 0){
	  dbuffer[doffset * 3 + 2] |= 0x80;
	}
      }
      break;
    case AGS_DEVOUT_RESOLUTION_32_BIT:
      {
	gint32 *dbuffer;
	gdouble scale;

	dbuffer = (gint32 *) destination->data;

	scale = exp2(16.0);
	
	dbuffer[doffset] = sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_64_BIT:
      {
	gint64 *dbuffer;
	gdouble scale;

	dbuffer = (gint64 *) destination->data;

	scale = exp2(48.0);
	
	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    }
  }
  void ags_audio_signal_scale_copy_24_bit(GList *source, GList *destination,
					  guint soffset, guint doffset,
					  guint dresolution){
    unsigned char *sbuffer;

    sbuffer = (char *) source->data;

    switch(dresolution){
    case AGS_DEVOUT_RESOLUTION_8_BIT:
      {
	gint8 *dbuffer;
	gdouble scale;
	gint16 *mask;
	
	dbuffer = (gint8 *) destination->data;

	scale = exp2(1.0 / 16.0);

	dbuffer[doffset] = (gint8) round(scale * (double) sbuffer[soffset]);
      }
      break;
    case AGS_DEVOUT_RESOLUTION_16_BIT:
      {
	gint16 *dbuffer;
	gdouble scale;

	dbuffer = (gint16 *) destination->data;

	scale = exp2(1.0 / 8.0);

	dbuffer[doffset] = (gint16) round(scale * (double) sbuffer[soffset]);
      }
      break;
    case AGS_DEVOUT_RESOLUTION_24_BIT:
      {
	unsigned char *dbuffer;
	gint16 mask;

	dbuffer = (unsigned char *) destination->data;

	dbuffer[doffset * 3] = sbuffer[soffset * 3];
	dbuffer[doffset * 3 + 1] = sbuffer[soffset * 3 + 1];
	dbuffer[doffset * 3 + 2] = sbuffer[soffset * 3 + 2];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_32_BIT:
      {
	gint32 *dbuffer;
	gdouble scale;

	dbuffer = (gint32 *) destination->data;

	scale = exp2(8.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_64_BIT:
      {
	gint64 *dbuffer;
	gdouble scale;

	dbuffer = (gint64 *) destination->data;

	scale = exp2(40.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    }
  }
  void ags_audio_signal_scale_copy_32_bit(GList *source, GList *destination,
					  guint soffset, guint doffset,
					  guint dresolution){
    gint32 *sbuffer;

    sbuffer = (gint32 *) source->data;

    switch(dresolution){
    case AGS_DEVOUT_RESOLUTION_8_BIT:
      {
	gint8 *dbuffer;
	gdouble scale;

	dbuffer = (gint8 *) destination->data;

	scale = exp2(1.0 / 24.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_16_BIT:
      {
	gint16 *dbuffer;
	gdouble scale;

	dbuffer = (gint16 *) destination->data;

	scale = exp2(1.0 / 16.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_24_BIT:
      {
	unsigned char *dbuffer;
	gint32 value;
	gdouble scale;
	gint32 mask;

	dbuffer = (unsigned char *) destination->data;

	scale = exp2(1.0 / 8.0);
	mask = 0xff;

	value = scale * sbuffer[soffset];
      
	dbuffer[doffset * 3] = mask & value;
	dbuffer[doffset * 3 + 1] = (mask << 8) & value;
	dbuffer[doffset * 3 + 2] = (mask << 16) & value;
	
	if(sbuffer[soffset] < 0){
	  dbuffer[doffset * 3 + 2] |= 0x80;
	}
      }
      break;
    case AGS_DEVOUT_RESOLUTION_32_BIT:
      {
	gint32 *dbuffer;

	dbuffer = (gint32 *) destination->data;

	dbuffer[doffset] = sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_64_BIT:
      {
	gint64 *dbuffer;
	gdouble scale;

	dbuffer = (gint64 *) destination->data;

	scale = exp2(32.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    }
  }
  void ags_audio_signal_scale_copy_64_bit(GList *source, GList *destination,
					  guint soffset, guint doffset,
					  guint dresolution){
    gint64 *sbuffer;

    sbuffer = (gint64 *) source->data;

    switch(dresolution){
    case AGS_DEVOUT_RESOLUTION_8_BIT:
      {
	gint8 *dbuffer;
	gdouble scale;

	dbuffer = (gint8 *) destination->data;

	scale = exp2(1 / 56.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_16_BIT:
      {
	gint16 *dbuffer;
	gdouble scale;

	dbuffer = (gint16 *) destination->data;

	scale = exp2(1 / 48.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_24_BIT:
      {
	unsigned char *dbuffer;
	gint32 value;
	gdouble scale;
	gint32 mask;

	dbuffer = (unsigned char *) destination->data;

	scale = exp2(1.0 / 40.0);
	mask = 0xff;

	value = scale * sbuffer[soffset];
      
	dbuffer[doffset * 3] = mask & value;
	dbuffer[doffset * 3 + 1] = (mask << 8) & value;
	dbuffer[doffset * 3 + 2] = (mask << 16) & value;
	
	if(sbuffer[soffset] < 0){
	  dbuffer[doffset * 3 + 2] |= 0x80;
	}
      }
      break;
    case AGS_DEVOUT_RESOLUTION_32_BIT:
      {
	gint32 *dbuffer;
	gdouble scale;

	dbuffer = (gint32 *) destination->data;

	scale = exp2(1 / 32.0);

	dbuffer[doffset] = scale * sbuffer[soffset];
      }
      break;
    case AGS_DEVOUT_RESOLUTION_64_BIT:
      {
	gint64 *dbuffer;

	dbuffer = (gint64 *) destination->data;

	dbuffer[doffset] = sbuffer[soffset];
      }
      break;
    }
  }

  source = template->stream_beginning;

  if(template->samplerate < audio_signal->samplerate){
    expand = TRUE;
  }else{
    expand = FALSE;
  }

  scale_factor = 1.0 / template->length * length;
  morph_factor = 1.0 / template->resolution * audio_signal->resolution;

  /* prepare destination */
  ags_audio_signal_stream_resize(audio_signal, length);

  /* create audio data */
  //TODO:JK: fix me
  j_stop = audio_signal->resolution; // lcm(audio_signal->resolution, template->resolution);

  stream_template = NULL;

  offset = 0;
  step = 0.0;

  for(i = 0; i < template->length; i++){
    for(; j < j_stop; j++){

      if(offset == audio_signal->buffer_size && step >= morph_factor){
	break;
      }

      if(offset == 0){
	if(expand){
	  data = (gpointer) malloc(sizeof(audio_signal->buffer_size * morph_factor));
	}else{
	  data = (gpointer) malloc(sizeof(audio_signal->buffer_size / morph_factor));
	}

	stream_template = g_list_prepend(stream_template,
					 data);

	destination = stream_template;
      }

      switch(template->resolution){
      case AGS_DEVOUT_RESOLUTION_8_BIT:
	{
	  ags_audio_signal_scale_copy_8_bit(destination, source,
					    j, offset,
					    audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_16_BIT:
	{
	  ags_audio_signal_scale_copy_16_bit(destination, source,
					     j, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_24_BIT:
	{
	  ags_audio_signal_scale_copy_24_bit(destination, source,
					     j, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_32_BIT:
	{
	  ags_audio_signal_scale_copy_32_bit(destination, source,
					     j, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_64_BIT:
	{
	  ags_audio_signal_scale_copy_64_bit(destination, source,
					     j, offset,
					     audio_signal->resolution);
	}
	break;
      }

      step += (1 / morph_factor);

      if(step >= morph_factor){
	step = 0.0;
	offset++;
      }      
    }


    if(j == j_stop){
      j = 0;
    }

    if(offset == template->buffer_size && step == 0.0){
      offset = 0;
      source = source->next;
    }
  }

  stream_template = g_list_reverse(stream_template);

  /* morph */
  //TODO:JK: implement me

  /* scale */
  source = stream_template;
  destination = audio_signal->stream_beginning;

  offset = 0;
  k = 0;
  template_k = 0;

  while(destination != NULL){
    for(i = 0;
	i < audio_signal->buffer_size &&
	  k < audio_signal->buffer_size &&
	  template_k < template->buffer_size;
	i++, k++, template_k++){

      switch(audio_signal->resolution){
      case AGS_DEVOUT_RESOLUTION_8_BIT:
	{
	  ags_audio_signal_scale_copy_8_bit(destination, source,
					    i, offset,
					    audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_16_BIT:
	{
	  ags_audio_signal_scale_copy_16_bit(destination, source,
					     i, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_24_BIT:
	{
	  ags_audio_signal_scale_copy_24_bit(destination, source,
					     i, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_32_BIT:
	{
	  ags_audio_signal_scale_copy_32_bit(destination, source,
					     i, offset,
					     audio_signal->resolution);
	}
	break;
      case AGS_DEVOUT_RESOLUTION_64_BIT:
	{
	  ags_audio_signal_scale_copy_64_bit(destination, source,
					     i, offset,
					     audio_signal->resolution);
	}
	break;
      }

      offset = (guint) floor(morph_factor * (double) i);
    }

    if(k == audio_signal->buffer_size){
      destination = destination->next;
    }

    if(template_k == template->buffer_size){
      source = source->next;
    }
  }
}

/**
 * ags_audio_signal_new:
 * @devout: the assigned #AgsDevout
 * @recycling: the #AgsRecycling
 * @recall_id: the #AgsRecallID, it can be NULL if %AGS_AUDIO_SIGNAL_TEMPLATE is set
 *
 * Creates a #AgsAudioSignal, with defaults of @devout, linking @recycling tree
 * and refering to @recall_id.
 *
 * Returns: a new #AgsAudioSignal
 *
 * Since: 0.3
 */
AgsAudioSignal*
ags_audio_signal_new(GObject *devout,
		     GObject *recycling,
		     GObject *recall_id)
{
  AgsAudioSignal *audio_signal;

  audio_signal = (AgsAudioSignal *) g_object_new(AGS_TYPE_AUDIO_SIGNAL,
						 "devout\0", devout,
						 "recycling\0", recycling,
						 "recall-id\0", recall_id,
						 NULL);

  return(audio_signal);
}

/**
 * ags_audio_signal_new_with_length:
 * @devout: the assigned #AgsDevout
 * @recycling: the #AgsRecycling
 * @recall_id: the #AgsRecallID, it can be NULL if %AGS_AUDIO_SIGNAL_TEMPLATE is set
 * @length: audio data frame count
 *
 * Creates a #AgsAudioSignal, with defaults of @devout, linking @recycling tree
 * and refering to @recall_id.
 * The audio data is tiled to @length frame count.
 *
 * Returns: a new #AgsAudioSignal
 *
 * Since: 0.4
 */
AgsAudioSignal*
ags_audio_signal_new_with_length(GObject *devout,
				 GObject *recycling,
				 GObject *recall_id,
				 guint length)
{
  AgsAudioSignal *audio_signal, *template;

  audio_signal = (AgsAudioSignal *) g_object_new(AGS_TYPE_AUDIO_SIGNAL,
						 "devout\0", devout,
						 "recycling\0", recycling,
						 "recall-id\0", recall_id,
						 NULL);

  template = ags_audio_signal_get_template(AGS_RECYCLING(recycling)->audio_signal);

  if(template != NULL){
    ags_audio_signal_tile(audio_signal,
			  template,
			  length);
  }

  return(audio_signal);
}
