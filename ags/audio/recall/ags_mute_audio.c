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

#include <ags/audio/recall/ags_mute_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio);
void ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable);
void ags_mute_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mute_audio_init(AgsMuteAudio *mute_audio);
void ags_mute_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_mute_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_mute_audio_dispose(GObject *gobject);
void ags_mute_audio_finalize(GObject *gobject);

void ags_mute_audio_set_ports(AgsPlugin *plugin, GList *port);

void ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPluginPort* ags_mute_audio_get_muted_plugin_port();

/**
 * SECTION:ags_mute_audio
 * @short_description: mute audio 
 * @title: AgsMuteAudio
 * @section_id:
 * @include: ags/audio/recall/ags_mute_audio.h
 *
 * The #AgsMuteAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_mute_audio_parent_class = NULL;
static AgsPluginInterface *ags_mute_audio_parent_plugin_interface;

static const gchar *ags_mute_audio_plugin_name = "ags-mute";
static const gchar *ags_mute_audio_specifier[] = {
  "./muted[0]",
};
static const gchar *ags_mute_audio_control_port[] = {
  "1/1",
};

GType
ags_mute_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mute_audio;

    static const GTypeInfo ags_mute_audio_info = {
      sizeof (AgsMuteAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsMuteAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_audio_init,
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mute_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						 "AgsMuteAudio",
						 &ags_mute_audio_info,
						 0);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_mute_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mute_audio_parent_class = g_type_class_peek_parent(mute_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_audio;

  gobject->set_property = ags_mute_audio_set_property;
  gobject->get_property = ags_mute_audio_get_property;

  gobject->dispose = ags_mute_audio_dispose;
  gobject->finalize = ags_mute_audio_finalize;

  /* properties */
  /**
   * AgsMuteAudio:muted:
   *
   * The mute port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("muted",
				   i18n_pspec("mute audio"),
				   i18n_pspec("Mute the audio"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable)
{
  mutable->set_muted = ags_mute_audio_set_muted;
}

void
ags_mute_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_mute_audio_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_mute_audio_set_ports;
}

void
ags_mute_audio_init(AgsMuteAudio *mute_audio)
{
  GList *port;

  AGS_RECALL(mute_audio)->name = "ags-mute";
  AGS_RECALL(mute_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_audio)->xml_type = "ags-mute-audio";

  port = NULL;

  /* muted */
  mute_audio->muted = g_object_new(AGS_TYPE_PORT,
				   "plugin-name", ags_mute_audio_plugin_name,
				   "specifier", ags_mute_audio_specifier[0],
				   "control-port", ags_mute_audio_control_port[0],
				   "port-value-is-pointer", FALSE,
				   "port-value-type", G_TYPE_FLOAT,
				   "port-value-size", sizeof(gfloat),
				   "port-value-length", 1,
				   NULL);
  g_object_ref(mute_audio->muted);

  mute_audio->muted->port_value.ags_port_float = (float) FALSE;

  /* plugin port */
  g_object_set(mute_audio->muted,
	       "plugin-port", ags_mute_audio_get_muted_plugin_port(),
	       NULL);

  /* add to port */
  port = g_list_prepend(port, mute_audio->muted);
  g_object_ref(mute_audio->muted);

  /* set port */
  AGS_RECALL(mute_audio)->port = port;
}

void
ags_mute_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  pthread_mutex_t *recall_mutex;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == mute_audio->muted){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(mute_audio->muted != NULL){
	g_object_unref(G_OBJECT(mute_audio->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      mute_audio->muted = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_mute_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  pthread_mutex_t *recall_mutex;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_MUTED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, mute_audio->muted);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_mute_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"muted[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_mute_audio_dispose(GObject *gobject)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* muted */
  if(mute_audio->muted != NULL){
    g_object_unref(G_OBJECT(mute_audio->muted));

    mute_audio->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_parent_class)->dispose(gobject);
}

void
ags_mute_audio_finalize(GObject *gobject)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* muted */
  if(mute_audio->muted != NULL){
    g_object_unref(G_OBJECT(mute_audio->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_parent_class)->finalize(gobject);
}

void
ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted)
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
}

static AgsPluginPort*
ags_mute_audio_get_muted_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);
  
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

  pthread_mutex_unlock(&mutex);
  
  return(plugin_port);
}

/**
 * ags_mute_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsMuteAudio
 *
 * Returns: the new #AgsMuteAudio
 *
 * Since: 2.0.0
 */
AgsMuteAudio*
ags_mute_audio_new(AgsAudio *audio)
{
  AgsMuteAudio *mute_audio;

  mute_audio = (AgsMuteAudio *) g_object_new(AGS_TYPE_MUTE_AUDIO,
					     "audio", audio,
					     NULL);

  return(mute_audio);
}
