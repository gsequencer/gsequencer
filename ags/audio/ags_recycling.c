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

#include <ags/audio/ags_recycling.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <string.h>
#include <math.h>

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

void ags_recycling_real_set_output_soundcard(AgsRecycling *recycling, GObject *output_soundcard);

void ags_recycling_real_set_input_soundcard(AgsRecycling *recycling, GObject *input_soundcard);

void ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
					 AgsAudioSignal *audio_signal);

void ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
					    AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_recycling
 * @short_description: Recycling container of audio signals
 * @title: AgsRecycling
 * @section_id:
 * @include: ags/audio/ags_recycling.h
 *
 * #AgsRecycling forms the nested tree of #AgsAudio and #AgsChannel. The recycling tree
 * is the actual audio processing tree, because it hosts #AgsAudioSignal.
 *
 * Every recycling node introduces a new #AgsAudioSignal for specific #AgsSoundScope-enum
 * specified by assigned #AgsRecallID.
 *
 * All audio processing happens on #AgsAudioSignal and is added to #AgsRecycling by
 * ags_recycling_add_audio_signal(). While you call it, the function takes the audio signal
 * with AGS_AUDIO_SIGNAL_TEMPLATE flag set and creates a copy #AgsAudioSignal with the very
 * same audio data of `stream` property.
 *
 * The actual audio processing functionality is provided by #AgsRecall implementations. Its
 * entry point is #AgsAudio or #AgsChannel.
 *
 * #AgsRecall objectss except templates are assigned with a #AgsRecallID with a relation to
 * #AgsRecyclingContext.
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

GType
ags_recycling_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recycling = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recycling);
  }

  return g_define_type_id__volatile;
}

GType
ags_recycling_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_RECYCLING_MUTED, "AGS_RECYCLING_MUTED", "recycling-muted" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsRecyclingFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("output-soundcard-channel",
				 i18n_pspec("output soundcard channel"),
				 i18n_pspec("The output soundcard channel"),
				 -1,
				 G_MAXINT32,
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("input-soundcard-channel",
				 i18n_pspec("input soundcard channel"),
				 i18n_pspec("The input soundcard channel"),
				 -1,
				 G_MAXINT32,
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsRecycling:audio-signal: (type GList(AgsAudioSignal)) (transfer full)
   *
   * The containing  #AgsAudioSignal.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
  AgsAudioSignal *audio_signal;
  
  AgsConfig *config;

  gchar *str;
  gchar *str0, *str1;

  recycling->flags = 0;
  recycling->connectable_flags = 0;

  /* add recycling mutex */
  g_rec_mutex_init(&(recycling->obj_mutex));
  
  /* uuid */
  recycling->uuid = ags_uuid_alloc();
  ags_uuid_generate(recycling->uuid);

  /* config */
  config = ags_config_get_instance();

  /* base init */
  recycling->channel = NULL;

  recycling->output_soundcard = NULL;
  recycling->output_soundcard_channel = 0;

  recycling->input_soundcard = NULL;
  recycling->input_soundcard_channel = 0;

  /* presets */
  recycling->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  recycling->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  recycling->format = ags_soundcard_helper_config_get_format(config);
  
  /* nested tree */
  recycling->parent = NULL;

  recycling->next = NULL;
  recycling->prev = NULL;

  /* audio signal */
  audio_signal = ags_audio_signal_new(NULL,
				      (GObject *) recycling,
				      NULL);
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

  recycling->audio_signal = g_list_alloc();
  recycling->audio_signal->data = audio_signal;
}

void
ags_recycling_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecycling *recycling;
  
  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(gobject);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recycling_mutex);

      if(recycling->channel == (GObject *) channel){
	g_rec_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->channel != NULL){
	g_object_unref(recycling->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      recycling->channel = (GObject *) channel;

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_recycling_real_set_output_soundcard(recycling, (GObject *) soundcard);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recycling_mutex);

      recycling->output_soundcard_channel = g_value_get_int(value);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_recycling_real_set_input_soundcard(recycling, (GObject *) soundcard);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recycling_mutex);

      recycling->input_soundcard_channel = g_value_get_int(value);

      g_rec_mutex_unlock(recycling_mutex);
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

      g_rec_mutex_lock(recycling_mutex);

      if(recycling->parent == parent){
	g_rec_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->parent != NULL){
	g_object_unref(recycling->parent);
      }

      if(parent != NULL){
	g_object_ref(parent);
      }

      recycling->parent = parent;

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_NEXT:
    {
      AgsRecycling *next;

      next = (AgsRecycling *) g_value_get_object(value);

      g_rec_mutex_lock(recycling_mutex);

      if(recycling->next == next){
	g_rec_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->next != NULL){
	g_object_unref(recycling->next);
      }

      if(next != NULL){
	g_object_ref(next);
      }

      recycling->next = next;

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_PREV:
    {
      AgsRecycling *prev;

      prev = (AgsRecycling *) g_value_get_object(value);

      g_rec_mutex_lock(recycling_mutex);

      if(recycling->prev == prev){
	g_rec_mutex_unlock(recycling_mutex);
	
	return;
      }

      if(recycling->prev != NULL){
	g_object_unref(recycling->prev);
      }

      if(prev != NULL){
	g_object_ref(prev);
      }

      recycling->prev = prev;

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = g_value_get_pointer(value);

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
  
  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(gobject);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);
  
  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->channel);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->output_soundcard);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_int(value, recycling->output_soundcard_channel);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->input_soundcard);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_int(value, recycling->input_soundcard_channel);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_uint(value, recycling->samplerate);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_uint(value, recycling->buffer_size);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_uint(value, recycling->format);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_PARENT:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->parent);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_NEXT:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->next);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_PREV:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_object(value, recycling->prev);

      g_rec_mutex_unlock(recycling_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      g_rec_mutex_lock(recycling_mutex);

      g_value_set_pointer(value, g_list_copy_deep(recycling->audio_signal,
						  (GCopyFunc) g_object_ref,
						  NULL));
 
      g_rec_mutex_unlock(recycling_mutex);
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

  GList *start_list, *list;

  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(gobject);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  g_rec_mutex_lock(recycling_mutex);
  
  /* channel */
  if(recycling->channel != NULL){
    AgsChannel *channel;

    channel = recycling->channel;
    
    recycling->channel = NULL;
    
    g_object_unref(channel);
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

  g_rec_mutex_unlock(recycling_mutex);

  /* AgsAudioSignal */
  g_rec_mutex_lock(recycling_mutex);
  
  list =
    start_list = recycling->audio_signal;

  recycling->audio_signal = NULL;

  g_rec_mutex_unlock(recycling_mutex);

  while(list != NULL){
    g_object_run_dispose(list->data);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->dispose(gobject);
}

void
ags_recycling_finalize(GObject *gobject)
{
  AgsRecycling *recycling;  
  
  GList *start_list, *list;
  
  recycling = AGS_RECYCLING(gobject);

  ags_uuid_free(recycling->uuid);

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
  list =
    start_list = recycling->audio_signal;

  while(list != NULL){
    g_object_run_dispose(list->data);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->finalize(gobject);
}

AgsUUID*
ags_recycling_get_uuid(AgsConnectable *connectable)
{
  AgsRecycling *recycling;
  
  AgsUUID *ptr;

  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling signal mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get UUID */
  g_rec_mutex_lock(recycling_mutex);

  ptr = recycling->uuid;

  g_rec_mutex_unlock(recycling_mutex);
  
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

  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* check is ready */
  g_rec_mutex_lock(recycling_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (recycling->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recycling_mutex);
  
  return(is_ready);
}

void
ags_recycling_add_to_registry(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *start_list, *list;
  
  GRecMutex *recycling_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  g_rec_mutex_lock(recycling_mutex);

  recycling->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(recycling_mutex);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) recycling);
    ags_registry_add_entry(registry,
			   entry);
  }
  
  /* add audio signal */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_recycling_remove_from_registry(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  GRecMutex *recycling_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  g_rec_mutex_lock(recycling_mutex);

  recycling->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(recycling_mutex);

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

  GRecMutex *recycling_mutex;

  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* check is connected */
  g_rec_mutex_lock(recycling_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (recycling->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recycling_mutex);
  
  return(is_connected);
}

void
ags_recycling_connect(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  GList *start_list, *list;

  GRecMutex *recycling_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  g_rec_mutex_lock(recycling_mutex);

  recycling->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(recycling_mutex);

#ifdef AGS_DEBUG
  g_message("connecting recycling");
#endif

  /* audio signal */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_recycling_disconnect(AgsConnectable *connectable)
{
  AgsRecycling *recycling;

  GList *start_list, *list;

  GRecMutex *recycling_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  recycling = AGS_RECYCLING(connectable);

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  g_rec_mutex_lock(recycling_mutex);

  recycling->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(recycling_mutex);

#ifdef AGS_DEBUG
  g_message("disconnecting recycling");
#endif

  /* audio signal */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_recycling_get_obj_mutex:
 * @recycling: the #AgsRecycling
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @recycling
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_recycling_get_obj_mutex(AgsRecycling *recycling)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  return(AGS_RECYCLING_GET_OBJ_MUTEX(recycling));
}

/**
 * ags_recycling_test_flags:
 * @recycling: the #AgsRecycling
 * @flags: the flags
 *
 * Test @flags to be set on @recycling.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recycling_test_flags(AgsRecycling *recycling, guint flags)
{
  gboolean retval;  
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return(FALSE);
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* test */
  g_rec_mutex_lock(recycling_mutex);

  retval = (flags & (recycling->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recycling_mutex);

  return(retval);
}

/**
 * ags_recycling_set_flags:
 * @recycling: the #AgsRecycling
 * @flags: see #AgsRecyclingFlags-enum
 *
 * Enable a feature of @recycling.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_flags(AgsRecycling *recycling, guint flags)
{
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(recycling_mutex);

  recycling->flags |= flags;
  
  g_rec_mutex_unlock(recycling_mutex);
}
    
/**
 * ags_recycling_unset_flags:
 * @recycling: the #AgsRecycling
 * @flags: see #AgsRecyclingFlags-enum
 *
 * Disable a feature of @recycling.
 *
 * Since: 3.0.0
 */
void
ags_recycling_unset_flags(AgsRecycling *recycling, guint flags)
{  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(recycling_mutex);

  recycling->flags &= (~flags);
  
  g_rec_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_get_channel:
 * @recycling: the #AgsRecycling
 * 
 * Get channel.
 * 
 * Returns: (transfer full): the #AgsChannel
 * 
 * Since: 3.1.0
 */
GObject*
ags_recycling_get_channel(AgsRecycling *recycling)
{
  GObject *channel;

  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  g_object_get(recycling,
	       "channel", &channel,
	       NULL);

  return(channel);
}

/**
 * ags_recycling_set_channel:
 * @recycling: the #AgsRecycling
 * @channel: the #AgsChannel
 * 
 * Set channel.
 * 
 * Since: 3.1.0
 */
void
ags_recycling_set_channel(AgsRecycling *recycling, GObject *channel)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  g_object_set(recycling,
	       "channel", channel,
	       NULL);
}

/**
 * ags_recycling_next:
 * @recycling: the #AgsRecycling
 * 
 * Iterate @recycling.
 * 
 * Returns: (transfer full): the next of #AgsRecycling if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsRecycling*
ags_recycling_next(AgsRecycling *recycling)
{
  AgsRecycling *next;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* next */
  g_rec_mutex_lock(recycling_mutex);

  next = recycling->next;

  if(next != NULL){
    g_object_ref(next);
  }
  
  g_rec_mutex_unlock(recycling_mutex);

  return(next);
}

/**
 * ags_recycling_prev:
 * @recycling: the #AgsRecycling
 * 
 * Iterate @recycling.
 * 
 * Returns: (transfer full): the prev of #AgsRecycling if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsRecycling*
ags_recycling_prev(AgsRecycling *recycling)
{
  AgsRecycling *prev;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* prev */
  g_rec_mutex_lock(recycling_mutex);

  prev = recycling->prev;

  if(prev != NULL){
    g_object_ref(prev);
  }
  
  g_rec_mutex_unlock(recycling_mutex);

  return(prev);
}

/**
 * ags_recycling_get_output_soundcard:
 * @recycling: the #AgsRecycling
 *
 * Get the output soundcard object of @recycling.
 *
 * Returns: (transfer full): the output soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_recycling_get_output_soundcard(AgsRecycling *recycling)
{
  GObject *output_soundcard;
  
  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  g_object_get(recycling,
	       "output-soundcard", &output_soundcard,
	       NULL);

  return(output_soundcard);
}

void
ags_recycling_real_set_output_soundcard(AgsRecycling *recycling, GObject *output_soundcard)
{
  GList *start_list, *list;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* recycling */
  g_rec_mutex_lock(recycling_mutex);

  if(recycling->output_soundcard == output_soundcard){
    g_rec_mutex_unlock(recycling_mutex);
    
    return;
  }

  if(recycling->output_soundcard != NULL){
    g_object_unref(recycling->output_soundcard);
  }
  
  if(output_soundcard != NULL){
    g_object_ref(output_soundcard);
  }
  
  recycling->output_soundcard = (GObject *) output_soundcard;

  g_rec_mutex_unlock(recycling_mutex);

  /* audio signal */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    g_object_set(list->data,
		 "output-soundcard", output_soundcard,
		 NULL);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_recycling_set_output_soundcard:
 * @recycling: the #AgsRecycling
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the output soundcard object of @recycling.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_output_soundcard(AgsRecycling *recycling, GObject *output_soundcard)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  g_object_set(recycling,
	       "output-soundcard", output_soundcard,
	       NULL);
}

/**
 * ags_recycling_get_input_soundcard:
 * @recycling: the #AgsRecycling
 *
 * Get the input soundcard object of @recycling.
 *
 * Returns: (transfer full): the input soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_recycling_get_input_soundcard(AgsRecycling *recycling)
{
  GObject *input_soundcard;
  
  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  g_object_get(recycling,
	       "input-soundcard", &input_soundcard,
	       NULL);

  return(input_soundcard);
}

void
ags_recycling_real_set_input_soundcard(AgsRecycling *recycling, GObject *input_soundcard)
{
  GList *start_list, *list;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* recycling */
  g_rec_mutex_lock(recycling_mutex);

  if(recycling->input_soundcard == input_soundcard){
    g_rec_mutex_unlock(recycling_mutex);
    
    return;
  }

  if(recycling->input_soundcard != NULL){
    g_object_unref(recycling->input_soundcard);
  }
  
  if(input_soundcard != NULL){
    g_object_ref(input_soundcard);
  }
  
  recycling->input_soundcard = (GObject *) input_soundcard;

  g_rec_mutex_unlock(recycling_mutex);

  /* audio signal */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    g_object_set(list->data,
		 "input-soundcard", input_soundcard,
		 NULL);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_recycling_set_input_soundcard:
 * @recycling: an #AgsRecycling
 * @input_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the input soundcard object of @recycling.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_input_soundcard(AgsRecycling *recycling, GObject *input_soundcard)
{
  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  g_object_set(recycling,
	       "input-soundcard", input_soundcard,
	       NULL);
}

/**
 * ags_recycling_get_samplerate:
 * @recycling: the #AgsRecycling
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_recycling_get_samplerate(AgsRecycling *recycling)
{
  guint samplerate;
  
  if(!AGS_IS_RECYCLING(recycling)){
    return(0);
  }

  g_object_get(recycling,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_recycling_set_samplerate:
 * @recycling: the #AgsRecycling
 * @samplerate: the samplerate
 *
 * Sets samplerate.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_samplerate(AgsRecycling *recycling, guint samplerate)
{
  AgsAudioSignal *template;

  GList *audio_signal;
  GList *start_rt_template, *rt_template;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal */
  g_object_get(recycling,
	       "audio-signal", &audio_signal,
	       NULL);

  /* get template */
  template = ags_audio_signal_get_template(audio_signal);
    
  if(template != NULL){
    g_object_set(template,
		 "samplerate", samplerate,
		 NULL);
  }

  g_object_unref(template);
  
  /* get rt-template */
  rt_template =
    start_rt_template = ags_audio_signal_get_rt_template(audio_signal);

  while(rt_template != NULL){
    g_object_set(rt_template->data,
		 "samplerate", samplerate,
		 NULL);

    rt_template = rt_template->next;
  }

  g_list_free_full(start_rt_template,
		   g_object_unref);

  /* free list */
  g_list_free_full(audio_signal,
		   g_object_unref);
}

/**
 * ags_recycling_get_buffer_size:
 * @recycling: the #AgsRecycling
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_recycling_get_buffer_size(AgsRecycling *recycling)
{
  guint buffer_size;
  
  if(!AGS_IS_RECYCLING(recycling)){
    return(0);
  }

  g_object_get(recycling,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_recycling_set_buffer_size:
 * @recycling: the #AgsRecycling
 * @buffer_size: the buffer size
 *
 * Set buffer size.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_buffer_size(AgsRecycling *recycling, guint buffer_size)
{
  AgsAudioSignal *template;

  GList *audio_signal;
  GList *start_rt_template, *rt_template;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal */
  g_object_get(recycling,
	       "audio-signal", &audio_signal,
	       NULL);

  /* get template */
  template = ags_audio_signal_get_template(audio_signal);
    
  if(template != NULL){
    g_object_set(template,
		 "buffer-size", buffer_size,
		 NULL);
  }

  g_object_unref(template);

  /* get rt-template */
  rt_template =
    start_rt_template = ags_audio_signal_get_rt_template(audio_signal);

  while(rt_template != NULL){
    g_object_set(rt_template->data,
		 "buffer-size", buffer_size,
		 NULL);

    rt_template = rt_template->next;
  }

  g_list_free_full(start_rt_template,
		   g_object_unref);

  /* free list */
  g_list_free_full(audio_signal,
		   g_object_unref);
}

/**
 * ags_recycling_get_format:
 * @recycling: the #AgsRecycling
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
guint
ags_recycling_get_format(AgsRecycling *recycling)
{
  guint format;
  
  if(!AGS_IS_RECYCLING(recycling)){
    return(0);
  }

  g_object_get(recycling,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_recycling_set_format:
 * @recycling: the #AgsRecycling
 * @format: the format
 *
 * Set format.
 *
 * Since: 3.0.0
 */
void
ags_recycling_set_format(AgsRecycling *recycling, guint format)
{
  AgsAudioSignal *template;

  GList *audio_signal;
  GList *start_rt_template, *rt_template;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal */
  g_object_get(recycling,
	       "audio-signal", &audio_signal,
	       NULL);

  /* get template */
  template = ags_audio_signal_get_template(audio_signal);
    
  if(template != NULL){
    g_object_set(template,
		 "format", format,
		 NULL);
  }

  g_object_unref(template);
  
  /* get rt-template */
  rt_template = 
    start_rt_template = ags_audio_signal_get_rt_template(audio_signal);

  while(rt_template != NULL){
    g_object_set(rt_template->data,
		 "format", format,
		 NULL);

    rt_template = rt_template->next;
  }

  g_list_free_full(start_rt_template,
		   g_object_unref);

  /* free list */
  g_list_free_full(audio_signal,
		   g_object_unref);
}

/**
 * ags_recycling_get_audio_signal:
 * @recycling: the #AgsRecycling
 * 
 * Get recall id.
 * 
 * Returns: (element-type AgsAudio.AudioSignal) (transfer full): the #GList-struct containig #AgsAudioSignal
 * 
 * Since: 3.1.0
 */
GList*
ags_recycling_get_audio_signal(AgsRecycling *recycling)
{
  GList *audio_signal;

  if(!AGS_IS_RECYCLING(recycling)){
    return(NULL);
  }

  g_object_get(recycling,
	       "audio-signal", &audio_signal,
	       NULL);

  return(audio_signal);
}

/**
 * ags_recycling_set_audio_signal:
 * @recycling: the #AgsRecycling
 * @audio_signal: (element-type AgsAudio.AudioSignal) (transfer full): the #GList-struct containing #AgsAudioSignal
 * 
 * Set recall id by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_recycling_set_audio_signal(AgsRecycling *recycling, GList *audio_signal)
{
  GList *start_audio_signal;
  
  GRecMutex *recycling_mutex;

  if(!AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);
    
  g_rec_mutex_lock(recycling_mutex);

  start_audio_signal = recycling->audio_signal;

  recycling->audio_signal = audio_signal;
  
  g_rec_mutex_unlock(recycling_mutex);

  g_list_free_full(start_audio_signal,
		   (GDestroyNotify) g_object_unref);
}

void
ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
				    AgsAudioSignal *audio_signal)
{
  AgsAudioSignal *old_template;

  GObject *output_soundcard;
  
  GHashTable *hash_table;
  GList *start_list, *list;

  GRecMutex *recycling_mutex;

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal */
  g_rec_mutex_lock(recycling_mutex);

  if(g_list_find(recycling->audio_signal,
		 audio_signal) != NULL){
    g_rec_mutex_unlock(recycling_mutex);

    return;
  }
  
  output_soundcard = recycling->output_soundcard;
    
  g_rec_mutex_unlock(recycling_mutex);

  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);
  
  /* get some fields */
  if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_TEMPLATE)){
    /* old template */
    old_template = ags_audio_signal_get_template(start_list);
    
    /* remove old template */
    ags_recycling_remove_audio_signal(recycling,
				      old_template);

    g_object_unref(old_template);
    
    /* add new template */
    g_rec_mutex_lock(recycling_mutex);
    
    recycling->audio_signal = g_list_prepend(recycling->audio_signal,
					     audio_signal);    
    g_object_ref(audio_signal);

    g_rec_mutex_unlock(recycling_mutex);

    /* add/remove */
    list = start_list;
    hash_table = g_hash_table_new_full(g_direct_hash, g_direct_equal,
				       NULL,
				       NULL);
      
    while(list != NULL){
      AgsAudioSignal *current_audio_signal;
      AgsAudioSignal *rt_template, *old_rt_template;
      AgsRecallID *current_recall_id;
      
      current_audio_signal = list->data;
      
      /* get some fields */
      if(ags_audio_signal_test_flags(current_audio_signal, AGS_AUDIO_SIGNAL_RT_TEMPLATE)){
	current_recall_id = (AgsRecallID *) current_audio_signal->recall_id;

	/* create rt-template */
	rt_template = ags_audio_signal_new(output_soundcard,
					   (GObject *) recycling,
					   (GObject *) current_recall_id);
	ags_audio_signal_set_flags(rt_template, AGS_AUDIO_SIGNAL_RT_TEMPLATE);

	g_hash_table_insert(hash_table,
			    current_audio_signal, rt_template);

	/* remove old rt-template */
	ags_recycling_remove_audio_signal(recycling,
					  current_audio_signal);

	/* add new rt-template */
	ags_recycling_add_audio_signal(recycling,
				       rt_template);
      }
	
      list = list->next;
    }

    /* update */
    list = start_list;
      
    while(list != NULL){
      AgsAudioSignal *current_audio_signal;
      AgsAudioSignal *rt_template;

      current_audio_signal = list->data;

      /* get some fields */
      g_object_get(current_audio_signal,
		   "rt-template", &rt_template,
		   NULL);

      if(rt_template != NULL){
	g_object_set(list->data,
		     "rt-template", g_hash_table_lookup(hash_table,
							rt_template),
		     NULL);

	g_object_unref(rt_template);
      }
	
      list = list->next;
    }

    g_hash_table_destroy(hash_table);
  }else{
    /* add new audio signal */
    g_rec_mutex_lock(recycling_mutex);
    
    recycling->audio_signal = g_list_prepend(recycling->audio_signal,
					     audio_signal);    
    g_object_ref(audio_signal);

    g_rec_mutex_unlock(recycling_mutex);
  }

  g_list_free_full(start_list,
		   g_object_unref);

  g_object_set(audio_signal,
	       "recycling", recycling,
	       NULL);
}

/**
 * ags_recycling_add_audio_signal:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to add
 *
 * Add @audio_signal to @recycling.
 *
 * Since: 3.0.0
 */
void
ags_recycling_add_audio_signal(AgsRecycling *recycling,
			       AgsAudioSignal *audio_signal)
{
  GRecMutex *recycling_mutex;
  
  g_return_if_fail(AGS_IS_RECYCLING(recycling) &&
		   AGS_IS_AUDIO_SIGNAL(audio_signal));

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal */
  g_rec_mutex_lock(recycling_mutex);

  if(g_list_find(recycling->audio_signal,
		 audio_signal) != NULL){
    g_rec_mutex_unlock(recycling_mutex);

    return;
  }
    
  g_rec_mutex_unlock(recycling_mutex);
  
  /* emit signal */
  g_object_ref(G_OBJECT(recycling));
  g_object_ref(G_OBJECT(audio_signal));
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[ADD_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref(G_OBJECT(audio_signal));
  g_object_unref(G_OBJECT(recycling));
}

void
ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
				       AgsAudioSignal *audio_signal)
{
  GRecMutex *recycling_mutex;
  
  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* check audio signal */
  g_rec_mutex_lock(recycling_mutex);

  if(g_list_find(recycling->audio_signal,
		 audio_signal) == NULL){
    g_rec_mutex_unlock(recycling_mutex);
    
    return;
  }
  
  recycling->audio_signal = g_list_remove(recycling->audio_signal,
					  audio_signal);

  g_rec_mutex_unlock(recycling_mutex);

  g_object_set(audio_signal,
	       "recycling", NULL,
	       NULL);
  
  g_object_unref(audio_signal);
}

/**
 * ags_recycling_remove_audio_signal:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to remove
 *
 * Remove @audio_signal of @recycling.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
void
ags_recycling_create_audio_signal_with_defaults(AgsRecycling *recycling,
						AgsAudioSignal *audio_signal,
						gdouble delay, guint attack)
{
  AgsAudioSignal *template;

  GObject *output_soundcard;
  
  GList *start_list, *list;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint last_frame;
  guint loop_start, loop_end;
  guint length;
  guint frame_count;
  
  GRecMutex *recycling_mutex;
  GRecMutex *audio_signal_mutex;
  GRecMutex *template_mutex;

  if(!AGS_IS_RECYCLING(recycling) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal mutex */  
  audio_signal_mutex = AGS_AUDIO_SIGNAL_GET_OBJ_MUTEX(audio_signal);

  /* get audio signal list */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);

  /* get template */
  template = ags_audio_signal_get_template(start_list);

  g_list_free_full(start_list,
		   g_object_unref);  

  /* set delay and attack */
  g_object_set(audio_signal,
	       "delay", delay,
	       "attack", attack,
	       NULL);

  if(template == NULL){
    ags_audio_signal_stream_resize(audio_signal,
				   0);

    return;
  }

  /* get template mutex */  
  template_mutex = AGS_AUDIO_SIGNAL_GET_OBJ_MUTEX(template);

  /* get some fields */
  g_rec_mutex_lock(template_mutex);

  output_soundcard = template->output_soundcard;
  
  samplerate = template->samplerate;
  buffer_size = template->buffer_size;
  format = template->format;

  length = template->length;
  frame_count = template->frame_count;

  last_frame = template->last_frame;
  loop_start = template->loop_start;
  loop_end = template->loop_end;

  g_rec_mutex_unlock(template_mutex);

  /* apply delay and attack */
  last_frame = (((guint)(delay *
			 buffer_size) +
		 attack +
		 last_frame) %
		buffer_size);
  loop_start = (((guint) (delay *
			  buffer_size) +
		 attack +
		 loop_start) %
		buffer_size);
  loop_end = (((guint)(delay *
		       buffer_size) +
	       attack +
	       loop_end) %
	      buffer_size);

  /* apply defaults */
  g_object_set(audio_signal,
	       "recycling", recycling,
	       "output-soundcard", output_soundcard,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       "frame-count", frame_count,
	       "last-frame", last_frame,
	       "loop-start", loop_start,
	       "loop-end", loop_end,
	       NULL);

  /* resize and duplicate */
  ags_audio_signal_stream_resize(audio_signal,
				 length);
  ags_audio_signal_duplicate_stream(audio_signal,
				    template);  

  g_object_unref(template);
}

/**
 * ags_recycling_create_audio_signal_with_frame_count:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to apply defaults 
 * @frame_count: the audio data size
 * @delay: the delay
 * @attack: the attack
 *
 * Create audio signal with frame count.
 *
 * Since: 3.0.0
 */
void
ags_recycling_create_audio_signal_with_frame_count(AgsRecycling *recycling,
						   AgsAudioSignal *audio_signal,
						   guint frame_count,
						   gdouble delay, guint attack)
{
  AgsAudioSignal *template;

  GObject *output_soundcard;
  
  GList *start_list, *list;
  GList *stream, *template_stream;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint last_frame;
  guint loop_start, loop_end;
  guint new_last_frame;
  guint new_loop_start, new_loop_end;
  guint template_length;
  guint loop_length;
  guint loop_frame_count;
  guint n_frames;
  guint copy_n_frames;
  guint nth_loop;
  guint i, j;
  guint copy_mode;

  GRecMutex *recycling_mutex;
  GRecMutex *audio_signal_mutex;
  GRecMutex *template_mutex;
  GRecMutex *template_stream_mutex;

  if(!AGS_IS_RECYCLING(recycling) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get recycling mutex */  
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

  /* get audio signal mutex */  
  audio_signal_mutex = AGS_AUDIO_SIGNAL_GET_OBJ_MUTEX(audio_signal);

  /* get audio signal list */
  g_object_get(recycling,
	       "audio-signal", &start_list,
	       NULL);  

  /* get template */
  template = ags_audio_signal_get_template(start_list);
  
  g_list_free_full(start_list,
		   g_object_unref);  

  /* set delay and attack */
  g_object_set(audio_signal,
	       "delay", delay,
	       "attack", attack,
	       NULL);

  if(template == NULL){
    g_rec_mutex_lock(audio_signal_mutex);

    buffer_size = audio_signal->buffer_size;
    
    g_rec_mutex_unlock(audio_signal_mutex);

    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil((attack + frame_count) / buffer_size) + 1);
    
    return;
  }

  /* get template mutex */  
  template_mutex = AGS_AUDIO_SIGNAL_GET_OBJ_MUTEX(template);

  /* get some fields */
  g_rec_mutex_lock(template_mutex);

  output_soundcard = template->output_soundcard;
  
  samplerate = template->samplerate;
  buffer_size = template->buffer_size;
  format = template->format;

  last_frame = template->last_frame;
  loop_start = template->loop_start;
  loop_end = template->loop_end;

  template_length = template->length;
  
  g_rec_mutex_unlock(template_mutex);

  /* apply delay and attack */
  new_last_frame = (((guint)(delay *
			     buffer_size) +
		     attack +
		     last_frame) %
		    buffer_size);
  new_loop_start = ((guint) (delay *
			     buffer_size) +
		    attack +
		    loop_start);
  new_loop_end = ((guint)(delay *
			  buffer_size) +
		  attack +
		  loop_end);

  /* apply defaults */
  g_object_set(audio_signal,
	       "recycling", recycling,
	       "output-soundcard", output_soundcard,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);

  /* resize */
  if(loop_end > loop_start){
    loop_length = loop_end - loop_start;
    
    if((frame_count - loop_start) > (last_frame - loop_end) &&
       last_frame >= loop_end){
      loop_frame_count = (frame_count - loop_start) - (last_frame - loop_end);
    }else{
      loop_frame_count = loop_length;
    }
    
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / buffer_size) + 1);    
  }else{
    ags_audio_signal_duplicate_stream(audio_signal,
				      template);
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / buffer_size) + 1);

    return;
  }

  new_last_frame = ((guint) (delay * buffer_size) + frame_count + attack) % buffer_size;

  g_object_set(audio_signal,
	       "last-frame", new_last_frame,
	       NULL);
  
  if(template_length == 0){
    g_object_unref(template);
    
    return;
  }

  /* get template stream mutex */  
  template_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(template);

  /* loop related copying */
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
 						  ags_audio_buffer_util_format_from_soundcard(format));

  /* generic copying */
  stream = g_list_nth(audio_signal->stream,
		      (guint) ((delay * buffer_size) + attack) / buffer_size);
  
  g_rec_mutex_lock(template_stream_mutex);
  
  template_stream = template->stream;

  for(i = 0, j = attack, nth_loop = 0; i < frame_count && stream != NULL && template_stream != NULL;){    
    /* compute count of frames to copy */
    copy_n_frames = buffer_size;

    if(loop_start < loop_end &&
       i + copy_n_frames < loop_start + loop_frame_count){
      if(j + copy_n_frames > loop_end){
	copy_n_frames = loop_end - j;
      }
    }

    if((i % buffer_size) + copy_n_frames > buffer_size){
      copy_n_frames = buffer_size - (i % buffer_size);
    }

    if((j % buffer_size) + copy_n_frames > buffer_size){
      copy_n_frames = buffer_size - (j % buffer_size);
    }

    if(i + copy_n_frames > frame_count){
      copy_n_frames = frame_count - i;
    }
    
    /* copy */
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, i % buffer_size,
						template_stream->data, 1, j % buffer_size,
						copy_n_frames, copy_mode);
    
    if((i + copy_n_frames) % buffer_size == 0){
      stream = stream->next;
    }

    if((j + copy_n_frames) % buffer_size == 0){
      template_stream = template_stream->next;
    }

    i += copy_n_frames;

    if(loop_start < loop_end){
      if(j + copy_n_frames == loop_end &&
	 i + copy_n_frames < loop_start + loop_frame_count){
	template_stream = g_list_nth(template->stream,
				     floor(loop_start / buffer_size));
	
	j = loop_start;
	
	nth_loop++;
      }else{
	j += copy_n_frames;
      }
    }else{
      j += copy_n_frames;
    }
  }

  g_rec_mutex_unlock(template_stream_mutex);

  g_object_unref(template);
}

/**
 * ags_recycling_find_next_channel:
 * @start_region: boundary start
 * @end_region: boundary end
 * @prev_channel: previous channel
 *
 * Retrieve next recycling with different channel.
 *
 * Returns: (transfer full): Matching recycling.
 *
 * Since: 3.0.0
 */
AgsRecycling*
ags_recycling_find_next_channel(AgsRecycling *start_region, AgsRecycling *end_region,
				GObject *prev_channel)
{
  AgsRecycling *recycling, *next_recycling;

  /* verify objects and get pointer for safe access */
  if(!AGS_IS_RECYCLING(start_region)){
    return(NULL);
  }

  /* find */
  recycling = start_region;

  if(recycling != NULL){
    g_object_ref(recycling);
  }
  
  while(recycling != NULL &&
	recycling != end_region){
    GObject *current_channel;

    gboolean success;
    
    g_object_get(recycling,
		 "channel", &current_channel,
		 NULL);
    
    /* check if new match */
    success = (current_channel != prev_channel) ? TRUE: FALSE;
    g_object_unref(current_channel);
    
    if(success){      
      return(recycling);
    }
    
    /* iterate */
    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);
    
    recycling = next_recycling;
  }

  if(recycling != NULL){
    g_object_unref(recycling);
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
 * Since: 3.0.0
 */
gint
ags_recycling_position(AgsRecycling *start_region, AgsRecycling *end_region,
		       AgsRecycling *recycling)
{
  AgsRecycling *current, *next;

  gint position;

  if(!AGS_IS_RECYCLING(start_region)){
    return(-1);
  }

  /* determine position */
  current = start_region;
  g_object_ref(current);

  position = -1;
    
  while(current != NULL && current != end_region){
    position++;

    /* check if new match */
    if(current == recycling){
      break;
    }

    /* iterate */
    next = ags_recycling_next(current);

    g_object_unref(current);

    current = next;
  }

  if(current != NULL){
    g_object_unref(current);
  }
  
  return(position);
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
 * Since: 3.0.0
 */
gboolean
ags_recycling_is_active(AgsRecycling *start_region, AgsRecycling *end_region,
			GObject *recall_id)
{
  AgsRecycling *current, *next;
  AgsRecyclingContext *recycling_context;

  GList *start_list, *list;

  gboolean is_active;
  gboolean success;
  
  if(!AGS_IS_RECYCLING(start_region) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }
  
  current = start_region;
  g_object_ref(current);

  success = FALSE;
    
  while(current != end_region){
    /* get audio signal */
    g_object_get(current,
		 "audio-signal", &start_list,
		 NULL);

    /* is active */
    is_active = (ags_audio_signal_is_active(start_list,
					    recall_id)) ? TRUE: FALSE;

    g_list_free_full(start_list,
		     g_object_unref);
    
    if(is_active){      
      success = TRUE;

      break;
    }

    /* iterate */
    next = ags_recycling_next(current);

    g_object_unref(next);

    current = next;
  }

  if(current != NULL){
    g_object_unref(current);
  }
  
  return(success);
}

/**
 * ags_recycling_new:
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Creates a #AgsRecycling, with defaults of @soundcard.
 *
 * Returns: a new #AgsRecycling
 *
 * Since: 3.0.0
 */
AgsRecycling*
ags_recycling_new(GObject *output_soundcard)
{
  AgsRecycling *recycling;

  recycling = (AgsRecycling *) g_object_new(AGS_TYPE_RECYCLING,
					    "output-soundcard", output_soundcard,
					    NULL);

  return(recycling);
}
