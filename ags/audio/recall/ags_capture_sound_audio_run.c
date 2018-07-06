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

#include <ags/audio/recall/ags_capture_sound_audio_run.h>
#include <ags/audio/recall/ags_capture_sound_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/i18n.h>

void ags_capture_sound_audio_run_class_init(AgsCaptureSoundAudioRunClass *capture_sound_audio_run);
void ags_capture_sound_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_capture_sound_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_capture_sound_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_capture_sound_audio_run_init(AgsCaptureSoundAudioRun *capture_sound_audio_run);
void ags_capture_sound_audio_run_dispose(GObject *gobject);
void ags_capture_sound_audio_run_finalize(GObject *gobject);
void ags_capture_sound_audio_run_connect(AgsConnectable *connectable);
void ags_capture_sound_audio_run_disconnect(AgsConnectable *connectable);
void ags_capture_sound_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_capture_sound_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_capture_sound_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_capture_sound_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

AgsRecall* ags_capture_sound_audio_run_duplicate(AgsRecall *recall,
						 AgsRecallID *recall_id,
						 guint *n_params, GParameter *parameter);
void ags_capture_sound_audio_run_run_init_pre(AgsRecall *recall);
void ags_capture_sound_audio_run_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_capture_sound_audio_run
 * @short_description: capture sound
 * @title: AgsCaptureSoundAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_capture_sound_audio_run.h
 *
 * The #AgsCaptureSoundAudioRun does capture sound.
 */

static gpointer ags_capture_sound_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_capture_sound_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_capture_sound_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_capture_sound_audio_run_parent_plugin_interface;

GType
ags_capture_sound_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_capture_sound_audio_run;

    static const GTypeInfo ags_capture_sound_audio_run_info = {
      sizeof (AgsCaptureSoundAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_sound_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCaptureSoundAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_sound_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_capture_sound_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							      "AgsCaptureSoundAudioRun",
							      &ags_capture_sound_audio_run_info,
							      0);

    g_type_add_interface_static(ags_type_capture_sound_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_capture_sound_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_capture_sound_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_capture_sound_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_capture_sound_audio_run_class_init(AgsCaptureSoundAudioRunClass *capture_sound_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_capture_sound_audio_run_parent_class = g_type_class_peek_parent(capture_sound_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_sound_audio_run;

  gobject->dispose = ags_capture_sound_audio_run_dispose;
  gobject->finalize = ags_capture_sound_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) capture_sound_audio_run;

  recall->duplicate = ags_capture_sound_audio_run_duplicate;
  recall->run_init_pre = ags_capture_sound_audio_run_run_init_pre;
  recall->run_pre = ags_capture_sound_audio_run_run_pre;
}

void
ags_capture_sound_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_capture_sound_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_capture_sound_audio_run_connect;
  connectable->disconnect = ags_capture_sound_audio_run_disconnect;
}

void
ags_capture_sound_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_capture_sound_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_capture_sound_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_capture_sound_audio_run_disconnect_dynamic;
}

void
ags_capture_sound_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_capture_sound_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_capture_sound_audio_run_read;
  plugin->write = ags_capture_sound_audio_run_write;
}

void
ags_capture_sound_audio_run_init(AgsCaptureSoundAudioRun *capture_sound_audio_run)
{
  AGS_RECALL(capture_sound_audio_run)->name = "ags-capture-sound";
  AGS_RECALL(capture_sound_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_sound_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_sound_audio_run)->xml_type = "ags-capture-sound-audio-run";
  AGS_RECALL(capture_sound_audio_run)->port = NULL;

  capture_sound_audio_run->audio_file = NULL;
}

void
ags_capture_sound_audio_run_dispose(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_run_parent_class)->dispose(gobject);
}

void
ags_capture_sound_audio_run_finalize(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_run_parent_class)->finalize(gobject);
}

void
ags_capture_sound_audio_run_connect(AgsConnectable *connectable)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_capture_sound_audio_run_parent_connectable_interface->connect(connectable);

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(connectable);
}

void
ags_capture_sound_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_capture_sound_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_capture_sound_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_capture_sound_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);  
}

void
ags_capture_sound_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  /* call parent */
  ags_capture_sound_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(dynamic_connectable);
}

void
ags_capture_sound_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  /* read parent */
  ags_capture_sound_audio_run_parent_plugin_interface->read(file, node, plugin);
}

xmlNode*
ags_capture_sound_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  xmlNode *node;
  
  /* write parent */
  node = ags_capture_sound_audio_run_parent_plugin_interface->write(file, parent, plugin);

  return(node);
}

AgsRecall*
ags_capture_sound_audio_run_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, GParameter *parameter)
{
  AgsCaptureSoundAudioRun *copy;

  copy = AGS_CAPTURE_SOUND_AUDIO_RUN(AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->duplicate(recall,
													   recall_id,
													   n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_capture_sound_audio_run_run_init_pre(AgsRecall *recall)
{
  //TODO:JK: implement me

  /* call parent */
  AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_init_pre(recall);
}

void
ags_capture_sound_audio_run_run_pre(AgsRecall *recall)
{
  //TODO:JK: implement me
  
  /* call parent */
  AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_pre(recall);
}

/**
 * ags_capture_sound_audio_run_new:
 *
 * Creates an #AgsCaptureSoundAudioRun
 *
 * Returns: a new #AgsCaptureSoundAudioRun
 *
 * Since: 1.4.0
 */
AgsCaptureSoundAudioRun*
ags_capture_sound_audio_run_new()
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = (AgsCaptureSoundAudioRun *) g_object_new(AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN,
								     NULL);

  return(capture_sound_audio_run);
}
