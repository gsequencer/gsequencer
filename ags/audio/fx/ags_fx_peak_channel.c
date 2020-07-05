/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_peak_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/task/ags_reset_fx_peak.h>

#include <ags/i18n.h>

void ags_fx_peak_channel_class_init(AgsFxPeakChannelClass *fx_peak_channel);
void ags_fx_peak_channel_init(AgsFxPeakChannel *fx_peak_channel);
void ags_fx_peak_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_peak_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_peak_channel_dispose(GObject *gobject);
void ags_fx_peak_channel_finalize(GObject *gobject);

void ags_fx_peak_channel_notify_buffer_size_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data);

static AgsPluginPort* ags_fx_peak_channel_get_peak_plugin_port();

/**
 * SECTION:ags_fx_peak_channel
 * @short_description: fx peak channel
 * @title: AgsFxPeakChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_peak_channel.h
 *
 * The #AgsFxPeakChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_peak_channel_parent_class = NULL;

const gchar *ags_fx_peak_channel_plugin_name = "ags-fx-peak";

const gchar* ags_fx_peak_channel_specifier[] = {
  "./peak[0]",
  NULL,
};

const gchar* ags_fx_peak_channel_control_port[] = {
  "1/1",
  NULL,
};

enum{
  PROP_0,
  PROP_PEAK,
};

GType
ags_fx_peak_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_peak_channel = 0;

    static const GTypeInfo ags_fx_peak_channel_info = {
      sizeof (AgsFxPeakChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_peak_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxPeakChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_peak_channel_init,
    };

    ags_type_fx_peak_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsFxPeakChannel",
						      &ags_fx_peak_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_peak_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_peak_channel_class_init(AgsFxPeakChannelClass *fx_peak_channel)
{
  GObjectClass *gobject;
  
  GParamSpec *param_spec;

  ags_fx_peak_channel_parent_class = g_type_class_peek_parent(fx_peak_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_peak_channel;

  gobject->set_property = ags_fx_peak_channel_set_property;
  gobject->get_property = ags_fx_peak_channel_get_property;

  gobject->dispose = ags_fx_peak_channel_dispose;
  gobject->finalize = ags_fx_peak_channel_finalize;

  /* properties */
  /**
   * AgsFxPeakChannel:peak:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("peak",
				   i18n_pspec("peak of recall"),
				   i18n_pspec("The recall's peak"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK,
				  param_spec);
}

void
ags_fx_peak_channel_init(AgsFxPeakChannel *fx_peak_channel)
{
  AgsResetFxPeak *reset_fx_peak;

  guint buffer_size;
  guint i;
  
  g_signal_connect(fx_peak_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_peak_channel_notify_buffer_size_callback), NULL);

  AGS_RECALL(fx_peak_channel)->name = "ags-fx-peak";
  AGS_RECALL(fx_peak_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_peak_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_peak_channel)->xml_type = "ags-fx-peak-channel";

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  fx_peak_channel->peak_reseted = TRUE;

  g_object_get(fx_peak_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* peak */
  fx_peak_channel->peak = g_object_new(AGS_TYPE_PORT,
				       "plugin-name", ags_fx_peak_channel_plugin_name,
				       "specifier", ags_fx_peak_channel_specifier[0],
				       "control-port", ags_fx_peak_channel_control_port[0],
				       "port-value-is-pointer", FALSE,
				       "port-value-type", G_TYPE_FLOAT,
				       "port-value-size", sizeof(gfloat),
				       "port-value-length", 1,
				       NULL);
  ags_port_set_flags(fx_peak_channel->peak, AGS_PORT_IS_OUTPUT);
  
  fx_peak_channel->peak->port_value.ags_port_float = 0.0;

  g_object_set(fx_peak_channel->peak,
	       "plugin-port", ags_fx_peak_channel_get_peak_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_peak_channel,
		      fx_peak_channel->peak);

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_peak_channel->input_data[i] = ags_fx_peak_channel_input_data_alloc();
      
    fx_peak_channel->input_data[i]->parent = fx_peak_channel;

    fx_peak_channel->input_data[i]->buffer = (gdouble *) g_malloc(buffer_size * sizeof(gdouble));
  }

  /* add to reset peak task */
  reset_fx_peak = ags_reset_fx_peak_get_instance();

  ags_reset_fx_peak_add(reset_fx_peak,
			fx_peak_channel);
}

void
ags_fx_peak_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxPeakChannel *fx_peak_channel;

  GRecMutex *recall_mutex;

  fx_peak_channel = AGS_FX_PEAK_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel);

  switch(prop_id){
  case PROP_PEAK:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_peak_channel->peak){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_peak_channel->peak != NULL){
      g_object_unref(G_OBJECT(fx_peak_channel->peak));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_peak_channel->peak = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_peak_channel_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxPeakChannel *fx_peak_channel;

  GRecMutex *recall_mutex;

  fx_peak_channel = AGS_FX_PEAK_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel);

  switch(prop_id){
  case PROP_PEAK:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_peak_channel->peak);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_peak_channel_dispose(GObject *gobject)
{
  AgsFxPeakChannel *fx_peak_channel;
  
  fx_peak_channel = AGS_FX_PEAK_CHANNEL(gobject);

  /* peak */
  if(fx_peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(fx_peak_channel->peak));

    fx_peak_channel->peak = NULL;
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_channel_parent_class)->dispose(gobject);
}

void
ags_fx_peak_channel_finalize(GObject *gobject)
{
  AgsFxPeakChannel *fx_peak_channel;

  AgsResetFxPeak *reset_fx_peak;

  guint i;
  
  fx_peak_channel = AGS_FX_PEAK_CHANNEL(gobject);

  /* peak */
  if(fx_peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(fx_peak_channel->peak));
  }

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_peak_channel_input_data_free(fx_peak_channel->input_data[i]);
  }

  /* reset ags-fx-peak task */
  reset_fx_peak = ags_reset_fx_peak_get_instance();
  
  ags_reset_fx_peak_remove(reset_fx_peak,
			   fx_peak_channel);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_channel_parent_class)->finalize(gobject);
}

void
ags_fx_peak_channel_notify_buffer_size_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsFxPeakChannel *fx_peak_channel;

  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_peak_channel = AGS_FX_PEAK_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel);

  /* get buffer size */
  g_object_get(fx_peak_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxPeakChannelInputData *input_data;

    input_data = fx_peak_channel->input_data[i];

    if(buffer_size > 0){
      if(input_data->buffer == NULL){
	input_data->buffer = (gdouble *) g_malloc(buffer_size * sizeof(gdouble));
      }else{
	input_data->buffer = (gdouble *) g_realloc(input_data->buffer,
						   buffer_size * sizeof(gdouble));	    
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_peak_channel_input_data_alloc:
 * 
 * Allocate #AgsFxPeakChannelInputData-struct
 * 
 * Returns: the new #AgsFxPeakChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxPeakChannelInputData*
ags_fx_peak_channel_input_data_alloc()
{
  AgsFxPeakChannelInputData *input_data;

  input_data = (AgsFxPeakChannelInputData *) g_malloc(sizeof(AgsFxPeakChannelInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->buffer = NULL;

  return(input_data);
}

/**
 * ags_fx_peak_channel_input_data_free:
 * @input_data: the #AgsFxPeakChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_peak_channel_input_data_free(AgsFxPeakChannelInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  ags_stream_free(input_data->buffer);
  
  g_free(input_data);
}

static AgsPluginPort*
ags_fx_peak_channel_get_peak_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_OUTPUT |
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
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_peak_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxPeakChannel
 *
 * Returns: the new #AgsFxPeakChannel
 *
 * Since: 3.3.0
 */
AgsFxPeakChannel*
ags_fx_peak_channel_new(AgsChannel *channel)
{
  AgsFxPeakChannel *fx_peak_channel;

  fx_peak_channel = (AgsFxPeakChannel *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL,
						      "source", channel,
						      NULL);

  return(fx_peak_channel);
}
