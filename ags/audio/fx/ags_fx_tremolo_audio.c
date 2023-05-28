/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_tremolo_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_tremolo_audio_class_init(AgsFxTremoloAudioClass *fx_tremolo_audio);
void ags_fx_tremolo_audio_init(AgsFxTremoloAudio *fx_tremolo_audio);
void ags_fx_tremolo_audio_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_fx_tremolo_audio_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_fx_tremolo_audio_dispose(GObject *gobject);
void ags_fx_tremolo_audio_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_tremolo_audio_get_tremolo_enabled_plugin_port();
static AgsPluginPort* ags_fx_tremolo_audio_get_tremolo_gain_plugin_port();
static AgsPluginPort* ags_fx_tremolo_audio_get_tremolo_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_tremolo_audio_get_tremolo_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_tremolo_audio_get_tremolo_tuning_plugin_port();

/**
 * SECTION:ags_fx_tremolo_audio
 * @short_description: fx tremolo audio
 * @title: AgsFxTremoloAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_tremolo_audio.h
 *
 * The #AgsFxTremoloAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_tremolo_audio_parent_class = NULL;

const gchar *ags_fx_tremolo_audio_plugin_name = "ags-fx-tremolo";

const gchar* ags_fx_tremolo_audio_specifier[] = {
  "./tremolo-enabled[0]",
  "./tremolo-gain[0]",
  "./tremolo-lfo-depth[0]",
  "./tremolo-lfo-freq[0]",
  "./tremolo-tuning[0]",
  NULL,
};

const gchar* ags_fx_tremolo_audio_control_port[] = {
  "1/5",
  "2/5",
  "3/5",
  "4/5",
  "5/5",
  NULL,
};

enum{
  PROP_0,
  PROP_TREMOLO_ENABLED,
  PROP_TREMOLO_GAIN,
  PROP_TREMOLO_LFO_DEPTH,
  PROP_TREMOLO_LFO_FREQ,
  PROP_TREMOLO_TUNING,
};

GType
ags_fx_tremolo_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_tremolo_audio = 0;

    static const GTypeInfo ags_fx_tremolo_audio_info = {
      sizeof (AgsFxTremoloAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_tremolo_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxTremoloAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_tremolo_audio_init,
    };

    ags_type_fx_tremolo_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						       "AgsFxTremoloAudio",
						       &ags_fx_tremolo_audio_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_tremolo_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_tremolo_audio_class_init(AgsFxTremoloAudioClass *fx_tremolo_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_tremolo_audio_parent_class = g_type_class_peek_parent(fx_tremolo_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_tremolo_audio;

  gobject->set_property = ags_fx_tremolo_audio_set_property;
  gobject->get_property = ags_fx_tremolo_audio_get_property;

  gobject->dispose = ags_fx_tremolo_audio_dispose;
  gobject->finalize = ags_fx_tremolo_audio_finalize;

  /* properties */
  /**
   * AgsFxTremoloAudio:tremolo-enabled:
   *
   * The tremolo's enabled port.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("tremolo-enabled",
				   i18n_pspec("enabled of tremolo recall"),
				   i18n_pspec("The tremolo recall's enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TREMOLO_ENABLED,
				  param_spec);

  /**
   * AgsFxTremoloAudio:tremolo-gain:
   *
   * The tremolo's gain port.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("tremolo-gain",
				   i18n_pspec("gain of tremolo recall"),
				   i18n_pspec("The tremolo recall's gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TREMOLO_GAIN,
				  param_spec);

  /**
   * AgsFxTremoloAudio:tremolo-lfo-depth:
   *
   * The tremolo's lfo-depth port.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("tremolo-lfo-depth",
				   i18n_pspec("LFO depthuency of tremolo recall"),
				   i18n_pspec("The tremolo recall's LFO depthuency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TREMOLO_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxTremoloAudio:tremolo-lfo-freq:
   *
   * The tremolo's lfo-freq port.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("tremolo-lfo-freq",
				   i18n_pspec("LFO frequency of tremolo recall"),
				   i18n_pspec("The tremolo recall's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TREMOLO_LFO_FREQ,
				  param_spec);

  /**
   * AgsFxTremoloAudio:tremolo-tuning:
   *
   * The tremolo's tuning port.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("tremolo-tuning",
				   i18n_pspec("tuning of tremolo recall"),
				   i18n_pspec("The tremolo recall's tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TREMOLO_TUNING,
				  param_spec);
}

void
ags_fx_tremolo_audio_init(AgsFxTremoloAudio *fx_tremolo_audio)
{
  AGS_RECALL(fx_tremolo_audio)->name = "ags-fx-tremolo";
  AGS_RECALL(fx_tremolo_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_tremolo_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_tremolo_audio)->xml_type = "ags-fx-tremolo-audio";

  /* tremolo enabled */
  fx_tremolo_audio->tremolo_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_tremolo_audio_plugin_name,
						   "specifier", ags_fx_tremolo_audio_specifier[0],
						   "control-port", ags_fx_tremolo_audio_control_port[0],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_tremolo_audio->tremolo_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_tremolo_audio->tremolo_enabled,
	       "plugin-port", ags_fx_tremolo_audio_get_tremolo_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_tremolo_audio,
		      fx_tremolo_audio->tremolo_enabled);

  /* tremolo gain */
  fx_tremolo_audio->tremolo_gain = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_tremolo_audio_plugin_name,
						"specifier", ags_fx_tremolo_audio_specifier[1],
						"control-port", ags_fx_tremolo_audio_control_port[1],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_tremolo_audio->tremolo_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_tremolo_audio->tremolo_gain,
	       "plugin-port", ags_fx_tremolo_audio_get_tremolo_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_tremolo_audio,
		      fx_tremolo_audio->tremolo_gain);

  /* tremolo LFO depth */
  fx_tremolo_audio->tremolo_lfo_depth = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_tremolo_audio_plugin_name,
						     "specifier", ags_fx_tremolo_audio_specifier[2],
						     "control-port", ags_fx_tremolo_audio_control_port[2],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_tremolo_audio->tremolo_lfo_depth->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_tremolo_audio->tremolo_lfo_depth,
	       "plugin-port", ags_fx_tremolo_audio_get_tremolo_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_tremolo_audio,
		      fx_tremolo_audio->tremolo_lfo_depth);

  /* tremolo LFO freq */
  fx_tremolo_audio->tremolo_lfo_freq = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_tremolo_audio_plugin_name,
						    "specifier", ags_fx_tremolo_audio_specifier[3],
						    "control-port", ags_fx_tremolo_audio_control_port[3],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_tremolo_audio->tremolo_lfo_freq->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_tremolo_audio->tremolo_lfo_freq,
	       "plugin-port", ags_fx_tremolo_audio_get_tremolo_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_tremolo_audio,
		      fx_tremolo_audio->tremolo_lfo_freq);

  /* tremolo tuning */
  fx_tremolo_audio->tremolo_tuning = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_tremolo_audio_plugin_name,
						  "specifier", ags_fx_tremolo_audio_specifier[4],
						  "control-port", ags_fx_tremolo_audio_control_port[4],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_tremolo_audio->tremolo_tuning->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_tremolo_audio->tremolo_tuning,
	       "plugin-port", ags_fx_tremolo_audio_get_tremolo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_tremolo_audio,
		      fx_tremolo_audio->tremolo_tuning);
}

void
ags_fx_tremolo_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsFxTremoloAudio *fx_tremolo_audio;

  GRecMutex *recall_mutex;

  fx_tremolo_audio = AGS_FX_TREMOLO_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_tremolo_audio);

  switch(prop_id){
  case PROP_TREMOLO_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_tremolo_audio->tremolo_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_tremolo_audio->tremolo_gain != NULL){
	g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_tremolo_audio->tremolo_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_tremolo_audio->tremolo_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_tremolo_audio->tremolo_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_tremolo_audio->tremolo_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_LFO_FREQ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_tremolo_audio->tremolo_lfo_freq){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_tremolo_audio->tremolo_lfo_freq != NULL){
	g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_freq));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_tremolo_audio->tremolo_lfo_freq = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_tremolo_audio->tremolo_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_tremolo_audio->tremolo_tuning != NULL){
	g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_tremolo_audio->tremolo_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_tremolo_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsFxTremoloAudio *fx_tremolo_audio;

  GRecMutex *recall_mutex;

  fx_tremolo_audio = AGS_FX_TREMOLO_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_tremolo_audio);

  switch(prop_id){
  case PROP_TREMOLO_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_tremolo_audio->tremolo_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_tremolo_audio->tremolo_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_LFO_FREQ:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_tremolo_audio->tremolo_lfo_freq);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_TREMOLO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_tremolo_audio->tremolo_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_tremolo_audio_dispose(GObject *gobject)
{
  AgsFxTremoloAudio *fx_tremolo_audio;
  
  fx_tremolo_audio = AGS_FX_TREMOLO_AUDIO(gobject);

  /* tremolo gain */
  if(fx_tremolo_audio->tremolo_gain != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_gain));

    fx_tremolo_audio->tremolo_gain = NULL;
  }

  /* tremolo LFO depth */
  if(fx_tremolo_audio->tremolo_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_depth));

    fx_tremolo_audio->tremolo_lfo_depth = NULL;
  }

  /* tremolo LFO freq */
  if(fx_tremolo_audio->tremolo_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_freq));

    fx_tremolo_audio->tremolo_lfo_freq = NULL;
  }

  /* tremolo tuning */
  if(fx_tremolo_audio->tremolo_tuning != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_tuning));

    fx_tremolo_audio->tremolo_tuning = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_audio_parent_class)->dispose(gobject);
}

void
ags_fx_tremolo_audio_finalize(GObject *gobject)
{
  AgsFxTremoloAudio *fx_tremolo_audio;
  
  fx_tremolo_audio = AGS_FX_TREMOLO_AUDIO(gobject);

  /* tremolo gain */
  if(fx_tremolo_audio->tremolo_gain != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_gain));
  }

  /* tremolo LFO depth */
  if(fx_tremolo_audio->tremolo_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_depth));
  }

  /* tremolo LFO freq */
  if(fx_tremolo_audio->tremolo_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_lfo_freq));
  }

  /* tremolo tuning */
  if(fx_tremolo_audio->tremolo_tuning != NULL){
    g_object_unref(G_OBJECT(fx_tremolo_audio->tremolo_tuning));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_audio_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_tremolo_audio_get_tremolo_enabled_plugin_port()
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
ags_fx_tremolo_audio_get_tremolo_gain_plugin_port()
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
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_tremolo_audio_get_tremolo_lfo_depth_plugin_port()
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
ags_fx_tremolo_audio_get_tremolo_lfo_freq_plugin_port()
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
		      6.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_tremolo_audio_get_tremolo_tuning_plugin_port()
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
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_tremolo_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxTremoloAudio
 *
 * Returns: the new #AgsFxTremoloAudio
 *
 * Since: 5.2.0
 */
AgsFxTremoloAudio*
ags_fx_tremolo_audio_new(AgsAudio *audio)
{
  AgsFxTremoloAudio *fx_tremolo_audio;

  fx_tremolo_audio = (AgsFxTremoloAudio *) g_object_new(AGS_TYPE_FX_TREMOLO_AUDIO,
							"audio", audio,
							NULL);

  return(fx_tremolo_audio);
}
