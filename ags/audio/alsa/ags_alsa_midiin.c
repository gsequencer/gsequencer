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

#include <ags/audio/alsa/ags_alsa_midiin.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/config.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef AGS_W32API
#include <sys/utsname.h>
#endif

#ifdef AGS_WITH_ALSA
#include <alsa/rawmidi.h>
#endif

#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/i18n.h>

/**
 * SECTION:ags_alsa_midiin
 * @short_description: Input from sequencer
 * @title: AgsAlsaMidiin
 * @section_id:
 * @include: ags/audio/ags_alsa_midiin.h
 *
 * #AgsAlsaMidiin represents a sequencer and supports midi input.
 */

void ags_alsa_midiin_class_init(AgsAlsaMidiinClass *alsa_midiin);
void ags_alsa_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_alsa_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_alsa_midiin_init(AgsAlsaMidiin *alsa_midiin);
void ags_alsa_midiin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_alsa_midiin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_alsa_midiin_dispose(GObject *gobject);
void ags_alsa_midiin_finalize(GObject *gobject);

AgsUUID* ags_alsa_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_alsa_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_alsa_midiin_is_ready(AgsConnectable *connectable);
void ags_alsa_midiin_add_to_registry(AgsConnectable *connectable);
void ags_alsa_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_alsa_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_alsa_midiin_xml_compose(AgsConnectable *connectable);
void ags_alsa_midiin_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_alsa_midiin_is_connected(AgsConnectable *connectable);
void ags_alsa_midiin_connect(AgsConnectable *connectable);
void ags_alsa_midiin_disconnect(AgsConnectable *connectable);

void ags_alsa_midiin_set_device(AgsSequencer *sequencer,
				gchar *device);
gchar* ags_alsa_midiin_get_device(AgsSequencer *sequencer);

void ags_alsa_midiin_list_cards(AgsSequencer *sequencer,
				GList **card_id, GList **card_name);

gboolean ags_alsa_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_alsa_midiin_is_recording(AgsSequencer *sequencer);

void ags_alsa_midiin_device_record_init(AgsSequencer *sequencer,
					GError **error);
void ags_alsa_midiin_device_record(AgsSequencer *sequencer,
				   GError **error);
void ags_alsa_midiin_device_free(AgsSequencer *sequencer);

void ags_alsa_midiin_tic(AgsSequencer *sequencer);
void ags_alsa_midiin_offset_changed(AgsSequencer *sequencer,
				    guint note_offset);

void ags_alsa_midiin_set_bpm(AgsSequencer *sequencer,
			     gdouble bpm);
gdouble ags_alsa_midiin_get_bpm(AgsSequencer *sequencer);

void ags_alsa_midiin_set_delay_factor(AgsSequencer *sequencer,
				      gdouble delay_factor);
gdouble ags_alsa_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_alsa_midiin_get_buffer(AgsSequencer *sequencer,
				 guint *buffer_length);
void* ags_alsa_midiin_get_next_buffer(AgsSequencer *sequencer,
				      guint *buffer_length);

void ags_alsa_midiin_lock_buffer(AgsSequencer *sequencer,
				 void *buffer);
void ags_alsa_midiin_unlock_buffer(AgsSequencer *sequencer,
				   void *buffer);

void ags_alsa_midiin_set_start_note_offset(AgsSequencer *sequencer,
					   guint start_note_offset);
guint ags_alsa_midiin_get_start_note_offset(AgsSequencer *sequencer);

void ags_alsa_midiin_set_note_offset(AgsSequencer *sequencer,
				     guint note_offset);
guint ags_alsa_midiin_get_note_offset(AgsSequencer *sequencer);

enum{
  PROP_0,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
};

static gpointer ags_alsa_midiin_parent_class = NULL;

GType
ags_alsa_midiin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_alsa_midiin = 0;

    static const GTypeInfo ags_alsa_midiin_info = {
      sizeof(AgsAlsaMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_alsa_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAlsaMidiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_alsa_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_alsa_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_alsa_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_alsa_midiin = g_type_register_static(G_TYPE_OBJECT,
						  "AgsAlsaMidiin",
						  &ags_alsa_midiin_info,
						  0);

    g_type_add_interface_static(ags_type_alsa_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_alsa_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_alsa_midiin);
  }

  return g_define_type_id__volatile;
}

void
ags_alsa_midiin_class_init(AgsAlsaMidiinClass *alsa_midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_alsa_midiin_parent_class = g_type_class_peek_parent(alsa_midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) alsa_midiin;

  gobject->set_property = ags_alsa_midiin_set_property;
  gobject->get_property = ags_alsa_midiin_get_property;

  gobject->dispose = ags_alsa_midiin_dispose;
  gobject->finalize = ags_alsa_midiin_finalize;

  /* properties */
  /**
   * AgsAlsaMidiin:device:
   *
   * The alsa sequencer indentifier
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_ALSA_MIDIIN_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsAlsaMidiin:buffer:
   *
   * The buffer
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsAlsaMidiin:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.13.2
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
   * AgsAlsaMidiin:delay-factor:
   *
   * tact
   * 
   * Since: 3.13.2
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
   * AgsAlsaMidiin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /* AgsAlsaMidiinClass */
}

GQuark
ags_alsa_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-alsa-midiin-error-quark"));
}

void
ags_alsa_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_alsa_midiin_get_uuid;
  connectable->has_resource = ags_alsa_midiin_has_resource;

  connectable->is_ready = ags_alsa_midiin_is_ready;
  connectable->add_to_registry = ags_alsa_midiin_add_to_registry;
  connectable->remove_from_registry = ags_alsa_midiin_remove_from_registry;

  connectable->list_resource = ags_alsa_midiin_list_resource;
  connectable->xml_compose = ags_alsa_midiin_xml_compose;
  connectable->xml_parse = ags_alsa_midiin_xml_parse;

  connectable->is_connected = ags_alsa_midiin_is_connected;  
  connectable->connect = ags_alsa_midiin_connect;
  connectable->disconnect = ags_alsa_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_alsa_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_device = ags_alsa_midiin_set_device;
  sequencer->get_device = ags_alsa_midiin_get_device;

  sequencer->list_cards = ags_alsa_midiin_list_cards;

  sequencer->is_starting =  ags_alsa_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_alsa_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_alsa_midiin_device_record_init;
  sequencer->record = ags_alsa_midiin_device_record;

  sequencer->stop = ags_alsa_midiin_device_free;

  sequencer->tic = ags_alsa_midiin_tic;
  sequencer->offset_changed = ags_alsa_midiin_offset_changed;
    
  sequencer->set_bpm = ags_alsa_midiin_set_bpm;
  sequencer->get_bpm = ags_alsa_midiin_get_bpm;

  sequencer->set_delay_factor = ags_alsa_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_alsa_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_alsa_midiin_get_buffer;
  sequencer->get_next_buffer = ags_alsa_midiin_get_next_buffer;

  sequencer->lock_buffer = ags_alsa_midiin_lock_buffer;
  sequencer->unlock_buffer = ags_alsa_midiin_unlock_buffer;

  sequencer->set_start_note_offset = ags_alsa_midiin_set_start_note_offset;
  sequencer->get_start_note_offset = ags_alsa_midiin_get_start_note_offset;

  sequencer->set_note_offset = ags_alsa_midiin_set_note_offset;
  sequencer->get_note_offset = ags_alsa_midiin_get_note_offset;
}

void
ags_alsa_midiin_init(AgsAlsaMidiin *alsa_midiin)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;

  alsa_midiin->flags = 0;
  
  /* insert alsa_midiin mutex */
  g_rec_mutex_init(&(alsa_midiin->obj_mutex));

  /* uuid */
  alsa_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(alsa_midiin->uuid);

  /* config */
  config = ags_config_get_instance();

  /* sync flags */
  g_atomic_int_set(&(alsa_midiin->sync_flags),
		   (AGS_ALSA_MIDIIN_PASS_THROUGH));

  /* device */
  alsa_midiin->handle = NULL;
  alsa_midiin->device = g_strdup(AGS_ALSA_MIDIIN_DEFAULT_ALSA_DEVICE);

  /* app buffer */
  alsa_midiin->app_buffer_mode = AGS_ALSA_MIDIIN_APP_BUFFER_0;

  alsa_midiin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    alsa_midiin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(alsa_midiin->app_buffer_mutex[i]);
  }

  alsa_midiin->app_buffer = (char **) g_malloc(AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(char *));
  
  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    alsa_midiin->app_buffer[i] = NULL;

    alsa_midiin->app_buffer_size[i] = 0;
  }
  
  alsa_midiin->backend_buffer_mode = AGS_ALSA_MIDIIN_BACKEND_BUFFER_0;
  
  alsa_midiin->backend_buffer = (char **) g_malloc(AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(char *));

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    alsa_midiin->backend_buffer[i] = NULL;

    alsa_midiin->backend_buffer_size[i] = 0;
  }

  /* bpm */
  alsa_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  alsa_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  alsa_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    alsa_midiin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }
  
  alsa_midiin->latency = AGS_NSEC_PER_SEC / 4000.0;
  
  /* counters */
  alsa_midiin->start_note_offset = 0;
  alsa_midiin->note_offset = 0;
  alsa_midiin->note_offset_absolute = 0;

  alsa_midiin->tact_counter = 0.0;
  alsa_midiin->delay_counter = 0;
  alsa_midiin->tic_counter = 0;
}

void
ags_alsa_midiin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(gobject);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(alsa_midiin_mutex);

    g_free(alsa_midiin->device);
    
    alsa_midiin->device = g_strdup(device);

    g_rec_mutex_unlock(alsa_midiin_mutex);
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

    g_rec_mutex_lock(alsa_midiin_mutex);

    if(bpm == alsa_midiin->bpm){
      g_rec_mutex_unlock(alsa_midiin_mutex);

      return;
    }

    alsa_midiin->bpm = bpm;

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(alsa_midiin_mutex);

    if(delay_factor == alsa_midiin->delay_factor){
      g_rec_mutex_unlock(alsa_midiin_mutex);

      return;
    }

    alsa_midiin->delay_factor = delay_factor;

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_midiin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(gobject);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(alsa_midiin_mutex);

    g_value_set_string(value, alsa_midiin->device);

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(alsa_midiin_mutex);

    g_value_set_pointer(value, alsa_midiin->app_buffer);

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(alsa_midiin_mutex);

    g_value_set_double(value, alsa_midiin->bpm);

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(alsa_midiin_mutex);

    g_value_set_double(value, alsa_midiin->delay_factor);

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_midiin_dispose(GObject *gobject)
{
  AgsAlsaMidiin *alsa_midiin;

  alsa_midiin = AGS_ALSA_MIDIIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_alsa_midiin_parent_class)->dispose(gobject);
}

void
ags_alsa_midiin_finalize(GObject *gobject)
{
  AgsAlsaMidiin *alsa_midiin;

  guint i;
  
  alsa_midiin = AGS_ALSA_MIDIIN(gobject);

  ags_uuid_free(alsa_midiin->uuid);
  
  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(alsa_midiin->app_buffer[i]);
  }

  g_free(alsa_midiin->app_buffer);

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(alsa_midiin->backend_buffer[i]);
  }

  g_free(alsa_midiin->backend_buffer);

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(alsa_midiin->app_buffer_mutex[i]);
    
    g_free(alsa_midiin->app_buffer_mutex[i]);
  }

  g_free(alsa_midiin->app_buffer_mutex);
  
  g_free(alsa_midiin->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_alsa_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_alsa_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;
  
  AgsUUID *ptr;

  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  /* get alsa_midiin signal mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* get UUID */
  g_rec_mutex_lock(alsa_midiin_mutex);

  ptr = alsa_midiin->uuid;

  g_rec_mutex_unlock(alsa_midiin_mutex);
  
  return(ptr);
}

gboolean
ags_alsa_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_alsa_midiin_is_ready(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;
  
  gboolean is_ready;

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  /* check is added */
  is_ready = ags_alsa_midiin_test_flags(alsa_midiin, AGS_ALSA_MIDIIN_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_alsa_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  ags_alsa_midiin_set_flags(alsa_midiin, AGS_ALSA_MIDIIN_ADDED_TO_REGISTRY);
}

void
ags_alsa_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  ags_alsa_midiin_unset_flags(alsa_midiin, AGS_ALSA_MIDIIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_alsa_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_alsa_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_alsa_midiin_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_alsa_midiin_is_connected(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;
  
  gboolean is_connected;

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  /* check is connected */
  is_connected = ags_alsa_midiin_test_flags(alsa_midiin, AGS_ALSA_MIDIIN_CONNECTED);
  
  return(is_connected);
}

void
ags_alsa_midiin_connect(AgsConnectable *connectable)
{
  AgsAlsaMidiin *alsa_midiin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);

  ags_alsa_midiin_set_flags(alsa_midiin, AGS_ALSA_MIDIIN_CONNECTED);
}

void
ags_alsa_midiin_disconnect(AgsConnectable *connectable)
{

  AgsAlsaMidiin *alsa_midiin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_midiin = AGS_ALSA_MIDIIN(connectable);
  
  ags_alsa_midiin_unset_flags(alsa_midiin, AGS_ALSA_MIDIIN_CONNECTED);
}

/**
 * ags_alsa_midiin_test_flags:
 * @alsa_midiin: the #AgsAlsaMidiin
 * @flags: the flags
 *
 * Test @flags to be set on @alsa_midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.2
 */
gboolean
ags_alsa_midiin_test_flags(AgsAlsaMidiin *alsa_midiin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *alsa_midiin_mutex;

  if(!AGS_IS_ALSA_MIDIIN(alsa_midiin)){
    return(FALSE);
  }

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* test */
  g_rec_mutex_lock(alsa_midiin_mutex);

  retval = (flags & (alsa_midiin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(retval);
}

/**
 * ags_alsa_midiin_set_flags:
 * @alsa_midiin: the #AgsAlsaMidiin
 * @flags: see #AgsAlsaMidiinFlags-enum
 *
 * Enable a feature of @alsa_midiin.
 *
 * Since: 3.13.2
 */
void
ags_alsa_midiin_set_flags(AgsAlsaMidiin *alsa_midiin, guint flags)
{
  GRecMutex *alsa_midiin_mutex;

  if(!AGS_IS_ALSA_MIDIIN(alsa_midiin)){
    return;
  }

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->flags |= flags;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);
}
    
/**
 * ags_alsa_midiin_unset_flags:
 * @alsa_midiin: the #AgsAlsaMidiin
 * @flags: see #AgsAlsaMidiinFlags-enum
 *
 * Disable a feature of @alsa_midiin.
 *
 * Since: 3.13.2
 */
void
ags_alsa_midiin_unset_flags(AgsAlsaMidiin *alsa_midiin, guint flags)
{  
  GRecMutex *alsa_midiin_mutex;

  if(!AGS_IS_ALSA_MIDIIN(alsa_midiin)){
    return;
  }

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->flags &= (~flags);
  
  g_rec_mutex_unlock(alsa_midiin_mutex);
}

void
ags_alsa_midiin_set_device(AgsSequencer *sequencer,
			   gchar *device)
{
  AgsAlsaMidiin *alsa_midiin;
  
  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set device */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->device = g_strdup(device);
  
  g_rec_mutex_unlock(alsa_midiin_mutex);
}

gchar*
ags_alsa_midiin_get_device(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gchar *device;

  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);
  
  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  device = NULL;
  
  g_rec_mutex_lock(alsa_midiin_mutex);

  device = g_strdup(alsa_midiin->device);

  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(device);
}

void
ags_alsa_midiin_list_cards(AgsSequencer *sequencer,
			   GList **card_id, GList **card_name)
{
  AgsAlsaMidiin *alsa_midiin;

#if defined(AGS_WITH_ALSA)
  snd_ctl_t *card_handle;
  snd_ctl_card_info_t *card_info;
#endif
  
  char *name;
  gchar *str;

  int card_num;
  int device;
  int error;

  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(alsa_midiin_mutex);

#if defined(AGS_WITH_ALSA)
  card_num = -1;

  while(TRUE){
    char *iface;
    char **hints, **iter;

    error = snd_card_next(&card_num);

    if(card_num < 0){
      break;
    }

    if(error < 0){
      continue;
    }

    str = g_strdup_printf("hw:%i", card_num);
    error = snd_ctl_open(&card_handle, str, 0);

    if(error < 0){
      g_free(str);

      continue;
    }

    snd_ctl_card_info_alloca(&card_info);
    error = snd_ctl_card_info(card_handle, card_info);

    if(error < 0){
      snd_ctl_close(card_handle);
      g_free(str);
	
      continue;
    }

    device = -1;
    error = snd_ctl_rawmidi_next_device(card_handle, &device);
    
    if(error < 0){
      snd_ctl_close(card_handle);
      g_free(str);
	
      continue;
    }

    iface = "rawmidi";
    hints = NULL;
            
    error = snd_device_name_hint(card_num,
				 iface,
				 &hints);

    if(hints != NULL){
      for(iter = hints; iter[0] != NULL; iter++){
	if(card_id != NULL){
	  char *hint;

	  hint = snd_device_name_get_hint(iter[0],
					  "NAME");
	    
	  *card_id = g_list_prepend(*card_id,
				    g_strdup(hint));

	  if(hint != NULL){
	    free(hint);
	  }
	}

	if(card_name != NULL){
	  *card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));
	}
      }

      snd_device_name_free_hint(hints);
    }
      
    snd_ctl_close(card_handle);
  }

  snd_config_update_free_global();
#endif

  g_rec_mutex_unlock(alsa_midiin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

gboolean
ags_alsa_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gboolean is_starting;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* check is starting */
  g_rec_mutex_lock(alsa_midiin_mutex);

  is_starting = ((AGS_ALSA_MIDIIN_START_RECORD & (alsa_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_alsa_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gboolean is_recording;
  
  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);
  
  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* check is starting */
  g_rec_mutex_lock(alsa_midiin_mutex);

  is_recording = ((AGS_ALSA_MIDIIN_RECORD & (alsa_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(is_recording);
}

void
ags_alsa_midiin_device_record_init(AgsSequencer *sequencer,
				   GError **error)
{
  AgsAlsaMidiin *alsa_midiin;

#ifdef AGS_WITH_ALSA
  int mode = SND_RAWMIDI_NONBLOCK;
  snd_rawmidi_t* handle = NULL;
#endif

  int err;
  
  GRecMutex *alsa_midiin_mutex;
  
  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /*  */
  g_rec_mutex_lock(alsa_midiin_mutex);

  /* prepare for record */
  alsa_midiin->flags |= (AGS_ALSA_MIDIIN_START_RECORD |
			 AGS_ALSA_MIDIIN_RECORD |
			 AGS_ALSA_MIDIIN_NONBLOCKING);

#ifdef AGS_WITH_ALSA
  mode = SND_RAWMIDI_NONBLOCK;
  
  if((err = snd_rawmidi_open(&handle, NULL, alsa_midiin->device, mode)) < 0) {
    alsa_midiin->flags &= (~(AGS_ALSA_MIDIIN_START_RECORD |
			     AGS_ALSA_MIDIIN_RECORD |
			     AGS_ALSA_MIDIIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_midiin_mutex);

    printf("Record midi open error: %s\n", snd_strerror(err));
    g_set_error(error,
		AGS_ALSA_MIDIIN_ERROR,
		AGS_ALSA_MIDIIN_ERROR_LOCKED_SEQUENCER,
		"unable to open midi device: %s\n",
		snd_strerror(err));

    return;
  }
  
  /*  */
  alsa_midiin->handle = handle;
#endif

  alsa_midiin->tact_counter = 0.0;
  alsa_midiin->delay_counter = floor(alsa_midiin->delay);
  alsa_midiin->tic_counter = 0;

  alsa_midiin->backend_buffer_mode = AGS_ALSA_MIDIIN_BACKEND_BUFFER_0;  

#ifdef AGS_WITH_ALSA
  alsa_midiin->flags |= AGS_ALSA_MIDIIN_INITIALIZED;
#endif

  alsa_midiin->app_buffer_mode = AGS_ALSA_MIDIIN_APP_BUFFER_0;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);
}

void
ags_alsa_midiin_device_record(AgsSequencer *sequencer,
			      GError **error)
{
  AgsAlsaMidiin *alsa_midiin;

  AgsTicDevice *tic_device;
  //  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

#ifdef AGS_WITH_ALSA
  snd_rawmidi_t *device_handle;
#else
  gpointer device_handle;
#endif

  char **backend_buffer;
  
  gboolean no_event;
  guint app_buffer_mode;
  guint backend_buffer_mode;
  guint backend_buffer_size;
  int status;
  unsigned char c;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();
  
  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* prepare poll */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->flags &= (~AGS_ALSA_MIDIIN_START_RECORD);

  if((AGS_ALSA_MIDIIN_INITIALIZED & (alsa_midiin->flags)) == 0){
    g_rec_mutex_unlock(alsa_midiin_mutex);
     
    return;
  }

  device_handle = alsa_midiin->handle;      

  if(alsa_midiin->app_buffer_mode + 1 > AGS_ALSA_MIDIIN_APP_BUFFER_3){
    app_buffer_mode = AGS_ALSA_MIDIIN_APP_BUFFER_0;
  }else{
    app_buffer_mode = alsa_midiin->app_buffer_mode + 1;
  }

  if(alsa_midiin->backend_buffer_mode + 1 > AGS_ALSA_MIDIIN_BACKEND_BUFFER_7){
    backend_buffer_mode = AGS_ALSA_MIDIIN_BACKEND_BUFFER_0;
  }else{
    backend_buffer_mode = alsa_midiin->backend_buffer_mode + 1;
  }
  
  if(alsa_midiin->backend_buffer[backend_buffer_mode] != NULL){
    g_free(alsa_midiin->backend_buffer[backend_buffer_mode]);
  }
  
  alsa_midiin->backend_buffer[backend_buffer_mode] = NULL;
  alsa_midiin->backend_buffer_size[backend_buffer_mode] = 0;
      
  backend_buffer = alsa_midiin->backend_buffer;
  backend_buffer_size = alsa_midiin->backend_buffer_size[backend_buffer_mode];

  g_rec_mutex_unlock(alsa_midiin_mutex);

  /* poll MIDI device */
  status = 0;
  
  while(status >= 0){
    status = -1;
    
#ifdef AGS_WITH_ALSA
    status = snd_rawmidi_read(device_handle, &c, 1);

    if((status < 0) && (status != -EBUSY) && (status != -EAGAIN)){
      g_warning("Problem reading MIDI input: %s", snd_strerror(status));
    }

    if(status >= 0){
      if(backend_buffer_size % AGS_ALSA_MIDIIN_DEFAULT_BUFFER_SIZE == 0){
	if(backend_buffer[backend_buffer_mode] == NULL){
	  backend_buffer[backend_buffer_mode] = (char *) g_malloc(AGS_ALSA_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
	}else{
	  backend_buffer[backend_buffer_mode] = (char *) g_realloc(backend_buffer[backend_buffer_mode],
								   (backend_buffer_size + AGS_ALSA_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
	}
      }

      backend_buffer[backend_buffer_mode][backend_buffer_size] = (unsigned char) c;
      backend_buffer_size += 1;
    }
#endif
  }

  /* switch buffer */
  g_rec_mutex_lock(alsa_midiin_mutex);

  /* update byte array and buffer size */
  if(alsa_midiin->app_buffer[app_buffer_mode] != NULL){
    g_free(alsa_midiin->app_buffer[app_buffer_mode]);
  }

  alsa_midiin->app_buffer[app_buffer_mode] = NULL;
      
  alsa_midiin->backend_buffer_size[backend_buffer_mode] = backend_buffer_size;

  alsa_midiin->app_buffer_size[app_buffer_mode] = backend_buffer_size;

  /* fill buffer */
  if(backend_buffer_size > 0){
    alsa_midiin->app_buffer[app_buffer_mode] = (char *) g_malloc(backend_buffer_size * sizeof(char));
	
    memcpy(alsa_midiin->app_buffer[app_buffer_mode], backend_buffer[backend_buffer_mode], backend_buffer_size * sizeof(char));
  }
      
  g_rec_mutex_unlock(alsa_midiin_mutex);

  /*  */
  g_rec_mutex_lock(alsa_midiin_mutex);

  ags_alsa_midiin_switch_buffer_flag(alsa_midiin);

  g_rec_mutex_unlock(alsa_midiin_mutex);
  
  /* update sequencer */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;

  /* tic sequencer */
  tic_device = ags_tic_device_new((GObject *) alsa_midiin);
  task = g_list_append(task,
		       tic_device);
  
  /* reset - switch buffer flags */
  //  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) alsa_midiin);
  //  task = g_list_append(task,
  //		       switch_buffer_flag);

  /* add tasks all */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
}

void
ags_alsa_midiin_device_free(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  guint i;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /*  */
  g_rec_mutex_lock(alsa_midiin_mutex);

  if((AGS_ALSA_MIDIIN_INITIALIZED & (alsa_midiin->flags)) == 0){
    g_rec_mutex_unlock(alsa_midiin_mutex);
    
    return;
  }

  alsa_midiin->flags &= (~(AGS_ALSA_MIDIIN_RECORD |
			   AGS_ALSA_MIDIIN_INITIALIZED));

  g_rec_mutex_unlock(alsa_midiin_mutex);

  /*  */
  g_rec_mutex_lock(alsa_midiin_mutex);

#ifdef AGS_WITH_ALSA
  snd_rawmidi_close(alsa_midiin->handle);
#endif

  alsa_midiin->handle = NULL;

  alsa_midiin->app_buffer_mode = AGS_ALSA_MIDIIN_APP_BUFFER_0;

  alsa_midiin->backend_buffer_mode = AGS_ALSA_MIDIIN_BACKEND_BUFFER_0;

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(alsa_midiin->app_buffer[i]);

    alsa_midiin->app_buffer[i] = NULL;
    
    alsa_midiin->app_buffer_size[i] = 0;
  }

  for(i = 0; i < AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(alsa_midiin->backend_buffer[i]);

    alsa_midiin->backend_buffer[i] = NULL;

    alsa_midiin->backend_buffer_size[i] = 0;
  }

  g_atomic_int_or(&(alsa_midiin->sync_flags),
		  AGS_ALSA_MIDIIN_PASS_THROUGH);

  alsa_midiin->note_offset = alsa_midiin->start_note_offset;
  alsa_midiin->note_offset_absolute = alsa_midiin->start_note_offset;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);  
}

void
ags_alsa_midiin_tic(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  GRecMutex *alsa_midiin_mutex;

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);
  
  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(alsa_midiin_mutex);

  delay = alsa_midiin->delay;
  delay_counter = alsa_midiin->delay_counter;

  note_offset = alsa_midiin->note_offset;

  g_rec_mutex_unlock(alsa_midiin_mutex);

  if(delay_counter + 1.0 >= delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);

    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(alsa_midiin_mutex);

    alsa_midiin->delay_counter = delay_counter + 1.0 - delay;
    alsa_midiin->tact_counter += 1.0;

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }else{
    g_rec_mutex_lock(alsa_midiin_mutex);

    alsa_midiin->delay_counter += 1.0;

    g_rec_mutex_unlock(alsa_midiin_mutex);
  }
}

void
ags_alsa_midiin_offset_changed(AgsSequencer *sequencer,
			       guint note_offset)
{
  AgsAlsaMidiin *alsa_midiin;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* offset changed */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->tic_counter += 1;

  if(alsa_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    alsa_midiin->tic_counter = 0;
  }

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

void
ags_alsa_midiin_set_bpm(AgsSequencer *sequencer,
			gdouble bpm)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set bpm */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->bpm = bpm;

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

gdouble
ags_alsa_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gdouble bpm;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* get bpm */
  g_rec_mutex_lock(alsa_midiin_mutex);

  bpm = alsa_midiin->bpm;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(bpm);
}

void
ags_alsa_midiin_set_delay_factor(AgsSequencer *sequencer,
				 gdouble delay_factor)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set delay factor */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->delay_factor = delay_factor;

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

gdouble
ags_alsa_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  gdouble delay_factor;
  
  GRecMutex *alsa_midiin_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* get delay factor */
  g_rec_mutex_lock(alsa_midiin_mutex);

  delay_factor = alsa_midiin->delay_factor;
  
  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(delay_factor);
}

void*
ags_alsa_midiin_get_buffer(AgsSequencer *sequencer,
			   guint *buffer_length)
{
  AgsAlsaMidiin *alsa_midiin;

  char *buffer;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get buffer */
  buffer = alsa_midiin->app_buffer[alsa_midiin->app_buffer_mode];

  /* return the buffer's length */
  if(buffer_length != NULL){
    buffer_length[0] = alsa_midiin->app_buffer_size[alsa_midiin->app_buffer_mode];
  }
  
  return(buffer);
}

void*
ags_alsa_midiin_get_next_buffer(AgsSequencer *sequencer,
				guint *buffer_length)
{
  AgsAlsaMidiin *alsa_midiin;
  char *buffer;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get buffer */
  if(alsa_midiin->app_buffer_mode == AGS_ALSA_MIDIIN_APP_BUFFER_3){
    buffer = alsa_midiin->app_buffer[AGS_ALSA_MIDIIN_APP_BUFFER_0];
  }else{
    buffer = alsa_midiin->app_buffer[alsa_midiin->app_buffer_mode + 1];
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if(alsa_midiin->app_buffer_mode == AGS_ALSA_MIDIIN_APP_BUFFER_3){
      buffer_length[0] = alsa_midiin->app_buffer_size[AGS_ALSA_MIDIIN_APP_BUFFER_0];
    }else{
      buffer_length[0] = alsa_midiin->app_buffer_size[alsa_midiin->app_buffer_mode + 1];
    }
  }
  
  return(buffer);
}

void
ags_alsa_midiin_lock_buffer(AgsSequencer *sequencer,
			    void *buffer)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *buffer_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(alsa_midiin->app_buffer != NULL){
    if(buffer == alsa_midiin->app_buffer[0]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[0];
    }else if(buffer == alsa_midiin->app_buffer[1]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[1];
    }else if(buffer == alsa_midiin->app_buffer[2]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[2];
    }else if(buffer == alsa_midiin->app_buffer[3]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_alsa_midiin_unlock_buffer(AgsSequencer *sequencer,
			      void *buffer)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *buffer_mutex;
  
  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(alsa_midiin->app_buffer != NULL){
    if(buffer == alsa_midiin->app_buffer[0]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[0];
    }else if(buffer == alsa_midiin->app_buffer[1]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[1];
    }else if(buffer == alsa_midiin->app_buffer[2]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[2];
    }else if(buffer == alsa_midiin->app_buffer[3]){
      buffer_mutex = alsa_midiin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

void
ags_alsa_midiin_set_start_note_offset(AgsSequencer *sequencer,
				      guint start_note_offset)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;  

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set note offset */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

guint
ags_alsa_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  guint start_note_offset;
  
  GRecMutex *alsa_midiin_mutex;  

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set note offset */
  g_rec_mutex_lock(alsa_midiin_mutex);

  start_note_offset = alsa_midiin->start_note_offset;

  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(start_note_offset);
}

void
ags_alsa_midiin_set_note_offset(AgsSequencer *sequencer,
				guint note_offset)
{
  AgsAlsaMidiin *alsa_midiin;

  GRecMutex *alsa_midiin_mutex;  

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set note offset */
  g_rec_mutex_lock(alsa_midiin_mutex);

  alsa_midiin->note_offset = note_offset;

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

guint
ags_alsa_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsAlsaMidiin *alsa_midiin;

  guint note_offset;
  
  GRecMutex *alsa_midiin_mutex;  

  alsa_midiin = AGS_ALSA_MIDIIN(sequencer);

  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* set note offset */
  g_rec_mutex_lock(alsa_midiin_mutex);

  note_offset = alsa_midiin->note_offset;

  g_rec_mutex_unlock(alsa_midiin_mutex);

  return(note_offset);
}

/**
 * ags_alsa_midiin_switch_buffer_flag:
 * @alsa_midiin: the #AgsAlsaMidiin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.13.2
 */
void
ags_alsa_midiin_switch_buffer_flag(AgsAlsaMidiin *alsa_midiin)
{
  GRecMutex *alsa_midiin_mutex;  

  if(!AGS_IS_ALSA_MIDIIN(alsa_midiin)){
    return;
  }
  
  /* get alsa_midiin mutex */
  alsa_midiin_mutex = AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(alsa_midiin);

  /* switch buffer flag */
  g_rec_mutex_lock(alsa_midiin_mutex);

  if(alsa_midiin->app_buffer_mode < AGS_ALSA_MIDIIN_APP_BUFFER_3){
    alsa_midiin->app_buffer_mode += 1;
  }else{
    alsa_midiin->app_buffer_mode = AGS_ALSA_MIDIIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(alsa_midiin_mutex);
}

/**
 * ags_alsa_midiin_new:
 *
 * Creates a new instance of #AgsAlsaMidiin.
 *
 * Returns: the new #AgsAlsaMidiin
 *
 * Since: 3.13.2
 */
AgsAlsaMidiin*
ags_alsa_midiin_new()
{
  AgsAlsaMidiin *alsa_midiin;

  alsa_midiin = (AgsAlsaMidiin *) g_object_new(AGS_TYPE_ALSA_MIDIIN,
					       NULL);
  
  return(alsa_midiin);
}
