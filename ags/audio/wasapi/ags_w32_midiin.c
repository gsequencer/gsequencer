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

#include <ags/audio/wasapi/ags_w32_midiin.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef AGS_WITH_W32
#include <windows.h>
#include <ole2.h>
#include <mmeapi.h>
#include <wchar.h>
#endif

#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

/**
 * SECTION:ags_w32_midiin
 * @short_description: Input from sequencer
 * @title: AgsW32Midiin
 * @section_id:
 * @include: ags/audio/ags_w32_midiin.h
 *
 * #AgsW32Midiin represents a sequencer and supports midi input.
 */

void ags_w32_midiin_class_init(AgsW32MidiinClass *w32_midiin);
void ags_w32_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_w32_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_w32_midiin_init(AgsW32Midiin *w32_midiin);
void ags_w32_midiin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_w32_midiin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_w32_midiin_dispose(GObject *gobject);
void ags_w32_midiin_finalize(GObject *gobject);

AgsUUID* ags_w32_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_w32_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_w32_midiin_is_ready(AgsConnectable *connectable);
void ags_w32_midiin_add_to_registry(AgsConnectable *connectable);
void ags_w32_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_w32_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_w32_midiin_xml_compose(AgsConnectable *connectable);
void ags_w32_midiin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_w32_midiin_is_connected(AgsConnectable *connectable);
void ags_w32_midiin_connect(AgsConnectable *connectable);
void ags_w32_midiin_disconnect(AgsConnectable *connectable);

void ags_w32_midiin_set_device(AgsSequencer *sequencer,
			       gchar *device);
gchar* ags_w32_midiin_get_device(AgsSequencer *sequencer);

void ags_w32_midiin_list_cards(AgsSequencer *sequencer,
			       GList **card_id, GList **card_name);

gboolean ags_w32_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_w32_midiin_is_recording(AgsSequencer *sequencer);

void ags_w32_midiin_device_record_init(AgsSequencer *sequencer,
				       GError **error);
void ags_w32_midiin_device_record(AgsSequencer *sequencer,
				  GError **error);
void ags_w32_midiin_device_free(AgsSequencer *sequencer);

void ags_w32_midiin_tic(AgsSequencer *sequencer);
void ags_w32_midiin_offset_changed(AgsSequencer *sequencer,
				   guint note_offset);

void ags_w32_midiin_set_bpm(AgsSequencer *sequencer,
			    gdouble bpm);
gdouble ags_w32_midiin_get_bpm(AgsSequencer *sequencer);

void ags_w32_midiin_set_delay_factor(AgsSequencer *sequencer,
				     gdouble delay_factor);
gdouble ags_w32_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_w32_midiin_get_buffer(AgsSequencer *sequencer,
				guint *buffer_length);
void* ags_w32_midiin_get_next_buffer(AgsSequencer *sequencer,
				     guint *buffer_length);

void ags_w32_midiin_lock_buffer(AgsSequencer *sequencer,
				void *buffer);
void ags_w32_midiin_unlock_buffer(AgsSequencer *sequencer,
				  void *buffer);

void ags_w32_midiin_set_start_note_offset(AgsSequencer *sequencer,
					  guint start_note_offset);
guint ags_w32_midiin_get_start_note_offset(AgsSequencer *sequencer);

void ags_w32_midiin_set_note_offset(AgsSequencer *sequencer,
				    guint note_offset);
guint ags_w32_midiin_get_note_offset(AgsSequencer *sequencer);

enum{
  PROP_0,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
};

static gpointer ags_w32_midiin_parent_class = NULL;

#ifdef AGS_WITH_W32 // win32 - mmeapi.h
typedef struct midiincaps_tag {
  WORD wMid;
  WORD wPid;
  VERSION vDriverVersion;
  char szPname[MAXPNAMELEN];
  DWORD dwSupport;
} MIDIINCAPS, *PMIDIINCAPS, *NPMIDIINCAPS, *LPMIDIINCAPS;
#endif

GType
ags_w32_midiin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_w32_midiin = 0;

    static const GTypeInfo ags_w32_midiin_info = {
      sizeof(AgsW32MidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_w32_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsW32Midiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_w32_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_w32_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_w32_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_w32_midiin = g_type_register_static(G_TYPE_OBJECT,
						 "AgsW32Midiin",
						 &ags_w32_midiin_info,
						 0);

    g_type_add_interface_static(ags_type_w32_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_w32_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_w32_midiin);
  }

  return g_define_type_id__volatile;
}

void
ags_w32_midiin_class_init(AgsW32MidiinClass *w32_midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_w32_midiin_parent_class = g_type_class_peek_parent(w32_midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) w32_midiin;

  gobject->set_property = ags_w32_midiin_set_property;
  gobject->get_property = ags_w32_midiin_get_property;

  gobject->dispose = ags_w32_midiin_dispose;
  gobject->finalize = ags_w32_midiin_finalize;

  /* properties */
  /**
   * AgsW32Midiin:device:
   *
   * The W32 sequencer indentifier
   * 
   * Since: 4.5.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_W32_MIDIIN_DEFAULT_W32_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsW32Midiin:buffer:
   *
   * The buffer
   * 
   * Since: 4.5.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsW32Midiin:bpm:
   *
   * Beats per minute
   * 
   * Since: 4.5.0
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
   * AgsW32Midiin:delay-factor:
   *
   * tact
   * 
   * Since: 4.5.0
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
   * AgsW32Midiin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 4.5.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /* AgsW32MidiinClass */
}

GType
ags_w32_midiin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_W32_MIDIIN_INITIALIZED, "AGS_W32_MIDIIN_INITIALIZED", "w32-midiin-initialized" },
      { AGS_W32_MIDIIN_START_RECORD, "AGS_W32_MIDIIN_START_RECORD", "w32-midiin-start-record" },
      { AGS_W32_MIDIIN_RECORD, "AGS_W32_MIDIIN_RECORD", "w32-midiin-record" },
      { AGS_W32_MIDIIN_SHUTDOWN, "AGS_W32_MIDIIN_SHUTDOWN", "w32-midiin-shutdown" },
      { AGS_W32_MIDIIN_NONBLOCKING, "AGS_W32_MIDIIN_NONBLOCKING", "w32-midiin-nonblocking" },
      { AGS_W32_MIDIIN_ATTACK_FIRST, "AGS_W32_MIDIIN_ATTACK_FIRST", "w32-midiin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsW32MidiinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GQuark
ags_w32_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-w32-midiin-error-quark"));
}

void
ags_w32_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_w32_midiin_get_uuid;
  connectable->has_resource = ags_w32_midiin_has_resource;

  connectable->is_ready = ags_w32_midiin_is_ready;
  connectable->add_to_registry = ags_w32_midiin_add_to_registry;
  connectable->remove_from_registry = ags_w32_midiin_remove_from_registry;

  connectable->list_resource = ags_w32_midiin_list_resource;
  connectable->xml_compose = ags_w32_midiin_xml_compose;
  connectable->xml_parse = ags_w32_midiin_xml_parse;

  connectable->is_connected = ags_w32_midiin_is_connected;  
  connectable->connect = ags_w32_midiin_connect;
  connectable->disconnect = ags_w32_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_w32_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_device = ags_w32_midiin_set_device;
  sequencer->get_device = ags_w32_midiin_get_device;

  sequencer->list_cards = ags_w32_midiin_list_cards;

  sequencer->is_starting =  ags_w32_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_w32_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_w32_midiin_device_record_init;
  sequencer->record = ags_w32_midiin_device_record;
  
  sequencer->stop = ags_w32_midiin_device_free;

  sequencer->tic = ags_w32_midiin_tic;
  sequencer->offset_changed = ags_w32_midiin_offset_changed;
    
  sequencer->set_bpm = ags_w32_midiin_set_bpm;
  sequencer->get_bpm = ags_w32_midiin_get_bpm;

  sequencer->set_delay_factor = ags_w32_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_w32_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_w32_midiin_get_buffer;
  sequencer->get_next_buffer = ags_w32_midiin_get_next_buffer;

  sequencer->lock_buffer = ags_w32_midiin_lock_buffer;
  sequencer->unlock_buffer = ags_w32_midiin_unlock_buffer;

  sequencer->set_start_note_offset = ags_w32_midiin_set_start_note_offset;
  sequencer->get_start_note_offset = ags_w32_midiin_get_start_note_offset;

  sequencer->set_note_offset = ags_w32_midiin_set_note_offset;
  sequencer->get_note_offset = ags_w32_midiin_get_note_offset;
}

void
ags_w32_midiin_init(AgsW32Midiin *w32_midiin)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;

  w32_midiin->flags = 0;
  w32_midiin->connectable_flags = 0;
  
  /* insert w32_midiin mutex */
  g_rec_mutex_init(&(w32_midiin->obj_mutex));

  /* uuid */
  w32_midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(w32_midiin->uuid);

  /* config */
  config = ags_config_get_instance();

  /* sync flags */
  g_atomic_int_set(&(w32_midiin->sync_flags),
		   (AGS_W32_MIDIIN_PASS_THROUGH));

  /* device */
  w32_midiin->device_fd = ~0;
  w32_midiin->device = NULL; //NOTE:JK: g_strdup(AGS_W32_MIDIIN_DEFAULT_W32_DEVICE);

  /* app buffer */
  w32_midiin->app_buffer_mode = AGS_W32_MIDIIN_APP_BUFFER_0;

  w32_midiin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    w32_midiin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(w32_midiin->app_buffer_mutex[i]);
  }

  w32_midiin->app_buffer = (char **) g_malloc(AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE * sizeof(char *));
  
  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    w32_midiin->app_buffer[i] = NULL;

    w32_midiin->app_buffer_size[i] = 0;
  }
  
  w32_midiin->backend_buffer_mode = AGS_W32_MIDIIN_BACKEND_BUFFER_0;
  
  w32_midiin->backend_buffer = (char **) g_malloc(AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(char *));

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    w32_midiin->backend_buffer[i] = NULL;

    w32_midiin->backend_buffer_size[i] = 0;
  }

  /* bpm */
  w32_midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  w32_midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  w32_midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    w32_midiin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }
  
  w32_midiin->latency = AGS_NSEC_PER_SEC / 4000.0;
  
  /* counters */
  w32_midiin->start_note_offset = 0;
  w32_midiin->note_offset = 0;
  w32_midiin->note_offset_absolute = 0;

  w32_midiin->tact_counter = 0.0;
  w32_midiin->delay_counter = 0;
  w32_midiin->tic_counter = 0;
}

void
ags_w32_midiin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(gobject);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(w32_midiin_mutex);

    g_free(w32_midiin->device);
    
    w32_midiin->device = g_strdup(device);

    g_rec_mutex_unlock(w32_midiin_mutex);
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

    g_rec_mutex_lock(w32_midiin_mutex);

    if(bpm == w32_midiin->bpm){
      g_rec_mutex_unlock(w32_midiin_mutex);

      return;
    }

    w32_midiin->bpm = bpm;

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(w32_midiin_mutex);

    if(delay_factor == w32_midiin->delay_factor){
      g_rec_mutex_unlock(w32_midiin_mutex);

      return;
    }

    w32_midiin->delay_factor = delay_factor;

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_w32_midiin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(gobject);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(w32_midiin_mutex);

    g_value_set_string(value, w32_midiin->device);

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(w32_midiin_mutex);

    g_value_set_pointer(value, w32_midiin->app_buffer);

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(w32_midiin_mutex);

    g_value_set_double(value, w32_midiin->bpm);

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(w32_midiin_mutex);

    g_value_set_double(value, w32_midiin->delay_factor);

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_w32_midiin_dispose(GObject *gobject)
{
  AgsW32Midiin *w32_midiin;

  w32_midiin = AGS_W32_MIDIIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_w32_midiin_parent_class)->dispose(gobject);
}

void
ags_w32_midiin_finalize(GObject *gobject)
{
  AgsW32Midiin *w32_midiin;

  guint i;
  
  w32_midiin = AGS_W32_MIDIIN(gobject);

  ags_uuid_free(w32_midiin->uuid);
  
  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(w32_midiin->app_buffer[i]);
  }

  g_free(w32_midiin->app_buffer);

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(w32_midiin->backend_buffer[i]);
  }

  g_free(w32_midiin->backend_buffer);

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(w32_midiin->app_buffer_mutex[i]);
    
    g_free(w32_midiin->app_buffer_mutex[i]);
  }

  g_free(w32_midiin->app_buffer_mutex);
  
  g_free(w32_midiin->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_w32_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_w32_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;
  
  AgsUUID *ptr;

  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32_midiin signal mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* get UUID */
  g_rec_mutex_lock(w32_midiin_mutex);

  ptr = w32_midiin->uuid;

  g_rec_mutex_unlock(w32_midiin_mutex);
  
  return(ptr);
}

gboolean
ags_w32_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_w32_midiin_is_ready(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;
  
  gboolean is_ready;

  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* check is ready */
  g_rec_mutex_lock(w32_midiin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (w32_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(w32_midiin_mutex);
  
  return(is_ready);
}

void
ags_w32_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

void
ags_w32_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

xmlNode*
ags_w32_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_w32_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_w32_midiin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_w32_midiin_is_connected(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;
  
  gboolean is_connected;

  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* check is connected */
  g_rec_mutex_lock(w32_midiin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (w32_midiin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(w32_midiin_mutex);
  
  return(is_connected);
}

void
ags_w32_midiin_connect(AgsConnectable *connectable)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

void
ags_w32_midiin_disconnect(AgsConnectable *connectable)
{

  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  w32_midiin = AGS_W32_MIDIIN(connectable);

  /* get w32 midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

/**
 * ags_w32_midiin_test_flags:
 * @w32_midiin: the #AgsW32Midiin
 * @flags: the flags
 *
 * Test @flags to be set on @w32_midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 4.5.0
 */
gboolean
ags_w32_midiin_test_flags(AgsW32Midiin *w32_midiin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *w32_midiin_mutex;

  if(!AGS_IS_W32_MIDIIN(w32_midiin)){
    return(FALSE);
  }

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* test */
  g_rec_mutex_lock(w32_midiin_mutex);

  retval = (flags & (w32_midiin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(w32_midiin_mutex);

  return(retval);
}

/**
 * ags_w32_midiin_set_flags:
 * @w32_midiin: the #AgsW32Midiin
 * @flags: see #AgsW32MidiinFlags-enum
 *
 * Enable a feature of @w32_midiin.
 *
 * Since: 4.5.0
 */
void
ags_w32_midiin_set_flags(AgsW32Midiin *w32_midiin, guint flags)
{
  GRecMutex *w32_midiin_mutex;

  if(!AGS_IS_W32_MIDIIN(w32_midiin)){
    return;
  }

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->flags |= flags;
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}
    
/**
 * ags_w32_midiin_unset_flags:
 * @w32_midiin: the #AgsW32Midiin
 * @flags: see #AgsW32MidiinFlags-enum
 *
 * Disable a feature of @w32_midiin.
 *
 * Since: 4.5.0
 */
void
ags_w32_midiin_unset_flags(AgsW32Midiin *w32_midiin, guint flags)
{  
  GRecMutex *w32_midiin_mutex;

  if(!AGS_IS_W32_MIDIIN(w32_midiin)){
    return;
  }

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->flags &= (~flags);
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

void
ags_w32_midiin_set_device(AgsSequencer *sequencer,
			  gchar *device)
{
  AgsW32Midiin *w32_midiin;
  
  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set device */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->device = g_strdup(device);
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

gchar*
ags_w32_midiin_get_device(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gchar *device;

  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);
  
  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  device = NULL;
  
  g_rec_mutex_lock(w32_midiin_mutex);

  device = g_strdup(w32_midiin->device);

  g_rec_mutex_unlock(w32_midiin_mutex);

  return(device);
}

void
ags_w32_midiin_list_cards(AgsSequencer *sequencer,
			  GList **card_id, GList **card_name)
{
  AgsW32Midiin *w32_midiin;

  AgsApplicationContext *application_context;

#if defined(AGS_WITH_W32)
  MIDIINCAPS midiincaps;

  UINT retval;
  UINT i, i_stop;
#endif
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

#if defined(AGS_WITH_W32)
  CoInitialize(0);

  i_stop = midiInGetNumDevs();

  for(i = 0; i < i_stop; i++){
    retval = midiInGetDevCaps(i,
			      &midiincaps,
			      sizeof(midiincaps));

    if(retval == MMSYSERR_NOERROR){
      g_message("%d:%d %S", midiincaps.wMid, midiincaps.pMid, midiincaps.szPname);

      if(card_id != NULL){
	str = g_strdup_printf("%d:%d", midiincaps.wMid, midiincaps.pMid);

	*card_id = g_list_prepend(*card_id,
				  str);
      }

      if(card_name != NULL){
	str = g_strdup_printf("%S", midiincaps.szPname);
	      
	*card_name = g_list_prepend(*card_name,
				    str);
      }      
    }
  }

  CoUninitialize();
#endif

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

gboolean
ags_w32_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gboolean is_starting;
  
  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* check is starting */
  g_rec_mutex_lock(w32_midiin_mutex);

  is_starting = ((AGS_W32_MIDIIN_START_RECORD & (w32_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(w32_midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_w32_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gboolean is_recording;
  
  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(sequencer);
  
  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* check is starting */
  g_rec_mutex_lock(w32_midiin_mutex);

  is_recording = ((AGS_W32_MIDIIN_RECORD & (w32_midiin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(w32_midiin_mutex);

  return(is_recording);
}

void
ags_w32_midiin_device_record_init(AgsSequencer *sequencer,
				  GError **error)
{
  AgsW32Midiin *w32_midiin;
  
  gchar *str;

#if defined(AGS_WITH_W32)
  UINT product_id;
  UINT retval;
#endif

  gint position;
  guint word_size;
  int format;
  int tmp;

  GRecMutex *w32_midiin_mutex;

  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* prepare for playback */
  g_rec_mutex_lock(w32_midiin_mutex);
    
  w32_midiin->flags |= (AGS_W32_MIDIIN_START_RECORD |
			AGS_W32_MIDIIN_RECORD |
			AGS_W32_MIDIIN_NONBLOCKING);

#ifdef AGS_WITH_W32
  if((position = strchr(w32_midiin->device, ':')) >= 0){
    //FIXME:JK: not verified - see midiInOpen
    product_id = (UINT) g_ascii_strtoull(w32_midiin->device + position + 1,
					 NULL,
					 10);
  
    CoInitialize(0);

    //FIXME:JK: not verified
    retval = midiInOpen(w32_midiin->device_fd,
			product_id,
			NULL,
			w32_midiin,
			CALLBACK_NULL);

    if(retval != MMSYSERR_NOERROR){
      //TODO:JK: implement me
    }
  
    CoUninitialize();
  }
#endif
  
  w32_midiin->tact_counter = 0.0;
  w32_midiin->delay_counter = 0.0;
  w32_midiin->tic_counter = 0;

  w32_midiin->backend_buffer_mode = AGS_W32_MIDIIN_BACKEND_BUFFER_0;  

#ifdef AGS_WITH_W32
  w32_midiin->flags |= AGS_W32_MIDIIN_INITIALIZED;
#endif

  w32_midiin->app_buffer_mode = AGS_W32_MIDIIN_APP_BUFFER_0;
  
  g_rec_mutex_unlock(w32_midiin_mutex);
}

void
ags_w32_midiin_device_record(AgsSequencer *sequencer,
			     GError **error)
{
  AgsW32Midiin *w32_midiin;

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
  guint device_fd;
  int num_read;
  
  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  application_context = ags_application_context_get_instance();

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* poll MIDI device */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->flags &= (~AGS_W32_MIDIIN_START_RECORD);

  if((AGS_W32_MIDIIN_INITIALIZED & (w32_midiin->flags)) == 0){
    g_rec_mutex_unlock(w32_midiin_mutex);
    
    return;
  }

  device_fd = w32_midiin->device_fd;

  if(w32_midiin->app_buffer_mode + 1 > AGS_W32_MIDIIN_APP_BUFFER_3){
    app_buffer_mode = AGS_W32_MIDIIN_APP_BUFFER_0;
  }else{
    app_buffer_mode = w32_midiin->app_buffer_mode + 1;
  }

  if(w32_midiin->backend_buffer_mode + 1 > AGS_W32_MIDIIN_BACKEND_BUFFER_7){
    backend_buffer_mode = AGS_W32_MIDIIN_BACKEND_BUFFER_0;
  }else{
    backend_buffer_mode = w32_midiin->backend_buffer_mode + 1;
  }
  
  if(w32_midiin->backend_buffer[backend_buffer_mode] != NULL){
    g_free(w32_midiin->backend_buffer[backend_buffer_mode]);
  }

  w32_midiin->backend_buffer[backend_buffer_mode] = NULL;
  w32_midiin->backend_buffer_size[backend_buffer_mode] = 0;
      
  backend_buffer = w32_midiin->backend_buffer;
  backend_buffer_size = w32_midiin->backend_buffer_size[backend_buffer_mode];
  
  g_rec_mutex_unlock(w32_midiin_mutex);

  num_read = 1;
  
  while(num_read > 0){
    num_read = 0;
    
#ifdef AGS_WITH_W32
    //TODO:JK: implement me
    //    num_read = read(device_fd, buf, sizeof(buf));
    
    if((num_read < 0)){
      // g_warning("Problem reading MIDI input");
    }

    if(num_read > 0){
      if(backend_buffer[backend_buffer_mode] == NULL ||
	 ceil((backend_buffer_size + num_read) / AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE) > ceil(backend_buffer_size / AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE)){
	if(backend_buffer[backend_buffer_mode] == NULL){
	  backend_buffer[backend_buffer_mode] = (char *) g_malloc(AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
	}else{
	  backend_buffer[backend_buffer_mode] = (char *) g_realloc(backend_buffer[backend_buffer_mode],
								   (AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE * ceil(backend_buffer_size / AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE) + AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
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
  g_rec_mutex_lock(w32_midiin_mutex);

  /* update byte array and buffer size */
  if(w32_midiin->app_buffer[app_buffer_mode] != NULL){
    g_free(w32_midiin->app_buffer[app_buffer_mode]);
  }

  w32_midiin->app_buffer[app_buffer_mode] = NULL;
      
  w32_midiin->backend_buffer_size[backend_buffer_mode] = backend_buffer_size;

  w32_midiin->app_buffer_size[app_buffer_mode] = backend_buffer_size;

  /* fill buffer */
  if(backend_buffer_size > 0){
    w32_midiin->app_buffer[app_buffer_mode] = (char *) g_malloc(backend_buffer_size * sizeof(char));
	
    memcpy(w32_midiin->app_buffer[app_buffer_mode], backend_buffer[backend_buffer_mode], backend_buffer_size * sizeof(char));
  }
      
  g_rec_mutex_unlock(w32_midiin_mutex);

  g_rec_mutex_lock(w32_midiin_mutex);

  ags_w32_midiin_switch_buffer_flag(w32_midiin);
      
  g_rec_mutex_unlock(w32_midiin_mutex);

  /* update sequencer */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
  task = NULL;

  /* tic sequencer */
  tic_device = ags_tic_device_new((GObject *) w32_midiin);
  task = g_list_append(task,
		       tic_device);
  
  /* reset - switch buffer flags */
  //  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) w32_midiin);
  //  task = g_list_append(task,
  //		       switch_buffer_flag);

  /* add tasks all */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  g_object_unref(task_launcher);
}

void
ags_w32_midiin_device_free(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  guint i;

  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  g_rec_mutex_lock(w32_midiin_mutex);  

  if((AGS_W32_MIDIIN_INITIALIZED & (w32_midiin->flags)) == 0){
    g_rec_mutex_unlock(w32_midiin_mutex);  

    return;
  }

  w32_midiin->flags &= (~(AGS_W32_MIDIIN_RECORD |
			  AGS_W32_MIDIIN_INITIALIZED));
  
  g_rec_mutex_unlock(w32_midiin_mutex);  

  /*  */
  g_rec_mutex_lock(w32_midiin_mutex);  

  //TODO:JK: implement me
  //  close(w32_midiin->device_fd);
  //  w32_midiin->device_fd = -1;

  w32_midiin->app_buffer_mode = AGS_W32_MIDIIN_APP_BUFFER_0;

  w32_midiin->backend_buffer_mode = AGS_W32_MIDIIN_BACKEND_BUFFER_0;

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(w32_midiin->app_buffer[i]);

    w32_midiin->app_buffer[i] = NULL;
    
    w32_midiin->app_buffer_size[i] = 0;
  }

  for(i = 0; i < AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(w32_midiin->backend_buffer[i]);

    w32_midiin->backend_buffer[i] = NULL;

    w32_midiin->backend_buffer_size[i] = 0;
  }

  g_atomic_int_or(&(w32_midiin->sync_flags),
		  AGS_W32_MIDIIN_PASS_THROUGH);
  
  w32_midiin->note_offset = w32_midiin->start_note_offset;
  w32_midiin->note_offset_absolute = w32_midiin->start_note_offset;
  
  g_rec_mutex_unlock(w32_midiin_mutex);  
}  

void
ags_w32_midiin_tic(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  GRecMutex *w32_midiin_mutex;

  w32_midiin = AGS_W32_MIDIIN(sequencer);
  
  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(w32_midiin_mutex);

  delay = w32_midiin->delay;
  delay_counter = w32_midiin->delay_counter;

  note_offset = w32_midiin->note_offset;

  g_rec_mutex_unlock(w32_midiin_mutex);

  if(delay_counter + 1.0 >= delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);

    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(w32_midiin_mutex);

    w32_midiin->delay_counter = delay_counter + 1.0 - delay;
    w32_midiin->tact_counter += 1.0;

    g_rec_mutex_unlock(w32_midiin_mutex);
  }else{
    g_rec_mutex_lock(w32_midiin_mutex);

    w32_midiin->delay_counter += 1.0;

    g_rec_mutex_unlock(w32_midiin_mutex);
  }
}

void
ags_w32_midiin_offset_changed(AgsSequencer *sequencer,
			      guint note_offset)
{
  AgsW32Midiin *w32_midiin;
  
  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* offset changed */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->tic_counter += 1;

  if(w32_midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    w32_midiin->tic_counter = 0;
  }

  g_rec_mutex_unlock(w32_midiin_mutex);
}

void
ags_w32_midiin_set_bpm(AgsSequencer *sequencer,
		       gdouble bpm)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set bpm */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->bpm = bpm;

  g_rec_mutex_unlock(w32_midiin_mutex);
}

gdouble
ags_w32_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gdouble bpm;
  
  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* get bpm */
  g_rec_mutex_lock(w32_midiin_mutex);

  bpm = w32_midiin->bpm;
  
  g_rec_mutex_unlock(w32_midiin_mutex);

  return(bpm);
}

void
ags_w32_midiin_set_delay_factor(AgsSequencer *sequencer,
				gdouble delay_factor)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set delay factor */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->delay_factor = delay_factor;

  g_rec_mutex_unlock(w32_midiin_mutex);
}

gdouble
ags_w32_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  gdouble delay_factor;
  
  GRecMutex *w32_midiin_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* get delay factor */
  g_rec_mutex_lock(w32_midiin_mutex);

  delay_factor = w32_midiin->delay_factor;
  
  g_rec_mutex_unlock(w32_midiin_mutex);

  return(delay_factor);
}

void*
ags_w32_midiin_get_buffer(AgsSequencer *sequencer,
			  guint *buffer_length)
{
  AgsW32Midiin *w32_midiin;

  char *buffer;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get buffer */
  buffer = w32_midiin->app_buffer[w32_midiin->app_buffer_mode];

  /* return the buffer's length */
  if(buffer_length != NULL){
    buffer_length[0] = w32_midiin->app_buffer_size[w32_midiin->app_buffer_mode];
  }
  
  return(buffer);
}

void*
ags_w32_midiin_get_next_buffer(AgsSequencer *sequencer,
			       guint *buffer_length)
{
  AgsW32Midiin *w32_midiin;
  char *buffer;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get buffer */
  if(w32_midiin->app_buffer_mode == AGS_W32_MIDIIN_APP_BUFFER_3){
    buffer = w32_midiin->app_buffer[AGS_W32_MIDIIN_APP_BUFFER_0];
  }else{
    buffer = w32_midiin->app_buffer[w32_midiin->app_buffer_mode + 1];
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if(w32_midiin->app_buffer_mode == AGS_W32_MIDIIN_APP_BUFFER_3){
      buffer_length[0] = w32_midiin->app_buffer_size[AGS_W32_MIDIIN_APP_BUFFER_0];
    }else{
      buffer_length[0] = w32_midiin->app_buffer_size[w32_midiin->app_buffer_mode + 1];
    }
  }
  
  return(buffer);
}

void
ags_w32_midiin_lock_buffer(AgsSequencer *sequencer,
			   void *buffer)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *buffer_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(w32_midiin->app_buffer != NULL){
    if(buffer == w32_midiin->app_buffer[0]){
      buffer_mutex = w32_midiin->app_buffer_mutex[0];
    }else if(buffer == w32_midiin->app_buffer[1]){
      buffer_mutex = w32_midiin->app_buffer_mutex[1];
    }else if(buffer == w32_midiin->app_buffer[2]){
      buffer_mutex = w32_midiin->app_buffer_mutex[2];
    }else if(buffer == w32_midiin->app_buffer[3]){
      buffer_mutex = w32_midiin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_w32_midiin_unlock_buffer(AgsSequencer *sequencer,
			     void *buffer)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *buffer_mutex;
  
  w32_midiin = AGS_W32_MIDIIN(sequencer);

  buffer_mutex = NULL;

  if(w32_midiin->app_buffer != NULL){
    if(buffer == w32_midiin->app_buffer[0]){
      buffer_mutex = w32_midiin->app_buffer_mutex[0];
    }else if(buffer == w32_midiin->app_buffer[1]){
      buffer_mutex = w32_midiin->app_buffer_mutex[1];
    }else if(buffer == w32_midiin->app_buffer[2]){
      buffer_mutex = w32_midiin->app_buffer_mutex[2];
    }else if(buffer == w32_midiin->app_buffer[3]){
      buffer_mutex = w32_midiin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

void
ags_w32_midiin_set_start_note_offset(AgsSequencer *sequencer,
				     guint start_note_offset)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;  

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set note offset */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(w32_midiin_mutex);
}

guint
ags_w32_midiin_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  guint start_note_offset;
  
  GRecMutex *w32_midiin_mutex;  

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set note offset */
  g_rec_mutex_lock(w32_midiin_mutex);

  start_note_offset = w32_midiin->start_note_offset;

  g_rec_mutex_unlock(w32_midiin_mutex);

  return(start_note_offset);
}

void
ags_w32_midiin_set_note_offset(AgsSequencer *sequencer,
			       guint note_offset)
{
  AgsW32Midiin *w32_midiin;

  GRecMutex *w32_midiin_mutex;  

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set note offset */
  g_rec_mutex_lock(w32_midiin_mutex);

  w32_midiin->note_offset = note_offset;

  g_rec_mutex_unlock(w32_midiin_mutex);
}

guint
ags_w32_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsW32Midiin *w32_midiin;

  guint note_offset;
  
  GRecMutex *w32_midiin_mutex;  

  w32_midiin = AGS_W32_MIDIIN(sequencer);

  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* set note offset */
  g_rec_mutex_lock(w32_midiin_mutex);

  note_offset = w32_midiin->note_offset;

  g_rec_mutex_unlock(w32_midiin_mutex);

  return(note_offset);
}

/**
 * ags_w32_midiin_switch_buffer_flag:
 * @w32_midiin: the #AgsW32Midiin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 4.5.0
 */
void
ags_w32_midiin_switch_buffer_flag(AgsW32Midiin *w32_midiin)
{
  GRecMutex *w32_midiin_mutex;  

  if(!AGS_IS_W32_MIDIIN(w32_midiin)){
    return;
  }
  
  /* get w32_midiin mutex */
  w32_midiin_mutex = AGS_W32_MIDIIN_GET_OBJ_MUTEX(w32_midiin);

  /* switch buffer flag */
  g_rec_mutex_lock(w32_midiin_mutex);

  if(w32_midiin->app_buffer_mode < AGS_W32_MIDIIN_APP_BUFFER_3){
    w32_midiin->app_buffer_mode += 1;
  }else{
    w32_midiin->app_buffer_mode = AGS_W32_MIDIIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(w32_midiin_mutex);
}

/**
 * ags_w32_midiin_new:
 *
 * Creates a new instance of #AgsW32Midiin.
 *
 * Returns: the new #AgsW32Midiin
 *
 * Since: 4.5.0
 */
AgsW32Midiin*
ags_w32_midiin_new()
{
  AgsW32Midiin *w32_midiin;

  w32_midiin = (AgsW32Midiin *) g_object_new(AGS_TYPE_W32_MIDIIN,
					     NULL);
  
  return(w32_midiin);
}
