/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_peak_channel.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_peak_channel_class_init(AgsPeakChannelClass *peak_channel);
void ags_peak_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_peak_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_peak_channel_init(AgsPeakChannel *peak_channel);
void ags_peak_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_peak_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_peak_channel_connect(AgsConnectable *connectable);
void ags_peak_channel_disconnect(AgsConnectable *connectable);
void ags_peak_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_peak_channel_dispose(GObject *gobject);
void ags_peak_channel_finalize(GObject *gobject);

static AgsPortDescriptor* ags_peak_channel_get_peak_port_descriptor();

/**
 * SECTION:ags_peak_channel
 * @short_description: peaks channel
 * @title: AgsPeakChannel
 * @section_id:
 * @include: ags/audio/recall/ags_peak_channel.h
 *
 * The #AgsPeakChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_BUFFER_CLEARED,
  PROP_BUFFER_COMPUTED,
  PROP_PEAK,
};

static gpointer ags_peak_channel_parent_class = NULL;
static AgsConnectableInterface *ags_peak_channel_parent_connectable_interface;
static AgsPluginInterface *ags_peak_channel_parent_plugin_interface;

static const gchar *ags_peak_channel_plugin_name = "ags-peak";
static const gchar *ags_peak_channel_plugin_specifier[] = {
  "./buffer-cleared[0]",
  "./buffer-computed[0]",
  "./peak[0]",
};
static const gchar *ags_peak_channel_plugin_control_port[] = {
  "1/3",
  "2/3",
  "3/3",
};

GType
ags_peak_channel_get_type()
{
  static GType ags_type_peak_channel = 0;

  if(!ags_type_peak_channel){
    static const GTypeInfo ags_peak_channel_info = {
      sizeof (AgsPeakChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_peak_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsPeakChannel",
						   &ags_peak_channel_info,
						   0);

    g_type_add_interface_static(ags_type_peak_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_peak_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_peak_channel);
}

void
ags_peak_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_peak_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_peak_channel_connect;
  connectable->disconnect = ags_peak_channel_disconnect;
}

void
ags_peak_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_peak_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_peak_channel_set_ports;
}

void
ags_peak_channel_class_init(AgsPeakChannelClass *peak_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_peak_channel_parent_class = g_type_class_peek_parent(peak_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_channel;

  gobject->set_property = ags_peak_channel_set_property;
  gobject->get_property = ags_peak_channel_get_property;

  gobject->dispose = ags_peak_channel_dispose;
  gobject->finalize = ags_peak_channel_finalize;

  /* properties */
  /**
   * AgsAudioSignal:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 0.9.6
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioSignal:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 0.9.6
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudioSignal:format:
   *
   * The format to be used.
   * 
   * Since: 0.9.6
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsPeakChannel:buffer-cleared:
   * 
   * The property indicating if buffer was cleared.
   * 
   * Since: 0.9.6
   */
  param_spec = g_param_spec_object("buffer-cleared",
				   i18n_pspec("if buffer was cleared"),
				   i18n_pspec("The buffer was cleared during this run"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_CLEARED,
				  param_spec);

  /**
   * AgsPeakChannel:buffer-computed:
   * 
   * The property indicating if buffer was computed.
   * 
   * Since: 0.9.6
   */
  param_spec = g_param_spec_object("buffer-computed",
				   i18n_pspec("if buffer was computed"),
				   i18n_pspec("The buffer was computed during this run"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_COMPUTED,
				  param_spec);

  /**
   * AgsPeakChannel:peak:
   * 
   * The peak of the channel.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("peak",
				   i18n_pspec("peak of channel"),
				   i18n_pspec("The peak of channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK,
				  param_spec);
}

void
ags_peak_channel_init(AgsPeakChannel *peak_channel)
{
  AgsConfig *config;
  
  GList *port;

  gchar *str;
  
  AGS_RECALL(peak_channel)->flags |= AGS_RECALL_HAS_OUTPUT_PORT;

  AGS_RECALL(peak_channel)->name = "ags-peak";
  AGS_RECALL(peak_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_channel)->xml_type = "ags-peak-channel";

  config = ags_config_get_instance();

  /* buffer field */
  peak_channel->buffer_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(peak_channel->buffer_mutex,
		     NULL);

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
    peak_channel->samplerate = g_ascii_strtoull(str,
						NULL,
						10);
    free(str);
  }else{
    peak_channel->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  /* buffer-size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    peak_channel->buffer_size = g_ascii_strtoull(str,
						 NULL,
						 10);
    free(str);
  }else{
    peak_channel->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
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
    peak_channel->format = g_ascii_strtoull(str,
					    NULL,
					    10);
    free(str);
  }else{
    peak_channel->format = AGS_SOUNDCARD_SIGNED_16_BIT;
  }
  
  /* ports */
  port = NULL;

  /* buffer cleared */
  peak_channel->buffer_cleared = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_peak_channel_plugin_name,
					      "specifier", ags_peak_channel_plugin_specifier[0],
					      "control-port", ags_peak_channel_plugin_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_BOOLEAN,
					      "port-value-size", sizeof(gboolean),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(peak_channel->buffer_cleared);
  
  peak_channel->buffer_cleared->flags |= AGS_PORT_IS_OUTPUT;
  peak_channel->buffer_cleared->port_value.ags_port_boolean = FALSE;

  /* add to port */  
  port = g_list_prepend(port, peak_channel->peak);
  g_object_ref(peak_channel->buffer_cleared);

  /* buffer computed */
  peak_channel->buffer_computed = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_peak_channel_plugin_name,
					       "specifier", ags_peak_channel_plugin_specifier[1],
					       "control-port", ags_peak_channel_plugin_control_port[1],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_BOOLEAN,
					       "port-value-size", sizeof(gboolean),
					       "port-value-length", 1,
					       NULL);
  g_object_ref(peak_channel->buffer_computed);
  
  peak_channel->buffer_computed->flags |= AGS_PORT_IS_OUTPUT;
  peak_channel->buffer_computed->port_value.ags_port_float = FALSE;

  /* add to port */  
  port = g_list_prepend(port, peak_channel->peak);
  g_object_ref(peak_channel->peak);

  /* peak */
  peak_channel->peak = g_object_new(AGS_TYPE_PORT,
				    "plugin-name", ags_peak_channel_plugin_name,
				    "specifier", ags_peak_channel_plugin_specifier[2],
				    "control-port", ags_peak_channel_plugin_control_port[2],
				    "port-value-is-pointer", FALSE,
				    "port-value-type", G_TYPE_FLOAT,
				    "port-value-size", sizeof(gfloat),
				    "port-value-length", 1,
				    NULL);
  g_object_ref(peak_channel->peak);
  
  peak_channel->peak->flags |= AGS_PORT_IS_OUTPUT;
  peak_channel->peak->port_value.ags_port_float = FALSE;
  
  /* port descriptor */
  peak_channel->peak->port_descriptor = ags_peak_channel_get_peak_port_descriptor();

  /* add to port */  
  port = g_list_prepend(port, peak_channel->peak);
  g_object_ref(peak_channel->peak);

  /* set port */
  AGS_RECALL(peak_channel)->port = port;
}

void
ags_peak_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PEAK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == peak_channel->peak){
	return;
      }

      if(peak_channel->peak != NULL){
	g_object_unref(G_OBJECT(peak_channel->peak));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->peak = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_peak_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PEAK:
    {
      g_value_set_object(value, peak_channel->peak);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_peak_channel_dispose(GObject *gobject)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  /* buffer cleared */
  if(peak_channel->buffer_cleared != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_cleared));

    peak_channel->buffer_cleared = NULL;
  }

  /* buffer computed */
  if(peak_channel->buffer_computed != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_computed));

    peak_channel->buffer_computed = NULL;
  }

  /* peak */
  if(peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(peak_channel->peak));

    peak_channel->peak = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_parent_class)->dispose(gobject);
}

void
ags_peak_channel_finalize(GObject *gobject)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  /* buffer field */
  pthread_mutex_destroy(peak_channel->buffer_mutex);
  free(peak_channel->buffer_mutex);
  
  free(peak_channel->buffer);

  /* buffer cleared */
  if(peak_channel->buffer_cleared != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_cleared));
  }

  /* buffer computed */
  if(peak_channel->buffer_computed != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_computed));
  }
  
  /* peak */
  if(peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(peak_channel->peak));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_parent_class)->finalize(gobject);
}

void
ags_peak_channel_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  ags_peak_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_peak_channel_disconnect(AgsConnectable *connectable)
{
  ags_peak_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_peak_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./peak[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "peak", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_peak_channel_retrieve_peak(AgsPeakChannel *peak_channel,
			       gboolean is_play)
{
  AgsAudio *audio;
  AgsChannel *source;
  AgsRecycling *recycling, *recycling_end_region, *tmp;
  
  AgsMutexManager *mutex_manager;

  GList *audio_signal;

  gchar *str;

  static const double scale_precision = 10.0;

  signed short *buffer;
  double current_value;
  guint buffer_size;
  guint limit;
  guint i;
  guint copy_mode;
  gboolean output_has_recycling, input_has_recycling;
  
  GValue *value;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  if(peak_channel == NULL){
    return;
  }
  
  source = AGS_RECALL_CHANNEL(peak_channel)->source;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) source);
	
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  audio = AGS_AUDIO(source->audio);
  
  recycling = source->first_recycling;
  tmp = source->last_recycling;
  
  buffer_size = source->buffer_size;

  pthread_mutex_unlock(channel_mutex);

  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get some flags */
  pthread_mutex_lock(audio_mutex);

  input_has_recycling = ((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(source->audio)->flags)) != 0) ? TRUE: FALSE;
  output_has_recycling = ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(source->audio)->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(audio_mutex);

  /* lookup recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) tmp);
	
  pthread_mutex_unlock(application_mutex);

  /* get end region */
  pthread_mutex_lock(recycling_mutex);

  recycling_end_region = tmp->next;
  
  pthread_mutex_unlock(recycling_mutex);
  
  /* initialize buffer */
  buffer = (signed short *) malloc(buffer_size * sizeof(signed short));
  memset(buffer,
	 0,
	 buffer_size * sizeof(signed short));

  while(recycling != recycling_end_region){
    /* lookup recycling mutex */
    pthread_mutex_lock(application_mutex);

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);
	
    pthread_mutex_unlock(application_mutex);

    /* get current peak */
    pthread_mutex_lock(recycling_mutex);
      
    audio_signal = recycling->audio_signal;

    while(audio_signal != NULL){
      copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_16_BIT),
						      ags_audio_buffer_util_format_from_soundcard(AGS_AUDIO_SIGNAL(audio_signal->data)->format));

      if((AGS_IS_INPUT(source) &&
	  input_has_recycling) ||
	 (AGS_IS_OUTPUT(source) &&
	  output_has_recycling)){
	if((AGS_AUDIO_SIGNAL_TEMPLATE & (AGS_AUDIO_SIGNAL(audio_signal->data)->flags)) == 0 &&
	   AGS_AUDIO_SIGNAL(audio_signal->data)->stream_current != NULL){
	  /* copy buffer 1:1 */
	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
						      AGS_AUDIO_SIGNAL(audio_signal->data)->stream_current->data, 1, 0,
						      buffer_size, copy_mode);
	}
      }else{
	if((AGS_AUDIO_SIGNAL_TEMPLATE & (AGS_AUDIO_SIGNAL(audio_signal->data)->flags)) == 0 &&
	   AGS_AUDIO_SIGNAL(audio_signal->data)->stream_current != NULL){
	  /* copy buffer 1:1 */
	  ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
						      AGS_AUDIO_SIGNAL(audio_signal->data)->stream_current->data, 1, 0,
						      buffer_size, copy_mode);
	}
      }

      audio_signal = audio_signal->next;
    }

    recycling = recycling->next;

    pthread_mutex_unlock(recycling_mutex);
  }

  /* calculate average value */
  current_value = 0.0;
  
  limit = buffer_size - 8;

  for(i = 0; i < limit; i += 8){
    /* unrolled loop */
    if(buffer[i] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i]));
    }

    if(buffer[i + 1] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 1]));
    }

    if(buffer[i + 2] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 2]));
    }

    if(buffer[i + 3] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 3]));
    }
    
    if(buffer[i + 4] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 4]));
    }

    if(buffer[i + 5] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 5]));
    }
    
    if(buffer[i + 6] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 6]));
    }

    if(buffer[i + 7] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 7]));
    }
  }

  for(; i < buffer_size; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i]));
    }
  }
  
  /* break down to scale */
  //TODO:JK: verify me
  if(current_value != 0.0){
    current_value = scale_precision * (atan(1.0 / 440.0) / sin(current_value / 22000.0));
  }

  value = g_new0(GValue,
		 1);
  g_value_init(value, G_TYPE_FLOAT);

  if(current_value < 0.0){
    current_value *= -1.0;
  }

  g_value_set_float(value,
		    current_value);

  ags_port_safe_write(peak_channel->peak,
		      value);
  g_value_unset(value);
  g_free(value);
  
  /* free buffer */
  free(buffer);
}

void
ags_peak_channel_buffer_add(AgsPeakChannel *peak_channel,
			    void *buffer,
			    guint samplerate, guint buffer_size, guint format)
{
  void *buffer_source;
  
  guint copy_mode;
  gboolean resample;

  if(peak_channel == NULL){
    return;
  }

  resample = FALSE;
  
  pthread_mutex_lock(peak_channel->buffer_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(peak_channel->format),
						  format);

  if(samplerate != peak_channel->samplerate){
    buffer_source = ags_audio_buffer_util_resample(buffer, 1,
						   ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						   buffer_size,
						   peak_channel->samplerate);
    
    resample = TRUE;
  }else{
    buffer_source = buffer;
  }
  
  ags_audio_buffer_util_copy_buffer_to_buffer(peak_channel->buffer, 1, 0,
					      buffer_source, 1, 0,
					      peak_channel->buffer_size, copy_mode);


  pthread_mutex_unlock(peak_channel->buffer_mutex);

  if(resample){
    free(buffer_source);
  }
}

void
ags_peak_channel_retrieve_peak_internal(AgsPeakChannel *peak_channel)
{
  signed short *buffer;

  double current_value;
  guint limit;
  guint i;

  GValue *value;  

  if(peak_channel == NULL){
    return;
  }

  pthread_mutex_lock(peak_channel->buffer_mutex);

  //TODO:JK: support for different word size
  buffer = peak_channel->buffer;
  
  /* calculate average value */
  current_value = 0.0;
  
  limit = peak_channel->buffer_size - 8;

  for(i = 0; i < limit; i += 8){
    /* unrolled loop */
    if(buffer[i] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i]));
    }

    if(buffer[i + 1] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 1]));
    }

    if(buffer[i + 2] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 2]));
    }

    if(buffer[i + 3] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 3]));
    }
    
    if(buffer[i + 4] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 4]));
    }

    if(buffer[i + 5] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 5]));
    }
    
    if(buffer[i + 6] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 6]));
    }

    if(buffer[i + 7] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i + 7]));
    }
  }

  for(; i < peak_channel->buffer_size; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / (1.0 / (double) G_MAXUINT16 * buffer[i]));
    }
  }
  
  pthread_mutex_unlock(peak_channel->buffer_mutex);

  /* break down to scale */
  //TODO:JK: verify me
  if(current_value != 0.0){
    current_value = scale_precision * (atan(1.0 / 440.0) / sin(current_value / 22000.0));
  }

  value = g_new0(GValue,
		 1);
  g_value_init(value, G_TYPE_FLOAT);

  if(current_value < 0.0){
    current_value *= -1.0;
  }

  g_value_set_float(value,
		    current_value);

  ags_port_safe_write(peak_channel->peak,
		      value);
  g_value_unset(value);
  g_free(value);
}

static AgsPortDescriptor*
ags_peak_channel_get_peak_port_descriptor()
{
  static AgsPortDescriptor *port_descriptor = NULL;

  if(port_descriptor == NULL){
    port_descriptor = ags_port_descriptor_alloc();

    port_descriptor->flags |= (AGS_PORT_DESCRIPTOR_INPUT |
			       AGS_PORT_DESCRIPTOR_CONTROL);

    port_descriptor->port_index = 0;

    /* range */
    g_value_init(port_descriptor->default_value,
		 G_TYPE_FLOAT);
    g_value_init(port_descriptor->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(port_descriptor->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(port_descriptor->default_value,
		      0.0);
    g_value_set_float(port_descriptor->lower_value,
		      0.0);
    g_value_set_float(port_descriptor->upper_value,
		      10.0);
  }
  
  return(port_descriptor);
}

/**
 * ags_peak_channel_new:
 * @source: the #AgsChannel as source
 *
 * Creates an #AgsPeakChannel
 *
 * Returns: a new #AgsPeakChannel
 *
 * Since: 0.4
 */
AgsPeakChannel*
ags_peak_channel_new(AgsChannel *source)
{
  AgsPeakChannel *peak_channel;

  peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						 "channel", source,
						 NULL);

  return(peak_channel);
}
