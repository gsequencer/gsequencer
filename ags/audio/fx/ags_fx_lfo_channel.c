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

#include <ags/audio/fx/ags_fx_lfo_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_synth_enums.h>

#include <ags/i18n.h>

void ags_fx_lfo_channel_class_init(AgsFxLfoChannelClass *fx_lfo_channel);
void ags_fx_lfo_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_fx_lfo_channel_init(AgsFxLfoChannel *fx_lfo_channel);
void ags_fx_lfo_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_fx_lfo_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_fx_lfo_channel_dispose(GObject *gobject);
void ags_fx_lfo_channel_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_lfo_channel_get_enabled_plugin_port();
static AgsPluginPort* ags_fx_lfo_channel_get_lfo_wave_plugin_port();
static AgsPluginPort* ags_fx_lfo_channel_get_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_lfo_channel_get_lfo_phase_plugin_port();
static AgsPluginPort* ags_fx_lfo_channel_get_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_lfo_channel_get_lfo_tuning_plugin_port();

/**
 * SECTION:ags_fx_lfo_channel
 * @short_description: fx lfo channel
 * @title: AgsFxLfoChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lfo_channel.h
 *
 * The #AgsFxLfoChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_lfo_channel_parent_class = NULL;

const gchar *ags_fx_lfo_channel_plugin_name = "ags-fx-lfo";

const gchar* ags_fx_lfo_channel_specifier[] = {
  "./enabled[0]",
  "./lfo-wave[0]",
  "./lfo-freq[0]",
  "./lfo-phase[0]",
  "./lfo-depth[0]",
  "./lfo-tuning[0]",
  NULL,
};

const gchar* ags_fx_lfo_channel_control_port[] = {
  "1/6",
  "2/6",
  "3/6",
  "4/6",
  "5/6",
  "6/6",
  NULL,
};

enum{
  PROP_0,
  PROP_ENABLED,
  PROP_LFO_WAVE,
  PROP_LFO_FREQ,
  PROP_LFO_PHASE,
  PROP_LFO_DEPTH,
  PROP_LFO_TUNING,
};

GType
ags_fx_lfo_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lfo_channel = 0;

    static const GTypeInfo ags_fx_lfo_channel_info = {
      sizeof (AgsFxLfoChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lfo_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxLfoChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lfo_channel_init,
    };

    ags_type_fx_lfo_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsFxLfoChannel",
						     &ags_fx_lfo_channel_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lfo_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lfo_channel_class_init(AgsFxLfoChannelClass *fx_lfo_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_lfo_channel_parent_class = g_type_class_peek_parent(fx_lfo_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lfo_channel;

  gobject->set_property = ags_fx_lfo_channel_set_property;
  gobject->get_property = ags_fx_lfo_channel_get_property;

  gobject->dispose = ags_fx_lfo_channel_dispose;
  gobject->finalize = ags_fx_lfo_channel_finalize;

  /* properties */
  /**
   * AgsFxLfoChannel:enabled:
   *
   * The enable control of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("enabled",
				   i18n_pspec("LFO enabled"),
				   i18n_pspec("The enabled port to control LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENABLED,
				  param_spec);

  /**
   * AgsFxLfoChannel:lfo-wave:
   *
   * The wave of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("lfo-wave",
				   i18n_pspec("LFO wave"),
				   i18n_pspec("The wave of the LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_WAVE,
				  param_spec);

  /**
   * AgsFxLfoChannel:lfo-freq:
   *
   * The freq of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("lfo-freq",
				   i18n_pspec("LFO freq"),
				   i18n_pspec("The freq of the LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_FREQ,
				  param_spec);

  /**
   * AgsFxLfoChannel:lfo-phase:
   *
   * The phase of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("lfo-phase",
				   i18n_pspec("LFO phase"),
				   i18n_pspec("The phase of the LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_PHASE,
				  param_spec);

  /**
   * AgsFxLfoChannel:lfo-depth:
   *
   * The depth of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("lfo-depth",
				   i18n_pspec("LFO depth"),
				   i18n_pspec("The depth of the LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxLfoChannel:lfo-tuning:
   *
   * The tuning of LFO.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("lfo-tuning",
				   i18n_pspec("LFO tuning"),
				   i18n_pspec("The tuning of the LFO"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_TUNING,
				  param_spec);
}

void
ags_fx_lfo_channel_init(AgsFxLfoChannel *fx_lfo_channel)
{
  guint i;
  
  AGS_RECALL(fx_lfo_channel)->name = "ags-fx-lfo";
  AGS_RECALL(fx_lfo_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lfo_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lfo_channel)->xml_type = "ags-fx-lfo-channel";

  /* enabled */
  fx_lfo_channel->enabled = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", "ags-lfo",
					 "specifier", "./enabled[0]",
					 "control-port", "1/6",
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_FLOAT,
					 "port-value-size", sizeof(gfloat),
					 "port-value-length", 1,
					 NULL);
  
  fx_lfo_channel->enabled->port_value.ags_port_float = 0.0;

  g_object_set(fx_lfo_channel->enabled,
	       "plugin-port", ags_fx_lfo_channel_get_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->enabled);

  /* lfo wave */
  fx_lfo_channel->lfo_wave = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", "ags-lfo",
					  "specifier", "./lfo-wave[0]",
					  "control-port", "2/6",
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  
  fx_lfo_channel->lfo_wave->port_value.ags_port_float = 0.0;

  g_object_set(fx_lfo_channel->lfo_wave,
	       "plugin-port", ags_fx_lfo_channel_get_lfo_wave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->lfo_wave);

  /* lfo-freq */
  fx_lfo_channel->lfo_freq = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", "ags-lfo",
					  "specifier", "./lfo-freq[0]",
					  "control-port", "3/6",
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  
  fx_lfo_channel->lfo_freq->port_value.ags_port_float = 6.0;

  g_object_set(fx_lfo_channel->lfo_freq,
	       "plugin-port", ags_fx_lfo_channel_get_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->lfo_freq);

  /* lfo phase */
  fx_lfo_channel->lfo_phase = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", "ags-lfo",
					   "specifier", "./lfo-phase[0]",
					   "control-port", "4/6",
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);

  fx_lfo_channel->lfo_phase->port_value.ags_port_float = 0.0;

  g_object_set(fx_lfo_channel->lfo_phase,
	       "plugin-port", ags_fx_lfo_channel_get_lfo_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->lfo_phase);

  /* lfo depth */
  fx_lfo_channel->lfo_depth = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", "ags-lfo",
					   "specifier", "./lfo-depth[0]",
					   "control-port", "5/6",
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);
  
  fx_lfo_channel->lfo_depth->port_value.ags_port_float = 1.0;

  g_object_set(fx_lfo_channel->lfo_depth,
	       "plugin-port", ags_fx_lfo_channel_get_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->lfo_depth);

  /* lfo tuning */
  fx_lfo_channel->lfo_tuning = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", "ags-lfo",
					    "specifier", "./lfo-tuning[0]",
					    "control-port", "6/6",
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);

  fx_lfo_channel->lfo_tuning->port_value.ags_port_float = 0.0;

  g_object_set(fx_lfo_channel->lfo_tuning,
	       "plugin-port", ags_fx_lfo_channel_get_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_lfo_channel,
		      fx_lfo_channel->lfo_tuning);

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_lfo_channel->input_data[i] = ags_fx_lfo_channel_input_data_alloc();
      
    fx_lfo_channel->input_data[i]->parent = fx_lfo_channel;
  }
}

void
ags_fx_lfo_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsFxLfoChannel *fx_lfo_channel;

  GRecMutex *recall_mutex;

  fx_lfo_channel = AGS_FX_LFO_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lfo_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->enabled){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->enabled != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_WAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->lfo_wave){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->lfo_wave != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->lfo_wave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->lfo_wave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->lfo_freq){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->lfo_freq != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->lfo_freq));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->lfo_freq = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->lfo_phase){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->lfo_phase != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->lfo_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->lfo_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->lfo_depth){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->lfo_depth != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_lfo_channel->lfo_tuning){      
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_lfo_channel->lfo_tuning != NULL){
      g_object_unref(G_OBJECT(fx_lfo_channel->lfo_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_lfo_channel->lfo_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_lfo_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsFxLfoChannel *fx_lfo_channel;

  GRecMutex *recall_mutex;

  fx_lfo_channel = AGS_FX_LFO_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lfo_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_WAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->lfo_wave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->lfo_freq);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->lfo_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_lfo_channel->lfo_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_lfo_channel_dispose(GObject *gobject)
{
  AgsFxLfoChannel *fx_lfo_channel;
  
  fx_lfo_channel = AGS_FX_LFO_CHANNEL(gobject);

  /* enabled */
  if(fx_lfo_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->enabled));

    fx_lfo_channel->enabled = NULL;
  }

  /* lfo wave */
  if(fx_lfo_channel->lfo_wave != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_wave));

    fx_lfo_channel->lfo_wave = NULL;
  }

  /* lfo freq */
  if(fx_lfo_channel->lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_freq));

    fx_lfo_channel->lfo_freq = NULL;
  }

  /* lfo phase */
  if(fx_lfo_channel->lfo_phase != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_phase));

    fx_lfo_channel->lfo_phase = NULL;
  }

  /* lfo depth */
  if(fx_lfo_channel->lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_depth));

    fx_lfo_channel->lfo_depth = NULL;
  }

  /* lfo tuning */
  if(fx_lfo_channel->lfo_tuning != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_tuning));

    fx_lfo_channel->lfo_tuning = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_channel_parent_class)->dispose(gobject);
}

void
ags_fx_lfo_channel_finalize(GObject *gobject)
{
  AgsFxLfoChannel *fx_lfo_channel;

  guint i;
  
  fx_lfo_channel = AGS_FX_LFO_CHANNEL(gobject);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_lfo_channel_input_data_free(fx_lfo_channel->input_data[i]);

    fx_lfo_channel->input_data[i] = NULL;
  }

  /* enabled */
  if(fx_lfo_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->enabled));
  }

  /* lfo wave */
  if(fx_lfo_channel->lfo_wave != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_wave));
  }

  /* lfo freq */
  if(fx_lfo_channel->lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_freq));
  }

  /* lfo phase */
  if(fx_lfo_channel->lfo_phase != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_phase));
  }

  /* lfo depth */
  if(fx_lfo_channel->lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_depth));
  }

  /* lfo tuning */
  if(fx_lfo_channel->lfo_tuning != NULL){
    g_object_unref(G_OBJECT(fx_lfo_channel->lfo_tuning));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_channel_parent_class)->finalize(gobject);
}


static AgsPluginPort*
ags_fx_lfo_channel_get_enabled_plugin_port()
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
ags_fx_lfo_channel_get_lfo_wave_plugin_port()
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
		      (gfloat) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }
  
  g_mutex_unlock(&mutex);

  return(plugin_port);
}

static AgsPluginPort*
ags_fx_lfo_channel_get_lfo_freq_plugin_port()
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
ags_fx_lfo_channel_get_lfo_phase_plugin_port()
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
ags_fx_lfo_channel_get_lfo_depth_plugin_port()
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
ags_fx_lfo_channel_get_lfo_tuning_plugin_port()
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
 * ags_fx_lfo_channel_input_data_alloc:
 * 
 * Allocate #AgsFxLfoChannelInputData-struct
 * 
 * Returns: the new #AgsFxLfoChannelInputData-struct
 * 
 * Since: 4.0.0
 */
AgsFxLfoChannelInputData*
ags_fx_lfo_channel_input_data_alloc()
{
  AgsFxLfoChannelInputData *input_data;

  input_data = (AgsFxLfoChannelInputData *) g_malloc(sizeof(AgsFxLfoChannelInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->lfo_synth_util = ags_lfo_synth_util_alloc();

  return(input_data);
}

/**
 * ags_fx_lfo_channel_input_data_free:
 * @input_data: the #AgsFxLfoChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lfo_channel_input_data_free(AgsFxLfoChannelInputData *input_data)
{
  if(input_data == NULL){
    return;
  }
  ags_lfo_synth_util_free(input_data->lfo_synth_util);  
  
  g_free(input_data);
}

/**
 * ags_fx_lfo_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxLfoChannel
 *
 * Returns: the new #AgsFxLfoChannel
 *
 * Since: 3.8.0
 */
AgsFxLfoChannel*
ags_fx_lfo_channel_new(AgsChannel *channel)
{
  AgsFxLfoChannel *fx_lfo_channel;

  fx_lfo_channel = (AgsFxLfoChannel *) g_object_new(AGS_TYPE_FX_LFO_CHANNEL,
						    "source", channel,
						    NULL);

  return(fx_lfo_channel);
}
