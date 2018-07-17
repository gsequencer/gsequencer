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

#include <ags/audio/recall/ags_play_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_play_audio_class_init(AgsPlayAudioClass *play_audio);
void ags_play_audio_mutable_interface_init(AgsMutableInterface *mutable);
void ags_play_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_audio_init(AgsPlayAudio *play_audio);
void ags_play_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_play_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_play_audio_dispose(GObject *gobject);
void ags_play_audio_finalize(GObject *gobject);

void ags_play_audio_set_ports(AgsPlugin *plugin, GList *port);

void ags_play_audio_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPluginPort* ags_play_audio_get_muted_plugin_port();

/**
 * SECTION:ags_play_audio
 * @short_description: play audio 
 * @title: AgsPlayAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_audio.h
 *
 * The #AgsPlayAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_play_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_audio_parent_plugin_interface;

static const gchar *ags_play_audio_plugin_name = "ags-play";
static const gchar *ags_play_audio_specifier[] = {
  "./muted[0]",
};
static const gchar *ags_play_audio_control_port[] = {
  "1/1",
};

GType
ags_play_audio_get_type()
{
  static GType ags_type_play_audio = 0;

  if(!ags_type_play_audio){
    static const GTypeInfo ags_play_audio_info = {
      sizeof (AgsPlayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_init,
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						 "AgsPlayAudio",
						 &ags_play_audio_info,
						 0);

    g_type_add_interface_static(ags_type_play_audio,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_play_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_audio);
}

void
ags_play_audio_class_init(AgsPlayAudioClass *play_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_play_audio_parent_class = g_type_class_peek_parent(play_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_audio;

  gobject->set_property = ags_play_audio_set_property;
  gobject->get_property = ags_play_audio_get_property;

  gobject->dispose = ags_play_audio_dispose;
  gobject->finalize = ags_play_audio_finalize;

  /* properties */
  /**
   * AgsPlayAudio:muted:
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
ags_play_audio_init(AgsPlayAudio *play_audio)
{
  GList *port;

  AGS_RECALL(play_audio)->name = "ags-play";
  AGS_RECALL(play_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_audio)->xml_type = "ags-play-audio";

  port = NULL;

  /* muted */
  play_audio->muted = g_object_new(AGS_TYPE_PORT,
				   "plugin-name", ags_play_audio_plugin_name,
				   "specifier", ags_play_audio_specifier[0],
				   "control-port", ags_play_audio_control_port[0],
				   "port-value-is-pointer", FALSE,
				   "port-value-type", G_TYPE_FLOAT,
				   "port-value-size", sizeof(gfloat),
				   "port-value-length", 1,
				   NULL);
  g_object_ref(play_audio->muted);

  play_audio->muted->port_value.ags_port_float = (float) 0.0;

  /* plugin port */
  g_object_set(play_audio->muted,
	       "plugin-port", ags_play_audio_get_muted_plugin_port(),
	       NULL);

  /* add to port */
  port = g_list_prepend(port, play_audio->muted);
  g_object_ref(play_audio->muted);

  /* set port */
  AGS_RECALL(play_audio)->port = port;
}

void
ags_play_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayAudio *play_audio;

  pthread_mutex_t *recall_mutex;

  play_audio = AGS_PLAY_AUDIO(gobject);

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

      if(port == play_audio->muted){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(play_audio->muted != NULL){
	g_object_unref(G_OBJECT(play_audio->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_audio->muted = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_play_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayAudio *play_audio;

  pthread_mutex_t *recall_mutex;

  play_audio = AGS_PLAY_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_MUTED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_audio->muted);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_audio_set_ports(AgsPlugin *plugin, GList *port)
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
ags_play_audio_dispose(GObject *gobject)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  /* muted */
  if(play_audio->muted != NULL){
    g_object_unref(G_OBJECT(play_audio->muted));

    play_audio->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_parent_class)->dispose(gobject);
}

void
ags_play_audio_finalize(GObject *gobject)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  /* muted */
  if(play_audio->muted != NULL){
    g_object_unref(G_OBJECT(play_audio->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_parent_class)->finalize(gobject);
}

void
ags_play_audio_set_muted(AgsMutable *mutable, gboolean muted)
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
}

static AgsPluginPort*
ags_play_audio_get_muted_plugin_port()
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
 * ags_play_audio_new:
 * @audio: the #AgsAudio
 *
 * Creates an #AgsPlayAudio
 *
 * Returns: a new #AgsPlayAudio
 *
 * Since: 2.0.0
 */
AgsPlayAudio*
ags_play_audio_new(AgsAudio *audio)
{
  AgsPlayAudio *play_audio;

  play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
					     "audio", audio,
					     NULL);

  return(play_audio);
}
