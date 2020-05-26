/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_lfo_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_synth_enums.h>

#include <ags/i18n.h>

void ags_lfo_channel_class_init(AgsLfoChannelClass *lfo_channel);
void ags_lfo_channel_init(AgsLfoChannel *lfo_channel);
void ags_lfo_channel_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_lfo_channel_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_lfo_channel_dispose(GObject *gobject);
void ags_lfo_channel_finalize(GObject *gobject);

static AgsPluginPort* ags_lfo_channel_get_enabled_plugin_port();
static AgsPluginPort* ags_lfo_channel_get_lfo_wave_plugin_port();
static AgsPluginPort* ags_lfo_channel_get_lfo_freq_plugin_port();
static AgsPluginPort* ags_lfo_channel_get_lfo_phase_plugin_port();
static AgsPluginPort* ags_lfo_channel_get_lfo_depth_plugin_port();
static AgsPluginPort* ags_lfo_channel_get_lfo_tuning_plugin_port();

/**
 * SECTION:ags_lfo_channel
 * @short_description: lfos channel
 * @title: AgsLfoChannel
 * @section_id:
 * @include: ags/audio/recall/ags_lfo_channel.h
 *
 * The #AgsLfoChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_ENABLED,
  PROP_LFO_WAVE,
  PROP_LFO_FREQ,
  PROP_LFO_PHASE,
  PROP_LFO_DEPTH,
  PROP_LFO_TUNING,
};

static gpointer ags_lfo_channel_parent_class = NULL;

const gchar *ags_lfo_channel_plugin_name = "ags-lfo";
const gchar *ags_lfo_channel_specifier[] = {
  "./enabled[0]",
  "./lfo-wave[0]",
  "./lfo-freq[0]",
  "./lfo-phase[0]",
  "./lfo-depth[0]",
  "./lfo-tuning[0]"
};
const gchar *ags_lfo_channel_control_port[] = {
  "1/6",
  "2/6",
  "3/6",
  "4/6",
  "5/6",
  "6/6"
};

GType
ags_lfo_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lfo_channel = 0;

    static const GTypeInfo ags_lfo_channel_info = {
      sizeof (AgsLfoChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lfo_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLfoChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lfo_channel_init,
    };

    ags_type_lfo_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						  "AgsLfoChannel",
						  &ags_lfo_channel_info,
						  0);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_lfo_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_lfo_channel_class_init(AgsLfoChannelClass *lfo_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_lfo_channel_parent_class = g_type_class_peek_parent(lfo_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) lfo_channel;

  gobject->set_property = ags_lfo_channel_set_property;
  gobject->get_property = ags_lfo_channel_get_property;

  gobject->dispose = ags_lfo_channel_dispose;
  gobject->finalize = ags_lfo_channel_finalize;

  /* properties */
  /**
   * AgsLfoChannel:enabled:
   * 
   * The enabled port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("enabled",
				   i18n_pspec("enabled"),
				   i18n_pspec("If LFO is enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENABLED,
				  param_spec);

  /**
   * AgsLfoChannel:lfo-wave:
   * 
   * The lfo-wave port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("lfo-wave",
				   i18n_pspec("lfo wave to apply"),
				   i18n_pspec("The lfo wave to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_WAVE,
				  param_spec);

  /**
   * AgsLfoChannel:lfo-freq:
   * 
   * The lfo-freq port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("lfo-freq",
				   i18n_pspec("lfo frequency to apply"),
				   i18n_pspec("The lfo frequency to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_FREQ,
				  param_spec);

  /**
   * AgsLfoChannel:lfo-phase:
   * 
   * The lfo-phase port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("lfo-phase",
				   i18n_pspec("lfo phase to apply"),
				   i18n_pspec("The lfo phase to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_PHASE,
				  param_spec);

  /**
   * AgsLfoChannel:lfo-depth:
   * 
   * The lfo-depth port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("lfo-depth",
				   i18n_pspec("lfo depth to apply"),
				   i18n_pspec("The lfo depth to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_DEPTH,
				  param_spec);

  /**
   * AgsLfoChannel:lfo-tuning:
   * 
   * The lfo-tuning port.
   * 
   * Since: 3.0.0 
   */
  param_spec = g_param_spec_object("lfo-tuning",
				   i18n_pspec("lfo tuning to apply"),
				   i18n_pspec("The lfo tuning to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_TUNING,
				  param_spec);
}

void
ags_lfo_channel_init(AgsLfoChannel *lfo_channel)
{
  GList *port;

  AGS_RECALL(lfo_channel)->name = "ags-lfo";
  AGS_RECALL(lfo_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(lfo_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(lfo_channel)->xml_type = "ags-lfo-channel";

  /* initialize the port */
  port = NULL;

  /* enabled */
  lfo_channel->enabled = g_object_new(AGS_TYPE_PORT,
				      "plugin-name", "ags-lfo",
				      "specifier", "./enabled[0]",
				      "control-port", "1/5",
				      "port-value-is-pointer", FALSE,
				      "port-value-type", G_TYPE_FLOAT,
				      "port-value-size", sizeof(gfloat),
				      "port-value-length", 1,
				      NULL);
  g_object_ref(lfo_channel->enabled);
  
  lfo_channel->enabled->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->enabled,
	       "plugin-port", ags_lfo_channel_get_enabled_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->enabled);
  g_object_ref(lfo_channel->enabled);

  /* lfo wave */
  lfo_channel->lfo_wave = g_object_new(AGS_TYPE_PORT,
				       "plugin-name", "ags-lfo",
				       "specifier", "./lfo-wave[0]",
				       "control-port", "4/5",
				       "port-value-is-pointer", FALSE,
				       "port-value-type", G_TYPE_FLOAT,
				       "port-value-size", sizeof(gfloat),
				       "port-value-length", 1,
				       NULL);
  g_object_ref(lfo_channel->lfo_wave);
  
  lfo_channel->lfo_wave->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->lfo_wave,
	       "plugin-port", ags_lfo_channel_get_lfo_wave_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->lfo_wave);
  g_object_ref(lfo_channel->lfo_wave);

  /* lfo-freq */
  lfo_channel->lfo_freq = g_object_new(AGS_TYPE_PORT,
				       "plugin-name", "ags-lfo",
				       "specifier", "./lfo-freq[0]",
				       "control-port", "2/5",
				       "port-value-is-pointer", FALSE,
				       "port-value-type", G_TYPE_FLOAT,
				       "port-value-size", sizeof(gfloat),
				       "port-value-length", 1,
				       NULL);
  g_object_ref(lfo_channel->lfo_freq);
  
  lfo_channel->lfo_freq->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->lfo_freq,
	       "plugin-port", ags_lfo_channel_get_lfo_freq_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->lfo_freq);
  g_object_ref(lfo_channel->lfo_freq);

  /* lfo phase */
  lfo_channel->lfo_phase = g_object_new(AGS_TYPE_PORT,
					"plugin-name", "ags-lfo",
					"specifier", "./lfo-phase[0]",
					"control-port", "3/5",
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  g_object_ref(lfo_channel->lfo_phase);
  
  lfo_channel->lfo_phase->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->lfo_phase,
	       "plugin-port", ags_lfo_channel_get_lfo_phase_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->lfo_phase);
  g_object_ref(lfo_channel->lfo_phase);

  /* lfo depth */
  lfo_channel->lfo_depth = g_object_new(AGS_TYPE_PORT,
					"plugin-name", "ags-lfo",
					"specifier", "./lfo-depth[0]",
					"control-port", "4/5",
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  g_object_ref(lfo_channel->lfo_depth);
  
  lfo_channel->lfo_depth->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->lfo_depth,
	       "plugin-port", ags_lfo_channel_get_lfo_depth_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->lfo_depth);
  g_object_ref(lfo_channel->lfo_depth);

  /* lfo tuning */
  lfo_channel->lfo_tuning = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", "ags-lfo",
					 "specifier", "./lfo-tuning[0]",
					 "control-port", "5/5",
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_FLOAT,
					 "port-value-size", sizeof(gfloat),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(lfo_channel->lfo_tuning);
  
  lfo_channel->lfo_tuning->port_value.ags_port_float = 0.0;

  /* port descriptor */
  g_object_set(lfo_channel->lfo_tuning,
	       "plugin-port", ags_lfo_channel_get_lfo_tuning_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, lfo_channel->lfo_tuning);
  g_object_ref(lfo_channel->lfo_tuning);

  /* set port */
  AGS_RECALL(lfo_channel)->port = port;
}

void
ags_lfo_channel_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsLfoChannel *lfo_channel;

  GRecMutex *recall_mutex;

  lfo_channel = AGS_LFO_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(lfo_channel);
  
  switch(prop_id){
  case PROP_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->enabled){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->enabled != NULL){
      g_object_unref(G_OBJECT(lfo_channel->enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_WAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->lfo_wave){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->lfo_wave != NULL){
      g_object_unref(G_OBJECT(lfo_channel->lfo_wave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->lfo_wave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->lfo_freq){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->lfo_freq != NULL){
      g_object_unref(G_OBJECT(lfo_channel->lfo_freq));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->lfo_freq = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->lfo_phase){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->lfo_phase != NULL){
      g_object_unref(G_OBJECT(lfo_channel->lfo_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->lfo_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->lfo_depth){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->lfo_depth != NULL){
      g_object_unref(G_OBJECT(lfo_channel->lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == lfo_channel->lfo_tuning){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(lfo_channel->lfo_tuning != NULL){
      g_object_unref(G_OBJECT(lfo_channel->lfo_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    lfo_channel->lfo_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lfo_channel_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsLfoChannel *lfo_channel;

  GRecMutex *recall_mutex;

  lfo_channel = AGS_LFO_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(lfo_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_WAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->lfo_wave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->lfo_freq);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->lfo_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, lfo_channel->lfo_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lfo_channel_dispose(GObject *gobject)
{
  AgsLfoChannel *lfo_channel;

  lfo_channel = AGS_LFO_CHANNEL(gobject);

  /* enabled */
  if(lfo_channel->enabled != NULL){
    g_object_unref(G_OBJECT(lfo_channel->enabled));
  }

  /* lfo wave */
  if(lfo_channel->lfo_wave != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_wave));
  }

  /* lfo freq */
  if(lfo_channel->lfo_freq != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_freq));
  }

  /* lfo phase */
  if(lfo_channel->lfo_phase != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_phase));
  }

  /* lfo depth */
  if(lfo_channel->lfo_depth != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_depth));
  }

  /* lfo tuning */
  if(lfo_channel->lfo_tuning != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_tuning));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lfo_channel_parent_class)->dispose(gobject);
}

void
ags_lfo_channel_finalize(GObject *gobject)
{
  AgsLfoChannel *lfo_channel;

  lfo_channel = AGS_LFO_CHANNEL(gobject);

  /* enabled */
  if(lfo_channel->enabled != NULL){
    g_object_unref(G_OBJECT(lfo_channel->enabled));
  }

  /* lfo wave */
  if(lfo_channel->lfo_wave != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_wave));
  }

  /* lfo freq */
  if(lfo_channel->lfo_freq != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_freq));
  }

  /* lfo phase */
  if(lfo_channel->lfo_phase != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_phase));
  }

  /* lfo depth */
  if(lfo_channel->lfo_depth != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_depth));
  }

  /* lfo tuning */
  if(lfo_channel->lfo_tuning != NULL){
    g_object_unref(G_OBJECT(lfo_channel->lfo_tuning));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lfo_channel_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_lfo_channel_get_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

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
		      1.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_lfo_channel_get_lfo_wave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_ENUMERATION);

    plugin_port->port_index = 1;

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
		      (gdouble) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_lfo_channel_get_lfo_freq_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 2;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      6.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      20.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_lfo_channel_get_lfo_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 3;

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
		      G_MAXDOUBLE);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_lfo_channel_get_lfo_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 4;

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
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_lfo_channel_get_lfo_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 5;

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
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      -1200.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

/**
 * ags_lfo_channel_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsLfoChannel
 *
 * Returns: the new #AgsLfoChannel
 *
 * Since: 3.0.0
 */
AgsLfoChannel*
ags_lfo_channel_new(AgsChannel *source)
{
  AgsLfoChannel *lfo_channel;

  lfo_channel = (AgsLfoChannel *) g_object_new(AGS_TYPE_LFO_CHANNEL,
					       "source", source,
					       NULL);

  return(lfo_channel);
}
