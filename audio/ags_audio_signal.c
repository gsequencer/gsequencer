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

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <stdlib.h>
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

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_RECYCLING,
  PROP_RECALL_ID,
};

static gpointer ags_audio_signal_parent_class = NULL;

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

}

void
ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
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

  audio_signal->length = 0;
  audio_signal->last_frame = 0;

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

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0)
    fprintf(stdout, "AGS_AUDIO_SIGNAL_TEMPLATE: destroying\n\0");

  if(audio_signal->devout != NULL)
    g_object_unref(audio_signal->devout);

  if(audio_signal->recycling != NULL)
    g_object_unref(audio_signal->recycling);

  if(audio_signal->recall_id != NULL)
    g_object_unref(audio_signal->recall_id);

  ags_list_free_and_free_link(audio_signal->stream_beginning);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->finalize(gobject);
}

short*
ags_stream_alloc(guint buffer_size)
{
  short *buffer;

  buffer = (short *) malloc(buffer_size * sizeof(short));
  memset(buffer, 0, buffer_size * sizeof(short));

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

AgsAttack*
ags_attack_alloc(guint first_start, guint first_length,
		 guint second_start, guint second_length)
{
  AgsAttack *attack;

  attack = (AgsAttack *) malloc(sizeof(AgsAttack));

  attack->first_start = first_start;
  attack->first_length = first_length;
  attack->second_start = second_start;
  attack->second_length = second_length;

  return(attack);
}

AgsAttack*
ags_attack_duplicate(AgsAttack *attack)
{
  AgsAttack *copy;

  if(attack == NULL)
    return(NULL);

  copy = (AgsAttack *) malloc(sizeof(AgsAttack));

  copy->first_start = attack->first_start;
  copy->first_length = attack->first_length;
  copy->second_start = attack->second_start;
  copy->second_length = attack->second_length;

  return(copy);
}


AgsAttack*
ags_attack_duplicate_from_devout(GObject *devout0)
{
  AgsDevout *devout;
  AgsAttack *copy, *attack;
  GValue attack_value = {0,};

  devout = AGS_DEVOUT(devout0);

  g_value_init(&attack_value, G_TYPE_POINTER);
  g_object_get_property(G_OBJECT(devout),
			"attack\0",
			&attack_value);
  attack = g_value_get_pointer(&attack_value);
  g_value_unset(&attack_value);

  if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0)
    copy = ags_attack_alloc(attack->first_start, attack->first_length,
			    0, 0);
  else
    copy = ags_attack_alloc(attack->second_start, attack->second_length,
			    0, 0);

  return(copy);
}

void
ags_audio_signal_add_stream(AgsAudioSignal *audio_signal)
{
  GList *stream, *end_old;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  stream = g_list_alloc();
  stream->data = (gpointer) ags_stream_alloc(AGS_DEVOUT(audio_signal->devout)->buffer_size);

  end_old = audio_signal->stream_end;

  if(end_old != NULL){
    stream->prev = end_old;
    end_old->next = stream;
  }else{
    audio_signal->stream_beginning = stream;
    audio_signal->stream_current = stream;
  }

  audio_signal->stream_end = stream;

  audio_signal->length++;
}

/* 
 *  be carefull with shrinking
 */
void
ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint i;

  if(audio_signal->length < length){
    GList *start, *stream, *end_old;

    start =
      stream = g_list_alloc();
    stream->data = (gpointer) ags_stream_alloc(AGS_DEVOUT(audio_signal->devout)->buffer_size);

    for(i = audio_signal->length; i < length; i++){
      stream->next = g_list_alloc();
      stream->next->prev = stream;
      stream = stream->next;
      stream->data = (gpointer) ags_stream_alloc(AGS_DEVOUT(audio_signal->devout)->buffer_size);

    }

    end_old = audio_signal->stream_end;

    if(end_old != NULL){
      start->prev = end_old;
      end_old->next = stream;
    }else{
      audio_signal->stream_beginning = start;
      //      audio_signal->stream_current = start;
    }
    
    audio_signal->stream_end = stream;
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
    }else{
      audio_signal->stream_beginning = NULL;
      audio_signal->stream_current = NULL;
      audio_signal->stream_end = NULL;
    }

    while(stream != NULL){
      stream_next = stream->next;
      free((short *) stream->data);
      g_list_free1(stream);
      stream = stream_next;
    }
  }

  audio_signal->length = length;
}

void
ags_audio_signal_copy_buffer_to_buffer(short *destination, guint dchannels, short *source, guint schannels, guint size)
{
  guint i;

  for(i = 0; i < size; i++){
    destination[0] = (short) ((0xffff) & ((int)destination[0] + (int)source[0]));

    destination += dchannels;
    source += schannels;
  }
}

void
ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal, AgsAudioSignal *template)
{
  GList *source, *list, *start;
  short *buffer;
  guint size;

  source = template->stream_beginning;

  if(audio_signal->stream_beginning != NULL)
    ags_audio_signal_stream_resize(audio_signal, 0);

  if(source == NULL){
    audio_signal->stream_beginning = NULL;
    audio_signal->stream_current = NULL;
    audio_signal->stream_end = NULL;
  }else{
    size = AGS_DEVOUT(audio_signal->devout)->buffer_size * sizeof(short);

    start = 
      list = g_list_alloc();
    goto ags_audio_signal_duplicate_stream0;

    while(source != NULL){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;
    ags_audio_signal_duplicate_stream0:

      buffer = (short *) malloc(size);
      memcpy(buffer, (short *) source->data, size);
      list->data = buffer;
      
      source = source->next;
    }

    audio_signal->stream_beginning = start;
    audio_signal->stream_end = list;
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
