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

#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_sequencer.h>
#include <ags/object/ags_distributed_manager.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <jack/midiport.h>
#include <jack/weakmacros.h>
#include <jack/types.h>

#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <ags/config.h>

/**
 * SECTION:ags_jack_midiin
 * @short_description: Input from sequencer
 * @title: AgsJackMidiin
 * @section_id:
 * @include: ags/audio/jack/ags_jack_midiin.h
 *
 * #AgsJackMidiin represents a sequencer and supports midi input.
 */

void ags_jack_midiin_class_init(AgsJackMidiinClass *jack_midiin);
void ags_jack_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_jack_midiin_init(AgsJackMidiin *jack_midiin);
void ags_jack_midiin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_midiin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_midiin_disconnect(AgsConnectable *connectable);
void ags_jack_midiin_connect(AgsConnectable *connectable);
void ags_jack_midiin_dispose(GObject *gobject);
void ags_jack_midiin_finalize(GObject *gobject);

void ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin);

void ags_jack_midiin_set_application_context(AgsSequencer *sequencer,
					     AgsApplicationContext *application_context);
AgsApplicationContext* ags_jack_midiin_get_application_context(AgsSequencer *sequencer);

void ags_jack_midiin_set_application_mutex(AgsSequencer *sequencer,
					   pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_jack_midiin_get_application_mutex(AgsSequencer *sequencer);

void ags_jack_midiin_set_device(AgsSequencer *sequencer,
				gchar *device);
gchar* ags_jack_midiin_get_device(AgsSequencer *sequencer);

void ags_jack_midiin_list_cards(AgsSequencer *sequencer,
				GList **card_id, GList **card_name);

gboolean ags_jack_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_jack_midiin_is_recording(AgsSequencer *sequencer);

void ags_jack_midiin_port_init(AgsSequencer *sequencer,
			       GError **error);
void ags_jack_midiin_port_record(AgsSequencer *sequencer,
				 GError **error);
void ags_jack_midiin_port_free(AgsSequencer *sequencer);

void ags_jack_midiin_tic(AgsSequencer *sequencer);
void ags_jack_midiin_offset_changed(AgsSequencer *sequencer,
				    guint note_offset);

void ags_jack_midiin_set_bpm(AgsSequencer *sequencer,
			     gdouble bpm);
gdouble ags_jack_midiin_get_bpm(AgsSequencer *sequencer);

void ags_jack_midiin_set_delay_factor(AgsSequencer *sequencer,
				      gdouble delay_factor);
gdouble ags_jack_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_jack_midiin_get_buffer(AgsSequencer *sequencer,
				 guint *buffer_length);
void* ags_jack_midiin_get_next_buffer(AgsSequencer *sequencer,
				      guint *buffer_length);

void ags_jack_midiin_set_note_offset(AgsSequencer *sequencer,
				     guint note_offset);
guint ags_jack_midiin_get_note_offset(AgsSequencer *sequencer);

void ags_jack_midiin_set_audio(AgsSequencer *sequencer,
			       GList *audio);
GList* ags_jack_midiin_get_audio(AgsSequencer *sequencer);

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_APPLICATION_MUTEX,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_JACK_CLIENT,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_jack_midiin_parent_class = NULL;
static guint jack_midiin_signals[LAST_SIGNAL];

GType
ags_jack_midiin_get_type (void)
{
  static GType ags_type_jack_midiin = 0;

  if(!ags_type_jack_midiin){
    static const GTypeInfo ags_jack_midiin_info = {
      sizeof (AgsJackMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackMidiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_jack_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_midiin = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackMidiin\0",
						  &ags_jack_midiin_info,
						  0);

    g_type_add_interface_static(ags_type_jack_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);
  }

  return (ags_type_jack_midiin);
}

void
ags_jack_midiin_class_init(AgsJackMidiinClass *jack_midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_jack_midiin_parent_class = g_type_class_peek_parent(jack_midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_midiin;

  gobject->set_property = ags_jack_midiin_set_property;
  gobject->get_property = ags_jack_midiin_get_property;

  gobject->dispose = ags_jack_midiin_dispose;
  gobject->finalize = ags_jack_midiin_finalize;

  /* properties */
  /**
   * AgsJackMidiin:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("application-context\0",
				   "the application context object\0",
				   "The application context object\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsJackMidiin:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_pointer("application-mutex\0",
				    "the application mutex object\0",
				    "The application mutex object\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsJackMidiin:device:
   *
   * The JACK sequencer indentifier
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_string("device\0",
				   "the device identifier\0",
				   "The device to perform output to\0",
				   "hw:0\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsJackMidiin:buffer:
   *
   * The buffer
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_pointer("buffer\0",
				    "the buffer\0",
				    "The buffer to record\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsJackMidiin:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_double("bpm\0",
				   "beats per minute\0",
				   "Beats per minute to use\0",
				   1.0,
				   240.0,
				   120.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsJackMidiin:delay-factor:
   *
   * tact
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_double("delay-factor\0",
				   "delay factor\0",
				   "The delay factor\0",
				   0.0,
				   16.0,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_FACTOR,
				  param_spec);

  /**
   * AgsJackMidiin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_pointer("attack\0",
				    "attack of buffer\0",
				    "The attack to use for the buffer\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /**
   * AgsJackMidiin:jack-client:
   *
   * The assigned #AgsJackClient
   * 
   * Since: 0.7.3
   */
  param_spec = g_param_spec_object("jack-client\0",
				   "jack client object\0",
				   "The jack client object\0",
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /* AgsJackMidiinClass */
}

GQuark
ags_jack_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-jack_midiin-error-quark\0"));
}

void
ags_jack_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_jack_midiin_connect;
  connectable->disconnect = ags_jack_midiin_disconnect;
}

void
ags_jack_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_application_context = ags_jack_midiin_set_application_context;
  sequencer->get_application_context = ags_jack_midiin_get_application_context;

  sequencer->set_application_mutex = ags_jack_midiin_set_application_mutex;
  sequencer->get_application_mutex = ags_jack_midiin_get_application_mutex;

  sequencer->set_device = ags_jack_midiin_set_device;
  sequencer->get_device = ags_jack_midiin_get_device;

  sequencer->list_cards = ags_jack_midiin_list_cards;

  sequencer->is_starting =  ags_jack_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_jack_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_jack_midiin_port_init;
  sequencer->record = ags_jack_midiin_port_record;

  sequencer->stop = ags_jack_midiin_port_free;

  sequencer->tic = ags_jack_midiin_tic;
  sequencer->offset_changed = ags_jack_midiin_offset_changed;
    
  sequencer->set_bpm = ags_jack_midiin_set_bpm;
  sequencer->get_bpm = ags_jack_midiin_get_bpm;

  sequencer->set_delay_factor = ags_jack_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_jack_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_jack_midiin_get_buffer;
  sequencer->get_next_buffer = ags_jack_midiin_get_next_buffer;

  sequencer->set_note_offset = ags_jack_midiin_set_note_offset;
  sequencer->get_note_offset = ags_jack_midiin_get_note_offset;

  sequencer->set_audio = ags_jack_midiin_set_audio;
  sequencer->get_audio = ags_jack_midiin_get_audio;
}

void
ags_jack_midiin_init(AgsJackMidiin *jack_midiin)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert jack midiin mutex */
  //FIXME:JK: memory leak
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) jack_midiin,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  jack_midiin->flags = (AGS_JACK_MIDIIN_ALSA);
  g_atomic_int_set(&(jack_midiin->sync_flags),
		   AGS_JACK_MIDIIN_PASS_THROUGH);

  jack_midiin->card_uri = NULL;
  jack_midiin->jack_client = NULL;

  jack_midiin->port_name = NULL;
  jack_midiin->jack_port = NULL;

  /* buffer */
  jack_midiin->buffer = (char **) malloc(4 * sizeof(char*));

  jack_midiin->buffer[0] = NULL;
  jack_midiin->buffer[1] = NULL;
  jack_midiin->buffer[2] = NULL;
  jack_midiin->buffer[3] = NULL;

  jack_midiin->buffer_size[0] = 0;
  jack_midiin->buffer_size[1] = 0;
  jack_midiin->buffer_size[2] = 0;
  jack_midiin->buffer_size[3] = 0;

  /* bpm */
  jack_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  jack_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  jack_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
    
  /* counters */
  jack_midiin->note_offset = 0;
  jack_midiin->tact_counter = 0.0;
  jack_midiin->delay_counter = 0;
  jack_midiin->tic_counter = 0;

  /* callback mutex */
  jack_midiin->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_midiin->callback_mutex,
		     NULL);

  jack_midiin->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_midiin->callback_cond, NULL);

  /* callback finish mutex */
  jack_midiin->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_midiin->callback_finish_mutex,
		     NULL);

  jack_midiin->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_midiin->callback_finish_cond, NULL);

  /* parent */
  jack_midiin->application_context = NULL;
  jack_midiin->application_mutex = NULL;

  /* all AgsAudio */
  jack_midiin->audio = NULL;
}

void
ags_jack_midiin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(jack_midiin->application_context == (GObject *) application_context){
	return;
      }

      if(jack_midiin->application_context != NULL){
	g_object_unref(G_OBJECT(jack_midiin->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *str;
	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	jack_midiin->application_mutex = application_context->mutex;	
      }else{
	jack_midiin->application_mutex = NULL;
      }

      jack_midiin->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(jack_midiin->application_mutex == application_mutex){
	return;
      }
      
      jack_midiin->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      jack_midiin->card_uri = g_strdup(device);
    }
    break;
  case PROP_BUFFER:
    {
      //TODO:JK: implement me
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      jack_midiin->bpm = bpm;
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      jack_midiin->delay_factor = delay_factor;
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = g_value_get_object(value);

      if(jack_midiin->jack_client == (GObject *) jack_client){
	return;
      }

      if(jack_midiin->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_midiin->jack_client));
      }

      jack_midiin->jack_client = (GObject *) jack_client;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_midiin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, jack_midiin->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, jack_midiin->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, jack_midiin->card_uri);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, jack_midiin->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, jack_midiin->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, jack_midiin->delay_factor);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_value_set_object(value, jack_midiin->jack_client);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_midiin_dispose(GObject *gobject)
{
  AgsJackMidiin *jack_midiin;

  GList *list;

  jack_midiin = AGS_JACK_MIDIIN(gobject);

  /* audio */
  if(jack_midiin->audio != NULL){
    list = jack_midiin->audio;

    while(list != NULL){
      g_object_set(list->data,
		   "sequencer\0", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(jack_midiin->audio,
		     g_object_unref);

    jack_midiin->audio = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_midiin_parent_class)->dispose(gobject);
}

void
ags_jack_midiin_finalize(GObject *gobject)
{
  AgsJackMidiin *jack_midiin;

  AgsMutexManager *mutex_manager;
  
  GList *list;

  jack_midiin = AGS_JACK_MIDIIN(gobject);

  /* remove jack_midiin mutex */
  pthread_mutex_lock(jack_midiin->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(jack_midiin->application_mutex);

  /* free output buffer */
  if(jack_midiin->buffer[0] != NULL){
    free(jack_midiin->buffer[0]);
  }

  if(jack_midiin->buffer[1] != NULL){
    free(jack_midiin->buffer[1]);
  }
    
  if(jack_midiin->buffer[2] != NULL){
    free(jack_midiin->buffer[2]);
  }
  
  if(jack_midiin->buffer[3] != NULL){
    free(jack_midiin->buffer[3]);
  }
  
  /* free buffer array */
  free(jack_midiin->buffer);

  /* audio */
  if(jack_midiin->audio != NULL){
    list = jack_midiin->audio;

    while(list != NULL){
      g_object_set(list->data,
		   "sequencer\0", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(jack_midiin->audio,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_midiin_parent_class)->finalize(gobject);
}

void
ags_jack_midiin_connect(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;
  
  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* create jack_midiin mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  /* insert mutex */
  pthread_mutex_lock(jack_midiin->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) jack_midiin,
			   mutex);
  
  pthread_mutex_unlock(jack_midiin->application_mutex);

  /*  */  
  list = jack_midiin->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_jack_midiin_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_jack_midiin_switch_buffer_flag:
 * @jack_midiin: an #AgsJackMidiin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 0.7.0
 */
void
ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin)
{
  if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
    jack_midiin->flags &= (~AGS_JACK_MIDIIN_BUFFER0);
    jack_midiin->flags |= AGS_JACK_MIDIIN_BUFFER1;

    /* clear buffer */
    if(jack_midiin->buffer[3] != NULL){
      free(jack_midiin->buffer[3]);
    }

    jack_midiin->buffer[3] = NULL;
    jack_midiin->buffer_size[3] = 0;
  }else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
    jack_midiin->flags &= (~AGS_JACK_MIDIIN_BUFFER1);
    jack_midiin->flags |= AGS_JACK_MIDIIN_BUFFER2;

    /* clear buffer */
    if(jack_midiin->buffer[0] != NULL){
      free(jack_midiin->buffer[0]);
    }

    jack_midiin->buffer[0] = NULL;
    jack_midiin->buffer_size[0] = 0;
  }else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
    jack_midiin->flags &= (~AGS_JACK_MIDIIN_BUFFER2);
    jack_midiin->flags |= AGS_JACK_MIDIIN_BUFFER3;

    /* clear buffer */
    if(jack_midiin->buffer[1] != NULL){
      free(jack_midiin->buffer[1]);
    }

    jack_midiin->buffer[1] = NULL;
    jack_midiin->buffer_size[1] = 0;
  }else if((AGS_JACK_MIDIIN_BUFFER3 & (jack_midiin->flags)) != 0){
    jack_midiin->flags &= (~AGS_JACK_MIDIIN_BUFFER3);
    jack_midiin->flags |= AGS_JACK_MIDIIN_BUFFER0;

    /* clear buffer */
    if(jack_midiin->buffer[2] != NULL){
      free(jack_midiin->buffer[2]);
    }

    jack_midiin->buffer[2] = NULL;
    jack_midiin->buffer_size[2] = 0;
  }
}

void
ags_jack_midiin_set_application_context(AgsSequencer *sequencer,
					AgsApplicationContext *application_context)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  jack_midiin->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_jack_midiin_get_application_context(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  return((AgsApplicationContext *) jack_midiin->application_context);
}

void
ags_jack_midiin_set_application_mutex(AgsSequencer *sequencer,
				      pthread_mutex_t *application_mutex)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  jack_midiin->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_jack_midiin_get_application_mutex(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  return(jack_midiin->application_mutex);
}

void
ags_jack_midiin_set_device(AgsSequencer *sequencer,
			   gchar *device)
{
  AgsJackMidiin *jack_midiin;

  GList *jack_port, *jack_port_start;

  gchar *str;
  
  int ret;
  guint nth_card;
  guint i;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  if(jack_midiin->card_uri == device ||
     !g_ascii_strcasecmp(jack_midiin->card_uri,
			 device)){
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-midiin-\0")){
    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-midiin-%u\0",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid JACK device specifier");

    return;
  }

  if(jack_midiin->card_uri != NULL){
    g_free(jack_midiin->card_uri);
  }
  
  jack_midiin->card_uri = g_strdup(device);

  /* apply name to port */
  jack_port_start = 
    jack_port = g_list_copy(jack_midiin->jack_port);
  
  str = g_strdup_printf("ags-sequencer%d\0",
			nth_card);
    
  g_object_set(jack_port->data,
	       "port-name\0", str,
	       NULL);
  g_free(str);

  g_list_free(jack_port_start);
}

gchar*
ags_jack_midiin_get_device(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  return(jack_midiin->card_uri);
}

void
ags_jack_midiin_list_cards(AgsSequencer *sequencer,
			   GList **card_id, GList **card_name)
{
  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  application_context = (AgsApplicationContext *) jack_midiin->application_context;

  if(application_context == NULL){
    return;
  }
  
  application_mutex = jack_midiin->application_mutex;
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  pthread_mutex_lock(application_mutex);

  list_start = 
    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_MIDIIN(list->data)){
      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id,
				  g_strdup(AGS_JACK_MIDIIN(list->data)->card_uri));
      }

      if(card_name != NULL){
	if(AGS_JACK_MIDIIN(list->data)->jack_client != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(AGS_JACK_CLIENT(AGS_JACK_MIDIIN(list->data)->jack_client)->name));
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)\0"));

	  g_warning("ags_jack_midiin_list_cards() - JACK client not connected (null)\0");
	}
      }      
    }

    list = list->next;
  }

  pthread_mutex_unlock(application_mutex);
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

gboolean
ags_jack_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  return(((AGS_JACK_MIDIIN_START_RECORD & (jack_midiin->flags)) != 0) ? TRUE: FALSE);
}

gboolean
ags_jack_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  return(((AGS_JACK_MIDIIN_RECORD & (jack_midiin->flags)) != 0) ? TRUE: FALSE);
}

void
ags_jack_midiin_port_init(AgsSequencer *sequencer,
			  GError **error)
{
  AgsJackMidiin *jack_midiin;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

  /* prepare for record */
  jack_midiin->flags |= (AGS_JACK_MIDIIN_BUFFER3 |
			 AGS_JACK_MIDIIN_START_RECORD |
			 AGS_JACK_MIDIIN_RECORD |
			 AGS_JACK_MIDIIN_NONBLOCKING);

  jack_midiin->note_offset = 0;

  /* port setup */
  //TODO:JK: implement me
  
  /*  */
  jack_midiin->tact_counter = 0.0;
  jack_midiin->delay_counter = 0.0;
  jack_midiin->tic_counter = 0;

  jack_midiin->flags |= (AGS_JACK_MIDIIN_INITIALIZED |
			 AGS_JACK_MIDIIN_START_RECORD |
			 AGS_JACK_MIDIIN_RECORD);
  
  g_atomic_int_and(&(jack_midiin->sync_flags),
		   (~(AGS_JACK_MIDIIN_PASS_THROUGH)));
  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_INITIAL_CALLBACK);
  
  pthread_mutex_unlock(mutex);
}

void
ags_jack_midiin_port_record(AgsSequencer *sequencer,
			    GError **error)
{
  AgsJackClient *jack_client;
  AgsJackMidiin *jack_midiin;

  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  gboolean jack_client_activated;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  pthread_mutex_t *client_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /*  */
  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  task_thread = (AgsTaskThread *) application_context->task_thread;

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* client */
  pthread_mutex_lock(mutex);

  jack_client = (AgsJackClient *) jack_midiin->jack_client;
  
  callback_mutex = jack_midiin->callback_mutex;
  callback_finish_mutex = jack_midiin->callback_finish_mutex;

  pthread_mutex_unlock(mutex);

  /* do record */
  pthread_mutex_lock(mutex);
  
  jack_midiin->flags &= (~AGS_JACK_MIDIIN_START_RECORD);

  if((AGS_JACK_MIDIIN_INITIALIZED & (jack_midiin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  pthread_mutex_unlock(mutex);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  client_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) jack_client);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(client_mutex);

  if(jack_client_activated){
    /* signal client */
    if((AGS_JACK_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_midiin->sync_flags),
		      AGS_JACK_MIDIIN_CALLBACK_DONE);
    
      if((AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	pthread_cond_signal(jack_midiin->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_midiin->sync_flags),
			AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0 &&
	      (AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	  pthread_cond_wait(jack_midiin->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_midiin->sync_flags),
		       (~(AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT |
			  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_midiin->sync_flags),
		       (~AGS_JACK_MIDIIN_INITIAL_CALLBACK));
    }
  }

  if(task_thread != NULL){
    AgsTicDevice *tic_device;
    AgsSwitchBufferFlag *switch_buffer_flag;
      
    GList *task;
      
    task = NULL;
  
    /* tic sequencer */
    tic_device = ags_tic_device_new((GObject *) jack_midiin);
    task = g_list_append(task,
			 tic_device);
  
    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) jack_midiin);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  }else{
    /* tic */
    ags_sequencer_tic(AGS_SEQUENCER(jack_midiin));
	  
    /* reset - switch buffer flags */
    ags_jack_midiin_switch_buffer_flag(jack_midiin);
  }  
}

void
ags_jack_midiin_port_free(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(mutex);

  if((AGS_JACK_MIDIIN_INITIALIZED & (jack_midiin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  callback_mutex = jack_midiin->callback_mutex;
  callback_finish_mutex = jack_midiin->callback_finish_mutex;

  jack_midiin->flags &= (~(AGS_JACK_MIDIIN_BUFFER0 |
			   AGS_JACK_MIDIIN_BUFFER1 |
			   AGS_JACK_MIDIIN_BUFFER2 |
			   AGS_JACK_MIDIIN_BUFFER3 |
			   AGS_JACK_MIDIIN_RECORD));

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_PASS_THROUGH);
  g_atomic_int_and(&(jack_midiin->sync_flags),
		   (~AGS_JACK_MIDIIN_INITIAL_CALLBACK));

  pthread_mutex_unlock(mutex);

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_CALLBACK_DONE);
    
  if((AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
    pthread_cond_signal(jack_midiin->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
    pthread_cond_signal(jack_midiin->callback_finish_cond);
  }

  pthread_mutex_unlock(callback_finish_mutex);

  /*  */
  pthread_mutex_lock(mutex);

  if(jack_midiin->buffer[1] != NULL){
    free(jack_midiin->buffer[1]);
    jack_midiin->buffer_size[1] = 0;
  }

  if(jack_midiin->buffer[2] != NULL){
    free(jack_midiin->buffer[2]);
    jack_midiin->buffer_size[2] = 0;
  }

  if(jack_midiin->buffer[3] != NULL){
    free(jack_midiin->buffer[3]);
    jack_midiin->buffer_size[3] = 0;
  }

  if(jack_midiin->buffer[0] != NULL){
    free(jack_midiin->buffer[0]);
    jack_midiin->buffer_size[0] = 0;
  }

  pthread_mutex_unlock(mutex);
}

void
ags_jack_midiin_tic(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;
  gdouble delay;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* determine if attack should be switched */
  delay = jack_midiin->delay;

  if((guint) jack_midiin->delay_counter + 1 >= (guint) delay){
    ags_sequencer_set_note_offset(sequencer,
				  jack_midiin->note_offset_absolute + 1);
    
    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 jack_midiin->note_offset);
    
    /* reset - delay counter */
    jack_midiin->delay_counter = 0.0;
    jack_midiin->tact_counter += 1.0;
  }else{
    jack_midiin->delay_counter += 1.0;
  }
}

void
ags_jack_midiin_offset_changed(AgsSequencer *sequencer,
			       guint note_offset)
{
  AgsJackMidiin *jack_midiin;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  jack_midiin->tic_counter += 1;

  if(jack_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_midiin->tic_counter = 0;
  }
}

void
ags_jack_midiin_set_bpm(AgsSequencer *sequencer,
			gdouble bpm)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  jack_midiin->bpm = bpm;
}

gdouble
ags_jack_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  return(jack_midiin->bpm);
}

void
ags_jack_midiin_set_delay_factor(AgsSequencer *sequencer, gdouble delay_factor)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  jack_midiin->delay_factor = delay_factor;
}

gdouble
ags_jack_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  return(jack_midiin->delay_factor);
}

void*
ags_jack_midiin_get_buffer(AgsSequencer *sequencer,
			   guint *buffer_length)
{
  AgsJackMidiin *jack_midiin;
  char *buffer;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get buffer */
  if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[0];
  }else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[1];
  }else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[2];
  }else if((AGS_JACK_MIDIIN_BUFFER3 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[3];
  }else{
    buffer = NULL;
  }
  
  /* return the buffer's length */
  if(buffer_length != NULL){
    if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[0];
    }else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[1];
    }else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[2];
    }else if((AGS_JACK_MIDIIN_BUFFER3 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[3];
    }else{
      *buffer_length = 0;
    }
  }

  return(buffer);
}

void*
ags_jack_midiin_get_next_buffer(AgsSequencer *sequencer,
				guint *buffer_length)
{
  AgsJackMidiin *jack_midiin;
  unsigned char *buffer;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get buffer */
  if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[1];
  }else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[2];
  }else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[3];
  }else if((AGS_JACK_MIDIIN_BUFFER3 & (jack_midiin->flags)) != 0){
    buffer = jack_midiin->buffer[0];
  }else{
    buffer = NULL;
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[1];
    }else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[2];
    }else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[3];
    }else if((AGS_JACK_MIDIIN_BUFFER3 & (jack_midiin->flags)) != 0){
      *buffer_length = jack_midiin->buffer_size[0];
    }else{
      *buffer_length = 0;
    }
  }  

  return(buffer);
}

void
ags_jack_midiin_set_note_offset(AgsSequencer *sequencer,
				guint note_offset)
{
  AGS_JACK_MIDIIN(sequencer)->note_offset = note_offset;
}

guint
ags_jack_midiin_get_note_offset(AgsSequencer *sequencer)
{
  return(AGS_JACK_MIDIIN(sequencer)->note_offset);
}

void
ags_jack_midiin_set_audio(AgsSequencer *sequencer,
			  GList *audio)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  jack_midiin->audio = audio;
}

GList*
ags_jack_midiin_get_audio(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  return(jack_midiin->audio);
}

/**
 * ags_jack_midiin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsJackMidiin, refering to @application_context.
 *
 * Returns: a new #AgsJackMidiin
 *
 * Since: 0.7.0
 */
AgsJackMidiin*
ags_jack_midiin_new(GObject *application_context)
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = (AgsJackMidiin *) g_object_new(AGS_TYPE_JACK_MIDIIN,
					       "application-context\0", application_context,
					       NULL);
  
  return(jack_midiin);
}
