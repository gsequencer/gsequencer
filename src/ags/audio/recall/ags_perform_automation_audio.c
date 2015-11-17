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

#include <ags/audio/recall/ags_perform_automation_audio.h>

#include <ags/object/ags_plugin.h>

void ags_perform_automation_audio_class_init(AgsPerformAutomationAudioClass *perform_automation_audio);
void ags_perform_automation_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_perform_automation_audio_init(AgsPerformAutomationAudio *perform_automation_audio);
void ags_perform_automation_audio_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_perform_automation_audio_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_perform_automation_audio_finalize(GObject *gobject);
void ags_perform_automation_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_perform_automation_audio
 * @short_description: play audio notation
 * @title: AgsPerformAutomationAudio
 * @section_id:
 * @include: ags/audio/recall/ags_perform_automation_audio.h
 *
 * The #AgsPerformAutomationAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
};

static gpointer ags_perform_automation_audio_parent_class = NULL;
static AgsPluginInterface *ags_perform_automation_parent_plugin_interface;

static const gchar *ags_perform_automation_audio_plugin_name = "ags-play-notation\0";
static const gchar *ags_perform_automation_audio_specifier[] = {
};
static const gchar *ags_perform_automation_audio_control_port[] = {
};

GType
ags_perform_automation_audio_get_type()
{
  static GType ags_type_perform_automation_audio = 0;

  if(!ags_type_perform_automation_audio){
    static const GTypeInfo ags_perform_automation_audio_info = {
      sizeof (AgsPerformAutomationAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_perform_automation_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPerformAutomationAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_perform_automation_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_perform_automation_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_perform_automation_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							       "AgsPerformAutomationAudio\0",
							       &ags_perform_automation_audio_info,
							       0);

    g_type_add_interface_static(ags_type_perform_automation_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_perform_automation_audio);
}

void
ags_perform_automation_audio_class_init(AgsPerformAutomationAudioClass *perform_automation_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_perform_automation_audio_parent_class = g_type_class_peek_parent(perform_automation_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) perform_automation_audio;

  gobject->set_property = ags_perform_automation_audio_set_property;
  gobject->get_property = ags_perform_automation_audio_get_property;

  gobject->finalize = ags_perform_automation_audio_finalize;
}

void
ags_perform_automation_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_perform_automation_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_perform_automation_audio_set_ports;
}

void
ags_perform_automation_audio_init(AgsPerformAutomationAudio *perform_automation_audio)
{
  GList *port;

  AGS_RECALL(perform_automation_audio)->name = "ags-play-notation\0";
  AGS_RECALL(perform_automation_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(perform_automation_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(perform_automation_audio)->xml_type = "ags-play-notation-audio\0";

  port = NULL;

  /* set port */
  AGS_RECALL(perform_automation_audio)->port = port;
}

void
ags_perform_automation_audio_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsPerformAutomationAudio *perform_automation_audio;

  perform_automation_audio = AGS_PERFORM_AUTOMATION_AUDIO(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_perform_automation_audio_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsPerformAutomationAudio *perform_automation_audio;

  perform_automation_audio = AGS_PERFORM_AUTOMATION_AUDIO(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_perform_automation_audio_finalize(GObject *gobject)
{
  AgsPerformAutomationAudio *perform_automation_audio;

  perform_automation_audio = AGS_PERFORM_AUTOMATION_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_perform_automation_audio_parent_class)->finalize(gobject);
}

void
ags_perform_automation_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    port = port->next;
  }
}

/**
 * ags_perform_automation_audio_new:
 *
 * Creates an #AgsPerformAutomationAudio
 *
 * Returns: a new #AgsPerformAutomationAudio
 *
 * Since: 0.7.2
 */
AgsPerformAutomationAudio*
ags_perform_automation_audio_new()
{
  AgsPerformAutomationAudio *perform_automation_audio;

  perform_automation_audio = (AgsPerformAutomationAudio *) g_object_new(AGS_TYPE_PERFORM_AUTOMATION_AUDIO,
									NULL);

  return(perform_automation_audio);
}
