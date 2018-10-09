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

#include <ags/audio/recall/ags_analyse_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_reset_amplitude.h>

#include <math.h>

#include <ags/i18n.h>

void ags_analyse_channel_class_init(AgsAnalyseChannelClass *analyse_channel);
void ags_analyse_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_analyse_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_analyse_channel_init(AgsAnalyseChannel *analyse_channel);
void ags_analyse_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_analyse_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_analyse_channel_dispose(GObject *gobject);
void ags_analyse_channel_finalize(GObject *gobject);

void ags_analyse_channel_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_analyse_channel
 * @short_description: analyses channel
 * @title: AgsAnalyseChannel
 * @section_id:
 * @include: ags/audio/recall/ags_analyse_channel.h
 *
 * The #AgsAnalyseChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_CACHE_SAMPLERATE,
  PROP_CACHE_BUFFER_SIZE,
  PROP_CACHE_FORMAT,
  PROP_BUFFER_CLEARED,
  PROP_BUFFER_COMPUTED,
  PROP_FREQUENCY_BUFFER,
  PROP_MAGNITUDE_BUFFER,
};

static gpointer ags_analyse_channel_parent_class = NULL;
static AgsConnectableInterface *ags_analyse_channel_parent_connectable_interface;
static AgsPluginInterface *ags_analyse_channel_parent_plugin_interface;

static const gchar *ags_analyse_channel_plugin_name = "ags-analyse";
static const gchar *ags_analyse_channel_plugin_specifier[] = {
  "./buffer-cleared[0]",
  "./buffer-computed[0]",
  "./frequency-buffer[0]",
  "./magnitude-buffer[0]",
};
static const gchar *ags_analyse_channel_plugin_control_port[] = {
  "1/4",
  "2/4",
  "3/4",
  "4/4",
};

GType
ags_analyse_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_analyse_channel = 0;

    static const GTypeInfo ags_analyse_channel_info = {
      sizeof (AgsAnalyseChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_analyse_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAnalyseChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_analyse_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_analyse_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_analyse_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_analyse_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsAnalyseChannel",
						      &ags_analyse_channel_info,
						      0);

    g_type_add_interface_static(ags_type_analyse_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_analyse_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_analyse_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_analyse_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_analyse_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_analyse_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_analyse_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_analyse_channel_set_ports;
}

void
ags_analyse_channel_class_init(AgsAnalyseChannelClass *analyse_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_analyse_channel_parent_class = g_type_class_peek_parent(analyse_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) analyse_channel;

  gobject->set_property = ags_analyse_channel_set_property;
  gobject->get_property = ags_analyse_channel_get_property;

  gobject->dispose = ags_analyse_channel_dispose;
  gobject->finalize = ags_analyse_channel_finalize;

  /* properties */
  /**
   * AgsAnalyseChannel:cache-samplerate:
   *
   * The cache's samplerate.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("cache-samplerate",
				 i18n_pspec("cache samplerate"),
				 i18n_pspec("The samplerate of the cache"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CACHE_SAMPLERATE,
				  param_spec);

  /**
   * AgsAnalyseChannel:cache-buffer-size:
   *
   * The cache's buffer length.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("cache-buffer-size",
				 i18n_pspec("cache buffer size"),
				 i18n_pspec("The buffer size of the cache"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CACHE_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAnalyseChannel:cache-format:
   *
   * The cache's format.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("cache-format",
				 i18n_pspec("cache format"),
				 i18n_pspec("The format of the cache"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CACHE_FORMAT,
				  param_spec);

  /**
   * AgsAnalyseChannel:buffer-cleared:
   * 
   * The property indicating if buffer was cleared.
   * 
   * Since: 2.0.0
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
   * AgsAnalyseChannel:buffer-computed:
   * 
   * The property indicating if buffer was computed.
   * 
   * Since: 2.0.0
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
   * AgsAnalyseChannel:frequency-buffer:
   * 
   * The frequency buffer.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("frequency-buffer",
				   i18n_pspec("frequency buffer"),
				   i18n_pspec("The frequency buffer"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY_BUFFER,
				  param_spec);

  /**
   * AgsAnalyseChannel:magnitude-buffer:
   * 
   * The magnitude buffer.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("magnitude-buffer",
				   i18n_pspec("magnitude buffer"),
				   i18n_pspec("The magnitude buffer"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAGNITUDE_BUFFER,
				  param_spec);
}

void
ags_analyse_channel_init(AgsAnalyseChannel *analyse_channel)
{
  AgsResetAmplitude *reset_amplitude;
  
  AgsConfig *config;
  
  GList *port;

  gchar *str;

  pthread_mutexattr_t *attr;
  
  AGS_RECALL(analyse_channel)->flags |= AGS_RECALL_HAS_OUTPUT_PORT;

  AGS_RECALL(analyse_channel)->name = "ags-analyse";
  AGS_RECALL(analyse_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(analyse_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(analyse_channel)->xml_type = "ags-analyse-channel";

  /* buffer field */  
  analyse_channel->buffer_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  analyse_channel->buffer_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(analyse_channel->buffer_mutex,
		     attr);

  /* config */
  config = ags_config_get_instance();

  analyse_channel->cache_samplerate = ags_soundcard_helper_config_get_samplerate(config);
  analyse_channel->cache_buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  analyse_channel->cache_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE;
  
  /* FFTW */
  analyse_channel->in = (double *) fftw_malloc(analyse_channel->cache_buffer_size * sizeof(double));
  analyse_channel->out = (double *) fftw_malloc(analyse_channel->cache_buffer_size * sizeof(double));

  analyse_channel->comout = (fftw_complex *) fftw_malloc(analyse_channel->cache_buffer_size * sizeof(fftw_complex));

  analyse_channel->plan = fftw_plan_r2r_1d(analyse_channel->cache_buffer_size, analyse_channel->in, analyse_channel->out, FFTW_R2HC, FFTW_ESTIMATE);

  /* pre buffer */
  analyse_channel->frequency_pre_buffer = (double *) malloc(ceil(analyse_channel->cache_buffer_size / 2.0) * sizeof(double));
  analyse_channel->magnitude_pre_buffer = (double *) malloc(ceil(analyse_channel->cache_buffer_size / 2.0) * sizeof(double));
  
  /* ports */
  port = NULL;

  /* buffer cleared */
  analyse_channel->buffer_cleared = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_analyse_channel_plugin_name,
						 "specifier", ags_analyse_channel_plugin_specifier[0],
						 "control-port", ags_analyse_channel_plugin_control_port[0],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_BOOLEAN,
						 "port-value-size", sizeof(gboolean),
						 "port-value-length", 1,
						 NULL);
  g_object_ref(analyse_channel->buffer_cleared);
  
  analyse_channel->buffer_cleared->port_value.ags_port_boolean = FALSE;

  /* add to port */  
  port = g_list_prepend(port, analyse_channel->buffer_cleared);
  g_object_ref(analyse_channel->buffer_cleared);

  /* buffer computed */
  analyse_channel->buffer_computed = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_analyse_channel_plugin_name,
						  "specifier", ags_analyse_channel_plugin_specifier[1],
						  "control-port", ags_analyse_channel_plugin_control_port[1],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_BOOLEAN,
						  "port-value-size", sizeof(gboolean),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(analyse_channel->buffer_computed);
  
  analyse_channel->buffer_computed->port_value.ags_port_boolean = FALSE;

  /* add to port */  
  port = g_list_prepend(port, analyse_channel->buffer_computed);
  g_object_ref(analyse_channel->buffer_computed);

  /* frequency buffer */
  analyse_channel->frequency_buffer = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_analyse_channel_plugin_name,
						   "specifier", ags_analyse_channel_plugin_specifier[2],
						   "control-port", ags_analyse_channel_plugin_control_port[2],
						   "port-value-is-pointer", TRUE,
						   "port-value-type", G_TYPE_DOUBLE,
						   "port-value-size", sizeof(gdouble),
						   "port-value-length", (guint) ceil(analyse_channel->cache_buffer_size / 2.0),
						   NULL);
  g_object_ref(analyse_channel->frequency_buffer);
  
  analyse_channel->frequency_buffer->port_value.ags_port_double_ptr = (double *) malloc(analyse_channel->cache_buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(analyse_channel->frequency_buffer->port_value.ags_port_double_ptr, 1,
				     analyse_channel->cache_buffer_size);
  
  /* add to port */  
  port = g_list_prepend(port, analyse_channel->frequency_buffer);
  g_object_ref(analyse_channel->frequency_buffer);

  /* magnitude buffer */
  analyse_channel->magnitude_buffer = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_analyse_channel_plugin_name,
						   "specifier", ags_analyse_channel_plugin_specifier[3],
						   "control-port", ags_analyse_channel_plugin_control_port[3],
						   "port-value-is-pointer", TRUE,
						   "port-value-type", G_TYPE_DOUBLE,
						   "port-value-size", sizeof(gdouble),
						   "port-value-length", (guint) ceil(analyse_channel->cache_buffer_size / 2.0),
						   NULL);
  g_object_ref(analyse_channel->magnitude_buffer);
  
  analyse_channel->magnitude_buffer->port_value.ags_port_double_ptr = (double *) malloc(analyse_channel->cache_buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(analyse_channel->magnitude_buffer->port_value.ags_port_double_ptr, 1,
				     analyse_channel->cache_buffer_size);

  /* add to port */  
  port = g_list_prepend(port, analyse_channel->magnitude_buffer);
  g_object_ref(analyse_channel->magnitude_buffer);

  /* set port */
  AGS_RECALL(analyse_channel)->port = port;

  /* add to reset amplitude task */
  reset_amplitude = ags_reset_amplitude_get_instance();
  ags_reset_amplitude_add(reset_amplitude,
			  analyse_channel);
}

void
ags_analyse_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAnalyseChannel *analyse_channel;

  pthread_mutex_t *recall_mutex;

  analyse_channel = AGS_ANALYSE_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_CACHE_SAMPLERATE:
    { 
      pthread_mutex_lock(recall_mutex);
      
      analyse_channel->cache_samplerate = g_value_get_uint(value);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CACHE_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      analyse_channel->cache_buffer_size = g_value_get_uint(value);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CACHE_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      analyse_channel->cache_format = g_value_get_uint(value);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_COMPUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == analyse_channel->buffer_computed){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(analyse_channel->buffer_computed != NULL){
	g_object_unref(G_OBJECT(analyse_channel->buffer_computed));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      analyse_channel->buffer_computed = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FREQUENCY_BUFFER:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);
      
      if(port == analyse_channel->frequency_buffer){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(analyse_channel->frequency_buffer != NULL){
	g_object_unref(G_OBJECT(analyse_channel->frequency_buffer));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      analyse_channel->frequency_buffer = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MAGNITUDE_BUFFER:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == analyse_channel->magnitude_buffer){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(analyse_channel->magnitude_buffer != NULL){
	g_object_unref(G_OBJECT(analyse_channel->magnitude_buffer));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      analyse_channel->magnitude_buffer = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_analyse_channel_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAnalyseChannel *analyse_channel;

  pthread_mutex_t *recall_mutex;

  analyse_channel = AGS_ANALYSE_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_CACHE_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value,
		       analyse_channel->cache_samplerate);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CACHE_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value,
		       analyse_channel->cache_buffer_size);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CACHE_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value,
		       analyse_channel->cache_format);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_COMPUTED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, analyse_channel->buffer_computed);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FREQUENCY_BUFFER:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, analyse_channel->frequency_buffer);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MAGNITUDE_BUFFER:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, analyse_channel->magnitude_buffer);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_analyse_channel_dispose(GObject *gobject)
{
  AgsAnalyseChannel *analyse_channel;

  AgsResetAmplitude *reset_amplitude;

  analyse_channel = AGS_ANALYSE_CHANNEL(gobject);

  /* buffer cleared */
  if(analyse_channel->buffer_cleared != NULL){
    g_object_unref(G_OBJECT(analyse_channel->buffer_cleared));

    analyse_channel->buffer_cleared = NULL;
  }

  /* buffer computed */
  if(analyse_channel->buffer_computed != NULL){
    g_object_unref(G_OBJECT(analyse_channel->buffer_computed));

    analyse_channel->buffer_computed = NULL;
  }

  /* frequency buffer */
  if(analyse_channel->frequency_buffer != NULL){
    g_object_unref(G_OBJECT(analyse_channel->frequency_buffer));

    analyse_channel->frequency_buffer = NULL;
  }

  /* magnitude buffer */
  if(analyse_channel->magnitude_buffer != NULL){
    g_object_unref(G_OBJECT(analyse_channel->magnitude_buffer));

    analyse_channel->magnitude_buffer = NULL;
  }

  /* reset amplitude task */
  reset_amplitude = ags_reset_amplitude_get_instance();
  ags_reset_amplitude_remove(reset_amplitude,
			     analyse_channel);

  /* call parent */
  G_OBJECT_CLASS(ags_analyse_channel_parent_class)->dispose(gobject);
}

void
ags_analyse_channel_finalize(GObject *gobject)
{
  AgsAnalyseChannel *analyse_channel;

  AgsResetAmplitude *reset_amplitude;

  analyse_channel = AGS_ANALYSE_CHANNEL(gobject);

  /* buffer field */
  pthread_mutex_destroy(analyse_channel->buffer_mutex);
  free(analyse_channel->buffer_mutex);

  pthread_mutexattr_destroy(analyse_channel->buffer_mutexattr);
  free(analyse_channel->buffer_mutexattr);

  fftw_destroy_plan(analyse_channel->plan);
  fftw_free(analyse_channel->in);
  fftw_free(analyse_channel->out);
  
  /* buffer cleared */
  if(analyse_channel->buffer_cleared != NULL){
    g_object_unref(G_OBJECT(analyse_channel->buffer_cleared));
  }

  /* buffer computed */
  if(analyse_channel->buffer_computed != NULL){
    g_object_unref(G_OBJECT(analyse_channel->buffer_computed));
  }

  /* frequency buffer */
  if(analyse_channel->frequency_buffer != NULL){
    g_object_unref(G_OBJECT(analyse_channel->frequency_buffer));
  }

  /* magnitude buffer */
  if(analyse_channel->magnitude_buffer != NULL){
    g_object_unref(G_OBJECT(analyse_channel->magnitude_buffer));
  }
  
  /* reset amplitude task */
  reset_amplitude = ags_reset_amplitude_get_instance();
  ags_reset_amplitude_remove(reset_amplitude,
			     analyse_channel);
  
  /* call parent */
  G_OBJECT_CLASS(ags_analyse_channel_parent_class)->finalize(gobject);
}

void
ags_analyse_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./frequency-buffer[0]",
		22)){
      g_object_set(G_OBJECT(plugin),
		   "frequency-buffer", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./magnitude-buffer[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "magnitude-buffer", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_analyse_channel_buffer_add(AgsAnalyseChannel *analyse_channel,
			       void *buffer,
			       guint samplerate, guint buffer_size, guint format)
{
  void *buffer_source;
  
  guint copy_mode;
  guint cache_samplerate;
  guint cache_buffer_size;
  guint cache_format;
  gboolean resample;

  pthread_mutex_t *buffer_mutex;

  if(!AGS_IS_ANALYSE_CHANNEL(analyse_channel)){
    return;
  }

  /* get buffer mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  buffer_mutex = analyse_channel->buffer_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(analyse_channel,
	       "cache-samplerate", &cache_samplerate,
	       "cache-buffer-size", &cache_buffer_size,
	       "cache-format", &cache_format,
	       NULL);

  resample = FALSE;

  copy_mode = ags_audio_buffer_util_get_copy_mode(cache_format,
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  if(samplerate != cache_samplerate){
    buffer_source = ags_audio_buffer_util_resample(buffer, 1,
						   ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						   buffer_size,
						   cache_samplerate);
    
    resample = TRUE;
  }else{
    buffer_source = buffer;
  }
  
  pthread_mutex_lock(buffer_mutex);
  
  ags_audio_buffer_util_copy_buffer_to_buffer(analyse_channel->in, 1, 0,
					      buffer_source, 1, 0,
					      cache_buffer_size, copy_mode);


  pthread_mutex_unlock(buffer_mutex);

  if(resample){
    free(buffer_source);
  }
}

void
ags_analyse_channel_retrieve_frequency_and_magnitude(AgsAnalyseChannel *analyse_channel)
{
  double *out;
  double *frequency_pre_buffer;
  double *magnitude_pre_buffer;

  guint cache_samplerate;
  guint cache_buffer_size;
  guint cache_format;
  double frequency, magnitude;
  double correction;
  guint i;

  GValue value = {0,};

  pthread_mutex_t *buffer_mutex;

  if(!AGS_IS_ANALYSE_CHANNEL(analyse_channel)){
    return;
  }

  /* get buffer mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  buffer_mutex = analyse_channel->buffer_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(analyse_channel,
	       "cache-samplerate", &cache_samplerate,
	       "cache-buffer-size", &cache_buffer_size,
	       "cache-format", &cache_format,
	       NULL);

  /* get output buffer */
  pthread_mutex_lock(buffer_mutex);

  out = analyse_channel->out;
  
  /* execute plan */
  memset((void *) out, 0, cache_buffer_size * sizeof(double));

  fftw_execute(analyse_channel->plan);
  
  /* retrieve frequency and magnitude */
  correction = (double) cache_samplerate / (double) cache_buffer_size;
  
  for(i = 0; i < cache_buffer_size / 2; i++){
    frequency = i * correction;
    magnitude = sqrt(out[i] * out[i] + out[(cache_buffer_size / 2) + 1 - i] * out[(cache_buffer_size / 2) + 1 - i]);
    //    g_message("analyse[%d]: %f %f", i, frequency, magnitude);

    analyse_channel->frequency_pre_buffer[i] = frequency;
    analyse_channel->magnitude_pre_buffer[i] = magnitude;
  }

  frequency_pre_buffer = analyse_channel->frequency_pre_buffer;
  magnitude_pre_buffer = analyse_channel->magnitude_pre_buffer;
  
  pthread_mutex_unlock(buffer_mutex);

  /* frequency - write array position */
  g_value_init(&value,
	       G_TYPE_POINTER);

  g_value_set_pointer(&value, frequency_pre_buffer);

  ags_port_safe_write(analyse_channel->frequency_buffer, &value);
    
  /* magnitude - write array position */
  g_value_reset(&value);
  
  g_value_set_pointer(&value, analyse_channel->magnitude_pre_buffer);
  
  ags_port_safe_write(analyse_channel->magnitude_buffer, &value);
  
  g_value_unset(&value);
}

/**
 * ags_analyse_channel_new:
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsAnalyseChannel
 *
 * Returns: the new #AgsAnalyseChannel
 *
 * Since: 2.0.0
 */
AgsAnalyseChannel*
ags_analyse_channel_new(AgsChannel *source)
{
  AgsAnalyseChannel *analyse_channel;

  analyse_channel = (AgsAnalyseChannel *) g_object_new(AGS_TYPE_ANALYSE_CHANNEL,
						       "source", source,
						       NULL);

  return(analyse_channel);
}
