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

#include <ags/audio/fx/ags_fx_low_pass_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_low_pass_channel_class_init(AgsFxLowPassChannelClass *fx_low_pass_channel);
void ags_fx_low_pass_channel_init(AgsFxLowPassChannel *fx_low_pass_channel);
void ags_fx_low_pass_channel_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_fx_low_pass_channel_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_fx_low_pass_channel_dispose(GObject *gobject);
void ags_fx_low_pass_channel_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_low_pass_channel_get_q_lin_plugin_port();
static AgsPluginPort* ags_fx_low_pass_channel_get_filter_gain_plugin_port();

/**
 * SECTION:ags_fx_low_pass_channel
 * @short_description: fx low_pass channel
 * @title: AgsFxLowPassChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_low_pass_channel.h
 *
 * The #AgsFxLowPassChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_low_pass_channel_parent_class = NULL;

const gchar *ags_fx_low_pass_channel_plugin_name = "ags-fx-low_pass";

const gchar* ags_fx_low_pass_channel_specifier[] = {
  "./q-lin[0]",
  "./filter-gain[0]",
  NULL,
};

const gchar* ags_fx_low_pass_channel_control_port[] = {
  "1/2",
  "2/2",
  NULL,
};

enum{
  PROP_0,
  PROP_Q_LIN,
  PROP_FILTER_GAIN,
};

GType
ags_fx_low_pass_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_low_pass_channel = 0;

    static const GTypeInfo ags_fx_low_pass_channel_info = {
      sizeof (AgsFxLowPassChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_low_pass_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxLowPassChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_low_pass_channel_init,
    };

    ags_type_fx_low_pass_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							  "AgsFxLowPassChannel",
							  &ags_fx_low_pass_channel_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_low_pass_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_low_pass_channel_class_init(AgsFxLowPassChannelClass *fx_low_pass_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_low_pass_channel_parent_class = g_type_class_peek_parent(fx_low_pass_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_low_pass_channel;

  gobject->set_property = ags_fx_low_pass_channel_set_property;
  gobject->get_property = ags_fx_low_pass_channel_get_property;

  gobject->dispose = ags_fx_low_pass_channel_dispose;
  gobject->finalize = ags_fx_low_pass_channel_finalize;

  /* properties */
  /**
   * AgsFxLowPassChannel:q-lin:
   *
   * The q-lin port.
   * 
   * Since: 3.13.0
   */
  param_spec = g_param_spec_object("q-lin",
				   i18n_pspec("q-lin of recall"),
				   i18n_pspec("The recall's q-lin"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Q_LIN,
				  param_spec);

  /**
   * AgsFxLowPassChannel:filter-gain:
   *
   * The filter gain port.
   * 
   * Since: 3.13.0
   */
  param_spec = g_param_spec_object("filter-gain",
				   i18n_pspec("filter gain of recall"),
				   i18n_pspec("The recall's filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILTER_GAIN,
				  param_spec);
}

void
ags_fx_low_pass_channel_init(AgsFxLowPassChannel *fx_low_pass_channel)
{
  AGS_RECALL(fx_low_pass_channel)->name = "ags-fx-low_pass";
  AGS_RECALL(fx_low_pass_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_low_pass_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_low_pass_channel)->xml_type = "ags-fx-low_pass-channel";

  /* q_lin */
  fx_low_pass_channel->q_lin = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_low_pass_channel_plugin_name,
					    "specifier", ags_fx_low_pass_channel_specifier[0],
					    "control-port", ags_fx_low_pass_channel_control_port[0],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);
  
  fx_low_pass_channel->q_lin->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_low_pass_channel->q_lin,
	       "plugin-port", ags_fx_low_pass_channel_get_q_lin_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_low_pass_channel,
		      fx_low_pass_channel->q_lin);

  /* filter gain */
  fx_low_pass_channel->filter_gain = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_low_pass_channel_plugin_name,
					       "specifier", ags_fx_low_pass_channel_specifier[1],
					       "control-port", ags_fx_low_pass_channel_control_port[1],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_low_pass_channel->filter_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_low_pass_channel->filter_gain,
	       "plugin-port", ags_fx_filter_gain_channel_get_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_low_pass_channel,
		      fx_low_pass_channel->filter_gain);
}

void
ags_fx_low_pass_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsFxLowPassChannel *fx_low_pass_channel;

  GRecMutex *recall_mutex;

  fx_low_pass_channel = AGS_FX_LOW_PASS_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_low_pass_channel);

  switch(prop_id){
  case PROP_Q_LIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_low_pass_channel->q_lin){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_low_pass_channel->q_lin != NULL){
      g_object_unref(G_OBJECT(fx_low_pass_channel->q_lin));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_low_pass_channel->q_lin = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_FILTER_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_low_pass_channel->filter_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_low_pass_channel->filter_gain != NULL){
      g_object_unref(G_OBJECT(fx_low_pass_channel->filter_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_low_pass_channel->filter_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_low_pass_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsFxLowPassChannel *fx_low_pass_channel;

  GRecMutex *recall_mutex;

  fx_low_pass_channel = AGS_FX_LOW_PASS_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_low_pass_channel);

  switch(prop_id){
  case PROP_Q_LIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_low_pass_channel->q_lin);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_FILTER_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_low_pass_channel->filter_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_low_pass_channel_dispose(GObject *gobject)
{
  AgsFxLowPassChannel *fx_low_pass_channel;
  
  fx_low_pass_channel = AGS_FX_LOW_PASS_CHANNEL(gobject);

  /* q-lin */
  if(fx_low_pass_channel->q_lin != NULL){
    g_object_unref(G_OBJECT(fx_low_pass_channel->q_lin));

    fx_low_pass_channel->q_lin = NULL;
  }  

  /* filter gain */
  if(fx_low_pass_channel->filter_gain != NULL){
    g_object_unref(G_OBJECT(fx_low_pass_channel->filter_gain));

    fx_low_pass_channel->filter_gain = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_low_pass_channel_parent_class)->dispose(gobject);
}

void
ags_fx_low_pass_channel_finalize(GObject *gobject)
{
  AgsFxLowPassChannel *fx_low_pass_channel;
  
  fx_low_pass_channel = AGS_FX_LOW_PASS_CHANNEL(gobject);

  /* q-lin */
  if(fx_low_pass_channel->q_lin != NULL){
    g_object_unref(G_OBJECT(fx_low_pass_channel->q_lin));
  }

  /* filter gain */
  if(fx_low_pass_channel->filter_gain != NULL){
    g_object_unref(G_OBJECT(fx_low_pass_channel->filter_gain));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_low_pass_channel_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_low_pass_channel_get_q_lin_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_low_pass_channel_get_filter_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_low_pass_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxLowPassChannel
 *
 * Returns: the new #AgsFxLowPassChannel
 *
 * Since: 3.13.0
 */
AgsFxLowPassChannel*
ags_fx_low_pass_channel_new(AgsChannel *channel)
{
  AgsFxLowPassChannel *fx_low_pass_channel;

  fx_low_pass_channel = (AgsFxLowPassChannel *) g_object_new(AGS_TYPE_FX_LOW_PASS_CHANNEL,
							     "source", channel,
							     NULL);

  return(fx_low_pass_channel);
}
