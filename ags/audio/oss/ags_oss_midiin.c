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

#include <ags/audio/oss/ags_oss_midiin.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef AGS_W32API
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef __APPLE__
#include <sys/soundcard.h>
#endif

#include <sys/utsname.h>
#endif

#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

/**
 * SECTION:ags_oss_midiin
 * @short_description: Input from sequencer
 * @title: AgsOssMidiin
 * @section_id:
 * @include: ags/audio/ags_oss_midiin.h
 *
 * #AgsOssMidiin represents a sequencer and supports midi input.
 */

void ags_oss_midiin_class_init(AgsOssMidiinClass *oss_midiin);
void ags_oss_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_oss_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_oss_midiin_init(AgsOssMidiin *oss_midiin);
void ags_oss_midiin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_oss_midiin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_oss_midiin_dispose(GObject *gobject);
void ags_oss_midiin_finalize(GObject *gobject);

AgsUUID* ags_oss_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_oss_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_oss_midiin_is_ready(AgsConnectable *connectable);
void ags_oss_midiin_add_to_registry(AgsConnectable *connectable);
void ags_oss_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_oss_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_oss_midiin_xml_compose(AgsConnectable *connectable);
void ags_oss_midiin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_oss_midiin_is_connected(AgsConnectable *connectable);
void ags_oss_midiin_connect(AgsConnectable *connectable);
void ags_oss_midiin_disconnect(AgsConnectable *connectable);

void ags_oss_midiin_set_device(AgsSequencer *sequencer,
			       gchar *device);
gchar* ags_oss_midiin_get_device(AgsSequencer *sequencer);

void ags_oss_midiin_list_cards(AgsSequencer *sequencer,
			       GList **card_id, GList **card_name);

gboolean ags_oss_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_oss_midiin_is_recording(AgsSequencer *sequencer);

void ags_oss_midiin_device_record_init(AgsSequencer *sequencer,
				       GError **error);
void ags_oss_midiin_device_record(AgsSequencer *sequencer,
				  GError **error);
void ags_oss_midiin_device_free(AgsSequencer *sequencer);

void ags_oss_midiin_tic(AgsSequencer *sequencer);
void ags_oss_midiin_offset_changed(AgsSequencer *sequencer,
				   guint note_offset);

void ags_oss_midiin_set_bpm(AgsSequencer *sequencer,
			    gdouble bpm);
gdouble ags_oss_midiin_get_bpm(AgsSequencer *sequencer);

void ags_oss_midiin_set_delay_factor(AgsSequencer *sequencer,
				     gdouble delay_factor);
gdouble ags_oss_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_oss_midiin_get_buffer(AgsSequencer *sequencer,
				guint *buffer_length);
void* ags_oss_midiin_get_next_buffer(AgsSequencer *sequencer,
				     guint *buffer_length);

void ags_oss_midiin_lock_buffer(AgsSequencer *sequencer,
				void *buffer);
void ags_oss_midiin_unlock_buffer(AgsSequencer *sequencer,
				  void *buffer);

void ags_oss_midiin_set_start_note_offset(AgsSequencer *sequencer,
					  guint start_note_offset);
guint ags_oss_midiin_get_start_note_offset(AgsSequencer *sequencer);

void ags_oss_midiin_set_note_offset(AgsSequencer *sequencer,
				    guint note_offset);
guint ags_oss_midiin_get_note_offset(AgsSequencer *sequencer);

enum{
  PROP_0,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
};

static gpointer ags_oss_midiin_parent_class = NULL;

GType
ags_oss_midiin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_oss_midiin = 0;

    static const GTypeInfo ags_oss_midiin_info = {
      sizeof(AgsOssMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_oss_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOssMidiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_oss_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_oss_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_oss_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_oss_midiin = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOssMidiin",
						 &ags_oss_midiin_info,
						 0);

    g_type_add_interface_static(ags_type_oss_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_oss_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_oss_midiin);
  }

  return g_define_type_id__volatile;
}

void
ags_oss_midiin_class_init(AgsOssMidiinClass *oss_midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_oss_midiin_parent_class = g_type_class_peek_parent(oss_midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) oss_midiin;

  gobject->set_property = ags_oss_midiin_set_property;
  gobject->get_property = ags_oss_midiin_get_property;

  gobject->dispose = ags_oss_midiin_dispose;
  gobject->finalize = ags_oss_midiin_finalize;

  /* properties */
  /**
   * AgsOssMidiin:device:
   *
   * The OSS sequencer indentifier
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_OSS_MIDIIN_DEFAULT_OSS_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsOssMidiin:buffer:
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
   * AgsOssMidiin:bpm:
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
   * AgsOssMidiin:delay-factor:
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
   * AgsOssMidiin:attack:
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

  /* AgsOssMidiinClass */
}

GType
ags_oss_midiin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_OSS_MIDIIN_INITIALIZED, "AGS_OSS_MIDIIN_INITIALIZED", "oss-midiin-initialized" },
      { AGS_OSS_MIDIIN_START_RECORD, "AGS_OSS_MIDIIN_START_RECORD", "oss-midiin-start-record" },
      { AGS_OSS_MIDIIN_RECORD, "AGS_OSS_MIDIIN_RECORD", "oss-midiin-record" },
      { AGS_OSS_MIDIIN_SHUTDOWN, "AGS_OSS_MIDIIN_SHUTDOWN", "oss-midiin-shutdown" },
      { AGS_OSS_MIDIIN_NONBLOCKING, "AGS_OSS_MIDIIN_NONBLOCKING", "oss-midiin-nonblocking" },
      { AGS_OSS_MIDIIN_ATTACK_FIRST, "AGS_OSS_MIDIIN_ATTACK_FIRST", "oss-midiin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOssMidiinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GQuark
ags_oss_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-oss-midiin-error-quark"));
}

void
ags_oss_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_oss_midiin_get_uuid;
  connectable->has_resource = ags_oss_midiin_has_resource;

  connectable->is_ready = ags_oss_midiin_is_ready;
  connectable->add_to_registry = ags_oss_midiin_add_to_registry;
  connectable->remove_from_registry = ags_oss_midiin_remove_from_registry;

  connectable->list_resource = ags_oss_midiin_list_resource;
  connectable->xml_compose = ags_oss_midiin_xml_compose;
  connectable->xml_parse = ags_oss_midiin_xml_parse;

  connectable->is_connected = ags_oss_midiin_is_connected;  
  connectable->connect = ags_oss_midiin_connect;
  connectable->disconnect = ags_oss_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_oss_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_device = ags_oss_midiin_set_device;
  sequencer->get_device = ags_oss_midiin_get_device;

  sequencer->list_cards = ags_oss_midiin_list_cards;

  sequencer->is_starting =  ags_oss_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_oss_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_oss_midiin_device_record_init;
  sequencer->record = ags_oss_midiin_device_record;
  
  sequencer->stop = ags_oss_midiin_device_free;

  sequencer->tic = ags_oss_midiin_tic;
  sequencer->offset_changed = ags_oss_midiin_offset_changed;
    
  sequencer->set_bpm = ags_oss_midiin_set_bpm;
  sequencer->get_bpm = ags_oss_midiin_get_bpm;

  sequencer->set_delay_factor = ags_oss_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_oss_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_oss_midiin_get_buffer;
  sequencer->get_next_buffer = ags_oss_midiin_get_next_buffer;

  sequencer->lock_buffer = ags_oss_midiin_lock_buffer;
  sequencer->unlock_buffer = ags_oss_midiin_unlock_buffer;

  sequencer->set_start_note_offset = ags_oss_midiin_set_start_note_offset;
  sequencer->get_start_note_offset = ags_oss_midiin_get_start_note_offset;

  sequencer->set_note_offset = ags_oss_midiin_set_note_offset;
  sequencer->get_note_offset = ags_oss_midiin_get_note_offset;
}

void
ags_oss_midiin_init(AgsOssMidiin *oss_midiin)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;

  oss_midiin->flags = 0;
  oss_midiin->connectable_flags = 0;
  
  /* insert oss_midiin mutex */
  g_rec_mutex_init(&(oss_midiin->obj_mutex));

  /* uuid */
  oss_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(oss_midiin->uuid);

  /* config */
  config = ags_config_get_instance();

  /* sync flags */
  g_atomic_int_set(&(oss_midiin->sync_flags),
		   (AGS_OSS_MIDIIN_PASS_THROUGH));

  /* device */
  oss_midiin->device_fd = -1;
  oss_midiin->device = g_strdup(AGS_OSS_MIDIIN_DEFAULT_OSS_DEVICE);

  /* app buffer */
  oss_midiin->app_buffer_mode = AGS_OSS_MIDIIN_APP_BUFFER_0;

  oss_midiin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_midiin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(oss_midiin->app_buffer_mutex[i]);
  }

  oss_midiin->app_buffer = (char **) g_malloc(AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(char *));
  
  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_midiin->app_buffer[i] = NULL;

    oss_midiin->app_buffer_size[i] = 0;
  }
  
  oss_midiin->backend_buffer_mode = AGS_OSS_MIDIIN_BACKEND_BUFFER_0;
  
  oss_midiin->backend_buffer = (char **) g_malloc(AGS_OSS_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(char *));

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    oss_midiin->backend_buffer[i] = NULL;

    oss_midiin->backend_buffer_size[i] = 0;
  }

  /* bpm */
  oss_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  oss_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  oss_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    oss_midiin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }
  
  oss_midiin->latency = AGS_NSEC_PER_SEC / 4000.0;
  
  /* counters */
  oss_midiin->start_note_offset = 0;
  oss_midiin->note_offset = 0;
  oss_midiin->note_offset_absolute = 0;

  oss_midiin->tact_counter = 0.0;
  oss_midiin->delay_counter = 0;
  oss_midiin->tic_counter = 0;
}

void
ags_oss_midiin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(gobject);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(oss_midiin_mutex);

    g_free(oss_midiin->device);
    
    oss_midiin->device = g_strdup(device);

    g_rec_mutex_unlock(oss_midiin_mutex);
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

    g_rec_mutex_lock(oss_midiin_mutex);

    if(bpm == oss_midiin->bpm){
      g_rec_mutex_unlock(oss_midiin_mutex);

      return;
    }

    oss_midiin->bpm = bpm;

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(oss_midiin_mutex);

    if(delay_factor == oss_midiin->delay_factor){
      g_rec_mutex_unlock(oss_midiin_mutex);

      return;
    }

    oss_midiin->delay_factor = delay_factor;

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_oss_midiin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(gobject);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(oss_midiin_mutex);

    g_value_set_string(value, oss_midiin->device);

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(oss_midiin_mutex);

    g_value_set_pointer(value, oss_midiin->app_buffer);

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(oss_midiin_mutex);

    g_value_set_double(value, oss_midiin->bpm);

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(oss_midiin_mutex);

    g_value_set_double(value, oss_midiin->delay_factor);

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_oss_midiin_dispose(GObject *gobject)
{
  AgsOssMidiin *oss_midiin;

  oss_midiin = AGS_OSS_MIDIIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_oss_midiin_parent_class)->dispose(gobject);
}

void
ags_oss_midiin_finalize(GObject *gobject)
{
  AgsOssMidiin *oss_midiin;

  guint i;
  
  oss_midiin = AGS_OSS_MIDIIN(gobject);

  ags_uuid_free(oss_midiin->uuid);
  
  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(oss_midiin->app_buffer[i]);
  }

  g_free(oss_midiin->app_buffer);

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(oss_midiin->backend_buffer[i]);
  }

  g_free(oss_midiin->backend_buffer);

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(oss_midiin->app_buffer_mutex[i]);
    
    g_free(oss_midiin->app_buffer_mutex[i]);
  }

  g_free(oss_midiin->app_buffer_mutex);
  
  g_free(oss_midiin->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_oss_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_oss_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;
  
  AgsUUID *ptr;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss_midiin signal mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* get UUID */
  g_rec_mutex_lock(oss_midiin_mutex);

  ptr = oss_midiin->uuid;

  g_rec_mutex_unlock(oss_midiin_mutex);
  
  return(ptr);
}

gboolean
ags_oss_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_oss_midiin_is_ready(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;
  
  gboolean is_ready;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* check is ready */
  g_rec_mutex_lock(oss_midiin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (oss_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_midiin_mutex);
  
  return(is_ready);
}

void
ags_oss_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

void
ags_oss_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

xmlNode*
ags_oss_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_oss_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_oss_midiin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_oss_midiin_is_connected(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;
  
  gboolean is_connected;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* check is connected */
  g_rec_mutex_lock(oss_midiin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (oss_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_midiin_mutex);
  
  return(is_connected);
}

void
ags_oss_midiin_connect(AgsConnectable *connectable)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

void
ags_oss_midiin_disconnect(AgsConnectable *connectable)
{

  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  oss_midiin = AGS_OSS_MIDIIN(connectable);

  /* get oss midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

/**
 * ags_oss_midiin_test_flags:
 * @oss_midiin: the #AgsOssMidiin
 * @flags: the flags
 *
 * Test @flags to be set on @oss_midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.2
 */
gboolean
ags_oss_midiin_test_flags(AgsOssMidiin *oss_midiin, AgsOssMidiinFlags flags)
{
  gboolean retval;  
  
  GRecMutex *oss_midiin_mutex;

  if(!AGS_IS_OSS_MIDIIN(oss_midiin)){
    return(FALSE);
  }

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* test */
  g_rec_mutex_lock(oss_midiin_mutex);

  retval = (flags & (oss_midiin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(oss_midiin_mutex);

  return(retval);
}

/**
 * ags_oss_midiin_set_flags:
 * @oss_midiin: the #AgsOssMidiin
 * @flags: see #AgsOssMidiinFlags-enum
 *
 * Enable a feature of @oss_midiin.
 *
 * Since: 3.13.2
 */
void
ags_oss_midiin_set_flags(AgsOssMidiin *oss_midiin, AgsOssMidiinFlags flags)
{
  GRecMutex *oss_midiin_mutex;

  if(!AGS_IS_OSS_MIDIIN(oss_midiin)){
    return;
  }

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->flags |= flags;
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}
    
/**
 * ags_oss_midiin_unset_flags:
 * @oss_midiin: the #AgsOssMidiin
 * @flags: see #AgsOssMidiinFlags-enum
 *
 * Disable a feature of @oss_midiin.
 *
 * Since: 3.13.2
 */
void
ags_oss_midiin_unset_flags(AgsOssMidiin *oss_midiin, AgsOssMidiinFlags flags)
{  
  GRecMutex *oss_midiin_mutex;

  if(!AGS_IS_OSS_MIDIIN(oss_midiin)){
    return;
  }

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->flags &= (~flags);
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

void
ags_oss_midiin_set_device(AgsSequencer *sequencer,
			  gchar *device)
{
  AgsOssMidiin *oss_midiin;
  
  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set device */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->device = g_strdup(device);
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

gchar*
ags_oss_midiin_get_device(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gchar *device;

  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);
  
  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  device = NULL;
  
  g_rec_mutex_lock(oss_midiin_mutex);

  device = g_strdup(oss_midiin->device);

  g_rec_mutex_unlock(oss_midiin_mutex);

  return(device);
}

void
ags_oss_midiin_list_cards(AgsSequencer *sequencer,
			  GList **card_id, GList **card_name)
{
  AgsOssMidiin *oss_midiin;

#if defined(AGS_WITH_OSS)
  oss_sysinfo sysinfo;
  oss_midi_info mi;
#endif
  
  char *mixer_device;
    
  int mixerfd = -1;

  int n;
  int i;

  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(oss_midiin_mutex);

#if defined(AGS_WITH_OSS)
  if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
    mixer_device = AGS_OSS_MIDIIN_DEFAULT_OSS_MIXER_DEVICE;
  }

  if((mixerfd = open(mixer_device, O_RDONLY, 0)) == -1){
    int e = errno;
      
    switch(e){
    case ENXIO:
    case ENODEV:
    {
      g_warning("Open Sound System is not running in your system.");
    }
    break;
    case ENOENT:
    {
      g_warning("No %s device available in your system.\nPerhaps Open Sound System is not installed or running.", mixer_device);
    }
    break;  
    default:
      g_warning("%s", strerror(e));
    }
  }
      
  if(ioctl(mixerfd, SNDCTL_SYSINFO, &sysinfo) == -1){
    if(errno == ENXIO){
      g_warning("OSS has not detected any supported sound hardware in your system.");
    }else{
      g_warning("SNDCTL_SYSINFO");

      if(errno == EINVAL){
	g_warning("Error: OSS version 4.0 or later is required");
      }
    }

    n = 0;
  }else{
    n = sysinfo.nummidis;
  }

  memset(&mi, 0, sizeof(oss_midi_info));
  ioctl(mixerfd, SNDCTL_MIDI_INFO, &mi);

  for(i = 0; i < n; i++){
    mi.dev = i;

    if(ioctl(mixerfd, SNDCTL_ENGINEINFO, &mi) == -1){
      int e = errno;
	
      g_warning("Can't get device info for /dev/midi%2d (SNDCTL_AUDIOINFO)\nerrno = %d: %s", i, e, strerror(e));
	
      continue;
    }
      
    if((MIDI_CAP_INPUT & (mi.caps)) != 0){
      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id,
				  g_strdup_printf("/dev/midi2%i", i));
      }
	
      if(card_name != NULL){
	*card_name = g_list_prepend(*card_name,
				    g_strdup(mi.name));
      }
    }
  }
#endif

  g_rec_mutex_unlock(oss_midiin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

gboolean
ags_oss_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gboolean is_starting;
  
  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* check is starting */
  g_rec_mutex_lock(oss_midiin_mutex);

  is_starting = ((AGS_OSS_MIDIIN_START_RECORD & (oss_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_oss_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gboolean is_recording;
  
  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(sequencer);
  
  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* check is starting */
  g_rec_mutex_lock(oss_midiin_mutex);

  is_recording = ((AGS_OSS_MIDIIN_RECORD & (oss_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_midiin_mutex);

  return(is_recording);
}

void
ags_oss_midiin_device_record_init(AgsSequencer *sequencer,
				  GError **error)
{
  AgsOssMidiin *oss_midiin;
  
  gchar *str;

  guint word_size;
  int format;
  int tmp;

  GRecMutex *oss_midiin_mutex;

  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* prepare for playback */
  g_rec_mutex_lock(oss_midiin_mutex);
    
  oss_midiin->flags |= (AGS_OSS_MIDIIN_START_RECORD |
			AGS_OSS_MIDIIN_RECORD |
			AGS_OSS_MIDIIN_NONBLOCKING);

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = oss_midiin->device;
  oss_midiin->device_fd = open(str, (O_RDONLY | O_NONBLOCK), 0);

  if(oss_midiin->device_fd == -1){
    oss_midiin->flags &= (~(AGS_OSS_MIDIIN_START_RECORD |
			    AGS_OSS_MIDIIN_RECORD |
			    AGS_OSS_MIDIIN_NONBLOCKING));
    
    g_rec_mutex_unlock(oss_midiin_mutex);

    g_warning("couldn't open device %s", oss_midiin->device);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_MIDIIN_ERROR,
		  AGS_OSS_MIDIIN_ERROR_LOCKED_SEQUENCER,
		  "unable to open MIDI device: %s\n",
		  str);
    }

    return;
  }
#endif
  
  oss_midiin->tact_counter = 0.0;
  oss_midiin->delay_counter = 0.0;
  oss_midiin->tic_counter = 0;

  oss_midiin->backend_buffer_mode = AGS_OSS_MIDIIN_BACKEND_BUFFER_0;  

#ifdef AGS_WITH_OSS
  oss_midiin->flags |= AGS_OSS_MIDIIN_INITIALIZED;
#endif

  oss_midiin->app_buffer_mode = AGS_OSS_MIDIIN_APP_BUFFER_0;
  
  g_rec_mutex_unlock(oss_midiin_mutex);
}

void
ags_oss_midiin_device_record(AgsSequencer *sequencer,
			     GError **error)
{
  AgsOssMidiin *oss_midiin;

  AgsTicDevice *tic_device;
  //  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  char **backend_buffer;
  char buf[128];
  
  gboolean no_event;
  guint app_buffer_mode;
  guint backend_buffer_mode;
  guint backend_buffer_size;
  int device_fd;
  int num_read;
  
  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* poll MIDI device */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->flags &= (~AGS_OSS_MIDIIN_START_RECORD);

  if((AGS_OSS_MIDIIN_INITIALIZED & (oss_midiin->flags)) == 0){
    g_rec_mutex_unlock(oss_midiin_mutex);
    
    return;
  }

  device_fd = oss_midiin->device_fd;

  if(oss_midiin->app_buffer_mode + 1 > AGS_OSS_MIDIIN_APP_BUFFER_3){
    app_buffer_mode = AGS_OSS_MIDIIN_APP_BUFFER_0;
  }else{
    app_buffer_mode = oss_midiin->app_buffer_mode + 1;
  }

  if(oss_midiin->backend_buffer_mode + 1 > AGS_OSS_MIDIIN_BACKEND_BUFFER_7){
    backend_buffer_mode = AGS_OSS_MIDIIN_BACKEND_BUFFER_0;
  }else{
    backend_buffer_mode = oss_midiin->backend_buffer_mode + 1;
  }
  
  if(oss_midiin->backend_buffer[backend_buffer_mode] != NULL){
    g_free(oss_midiin->backend_buffer[backend_buffer_mode]);
  }

  oss_midiin->backend_buffer[backend_buffer_mode] = NULL;
  oss_midiin->backend_buffer_size[backend_buffer_mode] = 0;
      
  backend_buffer = oss_midiin->backend_buffer;
  backend_buffer_size = oss_midiin->backend_buffer_size[backend_buffer_mode];
  
  g_rec_mutex_unlock(oss_midiin_mutex);

  num_read = 1;
  
  while(num_read > 0){
    num_read = 0;
    
#ifdef AGS_WITH_OSS
    num_read = read(device_fd, buf, sizeof(buf));
    
    if((num_read < 0)){
      // g_warning("Problem reading MIDI input");
    }

    if(num_read > 0){
      if(backend_buffer[backend_buffer_mode] == NULL ||
	 ceil((backend_buffer_size + num_read) / AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE) > ceil(backend_buffer_size / AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE)){
	if(backend_buffer[backend_buffer_mode] == NULL){
	  backend_buffer[backend_buffer_mode] = (char *) g_malloc(AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
	}else{
	  backend_buffer[backend_buffer_mode] = (char *) g_realloc(backend_buffer[backend_buffer_mode],
								   (AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE * ceil(backend_buffer_size / AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE) + AGS_OSS_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
	}
      }

      memcpy(&(backend_buffer[backend_buffer_mode][backend_buffer_size]),
	     buf,
	     num_read);
      backend_buffer_size += num_read;
    }
#endif
  }
      
  /* switch buffer */
  g_rec_mutex_lock(oss_midiin_mutex);

  /* update byte array and buffer size */
  if(oss_midiin->app_buffer[app_buffer_mode] != NULL){
    g_free(oss_midiin->app_buffer[app_buffer_mode]);
  }

  oss_midiin->app_buffer[app_buffer_mode] = NULL;
      
  oss_midiin->backend_buffer_size[backend_buffer_mode] = backend_buffer_size;

  oss_midiin->app_buffer_size[app_buffer_mode] = backend_buffer_size;

  /* fill buffer */
  if(backend_buffer_size > 0){
    oss_midiin->app_buffer[app_buffer_mode] = (char *) g_malloc(backend_buffer_size * sizeof(char));
	
    memcpy(oss_midiin->app_buffer[app_buffer_mode], backend_buffer[backend_buffer_mode], backend_buffer_size * sizeof(char));
  }
      
  g_rec_mutex_unlock(oss_midiin_mutex);

  g_rec_mutex_lock(oss_midiin_mutex);

  ags_oss_midiin_switch_buffer_flag(oss_midiin);
      
  g_rec_mutex_unlock(oss_midiin_mutex);

  /* update sequencer */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
  task = NULL;

  /* tic sequencer */
  tic_device = ags_tic_device_new((GObject *) oss_midiin);
  task = g_list_append(task,
		       tic_device);
  
  /* reset - switch buffer flags */
  //  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) oss_midiin);
  //  task = g_list_append(task,
  //		       switch_buffer_flag);

  /* add tasks all */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  g_object_unref(task_launcher);
}

void
ags_oss_midiin_device_free(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  guint i;

  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  g_rec_mutex_lock(oss_midiin_mutex);  

  if((AGS_OSS_MIDIIN_INITIALIZED & (oss_midiin->flags)) == 0){
    g_rec_mutex_unlock(oss_midiin_mutex);  

    return;
  }

  oss_midiin->flags &= (~(AGS_OSS_MIDIIN_RECORD |
			  AGS_OSS_MIDIIN_INITIALIZED));
  
  g_rec_mutex_unlock(oss_midiin_mutex);  

  /*  */
  g_rec_mutex_lock(oss_midiin_mutex);  

  close(oss_midiin->device_fd);
  oss_midiin->device_fd = -1;

  oss_midiin->app_buffer_mode = AGS_OSS_MIDIIN_APP_BUFFER_0;

  oss_midiin->backend_buffer_mode = AGS_OSS_MIDIIN_BACKEND_BUFFER_0;

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(oss_midiin->app_buffer[i]);

    oss_midiin->app_buffer[i] = NULL;
    
    oss_midiin->app_buffer_size[i] = 0;
  }

  for(i = 0; i < AGS_OSS_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(oss_midiin->backend_buffer[i]);

    oss_midiin->backend_buffer[i] = NULL;

    oss_midiin->backend_buffer_size[i] = 0;
  }

  g_atomic_int_or(&(oss_midiin->sync_flags),
		  AGS_OSS_MIDIIN_PASS_THROUGH);
  
  oss_midiin->note_offset = oss_midiin->start_note_offset;
  oss_midiin->note_offset_absolute = oss_midiin->start_note_offset;
  
  g_rec_mutex_unlock(oss_midiin_mutex);  
}  

void
ags_oss_midiin_tic(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  GRecMutex *oss_midiin_mutex;

  oss_midiin = AGS_OSS_MIDIIN(sequencer);
  
  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(oss_midiin_mutex);

  delay = oss_midiin->delay;
  delay_counter = oss_midiin->delay_counter;

  note_offset = oss_midiin->note_offset;

  g_rec_mutex_unlock(oss_midiin_mutex);

  if(delay_counter + 1.0 >= delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);

    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(oss_midiin_mutex);

    oss_midiin->delay_counter = delay_counter + 1.0 - delay;
    oss_midiin->tact_counter += 1.0;

    g_rec_mutex_unlock(oss_midiin_mutex);
  }else{
    g_rec_mutex_lock(oss_midiin_mutex);

    oss_midiin->delay_counter += 1.0;

    g_rec_mutex_unlock(oss_midiin_mutex);
  }
}

void
ags_oss_midiin_offset_changed(AgsSequencer *sequencer,
			      guint note_offset)
{
  AgsOssMidiin *oss_midiin;
  
  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* offset changed */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->tic_counter += 1;

  if(oss_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    oss_midiin->tic_counter = 0;
  }

  g_rec_mutex_unlock(oss_midiin_mutex);
}

void
ags_oss_midiin_set_bpm(AgsSequencer *sequencer,
		       gdouble bpm)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set bpm */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->bpm = bpm;

  g_rec_mutex_unlock(oss_midiin_mutex);
}

gdouble
ags_oss_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gdouble bpm;
  
  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* get bpm */
  g_rec_mutex_lock(oss_midiin_mutex);

  bpm = oss_midiin->bpm;
  
  g_rec_mutex_unlock(oss_midiin_mutex);

  return(bpm);
}

void
ags_oss_midiin_set_delay_factor(AgsSequencer *sequencer,
				gdouble delay_factor)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set delay factor */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->delay_factor = delay_factor;

  g_rec_mutex_unlock(oss_midiin_mutex);
}

gdouble
ags_oss_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  gdouble delay_factor;
  
  GRecMutex *oss_midiin_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* get delay factor */
  g_rec_mutex_lock(oss_midiin_mutex);

  delay_factor = oss_midiin->delay_factor;
  
  g_rec_mutex_unlock(oss_midiin_mutex);

  return(delay_factor);
}

void*
ags_oss_midiin_get_buffer(AgsSequencer *sequencer,
			  guint *buffer_length)
{
  AgsOssMidiin *oss_midiin;

  char *buffer;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get buffer */
  buffer = oss_midiin->app_buffer[oss_midiin->app_buffer_mode];

  /* return the buffer's length */
  if(buffer_length != NULL){
    buffer_length[0] = oss_midiin->app_buffer_size[oss_midiin->app_buffer_mode];
  }
  
  return(buffer);
}

void*
ags_oss_midiin_get_next_buffer(AgsSequencer *sequencer,
			       guint *buffer_length)
{
  AgsOssMidiin *oss_midiin;
  char *buffer;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get buffer */
  if(oss_midiin->app_buffer_mode == AGS_OSS_MIDIIN_APP_BUFFER_3){
    buffer = oss_midiin->app_buffer[AGS_OSS_MIDIIN_APP_BUFFER_0];
  }else{
    buffer = oss_midiin->app_buffer[oss_midiin->app_buffer_mode + 1];
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if(oss_midiin->app_buffer_mode == AGS_OSS_MIDIIN_APP_BUFFER_3){
      buffer_length[0] = oss_midiin->app_buffer_size[AGS_OSS_MIDIIN_APP_BUFFER_0];
    }else{
      buffer_length[0] = oss_midiin->app_buffer_size[oss_midiin->app_buffer_mode + 1];
    }
  }
  
  return(buffer);
}

void
ags_oss_midiin_lock_buffer(AgsSequencer *sequencer,
			   void *buffer)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *buffer_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(oss_midiin->app_buffer != NULL){
    if(buffer == oss_midiin->app_buffer[0]){
      buffer_mutex = oss_midiin->app_buffer_mutex[0];
    }else if(buffer == oss_midiin->app_buffer[1]){
      buffer_mutex = oss_midiin->app_buffer_mutex[1];
    }else if(buffer == oss_midiin->app_buffer[2]){
      buffer_mutex = oss_midiin->app_buffer_mutex[2];
    }else if(buffer == oss_midiin->app_buffer[3]){
      buffer_mutex = oss_midiin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_oss_midiin_unlock_buffer(AgsSequencer *sequencer,
			     void *buffer)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *buffer_mutex;
  
  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(oss_midiin->app_buffer != NULL){
    if(buffer == oss_midiin->app_buffer[0]){
      buffer_mutex = oss_midiin->app_buffer_mutex[0];
    }else if(buffer == oss_midiin->app_buffer[1]){
      buffer_mutex = oss_midiin->app_buffer_mutex[1];
    }else if(buffer == oss_midiin->app_buffer[2]){
      buffer_mutex = oss_midiin->app_buffer_mutex[2];
    }else if(buffer == oss_midiin->app_buffer[3]){
      buffer_mutex = oss_midiin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

void
ags_oss_midiin_set_start_note_offset(AgsSequencer *sequencer,
				     guint start_note_offset)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;  

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set note offset */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(oss_midiin_mutex);
}

guint
ags_oss_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  guint start_note_offset;
  
  GRecMutex *oss_midiin_mutex;  

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set note offset */
  g_rec_mutex_lock(oss_midiin_mutex);

  start_note_offset = oss_midiin->start_note_offset;

  g_rec_mutex_unlock(oss_midiin_mutex);

  return(start_note_offset);
}

void
ags_oss_midiin_set_note_offset(AgsSequencer *sequencer,
			       guint note_offset)
{
  AgsOssMidiin *oss_midiin;

  GRecMutex *oss_midiin_mutex;  

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set note offset */
  g_rec_mutex_lock(oss_midiin_mutex);

  oss_midiin->note_offset = note_offset;

  g_rec_mutex_unlock(oss_midiin_mutex);
}

guint
ags_oss_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsOssMidiin *oss_midiin;

  guint note_offset;
  
  GRecMutex *oss_midiin_mutex;  

  oss_midiin = AGS_OSS_MIDIIN(sequencer);

  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* set note offset */
  g_rec_mutex_lock(oss_midiin_mutex);

  note_offset = oss_midiin->note_offset;

  g_rec_mutex_unlock(oss_midiin_mutex);

  return(note_offset);
}

/**
 * ags_oss_midiin_switch_buffer_flag:
 * @oss_midiin: the #AgsOssMidiin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.13.2
 */
void
ags_oss_midiin_switch_buffer_flag(AgsOssMidiin *oss_midiin)
{
  GRecMutex *oss_midiin_mutex;  

  if(!AGS_IS_OSS_MIDIIN(oss_midiin)){
    return;
  }
  
  /* get oss_midiin mutex */
  oss_midiin_mutex = AGS_OSS_MIDIIN_GET_OBJ_MUTEX(oss_midiin);

  /* switch buffer flag */
  g_rec_mutex_lock(oss_midiin_mutex);

  if(oss_midiin->app_buffer_mode < AGS_OSS_MIDIIN_APP_BUFFER_3){
    oss_midiin->app_buffer_mode += 1;
  }else{
    oss_midiin->app_buffer_mode = AGS_OSS_MIDIIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(oss_midiin_mutex);
}

/**
 * ags_oss_midiin_new:
 *
 * Creates a new instance of #AgsOssMidiin.
 *
 * Returns: the new #AgsOssMidiin
 *
 * Since: 3.13.2
 */
AgsOssMidiin*
ags_oss_midiin_new()
{
  AgsOssMidiin *oss_midiin;

  oss_midiin = (AgsOssMidiin *) g_object_new(AGS_TYPE_OSS_MIDIIN,
					     NULL);
  
  return(oss_midiin);
}
