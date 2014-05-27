/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_audio_signal.h>

#include <ags/object/ags_marshal.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>

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
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_object("recycling\0",
				   "assigned recycling\0",
				   "The devout it is assigned with\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  param_spec = g_param_spec_object("recall-id\0",
				   "assigned recall id\0",
				   "The recall id it is assigned with\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /*  */
  audio_signal->realloc_buffer_size = ags_audio_signal_real_realloc_buffer_size;
  audio_signal->morph_samplerate = ags_audio_signal_real_morph_samplerate;

  audio_signal_signals[REALLOC_BUFFER_SIZE] =
    g_signal_new("realloc_buffer_size\0",
		 G_TYPE_FROM_CLASS (audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsAudioSignalClass, realloc_buffer_size),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  audio_signal_signals[MORPH_SAMPLERATE] =
    g_signal_new("morph_samplerate\0",
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

  audio_signal->samplerate = AGS_DEVOUT_DEFAULT_SAMPLERATE;
  audio_signal->buffer_size = AGS_DEVOUT_DEFAULT_BUFFER_SIZE;
  audio_signal->resolution = AGS_DEVOUT_RESOLUTION_16_BIT;

  audio_signal->length = 0;
  audio_signal->last_frame = 0;
  audio_signal->loop_start = 0;
  audio_signal->loop_end = 0;

  audio_signal->delay = 0;
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

  g_message("finalize AgsAudioSignal\0");

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0)
    g_warning("AGS_AUDIO_SIGNAL_TEMPLATE: destroying\n\0");

  if(audio_signal->devout != NULL)
    g_object_unref(audio_signal->devout);

  if(audio_signal->recycling != NULL)
    g_object_unref(audio_signal->recycling);

  if(audio_signal->recall_id != NULL)
    g_object_unref(audio_signal->recall_id);

  if(audio_signal->stream_beginning != NULL)
    ags_list_free_and_free_link(audio_signal->stream_beginning);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->finalize(gobject);
}

signed short*
ags_stream_alloc(guint buffer_size)
{
  signed short *buffer;

  buffer = (signed short *) malloc(buffer_size * sizeof(signed short));
  memset(buffer, 0, buffer_size * sizeof(signed short));

  return(buffer);
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
 * ags_audio_signal_get_length_till_current:
 * @audio_signal an #AgsAudioSignal
 * Returns: the counted length.
 *
 * Counts the buffers from stream_beginning upto stream_current.
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
 * @audio_signal an #AgsAudioSignal
 *
 * Adds a buffer at the end of the stream.
 */
void
ags_audio_signal_add_stream(AgsAudioSignal *audio_signal)
{
  GList *stream, *end_old;
  signed short *buffer;

  stream = g_list_alloc();
  buffer = ags_stream_alloc(AGS_DEVOUT(audio_signal->devout)->buffer_size);
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
 * @audio_signal an #AgsAudioSignal to resize.
 * @length a guint as the new length.
 *
 * Resizes an #AgsAudioSignal's stream but be carefull with shrinking.
 * This function may crash the application.
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
      buffer = ags_stream_alloc(AGS_DEVOUT(audio_signal->devout)->buffer_size);

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

    while(stream != NULL){
      stream_next = stream->next;
      free((signed short *) stream->data);
      g_list_free1(stream);
      stream = stream_next;
    }
  }

  audio_signal->length = length;
}

/**
 * ags_audio_signal_stream_safe_resize:
 * @audio_signal an #AgsAudioSignal
 * @length a guint
 *
 * Resizes an #AgsAudioSignal's stream but doesn't shrink more than the
 * current stream position.
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
 * @audio_signal
 * @buffer_size
 *
 * Realloc the stream to the new buffer size. 
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
 * @audio_signal
 * @samplerate
 *
 * Morph audio quality to new samplerate.
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
 * @destination
 * @dchannels
 * @source
 * @schannels
 * @size
 *
 * Copy a buffer to an other buffer.
 */
void
ags_audio_signal_copy_buffer_to_buffer(signed short *destination, guint dchannels, signed short *source, guint schannels, guint size)
{
  guint i;

  for(i = 0; i < size; i++){
    destination[0] = (signed short) (((int)destination[0] + (int)source[0]));

    destination += dchannels;
    source += schannels;
  }
}

void
ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				  AgsAudioSignal *template)
{
  GList *template_stream, *stream, *start;
  signed short *buffer;
  guint size;
  guint k, template_k;

  if(audio_signal->stream_beginning != NULL)
    ags_audio_signal_stream_resize(audio_signal, 0);

  if(template->stream_beginning == NULL){
    audio_signal->stream_beginning = NULL;
    audio_signal->stream_current = NULL;
    audio_signal->stream_end = NULL;
  }else{
    AgsDevout *devout;

    devout = AGS_DEVOUT(audio_signal->devout);

    size = devout->buffer_size * sizeof(signed short);

    //TODO:JK: actualize me
    ags_audio_signal_stream_resize(audio_signal, (audio_signal->delay +
						  template->length)); /* (guint) ceil(((double) audio_signal->attack + /
									   (double) devout->buffer_size) */

    //    if(audio_signal->attack + template->last_frame > devout->buffer_size){
    //      ags_audio_signal_add_stream(audio_signal);
      //    }

    stream =
      start = g_list_nth(audio_signal->stream_beginning,
			 audio_signal->delay);

    template_stream = g_list_nth(template->stream_beginning,
				 template->delay);

    k = audio_signal->attack;
    template_k = template->attack;

    while(template_stream != NULL){
      if(k == devout->buffer_size){
	k = 0;
	
	stream = stream->next;
      }

      if(k == 0){
	if(stream->next == NULL){
	  ags_audio_signal_add_stream(audio_signal);
	}
      }

      if(template_k == devout->buffer_size){
	template_k = 0;
	template_stream = template_stream->next;
      }

      for(;
	  template_stream != NULL && k < devout->buffer_size && template_k < devout->buffer_size;
	  k++, template_k++){
	/* copy audio data from template to new AgsAudioSignal */
	((signed short*) stream->data)[k] = ((signed short*) template_stream->data)[template_k];
      }
    }

    // audio_signal->stream_beginning = start;
    // audio_signal->stream_end = stream;
  }
}

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

void
ags_audio_signal_tile(AgsAudioSignal *audio_signal,
		      AgsAudioSignal *template,
		      guint length)
{
  AgsDevout *devout;
  GList *template_stream, *audio_signal_stream, *audio_signal_stream_end;
  signed short *template_buffer, *audio_signal_buffer;
  guint buffer_size;
  guint template_length, mod_template_length, mod_template_length_odd;
  guint remaining_length;
  gboolean mod_template_even;
  guint i, j, j_offcut, j_offcut_odd, k;

  devout = AGS_DEVOUT(audio_signal->devout);

  audio_signal_stream = NULL;
  template_stream = template->stream_beginning;

  if(template_stream == NULL){
    return;
  }

  buffer_size = devout->buffer_size;

  template_length = template->length;
  mod_template_length = template_length % buffer_size;
  mod_template_length_odd = buffer_size - mod_template_length;
  mod_template_even = TRUE;

  j_offcut = 0;
  k = 1;

  /* write buffers */
  for(i = 0; i < length - buffer_size; i += buffer_size){
    audio_signal_buffer = (signed short *) malloc(buffer_size * sizeof(signed short));
    audio_signal_stream = g_list_prepend(audio_signal_stream,
					 audio_signal_buffer);

    template_buffer = (signed short *) template_stream->data;

    if(template_length < buffer_size){
      ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					     &(template_buffer[j_offcut]), 1, template_length - j_offcut);

      for(j = template_length - j_offcut; j < buffer_size - template_length; j += template_length){
	ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					       template_buffer, 1, template_length);
      }

      j_offcut = buffer_size - j;
      ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					     template_buffer, 1, j_offcut);
    }else{
      ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					     &(template_buffer[j_offcut]), 1, j_offcut_odd);

      if((guint) floor((double) (i + j_offcut_odd + buffer_size) / (double) template_length) < k){
	template_stream = template_stream->next;
      }else{
	template_stream = template->stream_beginning;
	k++;
      }
      
      ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j_offcut_odd]), 1,
					     template_buffer, 1, j_offcut);

      if(mod_template_even){
	j_offcut = mod_template_length;
	j_offcut_odd = mod_template_length_odd;
	
	mod_template_even = FALSE;
      }else{
	j_offcut = mod_template_length_odd;
	j_offcut_odd = mod_template_length;
	
	mod_template_even = TRUE;
      }
    }
  }
  
  /* write remaining buffer */
  remaining_length = length - (i * buffer_size);

  if(template_length < buffer_size){
    guint remaining_length_odd, remaining_length_overflow;

    if(remaining_length < template_length - j_offcut){
      ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					     &(template_buffer[j_offcut]), 1, remaining_length);
    }else{
      ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					     &(template_buffer[j_offcut]), 1, template_length - j_offcut);
    
      if(remaining_length >= template_length){
	for(j = template_length - j_offcut; j < remaining_length - template_length; j += template_length){
	  ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
						 template_buffer, 1, template_length);
	}
	
	j_offcut = remaining_length - j;
	ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					       template_buffer, 1, j_offcut);
      }else{
	j_offcut = remaining_length + j_offcut - template_length;
	ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j]), 1,
					       template_buffer, 1, j_offcut);
      }
    }
  }else{
    guint remaining_length_odd, remaining_length_overflow;

    remaining_length_overflow = j_offcut + remaining_length;

    if(remaining_length_overflow > buffer_size){
      remaining_length = j_offcut_odd;
      remaining_length_odd = buffer_size - remaining_length_overflow;
    }else{
      remaining_length_odd = 0;
    }

    ags_audio_signal_copy_buffer_to_buffer(audio_signal_buffer, 1,
					   &(template_buffer[j_offcut]), 1, remaining_length);

    if(remaining_length_odd != 0){
      if((i + j_offcut + buffer_size) % template_length < template_length){
	template_stream = template_stream->next;
      }else{
	template_stream = template->stream_beginning;
      }
      
      ags_audio_signal_copy_buffer_to_buffer(&(audio_signal_buffer[j_offcut_odd]), 1,
					     template_buffer, 1, j_offcut);
    }
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
  j_stop = smallest_common_factor(audio_signal->resolution, template->resolution);

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

  while(destination != NULL){
    for(i = 0; i < audio_signal->buffer_size; i++){

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

    destination = destination->next;
    source = source->next;
  }
}

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
