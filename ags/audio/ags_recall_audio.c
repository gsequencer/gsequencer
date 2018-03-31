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

#include <ags/audio/ags_recall_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_recall_container.h>

#include <math.h>

#include <ags/i18n.h>

void ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio);
void ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_audio_init(AgsRecallAudio *recall_audio);
void ags_recall_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_recall_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_recall_audio_dispose(GObject *gobject);
void ags_recall_audio_finalize(GObject *gobject);

gboolean ags_recall_audio_pack(AgsPackable *packable, GObject *recall_container);
gboolean ags_recall_audio_unpack(AgsPackable *packable);

void ags_recall_audio_load_automation(AgsRecall *recall,
				      GList *automation_port);
void ags_recall_audio_unload_automation(AgsRecall *recall);
void ags_recall_audio_automate(AgsRecall *recall);
AgsRecall* ags_recall_audio_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_recall_audio
 * @short_description: audio context of recall
 * @title: AgsRecallAudio
 * @section_id:
 * @include: ags/audio/ags_recall_audio.h
 *
 * #AgsRecallAudio acts as audio recall.
 */

enum{
  PROP_0,
  PROP_AUDIO,
};

static gpointer ags_recall_audio_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_parent_connectable_interface;
static AgsPackableInterface* ags_recall_audio_parent_packable_interface;

GType
ags_recall_audio_get_type()
{
  static GType ags_type_recall_audio = 0;

  if(!ags_type_recall_audio){
    static const GTypeInfo ags_recall_audio_info = {
      sizeof (AgsRecallAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsRecallAudio",
						   &ags_recall_audio_info,
						   0);

    g_type_add_interface_static(ags_type_recall_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);
  }

  return(ags_type_recall_audio);
}

void
ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_parent_class = g_type_class_peek_parent(recall_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio;

  gobject->dispose = ags_recall_audio_dispose;
  gobject->finalize = ags_recall_audio_finalize;

  gobject->set_property = ags_recall_audio_set_property;
  gobject->get_property = ags_recall_audio_get_property;

  /* properties */
  /**
   * AgsRecallAudio:audio:
   *
   * The assigned audio.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio object it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio;

  recall->load_automation = ags_recall_audio_load_automation;
  recall->unload_automation = ags_recall_audio_unload_automation;

  recall->automate = ags_recall_audio_automate;

  recall->duplicate = ags_recall_audio_duplicate;
}

void
ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_audio_packable_interface_init(AgsPackableInterface *packable)
{
  ags_recall_audio_parent_packable_interface = g_type_interface_peek_parent(packable);

  packable->pack = ags_recall_audio_pack;
  packable->unpack = ags_recall_audio_unpack;
}

void
ags_recall_audio_init(AgsRecallAudio *recall_audio)
{
  recall_audio->flags = 0;

  recall_audio->audio = NULL;
}

void
ags_recall_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsRecallAudio *recall_audio;

  pthread_mutex_t *recall_mutex;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;
      
      audio = (AgsAudio *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall_audio->audio == audio){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_audio->audio != NULL){
	g_object_unref(recall_audio->audio);
      }
      
      if(audio != NULL){
	g_object_ref(audio);
      }
      
      recall_audio->audio = audio;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsRecallAudio *recall_audio;

  pthread_mutex_t *recall_mutex;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_audio->audio);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_dispose(GObject *gobject)
{
  AgsRecallAudio *recall_audio;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* unpack */
  ags_packable_unpack(AGS_PACKABLE(recall_audio));
  
  /* audio */
  if(recall_audio->audio != NULL){
    g_object_unref(G_OBJECT(recall_audio->audio));

    recall_audio->audio = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_parent_class)->dispose(gobject);
}

void
ags_recall_audio_finalize(GObject *gobject)
{
  AgsRecallAudio *recall_audio;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* audio */
  if(recall_audio->audio != NULL){
    g_object_unref(G_OBJECT(recall_audio->audio));
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_parent_class)->finalize(gobject);
}

gboolean
ags_recall_audio_pack(AgsPackable *packable, GObject *recall_container)
{
  GList *list_start, *list;

  pthread_mutex_t *recall_container_mutex;

  if(ags_recall_audio_parent_packable_interface->pack(packable, container)){
    return(TRUE);
  }
  
  /* get recall container mutex */
  pthread_mutex_lock(ags_recall_container_get_class_mutex());
  
  recall_container_mutex = AGS_RECALL_CONTAINER(recall_container)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_container_get_class_mutex());

  /* set recall audio - recall container */
  g_object_set(recall_container,
	       "recall-audio", AGS_RECALL(packable),
	       NULL);

  /* set recall audio - recall audio run */
  pthread_mutex_lock(recall_container_mutex);
      
  list_start =
    list = g_list_copy(AGS_RECALL_CONTAINER(recall_container)->recall_audio_run);

  pthread_mutex_unlock(recall_container_mutex);

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "recall-audio", AGS_RECALL_AUDIO(packable),
		 NULL);

    list = list->next;
  }

  g_list_free(list_start);
  
  return(FALSE);
}

gboolean
ags_recall_audio_unpack(AgsPackable *packable)
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;

  GList *list_start, *list;

  pthread_mutex_t *recall_container_mutex;

  if(!AGS_IS_RECALL(packable)){
    return(TRUE);
  }

  recall = AGS_RECALL(packable);

  recall_container = recall->recall_container;

  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(TRUE);
  }

  /* get recall container mutex */
  pthread_mutex_lock(ags_recall_container_get_class_mutex());
  
  recall_container_mutex = AGS_RECALL_CONTAINER(recall_container)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_container_get_class_mutex());

  /* ref */
  g_object_ref(recall);
  g_object_ref(recall_container);

  /* unset recall audio - recall audio run */
  pthread_mutex_lock(recall_container_mutex);
      
  list_start =
    list = g_list_copy(AGS_RECALL_CONTAINER(recall_container)->recall_audio_run);

  pthread_mutex_unlock(recall_container_mutex);

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "recall_audio", NULL,
		 NULL);

    list = list->next;
  }

  g_list_free(list_start);

  /* call parent */
  if(ags_recall_audio_parent_packable_interface->unpack(packable)){
    g_object_unref(recall);
    g_object_unref(recall_container);

    return(TRUE);
  }

  /* unref */
  pthread_mutex_lock(recall_container_mutex);

  recall_container->recall_audio = NULL;

  pthread_mutex_unlock(recall_container_mutex);

  g_object_unref(recall);
  g_object_unref(recall_container);

  return(FALSE);
}

void
ags_recall_audio_load_automation(AgsRecall *recall,
				 GList *automation_port)
{
  AgsAudio *audio;

  GList *automation;
  
  pthread_mutex_t *recall_mutex;

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);
  
  audio = AGS_RECALL_AUDIO(recall)->audio;

  pthread_mutex_unlock(recall_mutex);

  while(automation_port != NULL){
    AgsPort *current_port;

    current_port = automation_port->data;
    
    if((AGS_PORT_IS_OUTPUT & (current_port->flags)) != 0){
      automation_port = automation_port->next;

      continue;
    }

    if(ags_automation_find_port(audio->automation,
				current_port) == NULL){
      AgsAutomation *current_automation;

      current_automation = ags_automation_new((GObject *) audio,
					      0,
					      G_TYPE_NONE,
					      current_port->specifier);
      g_object_set(current_automation,
		   "port", current_port,
		   NULL);
      ags_audio_add_automation(audio,
			       (GObject *) current_automation);

      g_object_set(current_port,
		   "automation", current_automation,
		   NULL);
    }else{
      g_object_set(current_port,
		   "automation", automation->data,
		   NULL);
    }

    /* iterate */
    automation_port = automation_port->next;
  }

  if(recall->automation_port == NULL){
    recall->automation_port = automation_port;
  }else{
    recall->automation_port = g_list_concat(recall->automation_port,
					    automation_port);
  }
}

void
ags_recall_audio_unload_automation(AgsRecall *recall)
{
  AgsAudio *audio;

  GList *automation_start, *automation;
  GList *automation_port_start, *automation_port;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_mutex;
  
  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);
  
  audio = AGS_RECALL_AUDIO(recall)->audio;

  automation_port =
    automation_port_start = g_list_copy(recall->automation_port);

  pthread_mutex_unlock(recall_mutex);
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* find automation port */
  while(automation_port != NULL){
    pthread_mutex_unlock(audio_mutex);

    automation =
      automation_start = g_list_copy(audio->automation);

    pthread_mutex_unlock(audio_mutex);

    /* remove automation */
    while((automation = ags_automation_find_port(automation,
						 (GObject *) automation_port->data)) != NULL){
      AgsAutomation *current;
  
      current = automation->data;
      ags_audio_remove_automation(audio,
				  (GObject *) current);

      /* iterate */
      automation = automation->next;
    }

    g_list_free(automation_start);

    /* iterate */
    automation_port = automation_port->next;
  }

  g_list_free(automation_port_start);

  /* free automation */
  g_list_free(recall->automation_port);
  recall->automation_port = NULL;
}

void
ags_recall_audio_automate(AgsRecall *recall)
{
  AgsAudio *audio;

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

  g_object_get(recall,
	       "audio", &audio,
	       NULL);
  
  g_object_get(audio,
	       "soundcard", &soundcard,
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
    g_object_get(AGS_PORT(port->data),
		 "automation", &automation_start,
		 NULL);

    /* find offset */
    automation = automation_start;

    while(automation != NULL){
      AgsAutomation *current_automation;

      AgsTimestamp *timestamp;
      
      guint current_automation_flags;
      
      current_automation = automation->data;

      current_automation_flags = current_automation->flags;
      
      timestamp = current_automation->timestamp;
      
      if(ags_timestamp_get_ags_offset(timestamp) + AGS_AUTOMATION_DEFAULT_OFFSET < x){
	automation = automation->next;
	
	continue;
      }
      
      if((AGS_AUTOMATION_BYPASS & (current_automation_flags)) == 0){
	GValue value = {0,};
	
	ret_x = ags_automation_get_value(current_automation,
					 floor(x), ceil(x + step),
					 return_prev_on_failure,
					 &value);

	if(ret_x != G_MAXUINT){
	  ags_port_safe_write(port->data,
			      &value);
	}
      }

      if(ags_timestamp_get_ags_offset(timestamp) > ceil(x + step)){
	break;
      }

      /* iterate */
      automation = automation->next;
    }

    g_list_free(automation_start);
    
    /* iterate */
    port = port->next;
  }

  g_list_free(port_start);
}

AgsRecall*
ags_recall_audio_duplicate(AgsRecall *recall,
			   AgsRecallID *recall_id,
			   guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecallAudio *copy;

  /* duplicate */
  copy = AGS_RECALL_AUDIO(AGS_RECALL_CLASS(ags_recall_audio_parent_class)->duplicate(recall,
										     recall_id,
										     n_params, parameter_name, value));

  g_message("ags warning - ags_recall_audio_duplicate: you shouldn't do this %s", G_OBJECT_TYPE_NAME(recall));
  
  return((AgsRecall *) copy);
}

/**
 * ags_recall_audio_new:
 * @audio: the assigned #AgsAudio
 *
 * Creates an #AgsRecallAudio.
 *
 * Returns: a new #AgsRecallAudio.
 *
 * Since: 1.0.0
 */
AgsRecallAudio*
ags_recall_audio_new(AgsAudio *audio)
{
  AgsRecallAudio *recall_audio;

  recall_audio = (AgsRecallAudio *) g_object_new(AGS_TYPE_RECALL_AUDIO,
						 "audio", audio,
						 NULL);

  return(recall_audio);
}
