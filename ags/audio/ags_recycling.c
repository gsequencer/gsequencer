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

#include <ags/audio/ags_recycling.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <string.h>
#include <math.h>

#include <pthread.h>

#include <ags/i18n.h>

void ags_recycling_class_init(AgsRecyclingClass *recycling_class);
void ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recycling_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_recycling_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_recycling_init(AgsRecycling *recycling);
void ags_recycling_dispose(GObject *gobject);
void ags_recycling_finalize(GObject *gobject);

AgsUUID* ags_recycling_get_uuid(AgsConnectable *connectable);
gboolean ags_recycling_has_resource(AgsConnectable *connectable);
gboolean ags_recycling_is_ready(AgsConnectable *connectable);
void ags_recycling_add_to_registry(AgsConnectable *connectable);
void ags_recycling_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_recycling_list_resource(AgsConnectable *connectable);
xmlNode* ags_recycling_xml_compose(AgsConnectable *connectable);
void ags_recycling_xml_parse(AgsConnectable *connectable,
			     xmlNode *node);
gboolean ags_recycling_is_connected(AgsConnectable *connectable);
void ags_recycling_connect(AgsConnectable *connectable);
void ags_recycling_disconnect(AgsConnectable *connectable);

void ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
					 AgsAudioSignal *audio_signal);

void ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
					    AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_recycling
 * @short_description: recycling container of audio signals
 * @title: AgsRecycling
 * @section_id:
 * @include: ags/audio/ags_recycling.h
 *
 * #AgsRecycling forms the nested tree of AgsChannel. Every channel
 * owning audio signal contains therefor an #AgsRecycling.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_OUTPUT_SOUNDCARD,
  PROP_OUTPUT_SOUNDCARD_CHANNEL,
  PROP_INPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_PARENT,
  PROP_NEXT,
  PROP_PREV,
  PROP_AUDIO_SIGNAL,
};

enum{
  ADD_AUDIO_SIGNAL,
  REMOVE_AUDIO_SIGNAL,
  DATA_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_recycling_parent_class = NULL;
static guint recycling_signals[LAST_SIGNAL];

static pthread_mutex_t ags_recycling_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_recycling_get_type(void)
{
  static GType ags_type_recycling = 0;

  if(!ags_type_recycling){
    static const GTypeInfo ags_recycling_info = {
      sizeof (AgsRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recycling = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecycling",
						&ags_recycling_info, 0);

    g_type_add_interface_static(ags_type_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_recycling);
}

void
ags_recycling_class_init(AgsRecyclingClass *recycling)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recycling_parent_class = g_type_class_peek_parent(recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) recycling;

  gobject->set_property = ags_recycling_set_property;
  gobject->get_property = ags_recycling_get_property;

  gobject->dispose = ags_recycling_dispose;
  gobject->finalize = ags_recycling_finalize;

  /* properties */
  /**
   * AgsRecycling:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("channel",
				   "assigned channel",
				   "The channel it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsRecycling:output-soundcard:
   *
   * The assigned output soundcard acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-soundcard",
				   "assigned output soundcard",
				   "The output soundcard it is assigned with",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecycling:output-soundcard-channel:
   *
   * The output soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("output-soundcard-channel",
				 i18n_pspec("output soundcard channel"),
				 i18n_pspec("The output soundcard channel"),
				 -1,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD_CHANNEL,
				  param_spec);
  
  /**
   * AgsRecycling:input-soundcard:
   *
   * The assigned input soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-soundcard",
				   "assigned input soundcard",
				   "The input soundcard it is assigned with",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecycling:input-soundcard-channel:
   *
   * The input soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("input-soundcard-channel",
				 i18n_pspec("input soundcard channel"),
				 i18n_pspec("The input soundcard channel"),
				 -1,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD_CHANNEL,
				  param_spec);
  
  /**
   * AgsRecycling:samplerate:
   *
   * The samplerate.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate"),
				  i18n_pspec("The samplerate"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsRecycling:buffer-size:
   *
   * The buffer size.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size"),
				  i18n_pspec("The buffer size"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsRecycling:format:
   *
   * The format.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format"),
				  i18n_pspec("The format"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsRecycling:parent:
   *
   * The assigned parent #AgsRecycling.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("parent",
				   "assigned parent",
				   "The parent it is assigned with",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);

  /**
   * AgsRecycling:prev:
   *
   * The assigned prev #AgsRecycling.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("prev",
				   "assigned prev",
				   "The prev it is assigned with",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PREV,
				  param_spec);

  /**
   * AgsRecycling:next:
   *
   * The assigned next #AgsRecycling.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("next",
				   "assigned next",
				   "The next it is assigned with",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT,
				  param_spec);

  /**
   * AgsRecycling:audio-signal:
   *
   * The containing  #AgsAudioSignal.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("audio-signal",
				    "containing audio signal",
				    "The audio signal it contains",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);
  
  /*  */
  recycling->add_audio_signal = ags_recycling_real_add_audio_signal;
  recycling->remove_audio_signal = ags_recycling_real_remove_audio_signal;

  /**
   * AgsRecycling::add-audio-signal
   * @recycling: an #AgsRecycling
   * @audio_signal: the #AgsAudioSignal to add
   *
   * The ::add-audio-signal signal is emited as adding #AgsAudioSignal.
   * 
   * Since: 2.0.0
   */
  recycling_signals[ADD_AUDIO_SIGNAL] =
    g_signal_new("add-audio-signal",
		 G_TYPE_FROM_CLASS(recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecyclingClass, add_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsRecycling::remove-audio-signal:
   * @recycling: an #AgsRecycling
   * @audio_signal: the #AgsAudioSignal to remove
   *
   * The ::remove-audio-signal signal is emited as removing #AgsAudioSignal.
   * 
   * Since: 2.0.0
   */
  recycling_signals[REMOVE_AUDIO_SIGNAL] =
    g_signal_new("remove-audio-signal",
		 G_TYPE_FROM_CLASS(recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecyclingClass, remove_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsRecycling::data-request
   * @recycling: an #AgsRecycling
   * @audio_signal: the #AgsAudioSignal to request
   *
   * The ::data-request signal is emited as requesting data for @audio_signal.
   * 
   * Since: 2.0.0
   */
  recycling_signals[DATA_REQUEST] =
    g_signal_new("data-request",
		 G_TYPE_FROM_CLASS(recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecyclingClass, data_request),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_recycling_get_uuid;
  connectable->has_resource = ags_recycling_has_resource;

  connectable->is_ready = ags_recycling_is_ready;
  connectable->add_to_registry = ags_recycling_add_to_registry;
  connectable->remove_from_registry = ags_recycling_remove_from_registry;

  connectable->list_resource = ags_recycling_list_resource;
  connectable->xml_compose = ags_recycling_xml_compose;
  connectable->xml_parse = ags_recycling_xml_parse;

  connectable->is_connected = ags_recycling_is_connected;  
  connectable->connect = ags_recycling_connect;
  connectable->disconnect = ags_recycling_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_recycling_init(AgsRecycling *recycling)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;
  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  recycling->flags = 0;

  /* add recycling mutex */
  recycling->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  recycling->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  recycling->uuid = ags_uuid_alloc();
  ags_uuid_generate(recycling->uuid);

  /* config */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  config = ags_config_get_instance();

  /* base init */
  recycling->channel = NULL;

  recycling->output_soundcard = NULL;
  recycling->output_soundcard_channel = 0;

  recycling->input_soundcard = NULL;
  recycling->input_soundcard_channel = 0;

  /* presets */
  recycling->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  recycling->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  recycling->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* read config */
  pthread_mutex_lock(application_mutex);

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    recycling->samplerate = g_ascii_strtoull(str,
					     NULL,
					     10);

    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    recycling->buffer_size = g_ascii_strtoull(str,
					      NULL,
					      10);

    free(str);
  }
  
  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    recycling->format = g_ascii_strtoull(str,
					 NULL,
					 10);

    free(str);
  }

  pthread_mutex_unlock(application_mutex);
  
  /* nested tree */
  recycling->parent = NULL;

  recycling->next = NULL;
  recycling->prev = NULL;

  recycling->audio_signal = NULL;
}

void
ags_recycling_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecycling *recycling;
  
  pthread_mutex_t *recycling_mutex;

  recycling = AGS_RECYCLING(gobject);

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      pthread_mutex_lock(recycling_mutex);

      if(recycling->channel == (GObject *) channel){
	pthread_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->channel != NULL){
	g_object_unref(recycling->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      recycling->channel = (GObject *) channel;

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_recycling_set_output_soundcard(recycling, (GObject *) soundcard);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recycling_mutex);

      channel->output_soundcard_channel = g_value_get_int(value);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_recycling_set_input_soundcard(recycling, (GObject *) soundcard);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recycling_mutex);

      channel->input_soundcard_channel = g_value_get_int(value);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_recycling_set_samplerate(recycling,
				   samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_recycling_set_buffer_size(recycling,
				    buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      ags_recycling_set_format(recycling,
			       format);
    }
    break;
  case PROP_PARENT:
    {
      AgsRecycling *parent;

      parent = (AgsRecycling *) g_value_get_object(value);

      pthread_mutex_lock(recycling_mutex);

      if(recycling->parent == parent){
	pthread_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->parent != NULL){
	g_object_unref(recycling->parent);
      }

      if(parent != NULL){
	g_object_ref(parent);
      }

      recycling->parent = parent;

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_NEXT:
    {
      AgsRecycling *next;

      next = (AgsRecycling *) g_value_get_object(value);

      pthread_mutex_lock(recycling_mutex);

      if(recycling->next == next){
	pthread_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->next != NULL){
	g_object_unref(recycling->next);
      }

      if(next != NULL){
	g_object_ref(next);
      }

      recycling->next = next;

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_PREV:
    {
      AgsRecycling *prev;

      prev = (AgsRecycling *) g_value_get_object(value);

      pthread_mutex_lock(recycling_mutex);

      if(recycling->prev == prev){
	pthread_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->prev != NULL){
	g_object_unref(recycling->prev);
      }

      if(prev != NULL){
	g_object_ref(prev);
      }

      recycling->prev = prev;

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = g_value_get_pointer(value);

      pthread_mutex_lock(recycling_mutex);

      if(audio_signal == NULL ||
	 g_list_find(recycling->audio_signal, audio_signal) != NULL){
	pthread_mutex_unlock(recycling_mutex);
	
	return;
      }

      pthread_mutex_unlock(recycling_mutex);

      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecycling *recycling;
  
  pthread_mutex_t *recycling_mutex;

  recycling = AGS_RECYCLING(gobject);

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());
  
  switch(prop_id){
  case PROP_CHANNEL:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->channel);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->output_soundcard);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_int(value, recycling->output_soundcard_channel);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->input_soundcard);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_int(value, recycling->input_soundcard_channel);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_unint(value, recycling->samplerate);

      pthread_mutex_unlock(recycling_mutex);
    }
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_unint(value, recycling->buffer_size);

      pthread_mutex_unlock(recycling_mutex);
    }
  case PROP_FORMAT:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_unint(value, recycling->format);

      pthread_mutex_unlock(recycling_mutex);
    }
  case PROP_PARENT:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->parent);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_NEXT:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->next);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_PREV:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->prev);

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      pthread_mutex_lock(recycling_mutex);

      g_value_set_pointer(value, g_list_copy(recycling->audio_signal));

      pthread_mutex_unlock(recycling_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_dispose(GObject *gobject)
{
  AgsRecycling *recycling;  

  GList *list;

  recycling = AGS_RECYCLING(gobject);

  /* channel */
  if(recycling->channel != NULL){
    g_object_unref(recycling->channel);

    recycling->channel = NULL;
  }

  /* output soundcard */
  if(recycling->output_soundcard != NULL){
    g_object_unref(recycling->output_soundcard);

    recycling->output_soundcard = NULL;
  }

  /* input soundcard */
  if(recycling->input_soundcard != NULL){
    g_object_unref(recycling->input_soundcard);

    recycling->input_soundcard = NULL;
  }

  /* parent */
  if(recycling->parent != NULL){
    g_object_unref(recycling->parent);

    recycling->parent = NULL;
  }

  /* next and prev */
  if(recycling->next != NULL){
    g_object_unref(recycling->next);

    recycling->next = NULL;
  }

  if(recycling->prev != NULL){
    g_object_unref(recycling->prev);

    recycling->prev = NULL;
  }

  /* AgsAudioSignal */
  list = recycling->audio_signal;

  while(list != NULL){
    g_object_run_dispose(list->data);
    
    list = list->next;
  }
  
  g_list_free_full(recycling->audio_signal,
		   g_object_unref);

  recycling->audio_signal = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->dispose(gobject);
}

void
ags_recycling_finalize(GObject *gobject)
{
  AgsRecycling *recycling;  
  
  GList *list;
  
  recycling = AGS_RECYCLING(gobject);

  pthread_mutex_destroy(recycling->obj_mutex);
  free(recycling->obj_mutex);

  pthread_mutexattr_destroy(recycling->obj_mutexattr);
  free(recycling->obj_mutexattr);

  /* channel */
  if(recycling->channel != NULL){
    g_object_unref(recycling->channel);
  }

  /* output soundcard */
  if(recycling->output_soundcard != NULL){
    g_object_unref(recycling->output_soundcard);
  }

  /* input soundcard */
  if(recycling->input_soundcard != NULL){
    g_object_unref(recycling->input_soundcard);
  }

  /* parent */
  if(recycling->parent != NULL){
    g_object_unref(recycling->parent);
  }

  /* next and prev */
  if(recycling->next != NULL){
    g_object_unref(recycling->next);
  }

  if(recycling->prev != NULL){
    g_object_unref(recycling->prev);
  }

  /* AgsAudioSignal */
  list = recycling->audio_signal;

  while(list != NULL){
    g_object_run_dispose(list->data);
    
    list = list->next;
  }
  
  g_list_free_full(recycling->audio_signal,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->finalize(gobject);
}

AgsUUID*
ags_recycling_get_uuid(AgsConnectable *connectable)
{
  AgsRecycling *recycling;
  
  AgsUUID *ptr;

  pthread_mutex_t *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling signal mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(recycling_mutex);

  ptr = recycling->uuid;

  pthread_mutex_unlock(recycling_mutex);
  
  return(ptr);
}

gboolean
ags_recycling_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_recycling_is_ready(AgsConnectable *connectable)
{
  AgsRecycling *recycling;
  
  gboolean is_ready;

  pthread_mutex_t *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(recycling_mutex);

  is_ready = (((AGS_RECYCLING_ADDED_TO_REGISTRY & (recycling->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(recycling_mutex);
  
  return(is_ready);
}

void
ags_recycling_add_to_registry(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list_start, *list;

  pthread_mutex_t *recycling_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  ags_recycling_set_flags(recycling, AGS_RECYCLING_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) recycling);
    ags_registry_add_entry(registry,
			   entry);
  }
  
  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* add audio signal */
  pthread_mutex_lock(recycling_mutex);

  list =
    list_start = g_list_copy(recycling->audio_signal);

  pthread_mutex_unlock(recycling_mutex);

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_recycling_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_recycling_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_recycling_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_recycling_xml_parse(AgsConnectable *connectable,
		    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_recycling_is_connected(AgsConnectable *connectable)
{
  AgsRecycling *recycling;
  
  gboolean is_connected;

  pthread_mutex_t *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(recycling_mutex);

  is_connected = (((AGS_RECYCLING_CONNECTED & (recycling->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(recycling_mutex);
  
  return(is_connected);
}

void
ags_recycling_connect(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  GList *list_start, *list;

  pthread_mutex_t *recycling_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  ags_recycling_set_flags(recycling, AGS_RECYCLING_CONNECTED);

#ifdef AGS_DEBUG
  g_message("connecting recycling");
#endif

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* audio signal */
  pthread_mutex_lock(recycling_mutex);

  list =
    list_start = g_list_copy(recycling->audio_signal);

  pthread_mutex_unlock(recycling_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_recycling_disconnect(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  GList *list_start, *list;

  pthread_mutex_t *recycling_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  ags_recycling_unset_flags(recycling, AGS_RECYCLING_CONNECTED);

#ifdef AGS_DEBUG
  g_message("disconnecting recycling");
#endif

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* audio signal */
  pthread_mutex_lock(recycling_mutex);

  list =
    list_start = g_list_copy(recycling->audio_signal);

  pthread_mutex_unlock(recycling_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_recycling_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_recycling_get_class_mutex()
{
  return(&ags_recycling_class_mutex);
}

/**
 * ags_recycling_set_flags:
 * @recycling: the #AgsRecycling
 * @flags: see #AgsRecyclingFlags-enum
 *
 * Enable a feature of @recycling.
 *
 * Since: 2.0.0
 */
void
ags_recycling_set_flags(AgsRecycling *recycling, guint flags)
{
  pthread_mutex_t *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(recycling_mutex);

  recycling->flags |= flags;
  
  pthread_mutex_unlock(recycling_mutex);
}
    
/**
 * ags_recycling_unset_flags:
 * @recycling: the #AgsRecycling
 * @flags: see #AgsRecyclingFlags-enum
 *
 * Disable a feature of @recycling.
 *
 * Since: 2.0.0
 */
void
ags_recycling_unset_flags(AgsRecycling *recycling, guint flags)
{  
  pthread_mutex_t *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(recycling_mutex);

  recycling->flags &= (~flags);
  
  pthread_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_set_output_soundcard:
 * @recycling:  an #AgsRecycling
 * @output_soundcard: the #GObject to set
 *
 * Set @output_soundcard to recycling.
 *
 * Since: 1.0.0
 */
void
ags_recycling_set_output_soundcard(AgsRecycling *recycling, GObject *output_soundcard)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* recycling */
  if(recycling->soundcard == soundcard){
    return;
  }

  if(recycling->soundcard != NULL){
    g_object_unref(recycling->soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  recycling->soundcard = (GObject *) soundcard;
}

/**
 * ags_recycling_set_input_soundcard:
 * @recycling: the #AgsRecycling
 * @soundcard: an #AgsSoundcard
 *
 * Set the input soundcard object on recycling.
 *
 * Since: 2.0.0
 */
void
ags_recycling_set_input_soundcard(AgsRecycling *recycling, GObject *input_soundcard)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }
  
  //TODO:JK: implement me
}

/**
 * ags_recycling_set_samplerate:
 * @recycling: the #AgsRecycling
 * @samplerate: the samplerate
 *
 * Sets buffer-size.
 *
 * Since: 2.0.0
 */
void
ags_recycling_set_samplerate(AgsRecycling *recycling, guint samplerate)
{
  AgsAudioSignal *template;

  GList *audio_signal;
  GList *rt_template;
  
  pthread_mutex_t *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */  
  pthread_mutex_lock(ags_recycling_get_class_mutex());

  recycling_mutex = recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* get audio signal */
  pthread_mutex_lock(recycling_mutex);

  audio_signal = g_list_copy(recycling->audio_signal);

  pthread_mutex_unlock(recycling_mutex);

  /* get template */
  template = ags_audio_signal_get_template(audio_signal);
    
  if(template != NULL){
    g_object_set(template,
		 "samplerate", samplerate,
		 NULL);
  }

  /* get rt-template */
  rt_template = ags_audio_signal_get_rt_template(audio_signal);

  while(rt_template != NULL){
    g_object_set(rt_template->data,
		 "samplerate", samplerate,
		 NULL);

    rt_template = rt_template->next;
  }

  /* free list */
  g_list_free(audio_signal);
}

/**
 * ags_recycling_set_buffer_size:
 * @recycling: the #AgsRecycling
 * @buffer_size: the buffer_size
 *
 * Sets buffer length.
 *
 * Since: 1.0.0
 */
void
ags_recycling_set_buffer_size(AgsRecycling *recycling, guint buffer_size)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_recycling_set_format:
 * @recycling: the #AgsRecycling
 * @format: the format
 *
 * Set format.
 *
 * Since: 1.0.0
 */
void
ags_recycling_set_format(AgsRecycling *recycling, guint format)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  //TODO:JK: implement me
}

void
ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
				    AgsAudioSignal *audio_signal)
{
  if(g_list_find(recycling->audio_signal,
		 audio_signal) == NULL){
    if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
      AgsAudioSignal *old_template;

      GHashTable *hash_table;
      GList *list, *list_start;
      
      /* old template */
      old_template = ags_audio_signal_get_template(recycling->audio_signal);
    
      /* remove old template */
      ags_recycling_remove_audio_signal(recycling,
					old_template);

      /* add new template */
      recycling->audio_signal = g_list_prepend(recycling->audio_signal,
					       (gpointer) audio_signal);
      g_object_ref(audio_signal);

      /* realtime template */
      list_start = 
	list = g_list_copy(recycling->audio_signal);

      /* add/remove */
      hash_table = g_hash_table_new_full(g_direct_hash, g_direct_equal,
					 NULL,
					 NULL);
      
      while(list != NULL){
	if((AGS_AUDIO_SIGNAL_RT_TEMPLATE & (AGS_AUDIO_SIGNAL(list->data)->flags)) != 0){
	  AgsAudioSignal *rt_template, *old_rt_template;
	  AgsRecallID *recall_id;

	  recall_id = AGS_AUDIO_SIGNAL(list->data)->recall_id;
	  rt_template = ags_audio_signal_new(recycling->soundcard,
					     recycling,
					     recall_id);	  
	  

	  g_hash_table_insert(hash_table,
			      list->data, rt_template);
	  
	  ags_recycling_remove_audio_signal(recycling,
					    list->data);

	  rt_template->flags |= AGS_AUDIO_SIGNAL_RT_TEMPLATE;
	  ags_recycling_add_audio_signal(recycling,
					 rt_template);
	}
	
	list = list->next;
      }

      /* update */
      list = list_start;
      
      while(list != NULL){
	if(AGS_AUDIO_SIGNAL(list->data)->rt_template != NULL){
	  g_object_set(list->data,
		       "rt-template", g_hash_table_lookup(hash_table,
							  AGS_AUDIO_SIGNAL(list->data)->rt_template),
		       NULL);
	}
	
	list = list->next;
      }

      g_hash_table_unref(hash_table);
      g_list_free(list_start);
    }else{
      recycling->audio_signal = g_list_prepend(recycling->audio_signal, (gpointer) audio_signal);
      g_object_ref(audio_signal);
    }
  }
  
  if(audio_signal->recycling != recycling){
    audio_signal->recycling = (GObject *) recycling;
    g_object_ref(recycling);
  }
}

/**
 * ags_recycling_add_audio_signal:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to add
 *
 * Add #AgsAudioSignal to recycling.
 *
 * Since: 1.0.0
 */
void
ags_recycling_add_audio_signal(AgsRecycling *recycling,
			       AgsAudioSignal *audio_signal)
{
  g_return_if_fail(AGS_IS_RECYCLING(recycling) && AGS_IS_AUDIO_SIGNAL(audio_signal));
  
  /* emit signal */
  g_object_ref(G_OBJECT(recycling));
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[ADD_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref(G_OBJECT(recycling));
}

void
ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
				       AgsAudioSignal *audio_signal)
{

  if(g_list_find(recycling->audio_signal,
		 (gpointer) audio_signal) == NULL){
    return;
  }
  
  recycling->audio_signal = g_list_remove(recycling->audio_signal,
					  (gpointer) audio_signal);
  audio_signal->recycling = NULL;
  g_object_unref(audio_signal);
  g_object_unref(recycling);
}

/**
 * ags_recycling_remove_audio_signal:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to remove
 *
 * Remove #AgsAudioSignal of recycling.
 *
 * Since: 1.0.0
 */
void
ags_recycling_remove_audio_signal(AgsRecycling *recycling,
				  AgsAudioSignal *audio_signal)
{
  g_return_if_fail(AGS_IS_RECYCLING(recycling) && AGS_IS_AUDIO_SIGNAL(audio_signal));

  /* emit signal */
  g_object_ref((GObject *) recycling);
  g_object_ref((GObject *) audio_signal);
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[REMOVE_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref((GObject *) audio_signal);
  g_object_unref((GObject *) recycling);
}

/**
 * ags_recycling_data_request:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal
 *
 * Request data of @audio_signal.
 *
 * Since: 2.0.0
 */
void
ags_recycling_data_request(AgsRecycling *recycling,
			   AgsAudioSignal *audio_signal)
{
  g_return_if_fail(AGS_IS_RECYCLING(recycling));

  /* emit signal */
  g_object_ref((GObject *) recycling);
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[DATA_REQUEST], 0,
		audio_signal);
  g_object_unref((GObject *) recycling);
}

/**
 * ags_recycling_create_audio_signal_with_defaults:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to apply defaults 
 * @delay: the delay
 * @attack: the attack
 *
 * Create audio signal with defaults.
 *
 * Since: 1.0.0
 */
void
ags_recycling_create_audio_signal_with_defaults(AgsRecycling *recycling,
						AgsAudioSignal *audio_signal,
						gdouble delay, guint attack)
{
  AgsAudioSignal *template;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(recycling_mutex);

  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* create audio signal */
  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->delay = delay;
  audio_signal->attack = attack;

  if(template == NULL){
    ags_audio_signal_stream_resize(audio_signal,
				   0);

    /* release lock */
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  audio_signal->soundcard = template->soundcard;

  audio_signal->recycling = (GObject *) recycling;

  audio_signal->samplerate = template->samplerate;
  audio_signal->buffer_size = template->buffer_size;
  audio_signal->format = template->format;

  audio_signal->last_frame = (((guint)(delay *
				       template->buffer_size) +
			       attack +
			       template->last_frame) %
			      template->buffer_size);
  audio_signal->loop_start = (((guint) (delay *
					template->buffer_size) +
			       attack +
			       template->loop_start) %
			      template->buffer_size);
  audio_signal->loop_end = (((guint)(delay *
				     template->buffer_size) +
			     attack +
			     template->loop_end) %
			    template->buffer_size);

  ags_audio_signal_stream_resize(audio_signal,
				 template->length);
  audio_signal->frame_count = template->frame_count;

  ags_audio_signal_duplicate_stream(audio_signal,
				    template);
  
  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_create_audio_signal_with_frame_count:
 * @recycling: an #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to apply defaults 
 * @frame_count: the audio data size
 * @delay: the delay
 * @attack: the attack
 *
 * Create audio signal with frame count.
 *
 * Since: 1.0.0
 */
void
ags_recycling_create_audio_signal_with_frame_count(AgsRecycling *recycling,
						   AgsAudioSignal *audio_signal,
						   guint frame_count,
						   gdouble delay, guint attack)
{
  AgsAudioSignal *template;

  AgsMutexManager *mutex_manager;

  GList *stream, *template_stream;

  guint loop_length;
  guint loop_frame_count;
  guint n_frames;
  guint copy_n_frames;
  guint nth_loop;
  guint i, j, k;
  guint copy_mode;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(recycling_mutex);
  
  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }
  
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* create audio signal */
  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->soundcard = template->soundcard;

  audio_signal->recycling = (GObject *) recycling;

  audio_signal->samplerate = template->samplerate;
  audio_signal->buffer_size = template->buffer_size;
  audio_signal->format = template->format;
  audio_signal->word_size = template->word_size;

  audio_signal->last_frame = (((guint)(delay *
				       template->buffer_size) +
			       attack +
			       template->last_frame) %
			      template->buffer_size);
  audio_signal->loop_start = ((guint) (delay *
				       template->buffer_size) +
			      attack +
			      template->loop_start);
  audio_signal->loop_end = ((guint)(delay *
				    template->buffer_size) +
			    attack +
			    template->loop_end);

  /* resize */
  if(template->loop_end > template->loop_start){
    loop_length = template->loop_end - template->loop_start;
    loop_frame_count = ((frame_count - template->loop_start) / loop_length) * template->buffer_size;

    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / audio_signal->buffer_size) + 1);
    
  }else{
    ags_audio_signal_duplicate_stream(audio_signal,
				      template);
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / audio_signal->buffer_size) + 1);

    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  audio_signal->last_frame = ((guint) (delay * audio_signal->buffer_size) + frame_count + attack) % audio_signal->buffer_size;

  if(template->length == 0){
    /* release lock */
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* generic copying */
  stream = g_list_nth(audio_signal->stream_beginning,
		      (guint) ((delay * audio_signal->buffer_size) + attack) / audio_signal->buffer_size);
  template_stream = template->stream_beginning;

  /* loop related copying */
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						  ags_audio_buffer_util_format_from_soundcard(template->format));
  
  for(i = 0, j = 0, k = attack, nth_loop = 0; i < frame_count;){    
    /* compute count of frames to copy */
    copy_n_frames = audio_signal->buffer_size;

    /* limit nth loop */
    if(i > template->loop_start &&
       i + copy_n_frames > template->loop_start + loop_length &&
       i + copy_n_frames < template->loop_start + loop_frame_count &&
       i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length){
      copy_n_frames = (template->loop_start + (nth_loop + 1) * loop_length) - i;
    }

    /* check boundaries */
    if((k % audio_signal->buffer_size) + copy_n_frames > audio_signal->buffer_size){
      copy_n_frames = audio_signal->buffer_size - (k % audio_signal->buffer_size);
    }

    if(j + copy_n_frames > audio_signal->buffer_size){
      copy_n_frames = audio_signal->buffer_size - j;
    }

    if(stream == NULL ||
       template_stream == NULL){
      break;
    }
    
    /* copy */
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, k % audio_signal->buffer_size,
						template_stream->data, 1, j,
						copy_n_frames, copy_mode);

    /* increment and iterate */
    if((i + copy_n_frames) % audio_signal->buffer_size == 0){
      stream = stream->next;
    }

    if(j + copy_n_frames == template->buffer_size){
      template_stream = template_stream->next;
    }
    
    if(template_stream == NULL ||
       (i > template->loop_start &&
	i + copy_n_frames > template->loop_start + loop_length &&
	i + copy_n_frames < template->loop_start + loop_frame_count &&
	i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length)){
      j = template->loop_start % template->buffer_size;
      template_stream = g_list_nth(template->stream_beginning,
				   floor(template->loop_start / template->buffer_size));

      nth_loop++;
    }else{
      j += copy_n_frames;
    }
    
    i += copy_n_frames;
    k += copy_n_frames;

    if(j == template->buffer_size){
      j = 0;
    }
  }

  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_find_next_channel:
 * @start_region: boundary start
 * @end_region: boundary end
 * @prev_channel: previous channel
 *
 * Retrieve next recycling with different channel.
 *
 * Returns: Matching recycling.
 *
 * Since: 1.0.0
 */
AgsRecycling*
ags_recycling_find_next_channel(AgsRecycling *start_region, AgsRecycling *end_region,
				GObject *prev_channel)
{
  AgsRecycling *recycling;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex, *start_recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  start_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) start_region);

  pthread_mutex_unlock(application_mutex);

  /* verify objects and get pointer for safe access */
  pthread_mutex_lock(start_recycling_mutex);

  if(!AGS_IS_RECYCLING(start_region)){
    pthread_mutex_unlock(start_recycling_mutex);
    
    return(NULL);
  }

  pthread_mutex_lock(start_recycling_mutex);

  /* find */
  recycling = start_region;

  while(recycling != NULL &&
	recycling != end_region){
    /* lock current */
    pthread_mutex_lock(application_mutex);
    
    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);

    pthread_mutex_unlock(application_mutex);

    /* check if new match */
    pthread_mutex_lock(recycling_mutex);

    if(recycling->channel != prev_channel){
      pthread_mutex_unlock(recycling_mutex);

      return(recycling);
    }

    recycling = recycling->next;
    pthread_mutex_unlock(recycling_mutex);
  }

  /* no new channel within region */
  return(NULL);
}

/**
 * ags_recycling_position:
 * @start_region: boundary start
 * @end_region: boundary end
 * @recycling: matching recycling
 *
 * Retrieve position of recycling.
 *
 * Returns: position within boundary.
 *
 * Since: 1.0.0
 */
gint
ags_recycling_position(AgsRecycling *start_region, AgsRecycling *end_region,
		       AgsRecycling *recycling)
{
  AgsRecycling *current;

  AgsMutexManager *mutex_manager;

  gint position;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *current_mutex;

  if(start_region == NULL){
    return(-1);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* determine position */
  current = start_region;
  position = -1;

  while(current != NULL && current != end_region){
    position++;

    /* lock current */
    pthread_mutex_lock(application_mutex);
    
    current_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) current);

    pthread_mutex_unlock(application_mutex);

    /* check if new match */

    if(current == recycling){
      return(position);
    }

    pthread_mutex_lock(current_mutex);

    current = current->next;

    pthread_mutex_unlock(current_mutex);
  }

  if(end_region == NULL){
    return(position);
  }else{
    return(-1);
  }
}

/**
 * ags_recycling_is_active:
 * @start_region: boundary start
 * @end_region: boundary end
 * @recall_id: the #AgsRecallID
 * 
 * Check if is active.
 * 
 * Returns: %TRUE if related audio signal to recall id is available, otherwise %FALSE
 * 
 * Since: 1.0.0.9
 */
gboolean
ags_recycling_is_active(AgsRecycling *start_region, AgsRecycling *end_region,
			GObject *recall_id)
{
  AgsRecycling *current;

  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *current_mutex;
  
  if(recall_id == NULL ||
     AGS_RECALL_ID(recall_id)->recycling_context == NULL){
    return(FALSE);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  current = start_region;

  while(current != end_region){
    /* lock current */
    pthread_mutex_lock(application_mutex);
    
    current_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) current);

    pthread_mutex_unlock(application_mutex);

    /* is active */
    pthread_mutex_lock(current_mutex);

    if(ags_audio_signal_is_active(current->audio_signal,
				  recall_id)){
      pthread_mutex_unlock(current_mutex);
      
      return(TRUE);
    }

    
    current = current->next;

    pthread_mutex_unlock(current_mutex);
  }

  return(FALSE);
}

/**
 * ags_recycling_new:
 * @soundcard: the #AgsSoundcard to use for output
 *
 * Creates a #AgsRecycling, with defaults of @soundcard.
 *
 * Returns: a new #AgsRecycling
 *
 * Since: 1.0.0
 */
AgsRecycling*
ags_recycling_new(GObject *soundcard)
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  recycling = (AgsRecycling *) g_object_new(AGS_TYPE_RECYCLING,
					    "output-soundcard", soundcard,
					    NULL);

  audio_signal = ags_audio_signal_new(soundcard,
				      (GObject *) recycling,
				      NULL);
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

  ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

  recycling->audio_signal = g_list_alloc();
  recycling->audio_signal->data = (gpointer) audio_signal;

  return(recycling);
}
