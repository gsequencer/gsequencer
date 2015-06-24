/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_play_notation_audio.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

void ags_play_notation_audio_class_init(AgsPlayNotationAudioClass *play_notation_audio);
void ags_play_notation_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_notation_audio_init(AgsPlayNotationAudio *play_notation_audio);
void ags_play_notation_audio_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_play_notation_audio_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_play_notation_audio_finalize(GObject *gobject);
void ags_play_notation_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_play_notation_audio
 * @short_description: play audio notation
 * @title: AgsPlayNotationAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_notation_audio.h
 *
 * The #AgsPlayNotationAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_MONOTONIC,
};

static gpointer ags_play_notation_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_notation_parent_plugin_interface;

static const gchar *ags_play_notation_audio_plugin_name = "ags-play-notation\0";
static const gchar *ags_play_notation_audio_specifier[] = {
  "./notation[0]\0"
};
static const gchar *ags_play_notation_audio_control_port[] = {
  "1/1\0",
};

GType
ags_play_notation_audio_get_type()
{
  static GType ags_type_play_notation_audio = 0;

  if(!ags_type_play_notation_audio){
    static const GTypeInfo ags_play_notation_audio_info = {
      sizeof (AgsPlayNotationAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_notation_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayNotationAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_notation_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_notation_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							  "AgsPlayNotationAudio\0",
							  &ags_play_notation_audio_info,
							  0);

    g_type_add_interface_static(ags_type_play_notation_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_notation_audio);
}

void
ags_play_notation_audio_class_init(AgsPlayNotationAudioClass *play_notation_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_notation_audio_parent_class = g_type_class_peek_parent(play_notation_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_notation_audio;

  gobject->set_property = ags_play_notation_audio_set_property;
  gobject->get_property = ags_play_notation_audio_get_property;

  gobject->finalize = ags_play_notation_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("notation\0",
				   "assigned notation\0",
				   "The notation this recall does play\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
}

void
ags_play_notation_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_notation_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_notation_audio_set_ports;
}

void
ags_play_notation_audio_init(AgsPlayNotationAudio *play_notation_audio)
{
  GList *port;

  AGS_RECALL(play_notation_audio)->name = "ags-play-notation\0";
  AGS_RECALL(play_notation_audio)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(play_notation_audio)->build_id = AGS_BUILD_ID;
  AGS_RECALL(play_notation_audio)->xml_type = "ags-play-notation-audio\0";

  port = NULL;

  play_notation_audio->notation = g_object_new(AGS_TYPE_PORT,
					       "plugin-name\0", ags_play_notation_audio_plugin_name,
					       "specifier\0", ags_play_notation_audio_specifier[0],
					       "control-port\0", ags_play_notation_audio_control_port[0],
					       "port-value-is-pointer\0", TRUE,
					       "port-value-type\0", G_TYPE_POINTER,
					       "port-value-size\0", sizeof(gpointer),
					       "port-value-length\0", 1,
					       NULL);
  play_notation_audio->notation->port_value.ags_port_pointer = NULL;

  port = g_list_prepend(port, play_notation_audio->notation);

  /* set port */
  AGS_RECALL(play_notation_audio)->port = port;
}

void
ags_play_notation_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = AGS_PLAY_NOTATION_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == play_notation_audio->notation){
	return;
      }

      if(play_notation_audio->notation != NULL){
	g_object_unref(G_OBJECT(play_notation_audio->notation));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_notation_audio->notation = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_play_notation_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = AGS_PLAY_NOTATION_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, play_notation_audio->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_notation_audio_finalize(GObject *gobject)
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = AGS_PLAY_NOTATION_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_notation_audio_parent_class)->finalize(gobject);
}

void
ags_play_notation_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./notation[0]\0",
		16)){
      g_object_set(G_OBJECT(plugin),
		   "notation\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

/**
 * ags_play_notation_audio_new:
 *
 * Creates an #AgsPlayNotationAudio
 *
 * Returns: a new #AgsPlayNotationAudio
 *
 * Since: 0.4
 */
AgsPlayNotationAudio*
ags_play_notation_audio_new()
{
  AgsPlayNotationAudio *play_notation_audio;

  play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
							      NULL);

  return(play_notation_audio);
}
