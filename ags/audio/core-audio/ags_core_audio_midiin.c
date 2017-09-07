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

#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_sequencer.h>
#include <ags/object/ags_distributed_manager.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_core_audio_midiin_class_init(AgsCoreAudioMidiinClass *core_audio_midiin);
void ags_core_audio_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_core_audio_midiin_init(AgsCoreAudioMidiin *core_audio_midiin);
void ags_core_audio_midiin_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_midiin_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_midiin_disconnect(AgsConnectable *connectable);
void ags_core_audio_midiin_connect(AgsConnectable *connectable);
void ags_core_audio_midiin_dispose(GObject *gobject);
void ags_core_audio_midiin_finalize(GObject *gobject);

void ags_core_audio_midiin_set_application_context(AgsSequencer *sequencer,
						   AgsApplicationContext *application_context);
AgsApplicationContext* ags_core_audio_midiin_get_application_context(AgsSequencer *sequencer);

void ags_core_audio_midiin_set_application_mutex(AgsSequencer *sequencer,
						 pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_core_audio_midiin_get_application_mutex(AgsSequencer *sequencer);

void ags_core_audio_midiin_set_device(AgsSequencer *sequencer,
				      gchar *device);
gchar* ags_core_audio_midiin_get_device(AgsSequencer *sequencer);

void ags_core_audio_midiin_list_cards(AgsSequencer *sequencer,
				      GList **card_id, GList **card_name);

gboolean ags_core_audio_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_core_audio_midiin_is_recording(AgsSequencer *sequencer);

void ags_core_audio_midiin_port_init(AgsSequencer *sequencer,
				     GError **error);
void ags_core_audio_midiin_port_record(AgsSequencer *sequencer,
				       GError **error);
void ags_core_audio_midiin_port_free(AgsSequencer *sequencer);

void ags_core_audio_midiin_tic(AgsSequencer *sequencer);
void ags_core_audio_midiin_offset_changed(AgsSequencer *sequencer,
					  guint note_offset);

void ags_core_audio_midiin_set_bpm(AgsSequencer *sequencer,
				   gdouble bpm);
gdouble ags_core_audio_midiin_get_bpm(AgsSequencer *sequencer);

void ags_core_audio_midiin_set_delay_factor(AgsSequencer *sequencer,
					    gdouble delay_factor);
gdouble ags_core_audio_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_core_audio_midiin_get_buffer(AgsSequencer *sequencer,
				       guint *buffer_length);
void* ags_core_audio_midiin_get_next_buffer(AgsSequencer *sequencer,
					    guint *buffer_length);

void ags_core_audio_midiin_set_note_offset(AgsSequencer *sequencer,
					   guint note_offset);
guint ags_core_audio_midiin_get_note_offset(AgsSequencer *sequencer);

void ags_core_audio_midiin_set_audio(AgsSequencer *sequencer,
				     GList *audio);
GList* ags_core_audio_midiin_get_audio(AgsSequencer *sequencer);

/**
 * SECTION:ags_core_audio_midiin
 * @short_description: Input from sequencer
 * @title: AgsCoreAudioMidiin
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_midiin.h
 *
 * #AgsCoreAudioMidiin represents a sequencer and supports midi input.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_APPLICATION_MUTEX,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_core_audio_midiin_parent_class = NULL;
static guint core_audio_midiin_signals[LAST_SIGNAL];

GType
ags_core_audio_midiin_get_type (void)
{
  static GType ags_type_core_audio_midiin = 0;

  if(!ags_type_core_audio_midiin){
    static const GTypeInfo ags_core_audio_midiin_info = {
      sizeof (AgsCoreAudioMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCoreAudioMidiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_midiin = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioMidiin",
							&ags_core_audio_midiin_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);
  }

  return (ags_type_core_audio_midiin);
}

void
ags_core_audio_midiin_class_init(AgsCoreAudioMidiinClass *core_audio_midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_core_audio_midiin_parent_class = g_type_class_peek_parent(core_audio_midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_midiin;

  gobject->set_property = ags_core_audio_midiin_set_property;
  gobject->get_property = ags_core_audio_midiin_get_property;

  gobject->dispose = ags_core_audio_midiin_dispose;
  gobject->finalize = ags_core_audio_midiin_finalize;

  /* properties */
  /**
   * AgsCoreAudioMidiin:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("the application context object"),
				   i18n_pspec("The application context object"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("application-mutex",
				    i18n_pspec("the application mutex object"),
				    i18n_pspec("The application mutex object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:device:
   *
   * The core audio sequencer indentifier
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "hw:0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:buffer:
   *
   * The buffer
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("beats per minute"),
				   i18n_pspec("Beats per minute to use"),
				   1.0,
				   240.0,
				   120.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:delay-factor:
   *
   * tact
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_double("delay-factor",
				   i18n_pspec("delay factor"),
				   i18n_pspec("The delay factor"),
				   0.0,
				   16.0,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_FACTOR,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /**
   * AgsCoreAudioMidiin:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("core audio client object"),
				   i18n_pspec("The core audio client object"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioMidiin:core-audio-port:
   *
   * The assigned #AgsCoreAudioPort
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("core-audio-port",
				   i18n_pspec("core audio port object"),
				   i18n_pspec("The core audio port object"),
				   AGS_TYPE_CORE_AUDIO_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_PORT,
				  param_spec);

  /* AgsCoreAudioMidiinClass */
}

GQuark
ags_core_audio_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_midiin-error-quark"));
}

void
ags_core_audio_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_core_audio_midiin_connect;
  connectable->disconnect = ags_core_audio_midiin_disconnect;
}

void
ags_core_audio_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_application_context = ags_core_audio_midiin_set_application_context;
  sequencer->get_application_context = ags_core_audio_midiin_get_application_context;

  sequencer->set_application_mutex = ags_core_audio_midiin_set_application_mutex;
  sequencer->get_application_mutex = ags_core_audio_midiin_get_application_mutex;

  sequencer->set_device = ags_core_audio_midiin_set_device;
  sequencer->get_device = ags_core_audio_midiin_get_device;

  sequencer->list_cards = ags_core_audio_midiin_list_cards;

  sequencer->is_starting =  ags_core_audio_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_core_audio_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_core_audio_midiin_port_init;
  sequencer->record = ags_core_audio_midiin_port_record;

  sequencer->stop = ags_core_audio_midiin_port_free;

  sequencer->tic = ags_core_audio_midiin_tic;
  sequencer->offset_changed = ags_core_audio_midiin_offset_changed;
    
  sequencer->set_bpm = ags_core_audio_midiin_set_bpm;
  sequencer->get_bpm = ags_core_audio_midiin_get_bpm;

  sequencer->set_delay_factor = ags_core_audio_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_core_audio_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_core_audio_midiin_get_buffer;
  sequencer->get_next_buffer = ags_core_audio_midiin_get_next_buffer;

  sequencer->set_note_offset = ags_core_audio_midiin_set_note_offset;
  sequencer->get_note_offset = ags_core_audio_midiin_get_note_offset;

  sequencer->set_audio = ags_core_audio_midiin_set_audio;
  sequencer->get_audio = ags_core_audio_midiin_get_audio;
}

void
ags_core_audio_midiin_init(AgsCoreAudioMidiin *core_audio_midiin)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert jack midiin mutex */
  core_audio_midiin->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  core_audio_midiin->mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_midiin,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  core_audio_midiin->flags = (AGS_CORE_AUDIO_MIDIIN_ALSA);
  g_atomic_int_set(&(core_audio_midiin->sync_flags),
		   AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH);

  core_audio_midiin->card_uri = NULL;
  core_audio_midiin->core_audio_client = NULL;

  core_audio_midiin->port_name = NULL;
  core_audio_midiin->core_audio_port = NULL;

  /* buffer */
  core_audio_midiin->buffer = (char **) malloc(4 * sizeof(char*));

  core_audio_midiin->buffer[0] = NULL;
  core_audio_midiin->buffer[1] = NULL;
  core_audio_midiin->buffer[2] = NULL;
  core_audio_midiin->buffer[3] = NULL;

  core_audio_midiin->buffer_size[0] = 0;
  core_audio_midiin->buffer_size[1] = 0;
  core_audio_midiin->buffer_size[2] = 0;
  core_audio_midiin->buffer_size[3] = 0;

  /* bpm */
  core_audio_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  core_audio_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  core_audio_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
    
  /* counters */
  core_audio_midiin->note_offset = 0;
  core_audio_midiin->tact_counter = 0.0;
  core_audio_midiin->delay_counter = 0;
  core_audio_midiin->tic_counter = 0;

  /* callback mutex */
  core_audio_midiin->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_midiin->callback_mutex,
		     NULL);

  core_audio_midiin->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_midiin->callback_cond, NULL);

  /* callback finish mutex */
  core_audio_midiin->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_midiin->callback_finish_mutex,
		     NULL);

  core_audio_midiin->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_midiin->callback_finish_cond, NULL);

  /* parent */
  core_audio_midiin->application_context = NULL;
  core_audio_midiin->application_mutex = NULL;

  /* all AgsAudio */
  core_audio_midiin->audio = NULL;
}

void
ags_core_audio_midiin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(core_audio_midiin->application_context == (GObject *) application_context){
	return;
      }

      if(core_audio_midiin->application_context != NULL){
	g_object_unref(G_OBJECT(core_audio_midiin->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *str;
	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	core_audio_midiin->application_mutex = application_context->mutex;	
      }else{
	core_audio_midiin->application_mutex = NULL;
      }

      core_audio_midiin->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(core_audio_midiin->application_mutex == application_mutex){
	return;
      }
      
      core_audio_midiin->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      core_audio_midiin->card_uri = g_strdup(device);
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

      core_audio_midiin->bpm = bpm;
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      core_audio_midiin->delay_factor = delay_factor;
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsJackClient *core_audio_client;

      core_audio_client = g_value_get_object(value);

      if(core_audio_midiin->core_audio_client == (GObject *) core_audio_client){
	return;
      }

      if(core_audio_midiin->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_midiin->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(G_OBJECT(core_audio_client));
      }

      core_audio_midiin->core_audio_client = (GObject *) core_audio_client;
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsJackPort *core_audio_port;

      core_audio_port = (AgsJackPort *) g_value_get_object(value);

      if(g_list_find(core_audio_midiin->core_audio_port, core_audio_port) != NULL){
	return;
      }

      if(core_audio_port != NULL){
	g_object_ref(core_audio_port);
	core_audio_midiin->core_audio_port = g_list_append(core_audio_midiin->core_audio_port,
							   core_audio_port);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_midiin_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, core_audio_midiin->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, core_audio_midiin->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, core_audio_midiin->card_uri);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, core_audio_midiin->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, core_audio_midiin->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, core_audio_midiin->delay_factor);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_value_set_object(value, core_audio_midiin->core_audio_client);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(core_audio_midiin->core_audio_port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_midiin_dispose(GObject *gobject)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GList *list;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  /* audio */
  if(core_audio_midiin->audio != NULL){
    list = core_audio_midiin->audio;

    while(list != NULL){
      g_object_set(list->data,
		   "sequencer", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(core_audio_midiin->audio,
		     g_object_unref);

    core_audio_midiin->audio = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_midiin_parent_class)->dispose(gobject);
}

void
ags_core_audio_midiin_finalize(GObject *gobject)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsMutexManager *mutex_manager;
  
  GList *list;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  /* remove core_audio_midiin mutex */
  pthread_mutex_lock(core_audio_midiin->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(core_audio_midiin->application_mutex);

  /* free output buffer */
  if(core_audio_midiin->buffer[0] != NULL){
    free(core_audio_midiin->buffer[0]);
  }

  if(core_audio_midiin->buffer[1] != NULL){
    free(core_audio_midiin->buffer[1]);
  }
    
  if(core_audio_midiin->buffer[2] != NULL){
    free(core_audio_midiin->buffer[2]);
  }
  
  if(core_audio_midiin->buffer[3] != NULL){
    free(core_audio_midiin->buffer[3]);
  }
  
  /* free buffer array */
  free(core_audio_midiin->buffer);

  /* audio */
  if(core_audio_midiin->audio != NULL){
    list = core_audio_midiin->audio;

    while(list != NULL){
      g_object_set(list->data,
		   "sequencer", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(core_audio_midiin->audio,
		     g_object_unref);
  }
  
  pthread_mutex_destroy(core_audio_midiin->mutex);
  free(core_audio_midiin->mutex);

  pthread_mutexattr_destroy(core_audio_midiin->mutexattr);
  free(core_audio_midiin->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_midiin_parent_class)->finalize(gobject);
}

void
ags_core_audio_midiin_connect(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* create core_audio_midiin mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  /* insert mutex */
  pthread_mutex_lock(core_audio_midiin->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_midiin,
			   mutex);
  
  pthread_mutex_unlock(core_audio_midiin->application_mutex);

  /*  */  
  list = core_audio_midiin->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_core_audio_midiin_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_core_audio_midiin_switch_buffer_flag:
 * @core_audio_midiin: an #AgsCoreAudioMidiin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_midiin_switch_buffer_flag(AgsCoreAudioMidiin *core_audio_midiin)
{
  if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
    core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_BUFFER0);
    core_audio_midiin->flags |= AGS_CORE_AUDIO_MIDIIN_BUFFER1;

    /* clear buffer */
    if(core_audio_midiin->buffer[3] != NULL){
      free(core_audio_midiin->buffer[3]);
    }

    core_audio_midiin->buffer[3] = NULL;
    core_audio_midiin->buffer_size[3] = 0;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
    core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_BUFFER1);
    core_audio_midiin->flags |= AGS_CORE_AUDIO_MIDIIN_BUFFER2;

    /* clear buffer */
    if(core_audio_midiin->buffer[0] != NULL){
      free(core_audio_midiin->buffer[0]);
    }

    core_audio_midiin->buffer[0] = NULL;
    core_audio_midiin->buffer_size[0] = 0;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
    core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_BUFFER2);
    core_audio_midiin->flags |= AGS_CORE_AUDIO_MIDIIN_BUFFER3;

    /* clear buffer */
    if(core_audio_midiin->buffer[1] != NULL){
      free(core_audio_midiin->buffer[1]);
    }

    core_audio_midiin->buffer[1] = NULL;
    core_audio_midiin->buffer_size[1] = 0;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
    core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_BUFFER3);
    core_audio_midiin->flags |= AGS_CORE_AUDIO_MIDIIN_BUFFER0;

    /* clear buffer */
    if(core_audio_midiin->buffer[2] != NULL){
      free(core_audio_midiin->buffer[2]);
    }

    core_audio_midiin->buffer[2] = NULL;
    core_audio_midiin->buffer_size[2] = 0;
  }
}

void
ags_core_audio_midiin_set_application_context(AgsSequencer *sequencer,
					      AgsApplicationContext *application_context)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  core_audio_midiin->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_core_audio_midiin_get_application_context(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  return((AgsApplicationContext *) core_audio_midiin->application_context);
}

void
ags_core_audio_midiin_set_application_mutex(AgsSequencer *sequencer,
					    pthread_mutex_t *application_mutex)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  core_audio_midiin->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_core_audio_midiin_get_application_mutex(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  return(core_audio_midiin->application_mutex);
}

void
ags_core_audio_midiin_set_device(AgsSequencer *sequencer,
				 gchar *device)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GList *core_audio_port, *core_audio_port_start;

  gchar *str;
  
  int ret;
  guint nth_card;
  guint i;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  if(core_audio_midiin->card_uri == device ||
     !g_ascii_strcasecmp(core_audio_midiin->card_uri,
			 device)){
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-midiin-")){
    g_warning("invalid core audio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-midiin-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid core audio device specifier");

    return;
  }

  if(core_audio_midiin->card_uri != NULL){
    g_free(core_audio_midiin->card_uri);
  }
  
  core_audio_midiin->card_uri = g_strdup(device);

  /* apply name to port */
  core_audio_port_start = 
    core_audio_port = g_list_copy(core_audio_midiin->core_audio_port);
  
  str = g_strdup_printf("ags-sequencer%d",
			nth_card);
    
  g_object_set(core_audio_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(core_audio_port_start);
}

gchar*
ags_core_audio_midiin_get_device(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  return(core_audio_midiin->card_uri);
}

void
ags_core_audio_midiin_list_cards(AgsSequencer *sequencer,
				 GList **card_id, GList **card_name)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = (AgsApplicationContext *) core_audio_midiin->application_context;

  if(application_context == NULL){
    return;
  }
  
  application_mutex = core_audio_midiin->application_mutex;
  
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
    if(AGS_IS_CORE_AUDIO_MIDIIN(list->data)){
      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id,
				  g_strdup(AGS_CORE_AUDIO_MIDIIN(list->data)->card_uri));
      }

      if(card_name != NULL){
	if(AGS_CORE_AUDIO_MIDIIN(list->data)->core_audio_client != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(AGS_CORE_AUDIO_CLIENT(AGS_CORE_AUDIO_MIDIIN(list->data)->core_audio_client)->name));
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_core_audio_midiin_list_cards() - core audio client not connected (null)");
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
ags_core_audio_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  return(((AGS_CORE_AUDIO_MIDIIN_START_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE);
}

gboolean
ags_core_audio_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  return(((AGS_CORE_AUDIO_MIDIIN_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE);
}

void
ags_core_audio_midiin_port_init(AgsSequencer *sequencer,
				GError **error)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

  /* prepare for record */
  core_audio_midiin->flags |= (AGS_CORE_AUDIO_MIDIIN_BUFFER3 |
			       AGS_CORE_AUDIO_MIDIIN_START_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_NONBLOCKING);

  core_audio_midiin->note_offset = 0;

  /* port setup */
  //TODO:JK: implement me
  
  /*  */
  core_audio_midiin->tact_counter = 0.0;
  core_audio_midiin->delay_counter = 0.0;
  core_audio_midiin->tic_counter = 0;

  core_audio_midiin->flags |= (AGS_CORE_AUDIO_MIDIIN_INITIALIZED |
			       AGS_CORE_AUDIO_MIDIIN_START_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_RECORD);
  
  g_atomic_int_and(&(core_audio_midiin->sync_flags),
		   (~(AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH)));
  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK);
  
  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_midiin_port_record(AgsSequencer *sequencer,
				  GError **error)
{
  AgsJackClient *core_audio_client;
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  gboolean core_audio_client_activated;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  pthread_mutex_t *client_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /*  */
  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  task_thread = (AgsTaskThread *) application_context->task_thread;

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* client */
  pthread_mutex_lock(mutex);

  core_audio_client = (AgsJackClient *) core_audio_midiin->core_audio_client;
  
  callback_mutex = core_audio_midiin->callback_mutex;
  callback_finish_mutex = core_audio_midiin->callback_finish_mutex;

  pthread_mutex_unlock(mutex);

  /* do record */
  pthread_mutex_lock(mutex);
  
  core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_START_RECORD);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  pthread_mutex_unlock(mutex);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  client_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) core_audio_client);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(client_mutex);

  if(core_audio_client_activated){
    /* signal client */
    if((AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(core_audio_midiin->sync_flags),
		      AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE);
    
      if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	pthread_cond_signal(core_audio_midiin->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_midiin->sync_flags),
			AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	  pthread_cond_wait(core_audio_midiin->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~(AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT |
			  AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK));
    }
  }

  if(task_thread != NULL){
    AgsTicDevice *tic_device;
    AgsSwitchBufferFlag *switch_buffer_flag;
      
    GList *task;
      
    task = NULL;
  
    /* tic sequencer */
    tic_device = ags_tic_device_new((GObject *) core_audio_midiin);
    task = g_list_append(task,
			 tic_device);
  
    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) core_audio_midiin);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  }else{
    /* tic */
    ags_sequencer_tic(AGS_SEQUENCER(core_audio_midiin));
	  
    /* reset - switch buffer flags */
    ags_core_audio_midiin_switch_buffer_flag(core_audio_midiin);
  }  
}

void
ags_core_audio_midiin_port_free(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = ags_sequencer_get_application_context(sequencer);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_midiin);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(mutex);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  callback_mutex = core_audio_midiin->callback_mutex;
  callback_finish_mutex = core_audio_midiin->callback_finish_mutex;

  core_audio_midiin->flags &= (~(AGS_CORE_AUDIO_MIDIIN_BUFFER0 |
				 AGS_CORE_AUDIO_MIDIIN_BUFFER1 |
				 AGS_CORE_AUDIO_MIDIIN_BUFFER2 |
				 AGS_CORE_AUDIO_MIDIIN_BUFFER3 |
				 AGS_CORE_AUDIO_MIDIIN_RECORD));

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH);
  g_atomic_int_and(&(core_audio_midiin->sync_flags),
		   (~AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK));

  pthread_mutex_unlock(mutex);

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_midiin->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_midiin->callback_finish_cond);
  }

  pthread_mutex_unlock(callback_finish_mutex);

  /*  */
  pthread_mutex_lock(mutex);

  if(core_audio_midiin->buffer[1] != NULL){
    free(core_audio_midiin->buffer[1]);
    core_audio_midiin->buffer_size[1] = 0;
  }

  if(core_audio_midiin->buffer[2] != NULL){
    free(core_audio_midiin->buffer[2]);
    core_audio_midiin->buffer_size[2] = 0;
  }

  if(core_audio_midiin->buffer[3] != NULL){
    free(core_audio_midiin->buffer[3]);
    core_audio_midiin->buffer_size[3] = 0;
  }

  if(core_audio_midiin->buffer[0] != NULL){
    free(core_audio_midiin->buffer[0]);
    core_audio_midiin->buffer_size[0] = 0;
  }

  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_midiin_tic(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  gdouble delay;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* determine if attack should be switched */
  delay = core_audio_midiin->delay;

  if((guint) core_audio_midiin->delay_counter + 1 >= (guint) delay){
    ags_sequencer_set_note_offset(sequencer,
				  core_audio_midiin->note_offset_absolute + 1);
    
    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 core_audio_midiin->note_offset);
    
    /* reset - delay counter */
    core_audio_midiin->delay_counter = 0.0;
    core_audio_midiin->tact_counter += 1.0;
  }else{
    core_audio_midiin->delay_counter += 1.0;
  }
}

void
ags_core_audio_midiin_offset_changed(AgsSequencer *sequencer,
				     guint note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  core_audio_midiin->tic_counter += 1;

  if(core_audio_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_midiin->tic_counter = 0;
  }
}

void
ags_core_audio_midiin_set_bpm(AgsSequencer *sequencer,
			      gdouble bpm)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  core_audio_midiin->bpm = bpm;
}

gdouble
ags_core_audio_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  return(core_audio_midiin->bpm);
}

void
ags_core_audio_midiin_set_delay_factor(AgsSequencer *sequencer, gdouble delay_factor)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  core_audio_midiin->delay_factor = delay_factor;
}

gdouble
ags_core_audio_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  return(core_audio_midiin->delay_factor);
}

void*
ags_core_audio_midiin_get_buffer(AgsSequencer *sequencer,
				 guint *buffer_length)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  char *buffer;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get buffer */
  if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[0];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[1];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[2];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[3];
  }else{
    buffer = NULL;
  }
  
  /* return the buffer's length */
  if(buffer_length != NULL){
    if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[0];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[1];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[2];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[3];
    }else{
      *buffer_length = 0;
    }
  }

  return(buffer);
}

void*
ags_core_audio_midiin_get_next_buffer(AgsSequencer *sequencer,
				      guint *buffer_length)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  unsigned char *buffer;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get buffer */
  if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[1];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[2];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[3];
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
    buffer = core_audio_midiin->buffer[0];
  }else{
    buffer = NULL;
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[1];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[2];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[3];
    }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
      *buffer_length = core_audio_midiin->buffer_size[0];
    }else{
      *buffer_length = 0;
    }
  }  

  return(buffer);
}

void
ags_core_audio_midiin_set_note_offset(AgsSequencer *sequencer,
				      guint note_offset)
{
  AGS_CORE_AUDIO_MIDIIN(sequencer)->note_offset = note_offset;
}

guint
ags_core_audio_midiin_get_note_offset(AgsSequencer *sequencer)
{
  return(AGS_CORE_AUDIO_MIDIIN(sequencer)->note_offset);
}

void
ags_core_audio_midiin_set_audio(AgsSequencer *sequencer,
				GList *audio)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  core_audio_midiin->audio = audio;
}

GList*
ags_core_audio_midiin_get_audio(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  return(core_audio_midiin->audio);
}

/**
 * ags_core_audio_midiin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsCoreAudioMidiin, refering to @application_context.
 *
 * Returns: a new #AgsCoreAudioMidiin
 *
 * Since: 0.9.24
 */
AgsCoreAudioMidiin*
ags_core_audio_midiin_new(GObject *application_context)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = (AgsCoreAudioMidiin *) g_object_new(AGS_TYPE_CORE_AUDIO_MIDIIN,
							  "application-context", application_context,
							  NULL);
  
  return(core_audio_midiin);
}
