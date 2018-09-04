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

#include <ags/audio/recall/ags_peak_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_reset_peak.h>

#include <math.h>

#include <ags/i18n.h>

void ags_peak_channel_class_init(AgsPeakChannelClass *peak_channel);
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
void ags_peak_channel_dispose(GObject *gobject);
void ags_peak_channel_finalize(GObject *gobject);

void ags_peak_channel_set_ports(AgsPlugin *plugin, GList *port);

static AgsPluginPort* ags_peak_channel_get_peak_plugin_port();

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
  PROP_SCALE_PRECISION,
  PROP_PEAK,
};

static gpointer ags_peak_channel_parent_class = NULL;
static AgsPluginInterface *ags_peak_channel_parent_plugin_interface;

static const gchar *ags_peak_channel_plugin_name = "ags-peak";
static const gchar *ags_peak_channel_plugin_specifier[] = {
  "./buffer-cleared[0]",
  "./buffer-computed[0]",
  "./scale-precision[0]",
  "./peak[0]",
};
static const gchar *ags_peak_channel_plugin_control_port[] = {
  "1/4",
  "2/4",
  "3/4",
  "4/4",
};

GType
ags_peak_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_peak_channel = 0;

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
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_peak_channel);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsPeakChannel:buffer-computed:
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
   * AgsPeakChannel:scale-precision:
   * 
   * The property indicating if scale was precision.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("scale-precision",
				   i18n_pspec("scale precision"),
				   i18n_pspec("The scale precision to multiply the peak"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);


  /**
   * AgsPeakChannel:peak:
   * 
   * The peak of the channel.
   * 
   * Since: 2.0.0
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
  AgsResetPeak *reset_peak;
  
  AgsConfig *config;
  
  GList *port;

  gchar *str;

  pthread_mutexattr_t *attr;
  
  AGS_RECALL(peak_channel)->flags |= AGS_RECALL_HAS_OUTPUT_PORT;

  AGS_RECALL(peak_channel)->name = "ags-peak";
  AGS_RECALL(peak_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_channel)->xml_type = "ags-peak-channel";

  /* buffer field */  
  peak_channel->buffer_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  peak_channel->buffer_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(peak_channel->buffer_mutex,
		     attr);

  /* read config */
  config = ags_config_get_instance();

  peak_channel->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  peak_channel->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  peak_channel->format = ags_soundcard_helper_config_get_format(config);

  /* allocate internal buffer */
  peak_channel->buffer = ags_stream_alloc(peak_channel->buffer_size,
					  peak_channel->format);
  
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
  
  peak_channel->buffer_cleared->port_value.ags_port_boolean = FALSE;

  /* add to port */  
  port = g_list_prepend(port, peak_channel->buffer_cleared);
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
  
  peak_channel->buffer_computed->port_value.ags_port_boolean = FALSE;

  /* add to port */  
  port = g_list_prepend(port, peak_channel->buffer_computed);
  g_object_ref(peak_channel->buffer_computed);

  /* peak */
  peak_channel->scale_precision = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_peak_channel_plugin_name,
					       "specifier", ags_peak_channel_plugin_specifier[2],
					       "control-port", ags_peak_channel_plugin_control_port[2],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  g_object_ref(peak_channel->scale_precision);
  
  peak_channel->scale_precision->port_value.ags_port_float = 10.0;
  
  /* add to port */  
  port = g_list_prepend(port, peak_channel->scale_precision);
  g_object_ref(peak_channel->scale_precision);

  /* peak */
  peak_channel->peak = g_object_new(AGS_TYPE_PORT,
				    "plugin-name", ags_peak_channel_plugin_name,
				    "specifier", ags_peak_channel_plugin_specifier[3],
				    "control-port", ags_peak_channel_plugin_control_port[3],
				    "port-value-is-pointer", FALSE,
				    "port-value-type", G_TYPE_FLOAT,
				    "port-value-size", sizeof(gfloat),
				    "port-value-length", 1,
				    NULL);
  g_object_ref(peak_channel->peak);
  
  peak_channel->peak->flags |= AGS_PORT_IS_OUTPUT;
  peak_channel->peak->port_value.ags_port_float = 0.0;
  
  /* plugin port */
  g_object_set(peak_channel->peak,
	       "plugin-port", ags_peak_channel_get_peak_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, peak_channel->peak);
  g_object_ref(peak_channel->peak);

  /* set port */
  AGS_RECALL(peak_channel)->port = port;

  /* add to reset peak task */
  reset_peak = ags_reset_peak_get_instance();
  ags_reset_peak_add(reset_peak,
		     peak_channel);
}

void
ags_peak_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPeakChannel *peak_channel;

  pthread_mutex_t *recall_mutex;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      peak_channel->samplerate = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      peak_channel->buffer_size = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      peak_channel->format = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_CLEARED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == peak_channel->buffer_cleared){      
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(peak_channel->buffer_cleared != NULL){
	g_object_unref(G_OBJECT(peak_channel->buffer_cleared));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->buffer_cleared = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_COMPUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == peak_channel->buffer_computed){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(peak_channel->buffer_computed != NULL){
	g_object_unref(G_OBJECT(peak_channel->buffer_computed));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->buffer_computed = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SCALE_PRECISION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == peak_channel->scale_precision){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(peak_channel->scale_precision != NULL){
	g_object_unref(G_OBJECT(peak_channel->scale_precision));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->scale_precision = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == peak_channel->peak){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(peak_channel->peak != NULL){
	g_object_unref(G_OBJECT(peak_channel->peak));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->peak = port;
      
      pthread_mutex_unlock(recall_mutex);	
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

  pthread_mutex_t *recall_mutex;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, peak_channel->samplerate);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, peak_channel->buffer_size);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, peak_channel->format);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_CLEARED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, peak_channel->buffer_cleared);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BUFFER_COMPUTED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, peak_channel->buffer_computed);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SCALE_PRECISION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, peak_channel->scale_precision);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, peak_channel->peak);
      
      pthread_mutex_unlock(recall_mutex);	
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

  AgsResetPeak *reset_peak;

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

  /* scale precision */
  if(peak_channel->scale_precision != NULL){
    g_object_unref(G_OBJECT(peak_channel->scale_precision));

    peak_channel->scale_precision = NULL;
  }

  /* peak */
  if(peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(peak_channel->peak));

    peak_channel->peak = NULL;
  }

  /* reset peak task */
  reset_peak = ags_reset_peak_get_instance();
  ags_reset_peak_remove(reset_peak,
			peak_channel);

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_parent_class)->dispose(gobject);
}

void
ags_peak_channel_finalize(GObject *gobject)
{
  AgsPeakChannel *peak_channel;

  AgsResetPeak *reset_peak;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  /* buffer field */
  pthread_mutex_destroy(peak_channel->buffer_mutex);
  free(peak_channel->buffer_mutex);

  pthread_mutexattr_destroy(peak_channel->buffer_mutexattr);
  free(peak_channel->buffer_mutexattr);
  
  free(peak_channel->buffer);

  /* buffer cleared */
  if(peak_channel->buffer_cleared != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_cleared));
  }

  /* buffer computed */
  if(peak_channel->buffer_computed != NULL){
    g_object_unref(G_OBJECT(peak_channel->buffer_computed));
  }

  /* scale precision */
  if(peak_channel->scale_precision != NULL){
    g_object_unref(G_OBJECT(peak_channel->scale_precision));
  }
  
  /* peak */
  if(peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(peak_channel->peak));
  }

  /* reset peak task */
  reset_peak = ags_reset_peak_get_instance();
  ags_reset_peak_remove(reset_peak,
			peak_channel);

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_parent_class)->finalize(gobject);
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

static AgsPluginPort*
ags_peak_channel_get_peak_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      10.0);
  }

  pthread_mutex_unlock(&mutex);
    
  return(plugin_port);
}

void
ags_peak_channel_buffer_add(AgsPeakChannel *peak_channel,
			    void *buffer,
			    guint samplerate, guint buffer_size, guint format)
{
  void *buffer_source;
  
  guint copy_mode;
  guint target_samplerate;
  guint target_buffer_size;
  guint target_format;
  gboolean resample;

  pthread_mutex_t *buffer_mutex;

  if(!AGS_IS_PEAK_CHANNEL(peak_channel)){
    return;
  }

  /* get buffer mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  buffer_mutex = peak_channel->buffer_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  g_object_get(peak_channel,
	       "samplerate", &target_samplerate,
	       "buffer-size", &target_buffer_size,
	       "format", &target_format,
	       NULL);
  
  resample = FALSE;
  
  pthread_mutex_lock(buffer_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
						  ags_audio_buffer_util_format_from_soundcard(format));

  if(samplerate != target_samplerate){
    buffer_source = ags_audio_buffer_util_resample(buffer, 1,
						   ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						   buffer_size,
						   target_samplerate);
    
    resample = TRUE;
  }else{
    buffer_source = buffer;
  }
  
  ags_audio_buffer_util_copy_buffer_to_buffer(peak_channel->buffer, 1, 0,
					      buffer_source, 1, 0,
					      target_buffer_size, copy_mode);


  pthread_mutex_unlock(buffer_mutex);

  if(resample){
    free(buffer_source);
  }
}

void
ags_peak_channel_retrieve_peak_internal(AgsPeakChannel *peak_channel)
{
  AgsPort *scale_precision;
  AgsPort *peak;
  
  void *buffer;
  
  gdouble current_scale_precision;
  gdouble current_value;
  guint target_format;
  
  GValue value = {0,};

  pthread_mutex_t *buffer_mutex;

  if(!AGS_IS_PEAK_CHANNEL(peak_channel)){
    return;
  }

  /* get buffer mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  buffer_mutex = peak_channel->buffer_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* calculate average value */
  g_object_get(peak_channel,
	       "format", &target_format,
	       NULL);
  
  pthread_mutex_lock(buffer_mutex);

  current_value = ags_audio_buffer_util_peak(peak_channel->buffer, 1,
					     ags_audio_buffer_util_format_from_soundcard(target_format),
					     peak_channel->buffer_size,
					     440.0,
					     22000.0,
					     1.0);

  pthread_mutex_unlock(buffer_mutex);

  g_object_get(peak_channel,
	       "scale-precision", &scale_precision,
	       "peak", &peak,
	       NULL);

  /* break down to scale */
  g_value_init(&value,
	       G_TYPE_FLOAT);

  ags_port_safe_read(scale_precision,
		     &value);

  current_scale_precision = g_value_get_float(&value);

  current_value *= current_scale_precision;
  
  if(current_value < 0.0){
    current_value *= -1.0;
  }

  /* set peak */
  g_value_set_float(&value,
		    current_value);

  ags_port_safe_write(peak,
		      &value);
  g_value_unset(&value);
}

/**
 * ags_peak_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPeakChannel
 *
 * Returns: the new #AgsPeakChannel
 *
 * Since: 2.0.0
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
