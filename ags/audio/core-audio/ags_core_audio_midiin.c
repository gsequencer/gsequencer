/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

void ags_core_audio_midiin_lock_buffer(AgsSequencer *sequencer,
				       void *buffer);
void ags_core_audio_midiin_unlock_buffer(AgsSequencer *sequencer,
					 void *buffer);

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
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
};

static gpointer ags_core_audio_midiin_parent_class = NULL;

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

GType
ags_core_audio_midiin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_CORE_AUDIO_MIDIIN_INITIALIZED, "AGS_CORE_AUDIO_MIDIIN_INITIALIZED", "core-audio-midiin-initialized" },
      { AGS_CORE_AUDIO_MIDIIN_START_RECORD, "AGS_CORE_AUDIO_MIDIIN_START_RECORD", "core-audio-midiin-start-record" },
      { AGS_CORE_AUDIO_MIDIIN_RECORD, "AGS_CORE_AUDIO_MIDIIN_RECORD", "core-audio-midiin-record" },
      { AGS_CORE_AUDIO_MIDIIN_SHUTDOWN, "AGS_CORE_AUDIO_MIDIIN_SHUTDOWN", "core-audio-midiin-shutdown" },
      { AGS_CORE_AUDIO_MIDIIN_NONBLOCKING, "AGS_CORE_AUDIO_MIDIIN_NONBLOCKING", "core-audio-midiin-nonblocking" },
      { AGS_CORE_AUDIO_MIDIIN_ATTACK_FIRST, "AGS_CORE_AUDIO_MIDIIN_ATTACK_FIRST", "core-audio-midiin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsCoreAudioMidiinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * AgsCoreAudioMidiin:device:
   *
   * The core audio sequencer indentifier
   * 
   * Since: 3.0.0
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
   * AgsCoreAudioMidiin:bpm:
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
   * AgsCoreAudioMidiin:attack:
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
   * AgsCoreAudioMidiin:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  sequencer->lock_buffer = ags_core_audio_midiin_lock_buffer;
  sequencer->unlock_buffer = ags_core_audio_midiin_unlock_buffer;

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

  guint i;
  guint denominator, numerator;

  /* flags */
  core_audio_midiin->flags = 0;
  core_audio_midiin->connectable_flags = 0;
  g_atomic_int_set(&(core_audio_midiin->sync_flags),
		   AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH);

  /* core-audio midiin mutex */
  g_rec_mutex_init(&(core_audio_midiin->obj_mutex));

  /* uuid */
  core_audio_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_midiin->uuid);

  core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0;
  
  /* buffer */
  core_audio_midiin->app_buffer_mutex = (GRecMutex **) g_malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    core_audio_midiin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_midiin->app_buffer_mutex[i]);
  }

  core_audio_midiin->app_buffer = (char **) g_malloc(4 * sizeof(char *));

  core_audio_midiin->app_buffer[0] = NULL;
  core_audio_midiin->app_buffer[1] = NULL;
  core_audio_midiin->app_buffer[2] = NULL;
  core_audio_midiin->app_buffer[3] = NULL;

  core_audio_midiin->app_buffer_size[0] = 0;
  core_audio_midiin->app_buffer_size[1] = 0;
  core_audio_midiin->app_buffer_size[2] = 0;
  core_audio_midiin->app_buffer_size[3] = 0;

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
	   &denominator,
	   &numerator);
    
    core_audio_midiin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* counters */
  core_audio_midiin->start_note_offset = 0;
  core_audio_midiin->note_offset = 0;
  core_audio_midiin->note_offset_absolute = 0;
  
  core_audio_midiin->tact_counter = 0.0;
  core_audio_midiin->delay_counter = 0;
  core_audio_midiin->tic_counter = 0;

  /* card and port */
  core_audio_midiin->card_uri = NULL;
  core_audio_midiin->core_audio_client = NULL;

  core_audio_midiin->port_name = NULL;
  core_audio_midiin->core_audio_port = NULL;

  /* callback mutex */
  g_mutex_init(&(core_audio_midiin->callback_mutex));

  g_cond_init(&(core_audio_midiin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(core_audio_midiin->callback_finish_mutex));

  g_cond_init(&(core_audio_midiin->callback_finish_cond));
}

void
ags_core_audio_midiin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);

  /* get core-audio client mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
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

      g_rec_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->bpm = bpm;

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(core_audio_midiin_mutex);

      core_audio_midiin->delay_factor = delay_factor;

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = g_value_get_object(value);

      g_rec_mutex_lock(core_audio_midiin_mutex);

      if(core_audio_midiin->core_audio_client == (GObject *) core_audio_client){
	g_rec_mutex_unlock(core_audio_midiin_mutex);
	
	return;
      }

      if(core_audio_midiin->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_midiin->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(G_OBJECT(core_audio_client));
      }

      core_audio_midiin->core_audio_client = (GObject *) core_audio_client;

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_midiin_mutex);

      if(g_list_find(core_audio_midiin->core_audio_port, core_audio_port) != NULL){
	g_rec_mutex_unlock(core_audio_midiin_mutex);

	return;
      }

      if(core_audio_port != NULL){
	g_object_ref(core_audio_port);
	core_audio_midiin->core_audio_port = g_list_append(core_audio_midiin->core_audio_port,
							   core_audio_port);
      }

      g_rec_mutex_unlock(core_audio_midiin_mutex);
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

  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(gobject);  

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_string(value, core_audio_midiin->card_uri);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_pointer(value, core_audio_midiin->app_buffer);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_double(value, core_audio_midiin->bpm);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_double(value, core_audio_midiin->delay_factor);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_object(value, core_audio_midiin->core_audio_client);

      g_rec_mutex_unlock(core_audio_midiin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_rec_mutex_lock(core_audio_midiin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_midiin->core_audio_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(core_audio_midiin_mutex);
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

  ags_uuid_free(core_audio_midiin->uuid);
  
  /* free output buffer */
  if(core_audio_midiin->app_buffer[0] != NULL){
    g_free(core_audio_midiin->app_buffer[0]);
  }

  if(core_audio_midiin->app_buffer[1] != NULL){
    g_free(core_audio_midiin->app_buffer[1]);
  }
    
  if(core_audio_midiin->app_buffer[2] != NULL){
    g_free(core_audio_midiin->app_buffer[2]);
  }
  
  if(core_audio_midiin->app_buffer[3] != NULL){
    g_free(core_audio_midiin->app_buffer[3]);
  }
  
  /* free buffer array */
  g_free(core_audio_midiin->app_buffer);

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

  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio_midiin signal mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* get UUID */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  ptr = core_audio_midiin->uuid;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
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

  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* check is ready */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_ready);
}

void
ags_core_audio_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
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

  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* check is connected */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_connected);
}

void
ags_core_audio_midiin_connect(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(connectable);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
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
 * Since: 3.0.0
 */
gboolean
ags_core_audio_midiin_test_flags(AgsCoreAudioMidiin *core_audio_midiin, AgsCoreAudioMidiinFlags flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return(FALSE);
  }

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* test */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  retval = (flags & (core_audio_midiin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(retval);
}

/**
 * ags_core_audio_midiin_set_flags:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 * @flags: see #AgsCoreAudioMidiinFlags-enum
 *
 * Enable a feature of @core_audio_midiin.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_midiin_set_flags(AgsCoreAudioMidiin *core_audio_midiin, AgsCoreAudioMidiinFlags flags)
{
  GRecMutex *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return;
  }

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
}
    
/**
 * ags_core_audio_midiin_unset_flags:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 * @flags: see #AgsCoreAudioMidiinFlags-enum
 *
 * Disable a feature of @core_audio_midiin.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_midiin_unset_flags(AgsCoreAudioMidiin *core_audio_midiin, AgsCoreAudioMidiinFlags flags)
{  
  GRecMutex *core_audio_midiin_mutex;

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return;
  }

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
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

  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);

  if(core_audio_midiin->card_uri == device ||
     (core_audio_midiin->card_uri != NULL &&
      !g_ascii_strcasecmp(core_audio_midiin->card_uri,
			  device))){
    g_rec_mutex_unlock(core_audio_midiin_mutex);
    
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-core-audio-midiin-")){
    g_warning("invalid CORE_AUDIO device prefix");

    g_rec_mutex_unlock(core_audio_midiin_mutex);

    return;
  }

  ret = sscanf(device,
	       "ags-core-audio-midiin-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid CORE_AUDIO device specifier");

    g_rec_mutex_unlock(core_audio_midiin_mutex);

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
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

gchar*
ags_core_audio_midiin_get_device(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gchar *device;
  
  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  device = NULL;
  
  g_rec_mutex_lock(core_audio_midiin_mutex);

  device = g_strdup(core_audio_midiin->card_uri);

  g_rec_mutex_unlock(core_audio_midiin_mutex);

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
ags_core_audio_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gboolean is_starting;
  
  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* check is starting */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  is_starting = ((AGS_CORE_AUDIO_MIDIIN_START_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_core_audio_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gboolean is_recording;
  
  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* check is starting */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  is_recording = ((AGS_CORE_AUDIO_MIDIIN_RECORD & (core_audio_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(is_recording);
}

void
ags_core_audio_midiin_port_init(AgsSequencer *sequencer,
				GError **error)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;
  
  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /*  */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  /* prepare for record */
  core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3;
  core_audio_midiin->flags |= (AGS_CORE_AUDIO_MIDIIN_START_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_NONBLOCKING);

  /* port setup */
  //TODO:JK: implement me
  
  /*  */
  core_audio_midiin->tact_counter = 0.0;
  core_audio_midiin->delay_counter = floor(core_audio_midiin->delay);
  core_audio_midiin->tic_counter = 0;

  core_audio_midiin->flags |= (AGS_CORE_AUDIO_MIDIIN_INITIALIZED |
			       AGS_CORE_AUDIO_MIDIIN_START_RECORD |
			       AGS_CORE_AUDIO_MIDIIN_RECORD);
  
  g_atomic_int_and(&(core_audio_midiin->sync_flags),
		   (~(AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH)));
  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK);
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);
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
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  gboolean core_audio_client_activated;
  gboolean do_sync;
  
  GRecMutex *core_audio_midiin_mutex;
  GRecMutex *core_audio_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* client */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_midiin->core_audio_client;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);

  /* do record */
  g_rec_mutex_lock(core_audio_midiin_mutex);
  
  core_audio_midiin->flags &= (~AGS_CORE_AUDIO_MIDIIN_START_RECORD);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    g_rec_mutex_unlock(core_audio_midiin_mutex);
    
    return;
  }

  callback_mutex = &(core_audio_midiin->callback_mutex);
  callback_finish_mutex = &(core_audio_midiin->callback_finish_mutex);
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);

  /* get core_audio client mutex */
  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(core_audio_client);

  /* check activated */
  g_rec_mutex_lock(core_audio_client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_client_mutex);

  do_sync = FALSE;
  
  if(core_audio_client_activated){
    if((AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
      if((AGS_CORE_AUDIO_MIDIIN_DO_SYNC & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	g_atomic_int_and(&(core_audio_midiin->sync_flags),
			 (~AGS_CORE_AUDIO_MIDIIN_DO_SYNC));
	do_sync = TRUE;
      
	/* signal client - wait callback */
	g_mutex_lock(callback_mutex);

	g_atomic_int_and(&(core_audio_midiin->sync_flags),
			 (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT));

	if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
	  g_cond_signal(&(core_audio_midiin->callback_cond));
	}

	g_mutex_unlock(callback_mutex);
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
    g_mutex_lock(callback_finish_mutex);

    if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE));

      while((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0 &&
	    (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	g_cond_wait(&(core_audio_midiin->callback_finish_cond),
		    callback_finish_mutex);
      }
    }

    g_mutex_unlock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_midiin->sync_flags),
		    (AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT |  AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE));
  }

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

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
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_core_audio_midiin_port_free(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* check initialized */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  if((AGS_CORE_AUDIO_MIDIIN_INITIALIZED & (core_audio_midiin->flags)) == 0){
    g_rec_mutex_unlock(core_audio_midiin_mutex);
    
    return;
  }

  callback_mutex = &(core_audio_midiin->callback_mutex);
  callback_finish_mutex = &(core_audio_midiin->callback_finish_mutex);

  core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0;
  core_audio_midiin->flags &= (~(AGS_CORE_AUDIO_MIDIIN_RECORD));

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH);
  g_atomic_int_and(&(core_audio_midiin->sync_flags),
		   (~AGS_CORE_AUDIO_MIDIIN_INITIAL_CALLBACK));

  g_rec_mutex_unlock(core_audio_midiin_mutex);

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_midiin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(core_audio_midiin->sync_flags),
		  AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_midiin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);

  /*  */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  if(core_audio_midiin->app_buffer[1] != NULL){
    g_free(core_audio_midiin->app_buffer[1]);
    core_audio_midiin->app_buffer_size[1] = 0;
  }

  if(core_audio_midiin->app_buffer[2] != NULL){
    g_free(core_audio_midiin->app_buffer[2]);
    core_audio_midiin->app_buffer_size[2] = 0;
  }

  if(core_audio_midiin->app_buffer[3] != NULL){
    g_free(core_audio_midiin->app_buffer[3]);
    core_audio_midiin->app_buffer_size[3] = 0;
  }

  if(core_audio_midiin->app_buffer[0] != NULL){
    g_free(core_audio_midiin->app_buffer[0]);
    core_audio_midiin->app_buffer_size[0] = 0;
  }

  core_audio_midiin->note_offset = core_audio_midiin->start_note_offset;
  core_audio_midiin->note_offset_absolute = core_audio_midiin->start_note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_tic(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  GRecMutex *core_audio_midiin_mutex;

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  delay = core_audio_midiin->delay;
  delay_counter = core_audio_midiin->delay_counter;

  note_offset = core_audio_midiin->note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);

  if(delay_counter + 1.0 >= delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);
    
    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(core_audio_midiin_mutex);

    core_audio_midiin->delay_counter = delay_counter + 1.0 - delay;
    core_audio_midiin->tact_counter += 1.0;

    g_rec_mutex_unlock(core_audio_midiin_mutex);
  }else{
    g_rec_mutex_lock(core_audio_midiin_mutex);

    core_audio_midiin->delay_counter += 1.0;

    g_rec_mutex_unlock(core_audio_midiin_mutex);
  }
}

void
ags_core_audio_midiin_offset_changed(AgsSequencer *sequencer,
				     guint note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;
  
  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* offset changed */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->tic_counter += 1;

  if(core_audio_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_midiin->tic_counter = 0;
  }

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

void
ags_core_audio_midiin_set_bpm(AgsSequencer *sequencer,
			      gdouble bpm)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set bpm */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->bpm = bpm;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

gdouble
ags_core_audio_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble bpm;
  
  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* get bpm */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  bpm = core_audio_midiin->bpm;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(bpm);
}

void
ags_core_audio_midiin_set_delay_factor(AgsSequencer *sequencer,
				       gdouble delay_factor)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set delay factor */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->delay_factor = delay_factor;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

gdouble
ags_core_audio_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  gdouble delay_factor;
  
  GRecMutex *core_audio_midiin_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio_midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* get delay factor */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  delay_factor = core_audio_midiin->delay_factor;
  
  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(delay_factor);
}

void*
ags_core_audio_midiin_get_buffer(AgsSequencer *sequencer,
				 guint *buffer_length)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  char *buffer;

  GRecMutex *core_audio_midiin_mutex;  
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);
  
  /* get buffer */
  if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0){
    buffer = core_audio_midiin->app_buffer[0];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[0];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_1){
    buffer = core_audio_midiin->app_buffer[1];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[1];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_2){
    buffer = core_audio_midiin->app_buffer[2];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[2];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3){
    buffer = core_audio_midiin->app_buffer[3];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[3];
    }
  }else{
    buffer = NULL;

    if(buffer_length != NULL){
      buffer_length[0] = 0;
    }
  }

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
  return(buffer);
}

void*
ags_core_audio_midiin_get_next_buffer(AgsSequencer *sequencer,
				      guint *buffer_length)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  char *buffer;

  GRecMutex *core_audio_midiin_mutex;  
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);
  
  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  g_rec_mutex_lock(core_audio_midiin_mutex);
  
  /* get buffer */
  if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0){
    buffer = core_audio_midiin->app_buffer[1];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[1];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_1){
    buffer = core_audio_midiin->app_buffer[2];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[2];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_2){
    buffer = core_audio_midiin->app_buffer[3];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[3];
    }
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3){
    buffer = core_audio_midiin->app_buffer[0];

    if(buffer_length != NULL){
      buffer_length[0] = core_audio_midiin->app_buffer_size[0];
    }
  }else{
    buffer = NULL;

    if(buffer_length != NULL){
      buffer_length[0] = 0;
    }
  }

  g_rec_mutex_unlock(core_audio_midiin_mutex);
  
  return(buffer);
}

void
ags_core_audio_midiin_lock_buffer(AgsSequencer *sequencer,
				  void *buffer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *app_buffer_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  app_buffer_mutex = NULL;

  if(core_audio_midiin->app_buffer != NULL){
    if(buffer == core_audio_midiin->app_buffer[0]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[0];
    }else if(buffer == core_audio_midiin->app_buffer[1]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[1];
    }else if(buffer == core_audio_midiin->app_buffer[2]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[2];
    }else if(buffer == core_audio_midiin->app_buffer[3]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[3];
    }
  }
  
  if(app_buffer_mutex != NULL){
    g_rec_mutex_lock(app_buffer_mutex);
  }
}

void
ags_core_audio_midiin_unlock_buffer(AgsSequencer *sequencer,
				    void *buffer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *app_buffer_mutex;
  
  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  app_buffer_mutex = NULL;

  if(core_audio_midiin->app_buffer != NULL){
    if(buffer == core_audio_midiin->app_buffer[0]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[0];
    }else if(buffer == core_audio_midiin->app_buffer[1]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[1];
    }else if(buffer == core_audio_midiin->app_buffer[2]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[2];
    }else if(buffer == core_audio_midiin->app_buffer[3]){
      app_buffer_mutex = core_audio_midiin->app_buffer_mutex[3];
    }
  }

  if(app_buffer_mutex != NULL){
    g_rec_mutex_unlock(app_buffer_mutex);
  }
}

void
ags_core_audio_midiin_set_start_note_offset(AgsSequencer *sequencer,
					    guint start_note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set note offset */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

guint
ags_core_audio_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  guint start_note_offset;
  
  GRecMutex *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set note offset */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  start_note_offset = core_audio_midiin->start_note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(start_note_offset);
}

void
ags_core_audio_midiin_set_note_offset(AgsSequencer *sequencer,
				      guint note_offset)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  GRecMutex *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set note offset */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  core_audio_midiin->note_offset = note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

guint
ags_core_audio_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsCoreAudioMidiin *core_audio_midiin;

  guint note_offset;
  
  GRecMutex *core_audio_midiin_mutex;  

  core_audio_midiin = AGS_CORE_AUDIO_MIDIIN(sequencer);

  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* set note offset */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  note_offset = core_audio_midiin->note_offset;

  g_rec_mutex_unlock(core_audio_midiin_mutex);

  return(note_offset);
}

/**
 * ags_core_audio_midiin_switch_buffer_flag:
 * @core_audio_midiin: the #AgsCoreAudioMidiin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_midiin_switch_buffer_flag(AgsCoreAudioMidiin *core_audio_midiin)
{
  GRecMutex *core_audio_midiin_mutex;  

  if(!AGS_IS_CORE_AUDIO_MIDIIN(core_audio_midiin)){
    return;
  }
    
  /* get core_audio midiin mutex */
  core_audio_midiin_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);

  /* switch buffer flag */
  g_rec_mutex_lock(core_audio_midiin_mutex);

  if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0){
    core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_1;

    /* clear buffer */
    if(core_audio_midiin->app_buffer[3] != NULL){
      g_free(core_audio_midiin->app_buffer[3]);
    }

    core_audio_midiin->app_buffer[3] = NULL;
    core_audio_midiin->app_buffer_size[3] = 0;
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_1){
    core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_2;

    /* clear buffer */
    if(core_audio_midiin->app_buffer[0] != NULL){
      g_free(core_audio_midiin->app_buffer[0]);
    }

    core_audio_midiin->app_buffer[0] = NULL;
    core_audio_midiin->app_buffer_size[0] = 0;
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_2){
    core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3;

    /* clear buffer */
    if(core_audio_midiin->app_buffer[1] != NULL){
      g_free(core_audio_midiin->app_buffer[1]);
    }

    core_audio_midiin->app_buffer[1] = NULL;
    core_audio_midiin->app_buffer_size[1] = 0;
  }else if(core_audio_midiin->app_buffer_mode == AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3){
    core_audio_midiin->app_buffer_mode = AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0;

    /* clear buffer */
    if(core_audio_midiin->app_buffer[2] != NULL){
      g_free(core_audio_midiin->app_buffer[2]);
    }

    core_audio_midiin->app_buffer[2] = NULL;
    core_audio_midiin->app_buffer_size[2] = 0;
  }

  g_rec_mutex_unlock(core_audio_midiin_mutex);
}

/**
 * ags_core_audio_midiin_new:
 *
 * Creates a new instance of #AgsCoreAudioMidiin.
 *
 * Returns: a new #AgsCoreAudioMidiin
 *
 * Since: 3.0.0
 */
AgsCoreAudioMidiin*
ags_core_audio_midiin_new()
{
  AgsCoreAudioMidiin *core_audio_midiin;

  core_audio_midiin = (AgsCoreAudioMidiin *) g_object_new(AGS_TYPE_CORE_AUDIO_MIDIIN,
							  NULL);
  
  return(core_audio_midiin);
}
