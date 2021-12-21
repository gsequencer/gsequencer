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

#include <ags/audio/fx/ags_fx_volume_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_volume_audio_class_init(AgsFxVolumeAudioClass *fx_volume_audio);
void ags_fx_volume_audio_mutable_interface_init(AgsMutableInterface *mutable);
void ags_fx_volume_audio_init(AgsFxVolumeAudio *fx_volume_audio);
void ags_fx_volume_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_volume_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_volume_audio_dispose(GObject *gobject);
void ags_fx_volume_audio_finalize(GObject *gobject);

void ags_fx_volume_audio_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPluginPort* ags_fx_volume_audio_get_muted_plugin_port();

/**
 * SECTION:ags_fx_volume_audio
 * @short_description: fx volume audio
 * @title: AgsFxVolumeAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_volume_audio.h
 *
 * The #AgsFxVolumeAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_volume_audio_parent_class = NULL;

const gchar *ags_fx_volume_audio_plugin_name = "ags-fx-volume";

const gchar* ags_fx_volume_audio_specifier[] = {
  "./muted[0]",
  NULL,
};

const gchar* ags_fx_volume_audio_control_port[] = {
  "1/1",
  NULL,
};

enum{
  PROP_0,
  PROP_MUTED,
};

GType
ags_fx_volume_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_volume_audio = 0;

    static const GTypeInfo ags_fx_volume_audio_info = {
      sizeof (AgsFxVolumeAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_volume_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxVolumeAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_volume_audio_init,
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_fx_volume_audio_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fx_volume_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsFxVolumeAudio",
						      &ags_fx_volume_audio_info,
						      0);

    g_type_add_interface_static(ags_type_fx_volume_audio,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_volume_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_volume_audio_class_init(AgsFxVolumeAudioClass *fx_volume_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_volume_audio_parent_class = g_type_class_peek_parent(fx_volume_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_volume_audio;

  gobject->set_property = ags_fx_volume_audio_set_property;
  gobject->get_property = ags_fx_volume_audio_get_property;

  gobject->dispose = ags_fx_volume_audio_dispose;
  gobject->finalize = ags_fx_volume_audio_finalize;

  /* properties */
  /**
   * AgsFxVolumeAudio:muted:
   *
   * The muted port.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("muted",
				   i18n_pspec("muted of recall"),
				   i18n_pspec("The recall's muted"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_fx_volume_audio_mutable_interface_init(AgsMutableInterface *mutable)
{
  mutable->set_muted = ags_fx_volume_audio_set_muted;
}

void
ags_fx_volume_audio_init(AgsFxVolumeAudio *fx_volume_audio)
{
  AGS_RECALL(fx_volume_audio)->name = "ags-fx-volume";
  AGS_RECALL(fx_volume_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_volume_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_volume_audio)->xml_type = "ags-fx-volume-audio";

  /* muted */
  fx_volume_audio->muted = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_volume_audio_plugin_name,
					"specifier", ags_fx_volume_audio_specifier[0],
					"control-port", ags_fx_volume_audio_control_port[0],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  
  fx_volume_audio->muted->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_volume_audio->muted,
	       "plugin-port", ags_fx_volume_audio_get_muted_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_volume_audio,
		      fx_volume_audio->muted);
}

void
ags_fx_volume_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxVolumeAudio *fx_volume_audio;

  GRecMutex *recall_mutex;

  fx_volume_audio = AGS_FX_VOLUME_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_volume_audio);

  switch(prop_id){
  case PROP_MUTED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_volume_audio->muted){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_volume_audio->muted != NULL){
      g_object_unref(G_OBJECT(fx_volume_audio->muted));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_volume_audio->muted = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_volume_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxVolumeAudio *fx_volume_audio;

  GRecMutex *recall_mutex;

  fx_volume_audio = AGS_FX_VOLUME_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_volume_audio);

  switch(prop_id){
  case PROP_MUTED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_volume_audio->muted);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_volume_audio_dispose(GObject *gobject)
{
  AgsFxVolumeAudio *fx_volume_audio;
  
  fx_volume_audio = AGS_FX_VOLUME_AUDIO(gobject);

  /* muted */
  if(fx_volume_audio->muted != NULL){
    g_object_unref(G_OBJECT(fx_volume_audio->muted));

    fx_volume_audio->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_audio_parent_class)->dispose(gobject);
}

void
ags_fx_volume_audio_finalize(GObject *gobject)
{
  AgsFxVolumeAudio *fx_volume_audio;
  
  fx_volume_audio = AGS_FX_VOLUME_AUDIO(gobject);

  /* muted */
  if(fx_volume_audio->muted != NULL){
    g_object_unref(G_OBJECT(fx_volume_audio->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_audio_parent_class)->finalize(gobject);
}

void
ags_fx_volume_audio_set_muted(AgsMutable *mutable, gboolean muted)
{
  AgsPort *port;
  
  GValue value = {0,};

  g_object_get(G_OBJECT(mutable),
	       "muted", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_FLOAT);

  g_value_set_float(&value,
		    (muted ? 1.0: 0.0));

  ags_port_safe_write(port,
		      &value);

  g_value_unset(&value);
  g_object_unref(port);
}

static AgsPluginPort*
ags_fx_volume_audio_get_muted_plugin_port()
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
 * ags_fx_volume_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxVolumeAudio
 *
 * Returns: the new #AgsFxVolumeAudio
 *
 * Since: 3.3.0
 */
AgsFxVolumeAudio*
ags_fx_volume_audio_new(AgsAudio *audio)
{
  AgsFxVolumeAudio *fx_volume_audio;

  fx_volume_audio = (AgsFxVolumeAudio *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO,
						      "audio", audio,
						      NULL);

  return(fx_volume_audio);
}
