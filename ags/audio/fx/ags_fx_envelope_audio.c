/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_envelope_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_envelope_audio_class_init(AgsFxEnvelopeAudioClass *fx_envelope_audio);
void ags_fx_envelope_audio_init(AgsFxEnvelopeAudio *fx_envelope_audio);
void ags_fx_envelope_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_envelope_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_envelope_audio_dispose(GObject *gobject);
void ags_fx_envelope_audio_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_envelope_audio_get_lfo_enabled_plugin_port();

/**
 * SECTION:ags_fx_envelope_audio
 * @short_description: fx envelope audio
 * @title: AgsFxEnvelopeAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_envelope_audio.h
 *
 * The #AgsFxEnvelopeAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_envelope_audio_parent_class = NULL;

const gchar *ags_fx_envelope_audio_plugin_name = "ags-fx-envelope";

const gchar* ags_fx_envelope_audio_specifier[] = {
  "./lfo-enabled[0]",
  NULL,
};

const gchar* ags_fx_envelope_audio_control_port[] = {
  "1/1",
  NULL,
};

enum{
  PROP_0,
  PROP_LFO_ENABLED,
};

GType
ags_fx_envelope_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_envelope_audio = 0;

    static const GTypeInfo ags_fx_envelope_audio_info = {
      sizeof (AgsFxEnvelopeAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_envelope_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxEnvelopeAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_envelope_audio_init,
    };

    ags_type_fx_envelope_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsFxEnvelopeAudio",
							&ags_fx_envelope_audio_info,
							0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_envelope_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_envelope_audio_class_init(AgsFxEnvelopeAudioClass *fx_envelope_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_envelope_audio_parent_class = g_type_class_peek_parent(fx_envelope_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_envelope_audio;

  gobject->set_property = ags_fx_envelope_audio_set_property;
  gobject->get_property = ags_fx_envelope_audio_get_property;

  gobject->dispose = ags_fx_envelope_audio_dispose;
  gobject->finalize = ags_fx_envelope_audio_finalize;

  /* properties */
  /**
   * AgsFxEnvelopeAudio:lfo-enabled:
   *
   * The LFO enabled port.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("lfo-enabled",
				   i18n_pspec("LFO enabled of recall"),
				   i18n_pspec("The recall's LFO enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_ENABLED,
				  param_spec);
}

void
ags_fx_envelope_audio_init(AgsFxEnvelopeAudio *fx_envelope_audio)
{
  AGS_RECALL(fx_envelope_audio)->name = "ags-fx-envelope";
  AGS_RECALL(fx_envelope_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_envelope_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_envelope_audio)->xml_type = "ags-fx-envelope-audio";

  /* LFO enabled */
  fx_envelope_audio->lfo_enabled = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_envelope_audio_plugin_name,
						"specifier", "./lfo-enabled[0]",
						"control-port", "1/1",
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_envelope_audio->lfo_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_envelope_audio->lfo_enabled,
	       "plugin-port", ags_fx_envelope_audio_get_lfo_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_envelope_audio,
		      fx_envelope_audio->lfo_enabled);
}

void
ags_fx_envelope_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxEnvelopeAudio *fx_envelope_audio;

  GRecMutex *recall_mutex;

  fx_envelope_audio = AGS_FX_ENVELOPE_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_envelope_audio);

  switch(prop_id){
  case PROP_LFO_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_envelope_audio->lfo_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_envelope_audio->lfo_enabled != NULL){
	g_object_unref(G_OBJECT(fx_envelope_audio->lfo_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_envelope_audio->lfo_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_envelope_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxEnvelopeAudio *fx_envelope_audio;

  GRecMutex *recall_mutex;

  fx_envelope_audio = AGS_FX_ENVELOPE_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_envelope_audio);

  switch(prop_id){
  case PROP_LFO_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_envelope_audio->lfo_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_envelope_audio_dispose(GObject *gobject)
{
  AgsFxEnvelopeAudio *fx_envelope_audio;
  
  fx_envelope_audio = AGS_FX_ENVELOPE_AUDIO(gobject);

  /* lfo_enabled */
  if(fx_envelope_audio->lfo_enabled != NULL){
    g_object_unref(G_OBJECT(fx_envelope_audio->lfo_enabled));

    fx_envelope_audio->lfo_enabled = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_audio_parent_class)->dispose(gobject);
}

void
ags_fx_envelope_audio_finalize(GObject *gobject)
{
  AgsFxEnvelopeAudio *fx_envelope_audio;
  
  fx_envelope_audio = AGS_FX_ENVELOPE_AUDIO(gobject);

  /* lfo_enabled */
  if(fx_envelope_audio->lfo_enabled != NULL){
    g_object_unref(G_OBJECT(fx_envelope_audio->lfo_enabled));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_audio_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_envelope_audio_get_lfo_enabled_plugin_port()
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

/**
 * ags_fx_envelope_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxEnvelopeAudio
 *
 * Returns: the new #AgsFxEnvelopeAudio
 *
 * Since: 3.3.0
 */
AgsFxEnvelopeAudio*
ags_fx_envelope_audio_new(AgsAudio *audio)
{
  AgsFxEnvelopeAudio *fx_envelope_audio;

  fx_envelope_audio = (AgsFxEnvelopeAudio *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO,
							  "audio", audio,
							  NULL);

  return(fx_envelope_audio);
}
