/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_recall_channel.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_recall_container.h>

#include <math.h>

#include <ags/i18n.h>

void ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel);
void ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_init(AgsRecallChannel *recall_channel);
void ags_recall_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_dispose(GObject *gobject);
void ags_recall_channel_finalize(GObject *gobject);

void ags_recall_channel_notify_recall_container_callback(GObject *gobject,
							 GParamSpec *pspec,
							 gpointer user_data);

void ags_recall_channel_automate(AgsRecall *recall);
AgsRecall* ags_recall_channel_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_recall_channel
 * @short_description: The recall base class of channel context
 * @title: AgsRecallChannel
 * @section_id:
 * @include: ags/audio/ags_recall_channel.h
 *
 * #AgsRecallChannel acts as channel recall and hosts #AgsPort for audio context ports.
 */

enum{
  PROP_0,
  PROP_RECALL_AUDIO,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_channel_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_parent_connectable_interface;

GType
ags_recall_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_channel = 0;

    static const GTypeInfo ags_recall_channel_info = {
      sizeof (AgsRecallChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsRecallChannel",
						     &ags_recall_channel_info,
						     0);

    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_parent_class = g_type_class_peek_parent(recall_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel;

  gobject->set_property = ags_recall_channel_set_property;
  gobject->get_property = ags_recall_channel_get_property;

  gobject->dispose = ags_recall_channel_dispose;
  gobject->finalize = ags_recall_channel_finalize;

  /* properties */
  /**
   * AgsRecallChannel:recall-audio:
   *
   * The associated recall within audio context.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-audio",
				   i18n_pspec("audio level recall"),
				   i18n_pspec("The recall which this recall channel has on audio level"),
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /**
   * AgsRecallChannel:destination:
   *
   * The assigned destination channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("assigned destination channel"),
				   i18n_pspec("The destination channel object it is assigned to"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);
  
  /**
   * AgsRecallChannel:source:
   *
   * The assigned source channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("assigned source channel"),
				   i18n_pspec("The source channel object it is assigned to"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel;

  recall->automate = ags_recall_channel_automate;
  recall->duplicate = ags_recall_channel_duplicate;
}

void
ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_channel_init(AgsRecallChannel *recall_channel)
{
  g_signal_connect_after(recall_channel, "notify::recall-container",
			 G_CALLBACK(ags_recall_channel_notify_recall_container_callback), NULL);

  recall_channel->destination = NULL;
  recall_channel->source = NULL;
}

void
ags_recall_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  GRecMutex *recall_mutex;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      GList *list_start, *list;
      
      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel->recall_audio == recall_audio){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_channel->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_channel->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_channel->recall_audio = recall_audio;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      AgsChannel *destination;

      destination = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel->destination == destination){      
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_channel->destination != NULL){
	g_object_unref(recall_channel->destination);
      }
       
      if(destination != NULL){
	g_object_ref(destination);
      }
       
      recall_channel->destination = destination;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SOURCE:
    {
      AgsChannel *source;

      source = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel->source == source){      
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_channel->source != NULL){
	g_object_unref(recall_channel->source);
      }
	
      if(source != NULL){
	g_object_ref(source);
      }
       
      recall_channel->source = source;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  GRecMutex *recall_mutex;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_channel->recall_audio);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_channel->destination);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SOURCE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_channel->source);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_dispose(GObject *gobject)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);
  
  /* source */
  if(recall_channel->source != NULL){
    gpointer tmp;

    tmp = recall_channel->source;
    
    recall_channel->source = NULL;

    g_object_unref(tmp);
  }

  /* destination */
  if(recall_channel->destination != NULL){
    gpointer tmp;

    tmp = recall_channel->destination;

    recall_channel->destination = NULL;

    g_object_unref(G_OBJECT(recall_channel->destination));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->dispose(gobject);
}

void
ags_recall_channel_finalize(GObject *gobject)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* source */
  if(recall_channel->source != NULL){
    gpointer tmp;

    tmp = recall_channel->source;
    
    recall_channel->source = NULL;

    g_object_unref(tmp);
  }

  /* destination */
  if(recall_channel->destination != NULL){
    gpointer tmp;

    tmp = recall_channel->destination;

    recall_channel->destination = NULL;

    g_object_unref(G_OBJECT(recall_channel->destination));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->finalize(gobject);
}

void
ags_recall_channel_notify_recall_container_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsRecallContainer *recall_container;
  AgsRecallChannel *recall_channel;
  
  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* get some fields */
  g_object_get(recall_channel,
	       "recall-container", &recall_container,
	       NULL);

  if(recall_container != NULL){
    AgsRecallAudio *recall_audio;

    GList *list_start, *list;

    /* recall audio */
    g_object_get(recall_container,
		 "recall-audio", &recall_audio,
		 NULL);
    
    g_object_set(recall_channel,
		 "recall-audio", recall_audio,
		 NULL);

    if(recall_audio != NULL){
      g_object_unref(recall_audio);
    }

    g_object_unref(recall_container);
  }else{
    g_object_set(recall_channel,
		 "recall-audio", NULL,
		 NULL);
  }
}

void
ags_recall_channel_automate(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  GObject *soundcard;

  GList *automation_start, *automation;
  GList *port_start, *port;

  gdouble delay;
  guint note_offset, delay_counter;
  
  guint loop_left, loop_right;
  gboolean do_loop;

  double x, step;
  guint ret_x;
  gboolean return_prev_on_failure;

  GRecMutex *audio_mutex;

  audio = NULL;
  channel = NULL;  

  soundcard = NULL;
  
  port_start = NULL;

  g_object_get(recall,
	       "source", &channel,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  if(audio->automation_port == NULL){  
    g_rec_mutex_unlock(audio_mutex);

    if(audio != NULL){
      g_object_unref(audio);
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    return;
  }
  
  g_rec_mutex_unlock(audio_mutex);
  
  g_object_get(audio,
	       "output-soundcard", &soundcard,
	       NULL);

  g_object_get(recall,
	       "port", &port_start,
	       NULL);

  /* retrieve position */
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(soundcard));

  /* retrieve loop information */
  ags_soundcard_get_loop(AGS_SOUNDCARD(soundcard),
			 &loop_left, &loop_right,
			 &do_loop);

  return_prev_on_failure = TRUE;

  if(do_loop &&
     loop_left <= note_offset){
    if(note_offset == loop_left){
      return_prev_on_failure = TRUE;
    }
  }

  /* apply automation */
  port = port_start;

  x = ((double) note_offset + (delay_counter / delay)) * ((1.0 / AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH) * AGS_NOTATION_MINIMUM_NOTE_LENGTH);
  step = ((1.0 / AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH) * AGS_NOTATION_MINIMUM_NOTE_LENGTH);

  while(port != NULL){
    gchar *specifier;

    gboolean success;
    
    g_object_get(AGS_PORT(port->data),
		 "specifier", &specifier,
		 NULL);
    
    g_rec_mutex_lock(audio_mutex);

    success = (specifier != NULL && g_strv_contains(audio->automation_port, specifier)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(audio_mutex);
      
    g_free(specifier);
    
    if(!success){      
      /* iterate */
      port = port->next;
      
      continue;
    }

    g_object_get(AGS_PORT(port->data),
		 "automation", &automation_start,
		 NULL);

    /* find offset */
    automation = automation_start;

    while(automation != NULL){
      AgsAutomation *current_automation;

      AgsTimestamp *timestamp;
      
      current_automation = automation->data;

      /* get some fields */
      g_object_get(current_automation,
		   "timestamp", &timestamp,
		   NULL);
      
      if(ags_timestamp_get_ags_offset(timestamp) + AGS_AUTOMATION_DEFAULT_OFFSET < x){
	automation = automation->next;

	g_object_unref(timestamp);
	
	continue;
      }
      
      if(!ags_automation_test_flags(current_automation, AGS_AUTOMATION_BYPASS)){
	GValue value = {0,};
	
	ret_x = ags_automation_get_value(current_automation,
					 floor(x), ceil(x + step),
					 return_prev_on_failure,
					 &value);

	if(ret_x != G_MAXUINT){
//	  g_message("automate -> %f", g_value_get_float(&value));
	  
	  ags_port_safe_write(port->data,
			      &value);
	}
      }

      if(ags_timestamp_get_ags_offset(timestamp) > ceil(x + step)){
	g_object_unref(timestamp);

	break;
      }

      /* unref */
      g_object_unref(timestamp);

      /* iterate */
      automation = automation->next;
    }

    g_list_free_full(automation_start,
		     g_object_unref);
    
    /* iterate */
    port = port->next;
  }

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }

  if(soundcard != NULL){
    g_object_unref(soundcard);
  }
  
  g_list_free_full(port_start,
		   g_object_unref);
}

AgsRecall*
ags_recall_channel_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecallChannel *copy_recall_channel;

  copy_recall_channel = AGS_RECALL_CHANNEL(AGS_RECALL_CLASS(ags_recall_channel_parent_class)->duplicate(recall,
													recall_id,
													n_params, parameter_name, value));

  g_message("ags warning - ags_recall_channel_duplicate: you shouldn't do this %s", G_OBJECT_TYPE_NAME(recall));

  return((AgsRecall *) copy_recall_channel);
}

/**
 * ags_recall_channel_get_destination:
 * @recall_channel: the #AgsRecallDestination
 * 
 * Get destination.
 * 
 * Returns: (transfer full): the #AgsDestination
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_recall_channel_get_destination(AgsRecallChannel *recall_channel)
{
  AgsChannel *destination;

  if(!AGS_IS_RECALL_CHANNEL(recall_channel)){
    return(NULL);
  }

  g_object_get(recall_channel,
	       "destination", &destination,
	       NULL);

  return(destination);
}

/**
 * ags_recall_channel_set_destination:
 * @recall_channel: the #AgsRecallChannel
 * @destination: the #AgsChannel
 * 
 * Set destination.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_set_destination(AgsRecallChannel *recall_channel, AgsChannel *destination)
{
  if(!AGS_IS_RECALL_CHANNEL(recall_channel)){
    return;
  }

  g_object_set(recall_channel,
	       "destination", destination,
	       NULL);
}

/**
 * ags_recall_channel_get_source:
 * @recall_channel: the #AgsRecallSource
 * 
 * Get source.
 * 
 * Returns: (transfer full): the #AgsSource
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_recall_channel_get_source(AgsRecallChannel *recall_channel)
{
  AgsChannel *source;

  if(!AGS_IS_RECALL_CHANNEL(recall_channel)){
    return(NULL);
  }

  g_object_get(recall_channel,
	       "source", &source,
	       NULL);

  return(source);
}

/**
 * ags_recall_channel_set_source:
 * @recall_channel: the #AgsRecallChannel
 * @source: the #AgsChannel
 * 
 * Set source.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_set_source(AgsRecallChannel *recall_channel, AgsChannel *source)
{
  if(!AGS_IS_RECALL_CHANNEL(recall_channel)){
    return;
  }

  g_object_set(recall_channel,
	       "source", source,
	       NULL);
}

/**
 * ags_recall_channel_find_channel:
 * @recall_channel: (element-type AgsAudio.RecallChannel) (transfer none): the #GList-struct containing #AgsRecallChannel
 * @source: the #AgsChannel to find
 *
 * Retrieve next recall assigned to channel.
 *
 * Returns: (element-type AgsAudio.RecallChannel) (transfer none): next matching #GList-struct or %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_recall_channel_find_channel(GList *recall_channel, AgsChannel *source)
{
  AgsRecallChannel *current_recall_channel;

  gboolean success;
  
  while(recall_channel != NULL){
    AgsChannel *current_source;
    
    current_recall_channel = AGS_RECALL_CHANNEL(recall_channel->data);

    g_object_get(current_recall_channel,
		 "source", &current_source,
		 NULL);

    success = (current_source == source) ? TRUE: FALSE;

    if(current_source != NULL){
      g_object_unref(current_source);
    }
    
    if(success){
      return(recall_channel);
    }

    recall_channel = recall_channel->next;
  }

  return(NULL);
}

/**
 * ags_recall_channel_new:
 *
 * Creates an #AgsRecallChannel.
 *
 * Returns: a new #AgsRecallChannel.
 *
 * Since: 3.0.0
 */
AgsRecallChannel*
ags_recall_channel_new()
{
  AgsRecallChannel *recall_channel;

  recall_channel = (AgsRecallChannel *) g_object_new(AGS_TYPE_RECALL_CHANNEL,
						     NULL);

  return(recall_channel);
}
