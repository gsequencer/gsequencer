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

#include <ags/audio/ags_midiin.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#ifndef __APPLE__
#include <sys/soundcard.h>
#endif
#include <sys/utsname.h>
#ifdef AGS_WITH_ALSA
#include <alsa/rawmidi.h>
#endif
#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

/**
 * SECTION:ags_midiin
 * @short_description: Input from sequencer
 * @title: AgsMidiin
 * @section_id:
 * @include: ags/audio/ags_midiin.h
 *
 * #AgsMidiin represents a sequencer and supports midi input.
 */

void ags_midiin_class_init(AgsMidiinClass *midiin);
void ags_midiin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer);
void ags_midiin_init(AgsMidiin *midiin);
void ags_midiin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_midiin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_midiin_dispose(GObject *gobject);
void ags_midiin_finalize(GObject *gobject);

AgsUUID* ags_midiin_get_uuid(AgsConnectable *connectable);
gboolean ags_midiin_has_resource(AgsConnectable *connectable);
gboolean ags_midiin_is_ready(AgsConnectable *connectable);
void ags_midiin_add_to_registry(AgsConnectable *connectable);
void ags_midiin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_midiin_list_resource(AgsConnectable *connectable);
xmlNode* ags_midiin_xml_compose(AgsConnectable *connectable);
void ags_midiin_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_midiin_is_connected(AgsConnectable *connectable);
void ags_midiin_connect(AgsConnectable *connectable);
void ags_midiin_disconnect(AgsConnectable *connectable);

void ags_midiin_set_application_context(AgsSequencer *sequencer,
					AgsApplicationContext *application_context);
AgsApplicationContext* ags_midiin_get_application_context(AgsSequencer *sequencer);

void ags_midiin_set_device(AgsSequencer *sequencer,
			   gchar *device);
gchar* ags_midiin_get_device(AgsSequencer *sequencer);

void ags_midiin_list_cards(AgsSequencer *sequencer,
			   GList **card_id, GList **card_name);

gboolean ags_midiin_is_starting(AgsSequencer *sequencer);
gboolean ags_midiin_is_recording(AgsSequencer *sequencer);

void ags_midiin_delegate_record_init(AgsSequencer *sequencer,
				     GError **error);
void ags_midiin_delegate_record(AgsSequencer *sequencer,
				GError **error);
void ags_midiin_delegate_stop(AgsSequencer *sequencer);

void ags_midiin_oss_init(AgsSequencer *sequencer,
			 GError **error);
void ags_midiin_oss_record(AgsSequencer *sequencer,
			   GError **error);
void ags_midiin_oss_free(AgsSequencer *sequencer);

void ags_midiin_alsa_init(AgsSequencer *sequencer,
			  GError **error);
void ags_midiin_alsa_record(AgsSequencer *sequencer,
			    GError **error);
void ags_midiin_alsa_free(AgsSequencer *sequencer);

void ags_midiin_tic(AgsSequencer *sequencer);
void ags_midiin_offset_changed(AgsSequencer *sequencer,
			       guint note_offset);

void ags_midiin_set_bpm(AgsSequencer *sequencer,
			gdouble bpm);
gdouble ags_midiin_get_bpm(AgsSequencer *sequencer);

void ags_midiin_set_delay_factor(AgsSequencer *sequencer,
				 gdouble delay_factor);
gdouble ags_midiin_get_delay_factor(AgsSequencer *sequencer);

void* ags_midiin_get_buffer(AgsSequencer *sequencer,
			    guint *buffer_length);
void* ags_midiin_get_next_buffer(AgsSequencer *sequencer,
				 guint *buffer_length);

void ags_midiin_set_note_offset(AgsSequencer *sequencer,
				guint note_offset);
guint ags_midiin_get_note_offset(AgsSequencer *sequencer);

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_DEVICE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
};

static gpointer ags_midiin_parent_class = NULL;

static pthread_mutex_t ags_midiin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_midiin_get_type (void)
{
  static GType ags_type_midiin = 0;

  if(!ags_type_midiin){
    static const GTypeInfo ags_midiin_info = {
      sizeof(AgsMidiinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midiin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMidiin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midiin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midiin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sequencer_interface_info = {
      (GInterfaceInitFunc) ags_midiin_sequencer_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midiin = g_type_register_static(G_TYPE_OBJECT,
					     "AgsMidiin",
					     &ags_midiin_info,
					     0);

    g_type_add_interface_static(ags_type_midiin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midiin,
				AGS_TYPE_SEQUENCER,
				&ags_sequencer_interface_info);
  }

  return (ags_type_midiin);
}

void
ags_midiin_class_init(AgsMidiinClass *midiin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_midiin_parent_class = g_type_class_peek_parent(midiin);

  /* GObjectClass */
  gobject = (GObjectClass *) midiin;

  gobject->set_property = ags_midiin_set_property;
  gobject->get_property = ags_midiin_get_property;

  gobject->dispose = ags_midiin_dispose;
  gobject->finalize = ags_midiin_finalize;

  /* properties */
  /**
   * AgsMidiin:application-context:
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
   * AgsMidiin:device:
   *
   * The alsa sequencer indentifier
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_MIDIIN_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsMidiin:buffer:
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
   * AgsMidiin:bpm:
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
				   AGS_SEQUENCER_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsMidiin:delay-factor:
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
   * AgsMidiin:attack:
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

  /* AgsMidiinClass */
}

GQuark
ags_midiin_error_quark()
{
  return(g_quark_from_static_string("ags-midiin-error-quark"));
}

void
ags_midiin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_midiin_get_uuid;
  connectable->has_resource = ags_midiin_has_resource;

  connectable->is_ready = ags_midiin_is_ready;
  connectable->add_to_registry = ags_midiin_add_to_registry;
  connectable->remove_from_registry = ags_midiin_remove_from_registry;

  connectable->list_resource = ags_midiin_list_resource;
  connectable->xml_compose = ags_midiin_xml_compose;
  connectable->xml_parse = ags_midiin_xml_parse;

  connectable->is_connected = ags_midiin_is_connected;  
  connectable->connect = ags_midiin_connect;
  connectable->disconnect = ags_midiin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_midiin_sequencer_interface_init(AgsSequencerInterface *sequencer)
{
  sequencer->set_application_context = ags_midiin_set_application_context;
  sequencer->get_application_context = ags_midiin_get_application_context;

  sequencer->set_device = ags_midiin_set_device;
  sequencer->get_device = ags_midiin_get_device;

  sequencer->list_cards = ags_midiin_list_cards;

  sequencer->is_starting =  ags_midiin_is_starting;
  sequencer->is_playing = NULL;
  sequencer->is_recording = ags_midiin_is_recording;

  sequencer->play_init = NULL;
  sequencer->play = NULL;

  sequencer->record_init = ags_midiin_delegate_record_init;
  sequencer->record = ags_midiin_delegate_record;
  
  sequencer->stop = ags_midiin_delegate_stop;

  sequencer->record_init = ags_midiin_alsa_init;
  sequencer->record = ags_midiin_alsa_record;

  sequencer->stop = ags_midiin_alsa_free;

  sequencer->tic = ags_midiin_tic;
  sequencer->offset_changed = ags_midiin_offset_changed;
    
  sequencer->set_bpm = ags_midiin_set_bpm;
  sequencer->get_bpm = ags_midiin_get_bpm;

  sequencer->set_delay_factor = ags_midiin_set_delay_factor;
  sequencer->get_delay_factor = ags_midiin_get_delay_factor;
  
  sequencer->get_buffer = ags_midiin_get_buffer;
  sequencer->get_next_buffer = ags_midiin_get_next_buffer;

  sequencer->set_note_offset = ags_midiin_set_note_offset;
  sequencer->get_note_offset = ags_midiin_get_note_offset;
}

void
ags_midiin_init(AgsMidiin *midiin)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  gboolean use_alsa;  

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  midiin->flags = 0;
  
  /* insert midiin mutex */
  midiin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  midiin->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  midiin->application_context = NULL;

  /* uuid */
  midiin->uuid = ags_uuid_alloc();
  ags_uuid_generate(midiin->uuid);

  /* flags */
  config = ags_config_get_instance();

#ifdef AGS_WITH_ALSA
  use_alsa = TRUE;
#else
  use_alsa = FALSE;
#endif

  str = ags_config_get_value(config,
			     AGS_CONFIG_SEQUENCER,
			     "backend");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SEQUENCER_0,
			       "backend");
  }
  
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "oss",
			  4)){
    use_alsa = FALSE;
  }

  /* flags */
  if(use_alsa){
    midiin->flags |= (AGS_MIDIIN_ALSA);
  }else{
    midiin->flags |= (AGS_MIDIIN_OSS);
  }

  /* sync flags */
  g_atomic_int_set(&(midiin->sync_flags),
		   (AGS_MIDIIN_PASS_THROUGH));

  /* device */
  if(use_alsa){
    midiin->in.alsa.handle = NULL;
    midiin->in.alsa.device = AGS_MIDIIN_DEFAULT_ALSA_DEVICE;
  }else{
    midiin->in.oss.device_fd = -1;
    midiin->in.oss.device = AGS_MIDIIN_DEFAULT_OSS_DEVICE;
  }

  /* ring buffer */
  midiin->ring_buffer = (char **) malloc(2 * sizeof(char *));

  midiin->ring_buffer[0] = NULL;
  midiin->ring_buffer[1] = NULL;
  
  midiin->ring_buffer_size[0] = 0;
  midiin->ring_buffer_size[1] = 0;
  
  /* buffer */
  midiin->buffer = (char **) malloc(4 * sizeof(char *));

  midiin->buffer[0] = NULL;
  midiin->buffer[1] = NULL;
  midiin->buffer[2] = NULL;
  midiin->buffer[3] = NULL;

  midiin->buffer_size[0] = 0;
  midiin->buffer_size[1] = 0;
  midiin->buffer_size[2] = 0;
  midiin->buffer_size[3] = 0;

  /* bpm */
  midiin->bpm = AGS_SEQUENCER_DEFAULT_BPM;

  /* delay and delay factor */
  midiin->delay = AGS_SEQUENCER_DEFAULT_DELAY;
  midiin->delay_factor = AGS_SEQUENCER_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    midiin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }
  
  midiin->latency = NSEC_PER_SEC / 4000.0;
  
  /* counters */
  midiin->note_offset = 0;

  midiin->tact_counter = 0.0;
  midiin->delay_counter = 0;
  midiin->tic_counter = 0;

  /* poll thread */
  midiin->poll_thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  /* poll mutex */
  midiin->poll_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(midiin->poll_mutex,
		     NULL);

  midiin->poll_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(midiin->poll_cond, NULL);

  /* poll finish mutex */
  midiin->poll_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(midiin->poll_finish_mutex,
		     NULL);

  midiin->poll_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(midiin->poll_finish_cond, NULL);
}

void
ags_midiin_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(gobject);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(midiin_mutex);

      if(midiin->application_context == application_context){
	pthread_mutex_unlock(midiin_mutex);

	return;
      }

      if(midiin->application_context != NULL){
	g_object_unref(G_OBJECT(midiin->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      midiin->application_context = application_context;

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(midiin_mutex);

      if((AGS_MIDIIN_OSS & (midiin->flags)) != 0){
	midiin->in.oss.device = g_strdup(device);
      }else if((AGS_MIDIIN_ALSA & (midiin->flags)) != 0){
	midiin->in.alsa.device = g_strdup(device);
      }

      pthread_mutex_unlock(midiin_mutex);
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

      pthread_mutex_lock(midiin_mutex);

      if(bpm == midiin->bpm){
	pthread_mutex_unlock(midiin_mutex);

	return;
      }

      midiin->bpm = bpm;

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(midiin_mutex);

      if(delay_factor == midiin->delay_factor){
	pthread_mutex_unlock(midiin_mutex);

	return;
      }

      midiin->delay_factor = delay_factor;

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midiin_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(gobject);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(midiin_mutex);

      g_value_set_object(value, midiin->application_context);

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(midiin_mutex);

      if((AGS_MIDIIN_OSS & (midiin->flags)) != 0){
	g_value_set_string(value, midiin->in.oss.device);
      }else if((AGS_MIDIIN_ALSA & (midiin->flags)) != 0){
	g_value_set_string(value, midiin->in.alsa.device);
      }

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(midiin_mutex);

      g_value_set_pointer(value, midiin->buffer);

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(midiin_mutex);

      g_value_set_double(value, midiin->bpm);

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(midiin_mutex);

      g_value_set_double(value, midiin->delay_factor);

      pthread_mutex_unlock(midiin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midiin_dispose(GObject *gobject)
{
  AgsMidiin *midiin;

  midiin = AGS_MIDIIN(gobject);

  /* application context */
  if(midiin->application_context != NULL){
    g_object_unref(midiin->application_context);

    midiin->application_context = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_midiin_parent_class)->dispose(gobject);
}

void
ags_midiin_finalize(GObject *gobject)
{
  AgsMidiin *midiin;

  midiin = AGS_MIDIIN(gobject);

  pthread_mutex_destroy(midiin->obj_mutex);
  free(midiin->obj_mutex);

  pthread_mutexattr_destroy(midiin->obj_mutexattr);
  free(midiin->obj_mutexattr);

  /* free input buffer */
  if(midiin->buffer[0] != NULL){
    free(midiin->buffer[0]);
  }

  if(midiin->buffer[1] != NULL){
    free(midiin->buffer[1]);
  }
    
  if(midiin->buffer[2] != NULL){
    free(midiin->buffer[2]);
  }
  
  if(midiin->buffer[3] != NULL){
    free(midiin->buffer[3]);
  }
  
  /* free buffer array */
  free(midiin->buffer);

  /* application context */
  if(midiin->application_context != NULL){
    g_object_unref(midiin->application_context);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_midiin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_midiin_get_uuid(AgsConnectable *connectable)
{
  AgsMidiin *midiin;
  
  AgsUUID *ptr;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(connectable);

  /* get midiin signal mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(midiin_mutex);

  ptr = midiin->uuid;

  pthread_mutex_unlock(midiin_mutex);
  
  return(ptr);
}

gboolean
ags_midiin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_midiin_is_ready(AgsConnectable *connectable)
{
  AgsMidiin *midiin;
  
  gboolean is_ready;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(connectable);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(midiin_mutex);

  is_ready = (((AGS_MIDIIN_ADDED_TO_REGISTRY & (midiin->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(midiin_mutex);
  
  return(is_ready);
}

void
ags_midiin_add_to_registry(AgsConnectable *connectable)
{
  AgsMidiin *midiin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  midiin = AGS_MIDIIN(connectable);

  ags_midiin_set_flags(midiin, AGS_MIDIIN_ADDED_TO_REGISTRY);
}

void
ags_midiin_remove_from_registry(AgsConnectable *connectable)
{
  AgsMidiin *midiin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  midiin = AGS_MIDIIN(connectable);

  ags_midiin_unset_flags(midiin, AGS_MIDIIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_midiin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_midiin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_midiin_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_midiin_is_connected(AgsConnectable *connectable)
{
  AgsMidiin *midiin;
  
  gboolean is_connected;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(connectable);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(midiin_mutex);

  is_connected = (((AGS_MIDIIN_CONNECTED & (midiin->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(midiin_mutex);
  
  return(is_connected);
}

void
ags_midiin_connect(AgsConnectable *connectable)
{
  AgsMidiin *midiin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  midiin = AGS_MIDIIN(connectable);

  ags_midiin_set_flags(midiin, AGS_MIDIIN_CONNECTED);
}

void
ags_midiin_disconnect(AgsConnectable *connectable)
{

  AgsMidiin *midiin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  midiin = AGS_MIDIIN(connectable);
  
  ags_midiin_unset_flags(midiin, AGS_MIDIIN_CONNECTED);
}

/**
 * ags_midiin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_midiin_get_class_mutex()
{
  return(&ags_midiin_class_mutex);
}

/**
 * ags_midiin_test_flags:
 * @midiin: the #AgsMidiin
 * @flags: the flags
 *
 * Test @flags to be set on @midiin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_midiin_test_flags(AgsMidiin *midiin, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *midiin_mutex;

  if(!AGS_IS_MIDIIN(midiin)){
    return(FALSE);
  }

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* test */
  pthread_mutex_lock(midiin_mutex);

  retval = (flags & (midiin->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(midiin_mutex);

  return(retval);
}

/**
 * ags_midiin_set_flags:
 * @midiin: the #AgsMidiin
 * @flags: see #AgsMidiinFlags-enum
 *
 * Enable a feature of @midiin.
 *
 * Since: 2.0.0
 */
void
ags_midiin_set_flags(AgsMidiin *midiin, guint flags)
{
  pthread_mutex_t *midiin_mutex;

  if(!AGS_IS_MIDIIN(midiin)){
    return;
  }

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(midiin_mutex);

  midiin->flags |= flags;
  
  pthread_mutex_unlock(midiin_mutex);
}
    
/**
 * ags_midiin_unset_flags:
 * @midiin: the #AgsMidiin
 * @flags: see #AgsMidiinFlags-enum
 *
 * Disable a feature of @midiin.
 *
 * Since: 2.0.0
 */
void
ags_midiin_unset_flags(AgsMidiin *midiin, guint flags)
{  
  pthread_mutex_t *midiin_mutex;

  if(!AGS_IS_MIDIIN(midiin)){
    return;
  }

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(midiin_mutex);

  midiin->flags &= (~flags);
  
  pthread_mutex_unlock(midiin_mutex);
}

void
ags_midiin_set_application_context(AgsSequencer *sequencer,
				   AgsApplicationContext *application_context)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set application context */
  pthread_mutex_lock(midiin_mutex);
  
  midiin->application_context = (GObject *) application_context;
  
  pthread_mutex_unlock(midiin_mutex);
}

AgsApplicationContext*
ags_midiin_get_application_context(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* get application context */
  pthread_mutex_lock(midiin_mutex);

  application_context = (AgsApplicationContext *) midiin->application_context;

  pthread_mutex_unlock(midiin_mutex);
  
  return(application_context);
}

void
ags_midiin_set_device(AgsSequencer *sequencer,
		      gchar *device)
{
  AgsMidiin *midiin;
  
  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set device */
  pthread_mutex_lock(midiin_mutex);

  if((AGS_MIDIIN_ALSA & (midiin->flags)) != 0){
    midiin->in.alsa.device = g_strdup(device);
  }else if((AGS_MIDIIN_OSS & (midiin->flags)) != 0){
    midiin->in.oss.device = g_strdup(device);
  }
  
  pthread_mutex_unlock(midiin_mutex);
}

gchar*
ags_midiin_get_device(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gchar *device;

  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);
  
  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  device = NULL;
  
  pthread_mutex_lock(midiin_mutex);

  if((AGS_MIDIIN_ALSA & (midiin->flags)) != 0){
    device = g_strdup(midiin->in.alsa.device);
  }else if((AGS_MIDIIN_OSS & (midiin->flags)) != 0){
    device = g_strdup(midiin->in.oss.device);
  }

  pthread_mutex_unlock(midiin_mutex);

  return(device);
}

void
ags_midiin_list_cards(AgsSequencer *sequencer,
		      GList **card_id, GList **card_name)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  pthread_mutex_lock(midiin_mutex);

  if((AGS_MIDIIN_ALSA & (midiin->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_ctl_t *card_handle;
    snd_ctl_card_info_t *card_info;
    char *name;
    gchar *str;
    int card_num;
    int device;
    int error;

    card_num = -1;

    while(TRUE){
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
	continue;
      }

      snd_ctl_card_info_alloca(&card_info);
      error = snd_ctl_card_info(card_handle, card_info);

      if(error < 0){
	continue;
      }

      device = -1;
      error = snd_ctl_rawmidi_next_device(card_handle, &device);
    
      if(error < 0){
	continue;
      }

      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id, str);
      }
      
      if(card_name != NULL){
	*card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));
      }
      
      snd_ctl_close(card_handle);
    }

    snd_config_update_free_global();
#endif
  }else{
#ifdef AGS_WITH_OSS
    oss_sysinfo sysinfo;
    oss_midi_info mi;

    char *mixer_device;
    
    int mixerfd = -1;

    int next, n;
    int i;

    if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
      mixer_device = "/dev/mixer";
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

      if(next <= 0){
	break;
      }
    }
#endif
  }

  pthread_mutex_unlock(midiin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

gboolean
ags_midiin_is_starting(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gboolean is_starting;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(midiin_mutex);

  is_starting = ((AGS_MIDIIN_START_RECORD & (midiin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(midiin_mutex);
  
  return(is_starting);
}

gboolean
ags_midiin_is_recording(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gboolean is_recording;
  
  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);
  
  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(midiin_mutex);

  is_recording = ((AGS_MIDIIN_RECORD & (midiin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(midiin_mutex);

  return(is_recording);
}

void
ags_midiin_delegate_record_init(AgsSequencer *sequencer,
				GError **error)
{
  AgsMidiin *midiin;

  midiin = AGS_MIDIIN(sequencer);

  if(ags_midiin_test_flags(midiin, AGS_MIDIIN_ALSA)){
    ags_midiin_alsa_init(sequencer,
			error);
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_OSS)){
    ags_midiin_oss_init(sequencer,
		       error);
  }
}

void
ags_midiin_delegate_record(AgsSequencer *sequencer,
			   GError **error)
{
  AgsMidiin *midiin;

  midiin = AGS_MIDIIN(sequencer);

  if(ags_midiin_test_flags(midiin, AGS_MIDIIN_ALSA)){
    ags_midiin_alsa_record(sequencer,
			  error);
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_OSS)){
    ags_midiin_oss_record(sequencer,
			 error);
  }
}

void
ags_midiin_delegate_stop(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  midiin = AGS_MIDIIN(sequencer);

  if(ags_midiin_test_flags(midiin, AGS_MIDIIN_ALSA)){
    ags_midiin_alsa_free(sequencer);
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_OSS)){
    ags_midiin_oss_free(sequencer);
  }
}

void
ags_midiin_oss_init(AgsSequencer *sequencer,
		    GError **error)
{
  AgsMidiin *midiin;
  
  gchar *str;

  guint word_size;
  int format;
  int tmp;

  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* prepare for playback */
  pthread_mutex_lock(midiin_mutex);
    
  midiin->flags |= (AGS_MIDIIN_BUFFER3 |
		    AGS_MIDIIN_START_RECORD |
		    AGS_MIDIIN_RECORD |
		    AGS_MIDIIN_NONBLOCKING);

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = midiin->in.oss.device;
  midiin->in.oss.device_fd = open(str, O_WRONLY, 0);

  if(midiin->in.oss.device_fd == -1){
    pthread_mutex_unlock(midiin_mutex);

    g_warning("couldn't open device %s", midiin->in.oss.device);

    if(error != NULL){
      g_set_error(error,
		  AGS_MIDIIN_ERROR,
		  AGS_MIDIIN_ERROR_LOCKED_SEQUENCER,
		  "unable to open MIDI device: %s\n",
		  str);
    }

    return;
  }
#endif
  
  midiin->tact_counter = 0.0;
  midiin->delay_counter = 0.0;
  midiin->tic_counter = 0;

#ifdef AGS_WITH_OSS
  midiin->flags |= AGS_MIDIIN_INITIALIZED;
#endif
  
  midiin->flags |= AGS_MIDIIN_BUFFER0;
  midiin->flags &= (~(AGS_MIDIIN_BUFFER1 |
		      AGS_MIDIIN_BUFFER2 |
		      AGS_MIDIIN_BUFFER3));
  
  pthread_mutex_unlock(midiin_mutex);
}

void
ags_midiin_oss_record(AgsSequencer *sequencer,
		      GError **error)
{
  AgsMidiin *midiin;

  AgsTicDevice *tic_device;
  //  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsThread *task_thread;

  AgsApplicationContext *application_context;

  GList *task;

  char **ring_buffer;
  char buf[128];
  
  gboolean no_event;
  guint nth_buffer;
  guint nth_ring_buffer;
  guint ring_buffer_size;
  int device_fd;
  int num_read;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* poll MIDI device */
  pthread_mutex_lock(midiin_mutex);

  midiin->flags &= (~AGS_MIDIIN_START_RECORD);

  if((AGS_MIDIIN_INITIALIZED & (midiin->flags)) == 0){
    pthread_mutex_unlock(midiin_mutex);
    
    return;
  }

  device_fd = midiin->in.oss.device_fd;

  /* nth buffer */
  if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
    nth_buffer = 1;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
    nth_buffer = 2;
    nth_ring_buffer = 1;
  }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
    nth_buffer = 3;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
    nth_buffer = 0;
    nth_ring_buffer = 1;
  }

  if(midiin->ring_buffer[nth_ring_buffer] != NULL){
    free(midiin->ring_buffer[nth_ring_buffer]);
  }
      
  midiin->ring_buffer[nth_ring_buffer] = NULL;
  midiin->ring_buffer_size[nth_ring_buffer] = 0;
      
  ring_buffer = midiin->ring_buffer;
  ring_buffer_size = midiin->ring_buffer_size[nth_ring_buffer];
  
  pthread_mutex_unlock(midiin_mutex);

  num_read = 1;
  
  while(num_read > 0){
    num_read = 0;
    
#ifdef AGS_WITH_OSS
    num_read = read(device_fd, buf, sizeof(buf));
    
    if((num_read < 0)){
      g_warning("Problem reading MIDI input");
    }

    if(num_read > 0){
      if(ceil((ring_buffer_size + num_read) / AGS_MIDIIN_DEFAULT_BUFFER_SIZE) > ceil(ring_buffer_size / AGS_MIDIIN_DEFAULT_BUFFER_SIZE)){
	if(ring_buffer[nth_ring_buffer] == NULL){
	  ring_buffer[nth_ring_buffer] = (char *) malloc(AGS_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
	}else{
	  ring_buffer[nth_ring_buffer] = (char *) realloc(ring_buffer[nth_ring_buffer],
							  (AGS_MIDIIN_DEFAULT_BUFFER_SIZE * ceil(ring_buffer_size / AGS_MIDIIN_DEFAULT_BUFFER_SIZE) + AGS_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
	}
      }

      memcpy(&(ring_buffer[nth_ring_buffer][ring_buffer_size]),
	     buf,
	     num_read);
      ring_buffer_size += num_read;
    }
#endif
  }
      
  /* switch buffer */
  pthread_mutex_lock(midiin_mutex);

  /* update byte array and buffer size */
  if(midiin->buffer[nth_buffer] != NULL){
    free(midiin->buffer[nth_buffer]);
  }

  midiin->buffer[nth_buffer] = NULL;
      
  midiin->ring_buffer_size[nth_ring_buffer] = ring_buffer_size;

  midiin->buffer_size[nth_buffer] = ring_buffer_size;

  /* fill buffer */
  if(ring_buffer_size > 0){
    midiin->buffer[nth_buffer] = (char *) malloc(ring_buffer_size * sizeof(char));
	
    memcpy(midiin->buffer[nth_buffer], ring_buffer[nth_ring_buffer], ring_buffer_size * sizeof(char));
  }
      
  pthread_mutex_unlock(midiin_mutex);

  pthread_mutex_lock(midiin_mutex);

  ags_midiin_switch_buffer_flag(midiin);
    
  device_fd = midiin->in.oss.device_fd;
      
  /* nth buffer */
  if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
    nth_buffer = 1;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
    nth_buffer = 2;
    nth_ring_buffer = 1;
  }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
    nth_buffer = 3;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
    nth_buffer = 0;
    nth_ring_buffer = 1;
  }

  if(midiin->ring_buffer[nth_ring_buffer] != NULL){
    free(midiin->ring_buffer[nth_ring_buffer]);
  }
      
  midiin->ring_buffer[nth_ring_buffer] = NULL;
  midiin->ring_buffer_size[nth_ring_buffer] = 0;
      
  pthread_mutex_unlock(midiin_mutex);

  /* update sequencer */
  task_thread = ags_thread_find_type((AgsThread *) application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
  task = NULL;

  /* tic sequencer */
  tic_device = ags_tic_device_new((GObject *) midiin);
  task = g_list_append(task,
		       tic_device);
  
  /* reset - switch buffer flags */
  //  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) midiin);
  //  task = g_list_append(task,
  //		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
}

void
ags_midiin_oss_free(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  AgsApplicationContext *application_context;

  GList *poll_fd;

  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  pthread_mutex_lock(midiin_mutex);  

  if((AGS_MIDIIN_INITIALIZED & (midiin->flags)) == 0){
    pthread_mutex_unlock(midiin_mutex);  

    return;
  }

  midiin->flags &= (~(AGS_MIDIIN_RECORD |
		      AGS_MIDIIN_INITIALIZED));
  
  pthread_mutex_unlock(midiin_mutex);  

  /*  */
  pthread_mutex_lock(midiin_mutex);  

  close(midiin->in.oss.device_fd);
  midiin->in.oss.device_fd = -1;

  midiin->flags &= (~(AGS_MIDIIN_BUFFER0 |
		      AGS_MIDIIN_BUFFER1 |
		      AGS_MIDIIN_BUFFER2 |
		      AGS_MIDIIN_BUFFER3));

  g_atomic_int_or(&(midiin->sync_flags),
		  AGS_MIDIIN_PASS_THROUGH);

  midiin->note_offset = 0;
  
  pthread_mutex_unlock(midiin_mutex);  
}  

void
ags_midiin_alsa_init(AgsSequencer *sequencer,
		     GError **error)
{
  AgsMidiin *midiin;

#ifdef AGS_WITH_ALSA
  int mode = SND_RAWMIDI_NONBLOCK;
  snd_rawmidi_t* handle = NULL;
#endif

  int err;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /*  */
  pthread_mutex_lock(midiin_mutex);

  /* prepare for record */
  midiin->flags |= (AGS_MIDIIN_BUFFER3 |
		    AGS_MIDIIN_START_RECORD |
		    AGS_MIDIIN_RECORD |
		    AGS_MIDIIN_NONBLOCKING);

  midiin->note_offset = 0;

#ifdef AGS_WITH_ALSA
  mode = SND_RAWMIDI_NONBLOCK;
  
  if((err = snd_rawmidi_open(&handle, NULL, midiin->in.alsa.device, mode)) < 0) {
    pthread_mutex_unlock(midiin_mutex);

    printf("Record midi open error: %s\n", snd_strerror(err));
    g_set_error(error,
		AGS_MIDIIN_ERROR,
		AGS_MIDIIN_ERROR_LOCKED_SEQUENCER,
		"unable to open midi device: %s\n",
		snd_strerror(err));

    return;
  }
  
  /*  */
  midiin->in.alsa.handle = handle;
#endif

  midiin->tact_counter = 0.0;
  midiin->delay_counter = 0.0;
  midiin->tic_counter = 0;

#ifdef AGS_WITH_ALSA
  midiin->flags |= AGS_MIDIIN_INITIALIZED;
#endif

  pthread_mutex_unlock(midiin_mutex);
}

void
ags_midiin_alsa_record(AgsSequencer *sequencer,
		       GError **error)
{
  AgsMidiin *midiin;

  AgsTicDevice *tic_device;
  //  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsThread *task_thread;

  AgsApplicationContext *application_context;

  GList *task;

#ifdef AGS_WITH_ALSA
  snd_rawmidi_t *device_handle;
#else
  gpointer device_handle;
#endif

  char **ring_buffer;
  
  gboolean no_event;
  guint nth_buffer;
  guint nth_ring_buffer;
  guint ring_buffer_size;
  int status;
  unsigned char c;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* prepare poll */
  pthread_mutex_lock(midiin_mutex);

  midiin->flags &= (~AGS_MIDIIN_START_RECORD);

  if((AGS_MIDIIN_INITIALIZED & (midiin->flags)) == 0){
    pthread_mutex_unlock(midiin_mutex);
    
    return;
  }

  device_handle = midiin->in.alsa.handle;
      
  /* nth buffer */
  if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
    nth_buffer = 1;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
    nth_buffer = 2;
    nth_ring_buffer = 1;
  }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
    nth_buffer = 3;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
    nth_buffer = 0;
    nth_ring_buffer = 1;
  }

  if(midiin->ring_buffer[nth_ring_buffer] != NULL){
    free(midiin->ring_buffer[nth_ring_buffer]);
  }
      
  midiin->ring_buffer[nth_ring_buffer] = NULL;
  midiin->ring_buffer_size[nth_ring_buffer] = 0;
      
  ring_buffer = midiin->ring_buffer;
  ring_buffer_size = midiin->ring_buffer_size[nth_ring_buffer];

  pthread_mutex_unlock(midiin_mutex);

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
      if(ring_buffer_size % AGS_MIDIIN_DEFAULT_BUFFER_SIZE == 0){
	if(ring_buffer[nth_ring_buffer] == NULL){
	  ring_buffer[nth_ring_buffer] = (char *) malloc(AGS_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
	}else{
	  ring_buffer[nth_ring_buffer] = (char *) realloc(ring_buffer[nth_ring_buffer],
							  (ring_buffer_size + AGS_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
	}
      }

      ring_buffer[nth_ring_buffer][ring_buffer_size] = (unsigned char) c;
      ring_buffer_size += 1;
    }
#endif
  }

  /* switch buffer */
  pthread_mutex_lock(midiin_mutex);

  /* update byte array and buffer size */
  if(midiin->buffer[nth_buffer] != NULL){
    free(midiin->buffer[nth_buffer]);
  }
      
  midiin->buffer[nth_buffer] = NULL;

  midiin->ring_buffer_size[nth_ring_buffer] = ring_buffer_size;
      
  midiin->buffer_size[nth_buffer] = ring_buffer_size;
      
  /* fill buffer */
  if(ring_buffer_size > 0){
    midiin->buffer[nth_buffer] = (char *) malloc(ring_buffer_size * sizeof(char));
	
    memcpy(midiin->buffer[nth_buffer], ring_buffer[nth_ring_buffer], ring_buffer_size * sizeof(char));
  }
      
  pthread_mutex_unlock(midiin_mutex);

  /*  */
  pthread_mutex_lock(midiin_mutex);

  ags_midiin_switch_buffer_flag(midiin);

  device_handle = midiin->in.alsa.handle;
      
  /* nth buffer */
  if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
    nth_buffer = 1;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
    nth_buffer = 2;
    nth_ring_buffer = 1;
  }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
    nth_buffer = 3;
    nth_ring_buffer = 0;
  }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
    nth_buffer = 0;
    nth_ring_buffer = 1;
  }

  if(midiin->ring_buffer[nth_ring_buffer] != NULL){
    free(midiin->ring_buffer[nth_ring_buffer]);
  }
      
  midiin->ring_buffer[nth_ring_buffer] = NULL;
  midiin->ring_buffer_size[nth_ring_buffer] = 0;

  pthread_mutex_unlock(midiin_mutex);
  
  /* update sequencer */
  task_thread = ags_thread_find_type((AgsThread *) application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
  task = NULL;

  /* tic sequencer */
  tic_device = ags_tic_device_new((GObject *) midiin);
  task = g_list_append(task,
		       tic_device);
  
  /* reset - switch buffer flags */
  //  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) midiin);
  //  task = g_list_append(task,
  //		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
}

void
ags_midiin_alsa_free(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /*  */
  pthread_mutex_lock(midiin_mutex);

  if((AGS_MIDIIN_INITIALIZED & (midiin->flags)) == 0){
    pthread_mutex_unlock(midiin_mutex);
    
    return;
  }

  midiin->flags &= (~(AGS_MIDIIN_RECORD |
		      AGS_MIDIIN_INITIALIZED));

  pthread_mutex_unlock(midiin_mutex);

  /*  */
  pthread_mutex_lock(midiin_mutex);

#ifdef AGS_WITH_ALSA
  snd_rawmidi_close(midiin->in.alsa.handle);
#endif

  midiin->in.alsa.handle = NULL;
  midiin->flags &= (~(AGS_MIDIIN_BUFFER0 |
		      AGS_MIDIIN_BUFFER1 |
		      AGS_MIDIIN_BUFFER2 |
		      AGS_MIDIIN_BUFFER3));

  g_atomic_int_or(&(midiin->sync_flags),
		  AGS_MIDIIN_PASS_THROUGH);

  if(midiin->buffer[1] != NULL){
    free(midiin->buffer[1]);
    midiin->buffer_size[1] = 0;
  }

  if(midiin->buffer[2] != NULL){
    free(midiin->buffer[2]);
    midiin->buffer_size[2] = 0;
  }

  if(midiin->buffer[3] != NULL){
    free(midiin->buffer[3]);
    midiin->buffer_size[3] = 0;
  }

  if(midiin->buffer[0] != NULL){
    free(midiin->buffer[0]);
    midiin->buffer_size[0] = 0;
  }

  pthread_mutex_unlock(midiin_mutex);  
}

void
ags_midiin_tic(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gdouble delay;
  gdouble delay_counter;    
  guint note_offset;
  
  pthread_mutex_t *midiin_mutex;

  midiin = AGS_MIDIIN(sequencer);
  
  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());
  
  /* determine if attack should be switched */
  pthread_mutex_lock(midiin_mutex);

  delay = midiin->delay;
  delay_counter = midiin->delay_counter;

  note_offset = midiin->note_offset;

  pthread_mutex_unlock(midiin_mutex);

  if((guint) delay_counter + 1 >= (guint) delay){
    ags_sequencer_set_note_offset(sequencer,
				  note_offset + 1);

    /* delay */
    ags_sequencer_offset_changed(sequencer,
				 note_offset);
    
    /* reset - delay counter */
    pthread_mutex_lock(midiin_mutex);

    midiin->delay_counter = 0.0;
    midiin->tact_counter += 1.0;

    pthread_mutex_unlock(midiin_mutex);
  }else{
    pthread_mutex_lock(midiin_mutex);

    midiin->delay_counter += 1.0;

    pthread_mutex_unlock(midiin_mutex);
  }
}

void
ags_midiin_offset_changed(AgsSequencer *sequencer,
			  guint note_offset)
{
  AgsMidiin *midiin;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* offset changed */
  pthread_mutex_lock(midiin_mutex);

  midiin->tic_counter += 1;

  if(midiin->tic_counter == AGS_SEQUENCER_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    midiin->tic_counter = 0;
  }

  pthread_mutex_unlock(midiin_mutex);
}

void
ags_midiin_set_bpm(AgsSequencer *sequencer,
		   gdouble bpm)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set bpm */
  pthread_mutex_lock(midiin_mutex);

  midiin->bpm = bpm;

  pthread_mutex_unlock(midiin_mutex);

  ags_midiin_adjust_delay_and_attack(midiin);
}

gdouble
ags_midiin_get_bpm(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gdouble bpm;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* get bpm */
  pthread_mutex_lock(midiin_mutex);

  bpm = midiin->bpm;
  
  pthread_mutex_unlock(midiin_mutex);

  return(bpm);
}

void
ags_midiin_set_delay_factor(AgsSequencer *sequencer,
			   gdouble delay_factor)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set delay factor */
  pthread_mutex_lock(midiin_mutex);

  midiin->delay_factor = delay_factor;

  pthread_mutex_unlock(midiin_mutex);

  ags_midiin_adjust_delay_and_attack(midiin);
}

gdouble
ags_midiin_get_delay_factor(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  gdouble delay_factor;
  
  pthread_mutex_t *midiin_mutex;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* get delay factor */
  pthread_mutex_lock(midiin_mutex);

  delay_factor = midiin->delay_factor;
  
  pthread_mutex_unlock(midiin_mutex);

  return(delay_factor);
}

void*
ags_midiin_get_buffer(AgsSequencer *sequencer,
		      guint *buffer_length)
{
  AgsMidiin *midiin;
  char *buffer;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get buffer */
  if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
    buffer = midiin->buffer[0];
  }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
    buffer = midiin->buffer[1];
  }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
    buffer = midiin->buffer[2];
  }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
    buffer = midiin->buffer[3];
  }else{
    buffer = NULL;
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if((AGS_MIDIIN_BUFFER0 & (midiin->flags)) != 0){
      *buffer_length = midiin->buffer_size[0];
    }else if((AGS_MIDIIN_BUFFER1 & (midiin->flags)) != 0){
      *buffer_length = midiin->buffer_size[1];
    }else if((AGS_MIDIIN_BUFFER2 & (midiin->flags)) != 0){
      *buffer_length = midiin->buffer_size[2];
    }else if((AGS_MIDIIN_BUFFER3 & (midiin->flags)) != 0){
      *buffer_length = midiin->buffer_size[3];
    }else{
      *buffer_length = 0;
    }
  }
  
  return(buffer);
}

void*
ags_midiin_get_next_buffer(AgsSequencer *sequencer,
			   guint *buffer_length)
{
  AgsMidiin *midiin;
  char *buffer;
  
  midiin = AGS_MIDIIN(sequencer);

  /* get buffer */
  if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER0)){
    buffer = midiin->buffer[1];
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER1)){
    buffer = midiin->buffer[2];
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER2)){
    buffer = midiin->buffer[3];
  }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER3)){
    buffer = midiin->buffer[0];
  }else{
    buffer = NULL;
  }

  /* return the buffer's length */
  if(buffer_length != NULL){
    if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER0)){
      *buffer_length = midiin->buffer_size[1];
    }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER1)){
      *buffer_length = midiin->buffer_size[2];
    }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER2)){
      *buffer_length = midiin->buffer_size[3];
    }else if(ags_midiin_test_flags(midiin, AGS_MIDIIN_BUFFER3)){
      *buffer_length = midiin->buffer_size[0];
    }else{
      *buffer_length = 0;
    }
  }
  
  return(buffer);
}

void
ags_midiin_set_note_offset(AgsSequencer *sequencer,
			  guint note_offset)
{
  AgsMidiin *midiin;

  pthread_mutex_t *midiin_mutex;  

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(midiin_mutex);

  midiin->note_offset = note_offset;

  pthread_mutex_unlock(midiin_mutex);
}

guint
ags_midiin_get_note_offset(AgsSequencer *sequencer)
{
  AgsMidiin *midiin;

  guint note_offset;
  
  pthread_mutex_t *midiin_mutex;  

  midiin = AGS_MIDIIN(sequencer);

  /* get midiin mutex */
  pthread_mutex_lock(ags_midiin_get_class_mutex());
  
  midiin_mutex = midiin->obj_mutex;
  
  pthread_mutex_unlock(ags_midiin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(midiin_mutex);

  note_offset = midiin->note_offset;

  pthread_mutex_unlock(midiin_mutex);

  return(note_offset);
}

/**
 * ags_midiin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates a new instance of #AgsMidiin.
 *
 * Returns: the new #AgsMidiin
 *
 * Since: 2.0.0
 */
AgsMidiin*
ags_midiin_new(GObject *application_context)
{
  AgsMidiin *midiin;

  midiin = (AgsMidiin *) g_object_new(AGS_TYPE_MIDIIN,
				      "application-context", application_context,
				      NULL);
  
  return(midiin);
}
