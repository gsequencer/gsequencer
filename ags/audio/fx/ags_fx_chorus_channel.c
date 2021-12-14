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

#include <ags/audio/fx/ags_fx_chorus_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_chorus_channel_class_init(AgsFxChorusChannelClass *fx_chorus_channel);
void ags_fx_chorus_channel_init(AgsFxChorusChannel *fx_chorus_channel);
void ags_fx_chorus_channel_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_chorus_channel_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_chorus_channel_dispose(GObject *gobject);
void ags_fx_chorus_channel_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_chorus_channel_get_enabled_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_input_volume_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_output_volume_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_depth_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_mix_plugin_port();
static AgsPluginPort* ags_fx_chorus_channel_get_delay_plugin_port();

/**
 * SECTION:ags_fx_chorus_channel
 * @short_description: fx chorus channel
 * @title: AgsFxChorusChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_chorus_channel.h
 *
 * The #AgsFxChorusChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_chorus_channel_parent_class = NULL;

const gchar *ags_fx_chorus_channel_plugin_name = "ags-fx-chorus";

const gchar* ags_fx_chorus_channel_specifier[] = {
  "./enabled[0]",
  "./input-volume[0]",
  "./output-volume[0]",
  "./lfo-oscillator[0]",
  "./lfo-frequency[0]",
  "./depth[0]",
  "./mix[0]",
  "./delay[0]",
  NULL,
};

const gchar* ags_fx_chorus_channel_control_port[] = {
  "1/8",
  "2/8",
  "3/8",
  "4/8",
  "5/8",
  "6/8",
  "7/8",
  "8/8",
  NULL,
};

enum{
  PROP_0,
  PROP_ENABLED,
  PROP_CHORUS,
};

GType
ags_fx_chorus_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_chorus_channel = 0;

    static const GTypeInfo ags_fx_chorus_channel_info = {
      sizeof (AgsFxChorusChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_chorus_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxChorusChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_chorus_channel_init,
    };

    ags_type_fx_chorus_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsFxChorusChannel",
							&ags_fx_chorus_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_chorus_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_chorus_channel_class_init(AgsFxChorusChannelClass *fx_chorus_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_chorus_channel_parent_class = g_type_class_peek_parent(fx_chorus_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_chorus_channel;

  gobject->set_property = ags_fx_chorus_channel_set_property;
  gobject->get_property = ags_fx_chorus_channel_get_property;

  gobject->dispose = ags_fx_chorus_channel_dispose;
  gobject->finalize = ags_fx_chorus_channel_finalize;

  /* properties */
  /**
   * AgsFxChorusChannel:enabled:
   *
   * The enabled port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("enabled",
				   i18n_pspec("enabled of recall"),
				   i18n_pspec("The recall's enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENABLED,
				  param_spec);

  /**
   * AgsFxChorusChannel:input-volume:
   *
   * The input-volume port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("input-volume",
				   i18n_pspec("input volume of recall"),
				   i18n_pspec("The recall's input volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_VOLUME,
				  param_spec);

  /**
   * AgsFxChorusChannel:output-volume:
   *
   * The output-volume port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("output-volume",
				   i18n_pspec("output volume of recall"),
				   i18n_pspec("The recall's output volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_VOLUME,
				  param_spec);

  /**
   * AgsFxChorusChannel:lfo-oscillator:
   *
   * The LFO oscillator port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("lfo-oscillator",
				   i18n_pspec("LFO oscillator of recall"),
				   i18n_pspec("The recall's LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxChorusChannel:lfo-frequency:
   *
   * The LFO frequency port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("lfo-frequency",
				   i18n_pspec("LFO frequency of recall"),
				   i18n_pspec("The recall's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxChorusChannel:depth:
   *
   * The depth port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("depth",
				   i18n_pspec("depth of recall"),
				   i18n_pspec("The recall's depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEPTH,
				  param_spec);

  /**
   * AgsFxChorusChannel:mix:
   *
   * The mix port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("mix",
				   i18n_pspec("mix of recall"),
				   i18n_pspec("The recall's mix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIX,
				  param_spec);

  /**
   * AgsFxChorusChannel:delay:
   *
   * The delay port.
   * 
   * Since: 3.14.0
   */
  param_spec = g_param_spec_object("delay",
				   i18n_pspec("delay of recall"),
				   i18n_pspec("The recall's delay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);
}

void
ags_fx_chorus_channel_init(AgsFxChorusChannel *fx_chorus_channel)
{
  AGS_RECALL(fx_chorus_channel)->name = "ags-fx-chorus";
  AGS_RECALL(fx_chorus_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_chorus_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_chorus_channel)->xml_type = "ags-fx-chorus-channel";

  /* enabled */
  fx_chorus_channel->enabled = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_chorus_channel_plugin_name,
					    "specifier", ags_fx_chorus_channel_specifier[0],
					    "control-port", ags_fx_chorus_channel_control_port[0],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);
  
  fx_chorus_channel->enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_chorus_channel->enabled,
	       "plugin-port", ags_fx_chorus_channel_get_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->enabled);

  /* input volume */
  fx_chorus_channel->input_volume = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_chorus_channel_plugin_name,
						 "specifier", ags_fx_chorus_channel_specifier[1],
						 "control-port", ags_fx_chorus_channel_control_port[1],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_chorus_channel->input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_chorus_channel->input_volume,
	       "plugin-port", ags_fx_chorus_channel_get_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->input_volume);

  /* output volume */
  fx_chorus_channel->output_volume = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_chorus_channel_plugin_name,
						  "specifier", ags_fx_chorus_channel_specifier[2],
						  "control-port", ags_fx_chorus_channel_control_port[2],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_chorus_channel->output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_chorus_channel->output_volume,
	       "plugin-port", ags_fx_chorus_channel_get_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->output_volume);

  /* LFO oscillator */
  fx_chorus_channel->lfo_oscillator = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_chorus_channel_plugin_name,
						   "specifier", ags_fx_chorus_channel_specifier[3],
						   "control-port", ags_fx_chorus_channel_control_port[3],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_chorus_channel->lfo_oscillator->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_chorus_channel->lfo_oscillator,
	       "plugin-port", ags_fx_chorus_channel_get_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->lfo_oscillator);

  /* LFO frequency */
  fx_chorus_channel->lfo_frequency = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_chorus_channel_plugin_name,
						  "specifier", ags_fx_chorus_channel_specifier[4],
						  "control-port", ags_fx_chorus_channel_control_port[4],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_chorus_channel->lfo_frequency->port_value.ags_port_float = (gfloat) 10.0;

  g_object_set(fx_chorus_channel->lfo_frequency,
	       "plugin-port", ags_fx_chorus_channel_get_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->lfo_frequency);


  /* depth */
  fx_chorus_channel->depth = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_fx_chorus_channel_plugin_name,
					  "specifier", ags_fx_chorus_channel_specifier[5],
					  "control-port", ags_fx_chorus_channel_control_port[5],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  
  fx_chorus_channel->depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_chorus_channel->depth,
	       "plugin-port", ags_fx_chorus_channel_get_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->depth);

  /* mix */
  fx_chorus_channel->mix = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_chorus_channel_plugin_name,
					"specifier", ags_fx_chorus_channel_specifier[6],
					"control-port", ags_fx_chorus_channel_control_port[6],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  
  fx_chorus_channel->mix->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_chorus_channel->mix,
	       "plugin-port", ags_fx_chorus_channel_get_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->mix);

  /* delay */
  fx_chorus_channel->delay = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_fx_chorus_channel_plugin_name,
					  "specifier", ags_fx_chorus_channel_specifier[7],
					  "control-port", ags_fx_chorus_channel_control_port[7],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  
  fx_chorus_channel->delay->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_chorus_channel->delay,
	       "plugin-port", ags_fx_chorus_channel_get_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_chorus_channel,
		      fx_chorus_channel->delay);
}

void
ags_fx_chorus_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxChorusChannel *fx_chorus_channel;

  GRecMutex *recall_mutex;

  fx_chorus_channel = AGS_FX_CHORUS_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_chorus_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->enabled != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_INPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->input_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->input_volume != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->input_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->input_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_OUTPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->output_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->output_volume != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->output_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->output_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->depth != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_MIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->mix){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->mix != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->mix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->mix = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_chorus_channel->delay){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_chorus_channel->delay != NULL){
      g_object_unref(G_OBJECT(fx_chorus_channel->delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_chorus_channel->delay = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_chorus_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxChorusChannel *fx_chorus_channel;

  GRecMutex *recall_mutex;

  fx_chorus_channel = AGS_FX_CHORUS_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_chorus_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_INPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->input_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_OUTPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->output_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_MIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->mix);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_chorus_channel->delay);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_chorus_channel_dispose(GObject *gobject)
{
  AgsFxChorusChannel *fx_chorus_channel;
  
  fx_chorus_channel = AGS_FX_CHORUS_CHANNEL(gobject);

  /* enabled */
  if(fx_chorus_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->enabled));

    fx_chorus_channel->enabled = NULL;
  }  

  /* input volume */
  if(fx_chorus_channel->input_volume != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->input_volume));

    fx_chorus_channel->input_volume = NULL;
  }

  /* output volume */
  if(fx_chorus_channel->output_volume != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->output_volume));

    fx_chorus_channel->output_volume = NULL;
  }

  /* LFO oscillator */
  if(fx_chorus_channel->lfo_oscillator != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->lfo_oscillator));

    fx_chorus_channel->lfo_oscillator = NULL;
  }  

  /* LFO frequency */
  if(fx_chorus_channel->lfo_frequency != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->lfo_frequency));

    fx_chorus_channel->lfo_frequency = NULL;
  }  

  /* depth */
  if(fx_chorus_channel->depth != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->depth));

    fx_chorus_channel->depth = NULL;
  }  

  /* mix */
  if(fx_chorus_channel->mix != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->mix));

    fx_chorus_channel->mix = NULL;
  }  

  /* delay */
  if(fx_chorus_channel->delay != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->delay));

    fx_chorus_channel->delay = NULL;
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_chorus_channel_parent_class)->dispose(gobject);
}

void
ags_fx_chorus_channel_finalize(GObject *gobject)
{
  AgsFxChorusChannel *fx_chorus_channel;
  
  fx_chorus_channel = AGS_FX_CHORUS_CHANNEL(gobject);

  /* enabled */
  if(fx_chorus_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->enabled));
  }

  /* input volume */
  if(fx_chorus_channel->input_volume != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->input_volume));
  }

  /* output volume */
  if(fx_chorus_channel->output_volume != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->output_volume));
  }

  /* LFO oscillator */
  if(fx_chorus_channel->lfo_oscillator != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->lfo_oscillator));
  }

  /* LFO frequency */
  if(fx_chorus_channel->lfo_frequency != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->lfo_frequency));
  }

  /* depth */
  if(fx_chorus_channel->depth != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->depth));
  }

  /* mix */
  if(fx_chorus_channel->mix != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->mix));
  }

  /* delay */
  if(fx_chorus_channel->delay != NULL){
    g_object_unref(G_OBJECT(fx_chorus_channel->delay));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_chorus_channel_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_chorus_channel_get_enabled_plugin_port()
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
ags_fx_chorus_channel_get_input_volume_plugin_port()
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

static AgsPluginPort*
ags_fx_chorus_channel_get_output_volume_plugin_port()
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

static AgsPluginPort*
ags_fx_chorus_channel_get_lfo_oscillator_plugin_port()
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
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_chorus_channel_get_lfo_frequency_plugin_port()
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
		      10.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      10.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_chorus_channel_get_depth_plugin_port()
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
ags_fx_chorus_channel_get_mix_plugin_port()
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
ags_fx_chorus_channel_get_delay_plugin_port()
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

/**
 * ags_fx_chorus_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxChorusChannel
 *
 * Returns: the new #AgsFxChorusChannel
 *
 * Since: 3.14.0
 */
AgsFxChorusChannel*
ags_fx_chorus_channel_new(AgsChannel *channel)
{
  AgsFxChorusChannel *fx_chorus_channel;

  fx_chorus_channel = (AgsFxChorusChannel *) g_object_new(AGS_TYPE_FX_CHORUS_CHANNEL,
							  "source", channel,
							  NULL);

  return(fx_chorus_channel);
}
