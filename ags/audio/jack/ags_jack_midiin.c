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

#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <ags/config.h>
#include <ags/i18n.h>

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
void ags_jack_midiin_dispose(GObject *gobject);
void ags_jack_midiin_finalize(GObject *gobject);

AgsUUID* ags_jack_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_jack_midiin_is_ready(AgsConnectable *connectable);
void ags_jack_midiin_add_to_registry(AgsConnectable *connectable);
void ags_jack_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_midiin_xml_compose(AgsConnectable *connectable);
void ags_jack_midiin_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_midiin_is_connected(AgsConnectable *connectable);
void ags_jack_midiin_connect(AgsConnectable *connectable);
void ags_jack_midiin_disconnect(AgsConnectable *connectable);

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

void ags_jack_midiin_lock_buffer(AgsSequencer *sequencer,
				 void *buffer);
void ags_jack_midiin_unlock_buffer(AgsSequencer *sequencer,
				   void *buffer);

void ags_jack_midiin_set_start_note_offset(AgsSequencer *sequencer,
					   guint start_note_offset);
guint ags_jack_midiin_get_start_note_offset(AgsSequencer *sequencer);

void ags_jack_midiin_set_note_offset(AgsSequencer *sequencer,
				     guint note_offset);
guint ags_jack_midiin_get_note_offset(AgsSequencer *sequencer);

/**
 * SECTION:ags_jack_midiin
 * @short_description: Input from sequencer
 * @title: AgsJackMidiin
 * @section_id:
 * @include: ags/audio/jack/ags_jack_midiin.h
 *
 * #AgsJackMidiin represents a sequencer and supports midi input.
 */

enum{
  PROP_0,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_JACK_CLIENT,
  PROP_JACK_PORT,
};

static gpointer ags_jack_midiin_parent_class = NULL;

GType
ags_jack_midiin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_midiin = 0;

    static const GTypeInfo ags_jack_midiin_info = {
      sizeof(AgsJackMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackMidiin),
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
						  "AgsJackMidiin",
						  &ags_jack_midiin_info,
						  0);

    g_type_add_interface_static(ags_type_jack_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_midiin);
  }

  return g_define_type_id__volatile;
}

GType
ags_jack_midiin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_JACK_MIDIIN_INITIALIZED, "AGS_JACK_MIDIIN_INITIALIZED", "jack-midiin-initialized" },
      { AGS_JACK_MIDIIN_START_RECORD, "AGS_JACK_MIDIIN_START_RECORD", "jack-midiin-start-record" },
      { AGS_JACK_MIDIIN_RECORD, "AGS_JACK_MIDIIN_RECORD", "jack-midiin-record" },
      { AGS_JACK_MIDIIN_SHUTDOWN, "AGS_JACK_MIDIIN_SHUTDOWN", "jack-midiin-shutdown" },
      { AGS_JACK_MIDIIN_NONBLOCKING, "AGS_JACK_MIDIIN_NONBLOCKING", "jack-midiin-nonblocking" },
      { AGS_JACK_MIDIIN_ATTACK_FIRST, "AGS_JACK_MIDIIN_ATTACK_FIRST", "jack-midiin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsJackMidiinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * AgsJackMidiin:device:
   *
   * The JACK sequencer indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsJackMidiin:buffer:
   *
   * The buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsJackMidiin:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("beats per minute"),
				   i18n_pspec("Beats per minute to use"),
				   1.0,
				   240.0,
				   AGS_SEQUENCER_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsJackMidiin:delay-factor:
   *
   * tact
   * 
   * Since: 3.0.0
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
   * AgsJackMidiin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /**
   * AgsJackMidiin:jack-client:
   *
   * The assigned #AgsJackClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("jack-client",
				   i18n_pspec("jack client object"),
				   i18n_pspec("The jack client object"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackMidiin:jack-port:
   *
   * The assigned #AgsJackPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("jack-port",
				   i18n_pspec("jack port object"),
				   i18n_pspec("The jack port object"),
				   AGS_TYPE_JACK_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_PORT,
				  param_spec);

  /* AgsJackMidiinClass */
}

GQuark
ags_jack_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-jack_midiin-error-quark"));
}

void
ags_jack_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_midiin_get_uuid;
  connectable->has_resource = ags_jack_midiin_has_resource;

  connectable->is_ready = ags_jack_midiin_is_ready;
  connectable->add_to_registry = ags_jack_midiin_add_to_registry;
  connectable->remove_from_registry = ags_jack_midiin_remove_from_registry;

  connectable->list_resource = ags_jack_midiin_list_resource;
  connectable->xml_compose = ags_jack_midiin_xml_compose;
  connectable->xml_parse = ags_jack_midiin_xml_parse;

  connectable->is_connected = ags_jack_midiin_is_connected;  
  connectable->connect = ags_jack_midiin_connect;
  connectable->disconnect = ags_jack_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
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

  sequencer->lock_buffer = ags_jack_midiin_lock_buffer;
  sequencer->unlock_buffer = ags_jack_midiin_unlock_buffer;

  sequencer->set_start_note_offset = ags_jack_midiin_set_start_note_offset;
  sequencer->get_start_note_offset = ags_jack_midiin_get_start_note_offset;

  sequencer->set_note_offset = ags_jack_midiin_set_note_offset;
  sequencer->get_note_offset = ags_jack_midiin_get_note_offset;
}

void
ags_jack_midiin_init(AgsJackMidiin *jack_midiin)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;

  jack_midiin->flags = 0;
  jack_midiin->connectable_flags = 0;
  g_atomic_int_set(&(jack_midiin->sync_flags),
		   AGS_JACK_MIDIIN_PASS_THROUGH);

  /* jack midiin mutex */
  g_rec_mutex_init(&(jack_midiin->obj_mutex));

  /* uuid */
  jack_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_midiin->uuid);

  jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_0;
  
  /* buffer */
  jack_midiin->app_buffer_mutex = (GRecMutex **) g_malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    jack_midiin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(jack_midiin->app_buffer_mutex[i]);
  }

  jack_midiin->app_buffer = (char **) g_malloc(4 * sizeof(char *));

  jack_midiin->app_buffer[0] = NULL;
  jack_midiin->app_buffer[1] = NULL;
  jack_midiin->app_buffer[2] = NULL;
  jack_midiin->app_buffer[3] = NULL;

  jack_midiin->app_buffer_size[0] = 0;
  jack_midiin->app_buffer_size[1] = 0;
  jack_midiin->app_buffer_size[2] = 0;
  jack_midiin->app_buffer_size[3] = 0;

  /* bpm */
  jack_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  jack_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  jack_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
    
  /* segmentation */
  config = ags_config_get_instance();

  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    jack_midiin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* counters */
  jack_midiin->start_note_offset = 0;
  jack_midiin->note_offset = 0;
  jack_midiin->note_offset_absolute = 0;

  jack_midiin->tact_counter = 0.0;
  jack_midiin->delay_counter = 0;
  jack_midiin->tic_counter = 0;

  /* card and port */
  jack_midiin->card_uri = NULL;
  jack_midiin->jack_client = NULL;

  jack_midiin->port_name = NULL;
  jack_midiin->jack_port = NULL;

  /* callback mutex */
  g_mutex_init(&(jack_midiin->callback_mutex));

  g_cond_init(&(jack_midiin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(jack_midiin->callback_finish_mutex));

  g_cond_init(&(jack_midiin->callback_finish_cond));
}

void
ags_jack_midiin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(gobject);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(jack_midiin_mutex);

      jack_midiin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(jack_midiin_mutex);
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

      g_rec_mutex_lock(jack_midiin_mutex);

      jack_midiin->bpm = bpm;

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(jack_midiin_mutex);

      jack_midiin->delay_factor = delay_factor;

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = g_value_get_object(value);

      g_rec_mutex_lock(jack_midiin_mutex);

      if(jack_midiin->jack_client == (GObject *) jack_client){
	g_rec_mutex_unlock(jack_midiin_mutex);
	
	return;
      }

      if(jack_midiin->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_midiin->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(G_OBJECT(jack_client));
      }

      jack_midiin->jack_client = (GObject *) jack_client;

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_object(value);

      g_rec_mutex_lock(jack_midiin_mutex);

      if(g_list_find(jack_midiin->jack_port, jack_port) != NULL){
	g_rec_mutex_unlock(jack_midiin_mutex);

	return;
      }

      if(jack_port != NULL){
	g_object_ref(jack_port);
	jack_midiin->jack_port = g_list_append(jack_midiin->jack_port,
					       jack_port);
      }

      g_rec_mutex_unlock(jack_midiin_mutex);
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

  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(gobject);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_string(value, jack_midiin->card_uri);

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_pointer(value, jack_midiin->app_buffer);

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_double(value, jack_midiin->bpm);

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_double(value, jack_midiin->delay_factor);

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_object(value, jack_midiin->jack_client);

      g_rec_mutex_unlock(jack_midiin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      g_rec_mutex_lock(jack_midiin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_midiin->jack_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_midiin_mutex);
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

  /* jack client */
  if(jack_midiin->jack_client != NULL){
    g_object_unref(jack_midiin->jack_client);

    jack_midiin->jack_client = NULL;
  }

  /* jack port */
  g_list_free_full(jack_midiin->jack_port,
		   g_object_unref);

  jack_midiin->jack_port = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_midiin_parent_class)->dispose(gobject);
}

void
ags_jack_midiin_finalize(GObject *gobject)
{
  AgsJackMidiin *jack_midiin;
  
  jack_midiin = AGS_JACK_MIDIIN(gobject);

  ags_uuid_free(jack_midiin->uuid);
  
  /* free output buffer */
  if(jack_midiin->app_buffer[0] != NULL){
    g_free(jack_midiin->app_buffer[0]);
  }

  if(jack_midiin->app_buffer[1] != NULL){
    g_free(jack_midiin->app_buffer[1]);
  }
    
  if(jack_midiin->app_buffer[2] != NULL){
    g_free(jack_midiin->app_buffer[2]);
  }
  
  if(jack_midiin->app_buffer[3] != NULL){
    g_free(jack_midiin->app_buffer[3]);
  }
  
  /* free buffer array */
  g_free(jack_midiin->app_buffer);

  /* jack client */
  if(jack_midiin->jack_client != NULL){
    g_object_unref(jack_midiin->jack_client);
  }

  /* jack port */
  g_list_free_full(jack_midiin->jack_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_jack_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;
  
  AgsUUID *ptr;

  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack_midiin signal mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* get UUID */
  g_rec_mutex_lock(jack_midiin_mutex);

  ptr = jack_midiin->uuid;

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(ptr);
}

gboolean
ags_jack_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_midiin_is_ready(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;
  
  gboolean is_ready;

  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* check is ready */
  g_rec_mutex_lock(jack_midiin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (jack_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(is_ready);
}

void
ags_jack_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

void
ags_jack_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

xmlNode*
ags_jack_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_midiin_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_midiin_is_connected(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;
  
  gboolean is_connected;

  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* check is connected */
  g_rec_mutex_lock(jack_midiin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (jack_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(is_connected);
}

void
ags_jack_midiin_connect(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

void
ags_jack_midiin_disconnect(AgsConnectable *connectable)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_midiin = AGS_JACK_MIDIIN(connectable);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

/**
 * ags_jack_midiin_test_flags:
 * @jack_midiin: the #AgsJackMidiin
 * @flags: the flags
 *
 * Test @flags to be set on @jack_midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_midiin_test_flags(AgsJackMidiin *jack_midiin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *jack_midiin_mutex;

  if(!AGS_IS_JACK_MIDIIN(jack_midiin)){
    return(FALSE);
  }

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* test */
  g_rec_mutex_lock(jack_midiin_mutex);

  retval = (flags & (jack_midiin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_midiin_mutex);

  return(retval);
}

/**
 * ags_jack_midiin_set_flags:
 * @jack_midiin: the #AgsJackMidiin
 * @flags: see #AgsJackMidiinFlags-enum
 *
 * Enable a feature of @jack_midiin.
 *
 * Since: 3.0.0
 */
void
ags_jack_midiin_set_flags(AgsJackMidiin *jack_midiin, guint flags)
{
  GRecMutex *jack_midiin_mutex;

  if(!AGS_IS_JACK_MIDIIN(jack_midiin)){
    return;
  }

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->flags |= flags;
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}
    
/**
 * ags_jack_midiin_unset_flags:
 * @jack_midiin: the #AgsJackMidiin
 * @flags: see #AgsJackMidiinFlags-enum
 *
 * Disable a feature of @jack_midiin.
 *
 * Since: 3.0.0
 */
void
ags_jack_midiin_unset_flags(AgsJackMidiin *jack_midiin, guint flags)
{  
  GRecMutex *jack_midiin_mutex;

  if(!AGS_IS_JACK_MIDIIN(jack_midiin)){
    return;
  }

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_midiin_mutex);
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

  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);

  if(jack_midiin->card_uri == device ||
     (jack_midiin->card_uri != NULL &&
      !g_ascii_strcasecmp(jack_midiin->card_uri,
			  device))){
    g_rec_mutex_unlock(jack_midiin_mutex);
    
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-midiin-")){
    g_warning("invalid JACK device prefix");

    g_rec_mutex_unlock(jack_midiin_mutex);

    return;
  }

  ret = sscanf(device,
	       "ags-jack-midiin-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid JACK device specifier");

    g_rec_mutex_unlock(jack_midiin_mutex);

    return;
  }

  if(jack_midiin->card_uri != NULL){
    g_free(jack_midiin->card_uri);
  }
  
  jack_midiin->card_uri = g_strdup(device);

  /* apply name to port */
  jack_port_start = 
    jack_port = g_list_copy(jack_midiin->jack_port);
  
  str = g_strdup_printf("ags-sequencer%d",
			nth_card);
    
  g_object_set(jack_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(jack_port_start);
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

gchar*
ags_jack_midiin_get_device(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  gchar *device;
  
  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  device = NULL;
  
  g_rec_mutex_lock(jack_midiin_mutex);

  device = g_strdup(jack_midiin->card_uri);

  g_rec_mutex_unlock(jack_midiin_mutex);

  return(device);
}

void
ags_jack_midiin_list_cards(AgsSequencer *sequencer,
			   GList **card_id, GList **card_name)
{
  AgsJackClient *jack_client;
  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  gchar *device, *client_name;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();
    
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  list_start = 
    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_MIDIIN(list->data)){
      g_object_get(list->data,
		   "device", &device,
		   "jack-client", &jack_client,
		   NULL);
      
      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id,
				  device);
      }

      if(card_name != NULL){	
	if(jack_client != NULL){
	  g_object_get(jack_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_jack_midiin_list_cards() - JACK client not connected (null)");
	}
      }      

      if(jack_client != NULL){
	g_object_unref(jack_client);
      }
    }

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
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

  gboolean is_starting;
  
  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* check is starting */
  g_rec_mutex_lock(jack_midiin_mutex);

  is_starting = ((AGS_JACK_MIDIIN_START_RECORD & (jack_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_jack_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  gboolean is_recording;
  
  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* check is starting */
  g_rec_mutex_lock(jack_midiin_mutex);

  is_recording = ((AGS_JACK_MIDIIN_RECORD & (jack_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_midiin_mutex);

  return(is_recording);
}

void
ags_jack_midiin_port_init(AgsSequencer *sequencer,
			  GError **error)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;

  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /*  */
  g_rec_mutex_lock(jack_midiin_mutex);

  /* prepare for record */
  jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_3;
  jack_midiin->flags |= (AGS_JACK_MIDIIN_START_RECORD |
			 AGS_JACK_MIDIIN_RECORD |
			 AGS_JACK_MIDIIN_NONBLOCKING);

  /* port setup */
  //TODO:JK: implement me
  
  /*  */
  jack_midiin->tact_counter = 0.0;
  jack_midiin->delay_counter = floor(jack_midiin->delay);
  jack_midiin->tic_counter = 0;

  jack_midiin->flags |= (AGS_JACK_MIDIIN_INITIALIZED |
			 AGS_JACK_MIDIIN_START_RECORD |
			 AGS_JACK_MIDIIN_RECORD);
  
  g_atomic_int_and(&(jack_midiin->sync_flags),
		   (~(AGS_JACK_MIDIIN_PASS_THROUGH)));
  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_INITIAL_CALLBACK);
  
  g_rec_mutex_unlock(jack_midiin_mutex);
}

void
ags_jack_midiin_port_record(AgsSequencer *sequencer,
			    GError **error)
{
  AgsJackClient *jack_client;
  AgsJackMidiin *jack_midiin;

  AgsTicDevice *tic_device;
  AgsSwitchBufferFlag *switch_buffer_flag;

  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  gboolean jack_client_activated;

  GRecMutex *jack_midiin_mutex;
  GRecMutex *jack_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();
    
  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* client */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_client = (AgsJackClient *) jack_midiin->jack_client;
  
  callback_mutex = &(jack_midiin->callback_mutex);
  callback_finish_mutex = &(jack_midiin->callback_finish_mutex);

  g_rec_mutex_unlock(jack_midiin_mutex);

  /* do record */
  g_rec_mutex_lock(jack_midiin_mutex);
  
  jack_midiin->flags &= (~AGS_JACK_MIDIIN_START_RECORD);

  if((AGS_JACK_MIDIIN_INITIALIZED & (jack_midiin->flags)) == 0){
    g_rec_mutex_unlock(jack_midiin_mutex);
    
    return;
  }

  g_rec_mutex_unlock(jack_midiin_mutex);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* check activated */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_client_mutex);

  if(jack_client_activated){
    /* signal client */
    if((AGS_JACK_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_midiin->sync_flags),
		      AGS_JACK_MIDIIN_CALLBACK_DONE);
    
      if((AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	g_cond_signal(&(jack_midiin->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_midiin->sync_flags),
			AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0 &&
	      (AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	  g_cond_wait(&(jack_midiin->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_midiin->sync_flags),
		       (~(AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT |
			  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_midiin->sync_flags),
		       (~AGS_JACK_MIDIIN_INITIAL_CALLBACK));
    }
  }

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

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
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_jack_midiin_port_free(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* check initialized */
  g_rec_mutex_lock(jack_midiin_mutex);

  if((AGS_JACK_MIDIIN_INITIALIZED & (jack_midiin->flags)) == 0){
    g_rec_mutex_unlock(jack_midiin_mutex);
    
    return;
  }

  callback_mutex = &(jack_midiin->callback_mutex);
  callback_finish_mutex = &(jack_midiin->callback_finish_mutex);

  jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_0;
  jack_midiin->flags &= (~(AGS_JACK_MIDIIN_RECORD));

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_PASS_THROUGH);
  g_atomic_int_and(&(jack_midiin->sync_flags),
		   (~AGS_JACK_MIDIIN_INITIAL_CALLBACK));

  g_rec_mutex_unlock(jack_midiin_mutex);

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_CALLBACK_DONE);
    
  if((AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
    g_cond_signal(&(jack_midiin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_midiin->sync_flags),
		  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
    g_cond_signal(&(jack_midiin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);

  /*  */
  g_rec_mutex_lock(jack_midiin_mutex);

  if(jack_midiin->app_buffer[1] != NULL){
    g_free(jack_midiin->app_buffer[1]);
    jack_midiin->app_buffer_size[1] = 0;
  }

  if(jack_midiin->app_buffer[2] != NULL){
    g_free(jack_midiin->app_buffer[2]);
    jack_midiin->app_buffer_size[2] = 0;
  }

  if(jack_midiin->app_buffer[3] != NULL){
    g_free(jack_midiin->app_buffer[3]);
    jack_midiin->app_buffer_size[3] = 0;
  }

  if(jack_midiin->app_buffer[0] != NULL){
    g_free(jack_midiin->app_buffer[0]);
    jack_midiin->app_buffer_size[0] = 0;
  }

  jack_midiin->note_offset = jack_midiin->start_note_offset;
  jack_midiin->note_offset_absolute = jack_midiin->start_note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);
}

void
ags_jack_midiin_tic(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  GRecMutex *jack_midiin_mutex;

  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(jack_midiin_mutex);

  delay = jack_midiin->delay;
  delay_counter = jack_midiin->delay_counter;

  note_offset = jack_midiin->note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);

  if(delay_counter + 1.0 >= delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);
    
    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(jack_midiin_mutex);

    jack_midiin->delay_counter = delay_counter + 1.0 - delay;
    jack_midiin->tact_counter += 1.0;

    g_rec_mutex_unlock(jack_midiin_mutex);
  }else{
    g_rec_mutex_lock(jack_midiin_mutex);

    jack_midiin->delay_counter += 1.0;

    g_rec_mutex_unlock(jack_midiin_mutex);
  }
}

void
ags_jack_midiin_offset_changed(AgsSequencer *sequencer,
			       guint note_offset)
{
  AgsJackMidiin *jack_midiin;
  
  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* offset changed */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->tic_counter += 1;

  if(jack_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_midiin->tic_counter = 0;
  }

  g_rec_mutex_unlock(jack_midiin_mutex);
}

void
ags_jack_midiin_set_bpm(AgsSequencer *sequencer,
			gdouble bpm)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set bpm */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->bpm = bpm;

  g_rec_mutex_unlock(jack_midiin_mutex);
}

gdouble
ags_jack_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  gdouble bpm;
  
  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* get bpm */
  g_rec_mutex_lock(jack_midiin_mutex);

  bpm = jack_midiin->bpm;
  
  g_rec_mutex_unlock(jack_midiin_mutex);

  return(bpm);
}

void
ags_jack_midiin_set_delay_factor(AgsSequencer *sequencer,
				 gdouble delay_factor)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set delay factor */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->delay_factor = delay_factor;

  g_rec_mutex_unlock(jack_midiin_mutex);
}

gdouble
ags_jack_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  gdouble delay_factor;
  
  GRecMutex *jack_midiin_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack_midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* get delay factor */
  g_rec_mutex_lock(jack_midiin_mutex);

  delay_factor = jack_midiin->delay_factor;
  
  g_rec_mutex_unlock(jack_midiin_mutex);

  return(delay_factor);
}

void*
ags_jack_midiin_get_buffer(AgsSequencer *sequencer,
			   guint *buffer_length)
{
  AgsJackMidiin *jack_midiin;

  char *buffer;

  GRecMutex *jack_midiin_mutex;  
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);
  
  /* get buffer */
  if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_0){
    buffer = jack_midiin->app_buffer[0];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[0];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_1){
    buffer = jack_midiin->app_buffer[1];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[1];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_2){
    buffer = jack_midiin->app_buffer[2];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[2];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_3){
    buffer = jack_midiin->app_buffer[3];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[3];
    }
  }else{
    buffer = NULL;

    if(buffer_length != NULL){
      buffer_length[0] = 0;
    }
  }

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(buffer);
}

void*
ags_jack_midiin_get_next_buffer(AgsSequencer *sequencer,
				guint *buffer_length)
{
  AgsJackMidiin *jack_midiin;

  char *buffer;

  GRecMutex *jack_midiin_mutex;  
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);
  
  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  g_rec_mutex_lock(jack_midiin_mutex);
  
  /* get buffer */
  if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_0){
    buffer = jack_midiin->app_buffer[1];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[1];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_1){
    buffer = jack_midiin->app_buffer[2];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[2];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_2){
    buffer = jack_midiin->app_buffer[3];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[3];
    }
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_3){
    buffer = jack_midiin->app_buffer[0];

    if(buffer_length != NULL){
      buffer_length[0] = jack_midiin->app_buffer_size[0];
    }
  }else{
    buffer = NULL;

    if(buffer_length != NULL){
      buffer_length[0] = 0;
    }
  }

  g_rec_mutex_unlock(jack_midiin_mutex);
  
  return(buffer);
}

void
ags_jack_midiin_lock_buffer(AgsSequencer *sequencer,
			    void *app_buffer)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *app_buffer_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  app_buffer_mutex = NULL;

  if(jack_midiin->app_buffer != NULL){
    if(app_buffer == jack_midiin->app_buffer[0]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[0];
    }else if(app_buffer == jack_midiin->app_buffer[1]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[1];
    }else if(app_buffer == jack_midiin->app_buffer[2]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[2];
    }else if(app_buffer == jack_midiin->app_buffer[3]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[3];
    }
  }
  
  if(app_buffer_mutex != NULL){
    g_rec_mutex_lock(app_buffer_mutex);
  }
}

void
ags_jack_midiin_unlock_buffer(AgsSequencer *sequencer,
			      void *app_buffer)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *app_buffer_mutex;
  
  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  app_buffer_mutex = NULL;

  if(jack_midiin->app_buffer != NULL){
    if(app_buffer == jack_midiin->app_buffer[0]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[0];
    }else if(app_buffer == jack_midiin->app_buffer[1]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[1];
    }else if(app_buffer == jack_midiin->app_buffer[2]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[2];
    }else if(app_buffer == jack_midiin->app_buffer[3]){
      app_buffer_mutex = jack_midiin->app_buffer_mutex[3];
    }
  }

  if(app_buffer_mutex != NULL){
    g_rec_mutex_unlock(app_buffer_mutex);
  }
}

void
ags_jack_midiin_set_start_note_offset(AgsSequencer *sequencer,
				      guint start_note_offset)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;  

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set note offset */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);
}

guint
ags_jack_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  guint start_note_offset;
  
  GRecMutex *jack_midiin_mutex;  

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set note offset */
  g_rec_mutex_lock(jack_midiin_mutex);

  start_note_offset = jack_midiin->start_note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);

  return(start_note_offset);
}


void
ags_jack_midiin_set_note_offset(AgsSequencer *sequencer,
				guint note_offset)
{
  AgsJackMidiin *jack_midiin;

  GRecMutex *jack_midiin_mutex;  

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set note offset */
  g_rec_mutex_lock(jack_midiin_mutex);

  jack_midiin->note_offset = note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);
}

guint
ags_jack_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsJackMidiin *jack_midiin;

  guint note_offset;
  
  GRecMutex *jack_midiin_mutex;  

  jack_midiin = AGS_JACK_MIDIIN(sequencer);

  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* set note offset */
  g_rec_mutex_lock(jack_midiin_mutex);

  note_offset = jack_midiin->note_offset;

  g_rec_mutex_unlock(jack_midiin_mutex);

  return(note_offset);
}

/**
 * ags_jack_midiin_switch_buffer_flag:
 * @jack_midiin: the #AgsJackMidiin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 3.0.0
 */
void
ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin)
{
  GRecMutex *jack_midiin_mutex;  

  if(!AGS_IS_JACK_MIDIIN(jack_midiin)){
    return;
  }
    
  /* get jack midiin mutex */
  jack_midiin_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(jack_midiin);

  /* switch buffer flag */
  g_rec_mutex_lock(jack_midiin_mutex);

  if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_0){
    jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_1;

    /* clear buffer */
    if(jack_midiin->app_buffer[3] != NULL){
      g_free(jack_midiin->app_buffer[3]);
    }

    jack_midiin->app_buffer[3] = NULL;
    jack_midiin->app_buffer_size[3] = 0;
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_1){
    jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_2;

    /* clear buffer */
    if(jack_midiin->app_buffer[0] != NULL){
      g_free(jack_midiin->app_buffer[0]);
    }

    jack_midiin->app_buffer[0] = NULL;
    jack_midiin->app_buffer_size[0] = 0;
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_2){
    jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_3;

    /* clear buffer */
    if(jack_midiin->app_buffer[1] != NULL){
      g_free(jack_midiin->app_buffer[1]);
    }

    jack_midiin->app_buffer[1] = NULL;
    jack_midiin->app_buffer_size[1] = 0;
  }else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_3){
    jack_midiin->app_buffer_mode = AGS_JACK_MIDIIN_APP_BUFFER_0;

    /* clear buffer */
    if(jack_midiin->app_buffer[2] != NULL){
      g_free(jack_midiin->app_buffer[2]);
    }

    jack_midiin->app_buffer[2] = NULL;
    jack_midiin->app_buffer_size[2] = 0;
  }

  g_rec_mutex_unlock(jack_midiin_mutex);
}

/**
 * ags_jack_midiin_new:
 *
 * Creates a new instance of #AgsJackMidiin.
 *
 * Returns: the new #AgsJackMidiin
 *
 * Since: 3.0.0
 */
AgsJackMidiin*
ags_jack_midiin_new()
{
  AgsJackMidiin *jack_midiin;

  jack_midiin = (AgsJackMidiin *) g_object_new(AGS_TYPE_JACK_MIDIIN,
					       NULL);
  
  return(jack_midiin);
}
