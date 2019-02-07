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

#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/libags.h>

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
void ags_core_audio_midiin_dispose(GObject *gobject);
void ags_core_audio_midiin_finalize(GObject *gobject);

AgsUUID* ags_core_audio_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_midiin_is_ready(AgsConnectable *connectable);
void ags_core_audio_midiin_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_midiin_xml_compose(AgsConnectable *connectable);
void ags_core_audio_midiin_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_core_audio_midiin_is_connected(AgsConnectable *connectable);
void ags_core_audio_midiin_connect(AgsConnectable *connectable);
void ags_core_audio_midiin_disconnect(AgsConnectable *connectable);

void ags_core_audio_midiin_set_application_context(AgsSequencer *sequencer,
						   AgsApplicationContext *application_context);
AgsApplicationContext* ags_core_audio_midiin_get_application_context(AgsSequencer *sequencer);

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

void ags_core_audio_midiin_set_start_note_offset(AgsSequencer *sequencer,
						 guint start_note_offset);
guint ags_core_audio_midiin_get_start_note_offset(AgsSequencer *sequencer);

void ags_core_audio_midiin_set_note_offset(AgsSequencer *sequencer,
					   guint note_offset);
guint ags_core_audio_midiin_get_note_offset(AgsSequencer *sequencer);

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
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
};

static gpointer ags_core_audio_midiin_parent_class = NULL;

static pthread_mutex_t ags_core_audio_midiin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_core_audio_midiin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_midiin = 0;

    static const GTypeInfo ags_core_audio_midiin_info = {
      sizeof(AgsCoreAudioMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioMidiin),
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_core_audio_midiin);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
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
   * AgsCoreAudioMidiin:device:
   *
   * The core audio sequencer indentifier
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
  connectable->get_uuid = ags_core_audio_midiin_get_uuid;
  connectable->has_resource = ags_core_audio_midiin_has_resource;

  connectable->is_ready = ags_core_audio_midiin_is_ready;
  connectable->add_to_registry = ags_core_audio_midiin_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_midiin_remove_from_registry;

  connectable->list_resource = ags_core_audio_midiin_list_resource;
  connectable->xml_compose = ags_core_audio_midiin_xml_compose;
  connectable->xml_parse = ags_core_audio_midiin_xml_parse;

  connectable->is_connected = ags_core_audio_midiin_is_connected;  
  connectable->connect = ags_core_audio_midiin_connect;
  connectable->disconnect = ags_core_audio_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_application_context = ags_core_audio_midiin_set_application_context;
  sequencer->get_application_context = ags_core_audio_midiin_get_application_context;

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

  sequencer->set_start_note_offset = ags_core_audio_midiin_set_start_note_offset;
  sequencer->get_start_note_offset = ags_core_audio_midiin_get_start_note_offset;

  sequencer->set_note_offset = ags_core_audio_midiin_set_note_offset;
  sequencer->get_note_offset = ags_core_audio_midiin_get_note_offset;
}

void
ags_core_audio_midiin_init(AgsCoreAudioMidiin *core_audio_midiin)
{
  AgsConfig *config;

  gchar *segmentation;

  guint denumerator, numerator;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  core_audio_midiin->flags = 0;
  g_atomic_int_set(&(core_audio_midiin->sync_flags),
		   AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH);

  /* core-audio midiin mutex */
  core_audio_midiin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  core_audio_midiin->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  core_audio_midiin->application_context = NULL;

  /* uuid */
  core_audio_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_midiin->uuid);

  /* card and port */
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
    
  /* segmentation */
  config = ags_config_get_instance();

  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    core_audio_midiin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* counters */
  core_audio_midiin->start_note_offset = 0;
  core_audio_midiin->note_offset = 0;
  core_audio_midiin->note_offset_absolute = 0;
  
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
}

void
ags_core_audio_midiin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  /* get core-audio client mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_midiin_mutex);

      if(core_audio_midiin->application_context == application_context){
	pthread_mutex_unlock(core_audio_midiin_mutex);

	return;
      }

      if(core_audio_midiin->application_context != NULL){
	g_object_unref(G_OBJECT(core_audio_midiin->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      core_audio_midiin->application_context = application_context;

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->card_uri = g_strdup(device);

      pthread_mutex_unlock(core_audio_midiin_mutex);
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

      pthread_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->bpm = bpm;

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->delay_factor = delay_factor;

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = g_value_get_object(value);

      pthread_mutex_lock(core_audio_midiin_mutex);

      if(core_audio_midiin->core_audio_client == (GObject *) core_audio_client){
	pthread_mutex_unlock(core_audio_midiin_mutex);
	
	return;
      }

      if(core_audio_midiin->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_midiin->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(G_OBJECT(core_audio_client));
      }

      core_audio_midiin->core_audio_client = (GObject *) core_audio_client;

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_midiin_mutex);

      if(g_list_find(core_audio_midiin->core_audio_port, core_audio_port) != NULL){
	pthread_mutex_unlock(core_audio_midiin_mutex);

	return;
      }

      if(core_audio_port != NULL){
	g_object_ref(core_audio_port);
	core_audio_midiin->core_audio_port = g_list_append(core_audio_midiin->core_audio_port,
							   core_audio_port);
      }

      pthread_mutex_unlock(core_audio_midiin_mutex);
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

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);  

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_object(value, core_audio_midiin->application_context);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_string(value, core_audio_midiin->card_uri);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_pointer(value, core_audio_midiin->buffer);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_double(value, core_audio_midiin->bpm);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_double(value, core_audio_midiin->delay_factor);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_object(value, core_audio_midiin->core_audio_client);

      pthread_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      pthread_mutex_lock(core_audio_midiin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_midiin->core_audio_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      pthread_mutex_unlock(core_audio_midiin_mutex);
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

  /* core_audio client */
  if(core_audio_midiin->core_audio_client != NULL){
    g_object_unref(core_audio_midiin->core_audio_client);

    core_audio_midiin->core_audio_client = NULL;
  }

  /* core_audio port */
  g_list_free_full(core_audio_midiin->core_audio_port,
		   g_object_unref);

  core_audio_midiin->core_audio_port = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_midiin_parent_class)->dispose(gobject);
}

void
ags_core_audio_midiin_finalize(GObject *gobject)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  pthread_mutex_destroy(core_audio_midiin->obj_mutex);
  free(core_audio_midiin->obj_mutex);

  pthread_mutexattr_destroy(core_audio_midiin->obj_mutexattr);
  free(core_audio_midiin->obj_mutexattr);

  ags_uuid_free(core_audio_midiin->uuid);
  
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

  /* core_audio client */
  if(core_audio_midiin->core_audio_client != NULL){
    g_object_unref(core_audio_midiin->core_audio_client);
  }

  /* core_audio port */
  g_list_free_full(core_audio_midiin->core_audio_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  AgsUUID *ptr;

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio_midiin signal mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(core_audio_midiin_mutex);

  ptr = core_audio_midiin->uuid;

  pthread_mutex_unlock(core_audio_midiin_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_midiin_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  gboolean is_ready;

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(core_audio_midiin_mutex);

  is_ready = (((AGS_CORE_AUDIO_MIDIIN_ADDED_TO_REGISTRY & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_ready);
}

void
ags_core_audio_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  ags_core_audio_midiin_set_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_ADDED_TO_REGISTRY);
}

void
ags_core_audio_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  ags_core_audio_midiin_unset_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_core_audio_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_midiin_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_midiin_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  gboolean is_connected;

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(core_audio_midiin_mutex);

  is_connected = (((AGS_CORE_AUDIO_MIDIIN_CONNECTED & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_connected);
}

void
ags_core_audio_midiin_connect(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  ags_core_audio_midiin_set_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_CONNECTED);
}

void
ags_core_audio_midiin_disconnect(AgsConnectable *connectable)
{

  AgsCoreAudioMidiin *core_audio_midiin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);
  
  ags_core_audio_midiin_unset_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_CONNECTED);
}

/**
 * ags_core_audio_midiin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_core_audio_midiin_get_class_mutex()
{
  return(&ags_core_audio_midiin_class_mutex);
}

/**
 * ags_core_audio_midiin_test_flags:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_core_audio_midiin_test_flags(AgsCoreAudioMidiin *core_audio_midiin, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return(FALSE);
  }

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* test */
  pthread_mutex_lock(core_audio_midiin_mutex);

  retval = (flags & (core_audio_midiin->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(retval);
}

/**
 * ags_core_audio_midiin_set_flags:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 * @flags: see #AgsCoreAudioMidiinFlags-enum
 *
 * Enable a feature of @core_audio_midiin.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_midiin_set_flags(AgsCoreAudioMidiin *core_audio_midiin, guint flags)
{
  pthread_mutex_t *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return;
  }

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->flags |= flags;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
}
    
/**
 * ags_core_audio_midiin_unset_flags:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 * @flags: see #AgsCoreAudioMidiinFlags-enum
 *
 * Disable a feature of @core_audio_midiin.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_midiin_unset_flags(AgsCoreAudioMidiin *core_audio_midiin, guint flags)
{  
  pthread_mutex_t *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return;
  }

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->flags &= (~flags);
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_set_application_context(AgsSequencer *sequencer,
					      AgsApplicationContext *application_context)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set application context */
  pthread_mutex_lock(core_audio_midiin_mutex);
  
  core_audio_midiin->application_context = (GObject *) application_context;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
}

AgsApplicationContext*
ags_core_audio_midiin_get_application_context(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* get application context */
  pthread_mutex_lock(core_audio_midiin_mutex);

  application_context = (AgsApplicationContext *) core_audio_midiin->application_context;

  pthread_mutex_unlock(core_audio_midiin_mutex);
  
  return(application_context);
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

  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  pthread_mutex_lock(core_audio_midiin_mutex);

  if(core_audio_midiin->card_uri == device ||
     !g_ascii_strcasecmp(core_audio_midiin->card_uri,
			 device)){
    pthread_mutex_unlock(core_audio_midiin_mutex);
    
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-core_audio-midiin-")){
    g_warning("invalid CORE_AUDIO device prefix");

    pthread_mutex_unlock(core_audio_midiin_mutex);

    return;
  }

  ret = sscanf(device,
	       "ags-core_audio-midiin-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid CORE_AUDIO device specifier");

    pthread_mutex_unlock(core_audio_midiin_mutex);

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
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
}

gchar*
ags_core_audio_midiin_get_device(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gchar *device;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  device = NULL;
  
  pthread_mutex_lock(core_audio_midiin_mutex);

  device = g_strdup(core_audio_midiin->card_uri);

  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(device);
}

void
ags_core_audio_midiin_list_cards(AgsSequencer *sequencer,
				 GList **card_id, GList **card_name)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  gchar *device, *client_name;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = (AgsApplicationContext *) core_audio_midiin->application_context;

  if(application_context == NULL){
    return;
  }
    
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  list_start = 
    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_CORE_AUDIO_MIDIIN(list->data)){
      g_object_get(list->data,
		   "device", &device,
		   "core-audio-client", &core_audio_client,
		   NULL);
      
      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id,
				  device);
      }

      if(card_name != NULL){	
	if(core_audio_client != NULL){
	  g_object_get(core_audio_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_core_audio_midiin_list_cards() - CORE_AUDIO client not connected (null)");
	}
      }

      g_object_unref(core_audio_client);
    }

    list = list->next;
  }
  
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

  gboolean is_starting;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(core_audio_midiin_mutex);

  is_starting = ((AGS_CORE_AUDIO_MIDIIN_START_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_core_audio_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gboolean is_recording;
  
  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(core_audio_midiin_mutex);

  is_recording = ((AGS_CORE_AUDIO_MIDIIN_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(is_recording);
}

void
ags_core_audio_midiin_port_init(AgsSequencer *sequencer,
				GError **error)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_midiin_mutex);

  /* prepare for record */
  core_audio_midiin->flags |= (AGS_CORE_AUDIO_MIDIIN_BUFFER3 |
			       AGS_CORE_AUDIO_MIDIIN_START_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_NONBLOCKING);

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
  
  pthread_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_port_record(AgsSequencer *sequencer,
				  GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioMidiin *core_audio_midiin;
  
  AgsTicDevice *tic_device;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  GList *task;

  gboolean core_audio_client_activated;
  gboolean do_sync;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  pthread_mutex_t *core_audio_client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* client */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_midiin->core_audio_client;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);

  /* do record */
  pthread_mutex_lock(core_audio_midiin_mutex);
  
  core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_START_RECORD);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    pthread_mutex_unlock(core_audio_midiin_mutex);
    
    return;
  }

  callback_mutex = core_audio_midiin->callback_mutex;
  callback_finish_mutex = core_audio_midiin->callback_finish_mutex;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* check activated */
  pthread_mutex_lock(core_audio_client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_client_mutex);

  do_sync = FALSE;
  
  if(core_audio_client_activated){
    if((AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
      if((AGS_CORE_AUDIO_MIDIIN_DO_SYNC & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	g_atomic_int_and(&(core_audio_midiin->sync_flags),
			 (~AGS_CORE_AUDIO_MIDIIN_DO_SYNC));
	do_sync = TRUE;
      
	/* signal client - wait callback */
	pthread_mutex_lock(callback_mutex);

	g_atomic_int_and(&(core_audio_midiin->sync_flags),
			 (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT));

	if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
	  pthread_cond_signal(core_audio_midiin->callback_cond);
	}

	pthread_mutex_unlock(callback_mutex);
      }  
    }else{
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_DO_SYNC));

      g_atomic_int_or(&(core_audio_midiin->sync_flags),
		      (AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT | AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE));
      g_atomic_int_or(&(core_audio_midiin->sync_flags),
		      (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT | AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE));
      
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK));
    }
  }

  /* implied wait - callback finish wait */
  if(do_sync){
    pthread_mutex_lock(callback_finish_mutex);

    if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE));

      while((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0 &&
	    (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	pthread_cond_wait(core_audio_midiin->callback_finish_cond,
			  core_audio_midiin->callback_finish_mutex);
      }
    }

    pthread_mutex_unlock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_midiin->sync_flags),
		    (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT |  AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE));
  }

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

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
}

void
ags_core_audio_midiin_port_free(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* check initialized */
  pthread_mutex_lock(core_audio_midiin_mutex);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    pthread_mutex_unlock(core_audio_midiin_mutex);
    
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

  pthread_mutex_unlock(core_audio_midiin_mutex);

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
  pthread_mutex_lock(core_audio_midiin_mutex);

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

  core_audio_midiin->note_offset = core_audio_midiin->start_note_offset;
  core_audio_midiin->note_offset_absolute = core_audio_midiin->start_note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_tic(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  pthread_mutex_t *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());
  
  /* determine if attack should be switched */
  pthread_mutex_lock(core_audio_midiin_mutex);

  delay = core_audio_midiin->delay;
  delay_counter = core_audio_midiin->delay_counter;

  note_offset = core_audio_midiin->note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);

  if((guint) delay_counter + 1 >= (guint) delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);
    
    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    pthread_mutex_lock(core_audio_midiin_mutex);

    core_audio_midiin->delay_counter = 0.0;
    core_audio_midiin->tact_counter += 1.0;

    pthread_mutex_unlock(core_audio_midiin_mutex);
  }else{
    pthread_mutex_lock(core_audio_midiin_mutex);

    core_audio_midiin->delay_counter += 1.0;

    pthread_mutex_unlock(core_audio_midiin_mutex);
  }
}

void
ags_core_audio_midiin_offset_changed(AgsSequencer *sequencer,
				     guint note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* offset changed */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->tic_counter += 1;

  if(core_audio_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_midiin->tic_counter = 0;
  }

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_set_bpm(AgsSequencer *sequencer,
			      gdouble bpm)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set bpm */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->bpm = bpm;

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

gdouble
ags_core_audio_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble bpm;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* get bpm */
  pthread_mutex_lock(core_audio_midiin_mutex);

  bpm = core_audio_midiin->bpm;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(bpm);
}

void
ags_core_audio_midiin_set_delay_factor(AgsSequencer *sequencer,
				       gdouble delay_factor)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set delay factor */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->delay_factor = delay_factor;

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

gdouble
ags_core_audio_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble delay_factor;
  
  pthread_mutex_t *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* get delay factor */
  pthread_mutex_lock(core_audio_midiin_mutex);

  delay_factor = core_audio_midiin->delay_factor;
  
  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(delay_factor);
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
  char *buffer;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get buffer */
  if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER0)){
    buffer = core_audio_midiin->buffer[1];
  }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER1)){
    buffer = core_audio_midiin->buffer[2];
  }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER2)){
    buffer = core_audio_midiin->buffer[3];
  }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER3)){
    buffer = core_audio_midiin->buffer[0];
  }else{
    buffer = NULL;
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER0)){
      *buffer_length = core_audio_midiin->buffer_size[1];
    }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER1)){
      *buffer_length = core_audio_midiin->buffer_size[2];
    }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER2)){
      *buffer_length = core_audio_midiin->buffer_size[3];
    }else if(ags_core_audio_midiin_test_flags(core_audio_midiin, AGS_CORE_AUDIO_MIDIIN_BUFFER3)){
      *buffer_length = core_audio_midiin->buffer_size[0];
    }else{
      *buffer_length = 0;
    }
  }
  
  return(buffer);
}

void
ags_core_audio_midiin_set_start_note_offset(AgsSequencer *sequencer,
					    guint start_note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->start_note_offset = start_note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

guint
ags_core_audio_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  guint start_note_offset;
  
  pthread_mutex_t *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_midiin_mutex);

  start_note_offset = core_audio_midiin->start_note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(start_note_offset);
}

void
ags_core_audio_midiin_set_note_offset(AgsSequencer *sequencer,
				      guint note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  pthread_mutex_t *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->note_offset = note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

guint
ags_core_audio_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  guint note_offset;
  
  pthread_mutex_t *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_midiin_mutex);

  note_offset = core_audio_midiin->note_offset;

  pthread_mutex_unlock(core_audio_midiin_mutex);

  return(note_offset);
}

/**
 * ags_core_audio_midiin_switch_buffer_flag:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_midiin_switch_buffer_flag(AgsCoreAudioMidiin *core_audio_midiin)
{
  pthread_mutex_t *core_audio_midiin_mutex;  

  /* get core_audio midiin mutex */
  pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());
  
  core_audio_midiin_mutex = core_audio_midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());

  /* switch buffer flag */
  pthread_mutex_lock(core_audio_midiin_mutex);

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

  pthread_mutex_unlock(core_audio_midiin_mutex);
}

/**
 * ags_core_audio_midiin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates a new instance of #AgsCoreAudioMidiin.
 *
 * Returns: a new #AgsCoreAudioMidiin
 *
 * Since: 2.0.0
 */
AgsCoreAudioMidiin*
ags_core_audio_midiin_new(AgsApplicationContext *application_context)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = (AgsCoreAudioMidiin *) g_object_new(AGS_TYPE_CORE_AUDIO_MIDIIN,
							  "application-context", application_context,
							  NULL);
  
  return(core_audio_midiin);
}
