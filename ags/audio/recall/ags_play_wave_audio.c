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

#include <ags/audio/recall/ags_play_wave_audio.h>

#include <ags/libags.h>

void ags_play_wave_audio_class_init(AgsPlayWaveAudioClass *play_wave_audio);
void ags_play_wave_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_wave_audio_init(AgsPlayWaveAudio *play_wave_audio);
void ags_play_wave_audio_finalize(GObject *gobject);
void ags_play_wave_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_play_wave_audio
 * @short_description: play audio wave
 * @title: AgsPlayWaveAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_wave_audio.h
 *
 * The #AgsPlayWaveAudio class provides ports to the effect processor.
 */

static gpointer ags_play_wave_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_wave_parent_plugin_interface;

static const gchar *ags_play_wave_audio_plugin_name = "ags-play-wave";
static const gchar *ags_play_wave_audio_specifier[] = {
};
static const gchar *ags_play_wave_audio_control_port[] = {
};

GType
ags_play_wave_audio_get_type()
{
  static GType ags_type_play_wave_audio = 0;

  if(!ags_type_play_wave_audio){
    static const GTypeInfo ags_play_wave_audio_info = {
      sizeof (AgsPlayWaveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayWaveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_wave_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsPlayWaveAudio",
						      &ags_play_wave_audio_info,
						      0);

    g_type_add_interface_static(ags_type_play_wave_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_wave_audio);
}

void
ags_play_wave_audio_class_init(AgsPlayWaveAudioClass *play_wave_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_wave_audio_parent_class = g_type_class_peek_parent(play_wave_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_audio;

  gobject->finalize = ags_play_wave_audio_finalize;
}

void
ags_play_wave_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_wave_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_wave_audio_set_ports;
}

void
ags_play_wave_audio_init(AgsPlayWaveAudio *play_wave_audio)
{
  GList *port;

  AGS_RECALL(play_wave_audio)->name = "ags-play-wave";
  AGS_RECALL(play_wave_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_audio)->xml_type = "ags-play-wave-audio";

  port = NULL;

  /* set port */
  AGS_RECALL(play_wave_audio)->port = port;
}

void
ags_play_wave_audio_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_parent_class)->finalize(gobject);
}

void
ags_play_wave_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    port = port->next;
  }
}

/**
 * ags_play_wave_audio_new:
 *
 * Creates an #AgsPlayWaveAudio
 *
 * Returns: a new #AgsPlayWaveAudio
 *
 * Since: 1.5.0
 */
AgsPlayWaveAudio*
ags_play_wave_audio_new()
{
  AgsPlayWaveAudio *play_wave_audio;

  play_wave_audio = (AgsPlayWaveAudio *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO,
						      NULL);

  return(play_wave_audio);
}
