/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/recall/ags_play_dssi_audio.h>

#include <ags/object/ags_plugin.h>

void ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio);
void ags_play_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio);
void ags_play_dssi_audio_finalize(GObject *gobject);
void ags_play_dssi_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_play_dssi_audio
 * @short_description: play audio dssi
 * @title: AgsPlayDssiAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_dssi_audio.h
 *
 * The #AgsPlayDssiAudio class provides ports to the effect processor.
 */

static gpointer ags_play_dssi_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_dssi_parent_plugin_interface;

static const gchar *ags_play_dssi_audio_plugin_name = "ags-play-dssi\0";
static const gchar *ags_play_dssi_audio_specifier[] = {
};
static const gchar *ags_play_dssi_audio_control_port[] = {
};

GType
ags_play_dssi_audio_get_type()
{
  static GType ags_type_play_dssi_audio = 0;

  if(!ags_type_play_dssi_audio){
    static const GTypeInfo ags_play_dssi_audio_info = {
      sizeof (AgsPlayDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_dssi_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_dssi_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsPlayDssiAudio\0",
						      &ags_play_dssi_audio_info,
						      0);

    g_type_add_interface_static(ags_type_play_dssi_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_dssi_audio);
}

void
ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_dssi_audio_parent_class = g_type_class_peek_parent(play_dssi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_dssi_audio;

  gobject->finalize = ags_play_dssi_audio_finalize;
}

void
ags_play_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_dssi_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_dssi_audio_set_ports;
}

void
ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio)
{
  GList *port;

  AGS_RECALL(play_dssi_audio)->name = "ags-play-dssi\0";
  AGS_RECALL(play_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_dssi_audio)->xml_type = "ags-play-dssi-audio\0";

  port = NULL;

  /* set port */
  AGS_RECALL(play_dssi_audio)->port = port;
}

void
ags_play_dssi_audio_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_parent_class)->finalize(gobject);
}

void
ags_play_dssi_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    port = port->next;
  }
}

/**
 * ags_play_dssi_audio_new:
 *
 * Creates an #AgsPlayDssiAudio
 *
 * Returns: a new #AgsPlayDssiAudio
 *
 * Since: 0.7.134
 */
AgsPlayDssiAudio*
ags_play_dssi_audio_new()
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = (AgsPlayDssiAudio *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO,
						      NULL);

  return(play_dssi_audio);
}
