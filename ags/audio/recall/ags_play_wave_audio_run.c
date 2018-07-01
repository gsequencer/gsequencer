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

#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/i18n.h>

void ags_play_wave_audio_run_class_init(AgsPlayWaveAudioRunClass *play_wave_audio_run);
void ags_play_wave_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_wave_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_wave_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_wave_audio_run_init(AgsPlayWaveAudioRun *play_wave_audio_run);
void ags_play_wave_audio_run_dispose(GObject *gobject);
void ags_play_wave_audio_run_finalize(GObject *gobject);
void ags_play_wave_audio_run_connect(AgsConnectable *connectable);
void ags_play_wave_audio_run_disconnect(AgsConnectable *connectable);
void ags_play_wave_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_wave_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

AgsRecall* ags_play_wave_audio_run_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_play_wave_audio_run
 * @short_description: play wave
 * @title: AgsPlayWaveAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_wave_audio_run.h
 *
 * The #AgsPlayWaveAudioRun class play wave.
 */

static gpointer ags_play_wave_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_wave_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_wave_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_play_wave_audio_run_parent_plugin_interface;

GType
ags_play_wave_audio_run_get_type()
{
  static GType ags_type_play_wave_audio_run = 0;

  if(!ags_type_play_wave_audio_run){
    static const GTypeInfo ags_play_wave_audio_run_info = {
      sizeof (AgsPlayWaveAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayWaveAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_wave_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							  "AgsPlayWaveAudioRun",
							  &ags_play_wave_audio_run_info,
							  0);

    g_type_add_interface_static(ags_type_play_wave_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_wave_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_wave_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_play_wave_audio_run);
}

void
ags_play_wave_audio_run_class_init(AgsPlayWaveAudioRunClass *play_wave_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_wave_audio_run_parent_class = g_type_class_peek_parent(play_wave_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_audio_run;

  gobject->dispose = ags_play_wave_audio_run_dispose;
  gobject->finalize = ags_play_wave_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_wave_audio_run;

  recall->resolve_dependencies = ags_play_wave_audio_run_resolve_dependencies;
  recall->duplicate = ags_play_wave_audio_run_duplicate;
}

void
ags_play_wave_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_wave_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_wave_audio_run_connect;
  connectable->disconnect = ags_play_wave_audio_run_disconnect;
}

void
ags_play_wave_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_wave_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);
}

void
ags_play_wave_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_wave_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_wave_audio_run_read;
  plugin->write = ags_play_wave_audio_run_write;
}

void
ags_play_wave_audio_run_init(AgsPlayWaveAudioRun *play_wave_audio_run)
{
  AGS_RECALL(play_wave_audio_run)->name = "ags-play-wave";
  AGS_RECALL(play_wave_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_audio_run)->xml_type = "ags-play-wave-audio-run";
  AGS_RECALL(play_wave_audio_run)->port = NULL;
}

void
ags_play_wave_audio_run_dispose(GObject *gobject)
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_wave_audio_run_finalize(GObject *gobject)
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_wave_audio_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_play_wave_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_wave_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_wave_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_wave_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  /* read parent */
  ags_play_wave_audio_run_parent_plugin_interface->read(file, node, plugin);
}

xmlNode*
ags_play_wave_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  xmlNode *node;

  /* write parent */
  node = ags_play_wave_audio_run_parent_plugin_interface->write(file, parent, plugin);

  return(node);
}

AgsRecall*
ags_play_wave_audio_run_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsPlayWaveAudioRun *copy;

  copy = AGS_PLAY_WAVE_AUDIO_RUN(AGS_RECALL_CLASS(ags_play_wave_audio_run_parent_class)->duplicate(recall,
												   recall_id,
												   n_params, parameter));

  return((AgsRecall *) copy);
}

/**
 * ags_play_wave_audio_run_new:
 *
 * Creates an #AgsPlayWaveAudioRun
 *
 * Returns: a new #AgsPlayWaveAudioRun
 *
 * Since: 1.5.0
 */
AgsPlayWaveAudioRun*
ags_play_wave_audio_run_new()
{
  AgsPlayWaveAudioRun *play_wave_audio_run;

  play_wave_audio_run = (AgsPlayWaveAudioRun *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO_RUN,
							     NULL);

  return(play_wave_audio_run);
}
