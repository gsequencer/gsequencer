/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_recall_container.h>

#include <math.h>

#include <ags/i18n.h>

void ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio);
void ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable);
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

GType
ags_recall_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_audio = 0;

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

    ags_type_recall_audio = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsRecallAudio",
						   &ags_recall_audio_info,
						   0);

    g_type_add_interface_static(ags_type_recall_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_audio);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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

  recall->automate = ags_recall_audio_automate;

  recall->duplicate = ags_recall_audio_duplicate;
}

void
ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);
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

  GRecMutex *recall_mutex;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;
      
      audio = (AgsAudio *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_audio->audio == audio){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_audio->audio != NULL){
	g_object_unref(recall_audio->audio);
      }
      
      if(audio != NULL){
	g_object_ref(audio);
      }
      
      recall_audio->audio = audio;

      g_rec_mutex_unlock(recall_mutex);
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

  GRecMutex *recall_mutex;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_audio->audio);

      g_rec_mutex_unlock(recall_mutex);
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
  
  /* audio */
  if(recall_audio->audio != NULL){
    gpointer tmp;

    tmp = recall_audio->audio;

    recall_audio->audio = NULL;
    
    g_object_unref(tmp);
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
    gpointer tmp;

    tmp = recall_audio->audio;

    recall_audio->audio = NULL;
    
    g_object_unref(tmp);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_parent_class)->finalize(gobject);
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

  GRecMutex *audio_mutex;

  audio = NULL;

  soundcard = NULL;

  port_start = NULL;
  
  g_object_get(recall,
	       "audio", &audio,
	       NULL);
  
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  if(audio->automation_port == NULL){  
    g_rec_mutex_unlock(audio_mutex);

    if(audio != NULL){
      g_object_unref(audio);
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

    success = g_strv_contains(audio->automation_port, specifier);
    
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

  if(soundcard != NULL){
    g_object_unref(soundcard);
  }
  
  g_list_free_full(port_start,
		   g_object_unref);
}

AgsRecall*
ags_recall_audio_duplicate(AgsRecall *recall,
			   AgsRecallID *recall_id,
			   guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecallAudio *copy_recall_audio;

  /* duplicate */
  copy_recall_audio = AGS_RECALL_AUDIO(AGS_RECALL_CLASS(ags_recall_audio_parent_class)->duplicate(recall,
												  recall_id,
												  n_params, parameter_name, value));
  
  g_warning("ags_recall_audio_duplicate - you shouldn't do this %s", G_OBJECT_TYPE_NAME(recall));
  
  return((AgsRecall *) copy_recall_audio);
}

/**
 * ags_recall_audio_get_audio:
 * @recall_audio: the #AgsRecallAudio
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
AgsAudio*
ags_recall_audio_get_audio(AgsRecallAudio *recall_audio)
{
  AgsAudio *audio;

  if(!AGS_IS_RECALL_AUDIO(recall_audio)){
    return(NULL);
  }

  g_object_get(recall_audio,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_recall_audio_set_audio:
 * @recall_audio: the #AgsRecallAudio
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_audio_set_audio(AgsRecallAudio *recall_audio, AgsAudio *audio)
{
  if(!AGS_IS_RECALL_AUDIO(recall_audio)){
    return;
  }

  g_object_set(recall_audio,
	       "audio", audio,
	       NULL);
}

/**
 * ags_recall_audio_new:
 * @audio: the assigned #AgsAudio
 *
 * Creates an #AgsRecallAudio.
 *
 * Returns: a new #AgsRecallAudio.
 *
 * Since: 3.0.0
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
