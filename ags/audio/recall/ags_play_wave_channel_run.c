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

#include <ags/audio/recall/ags_play_wave_channel_run.h>
#include <ags/audio/recall/ags_play_wave_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/i18n.h>

void ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run);
void ags_play_wave_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_wave_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_wave_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run);
void ags_play_wave_channel_run_dispose(GObject *gobject);
void ags_play_wave_channel_run_finalize(GObject *gobject);
void ags_play_wave_channel_run_connect(AgsConnectable *connectable);
void ags_play_wave_channel_run_disconnect(AgsConnectable *connectable);
void ags_play_wave_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_wave_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_wave_channel_run_run_inter(AgsRecall *recall);
AgsRecall* ags_play_wave_channel_run_duplicate(AgsRecall *recall,
					       AgsRecallID *recall_id,
					       guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_play_wave_channel_run
 * @short_description: play wave
 * @title: AgsPlayWaveChannelRun
 * @section_id:
 * @include: ags/channel/recall/ags_play_wave_channel_run.h
 *
 * The #AgsPlayWaveChannelRun class play wave.
 */

static gpointer ags_play_wave_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_wave_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_wave_channel_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_play_wave_channel_run_parent_plugin_interface;

GType
ags_play_wave_channel_run_get_type()
{
  static GType ags_type_play_wave_channel_run = 0;

  if(!ags_type_play_wave_channel_run){
    static const GTypeInfo ags_play_wave_channel_run_info = {
      sizeof (AgsPlayWaveChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayWaveChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_channel_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_wave_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							    "AgsPlayWaveChannelRun",
							    &ags_play_wave_channel_run_info,
							    0);

    g_type_add_interface_static(ags_type_play_wave_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_wave_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_wave_channel_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_play_wave_channel_run);
}

void
ags_play_wave_channel_run_class_init(AgsPlayWaveChannelRunClass *play_wave_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_wave_channel_run_parent_class = g_type_class_peek_parent(play_wave_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_channel_run;

  gobject->dispose = ags_play_wave_channel_run_dispose;
  gobject->finalize = ags_play_wave_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_wave_channel_run;

  recall->run_inter = ags_play_wave_channel_run_run_inter;
  recall->duplicate = ags_play_wave_channel_run_duplicate;
}

void
ags_play_wave_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_wave_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_wave_channel_run_connect;
  connectable->disconnect = ags_play_wave_channel_run_disconnect;
}

void
ags_play_wave_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_wave_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);
}

void
ags_play_wave_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_wave_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_wave_channel_run_read;
  plugin->write = ags_play_wave_channel_run_write;
}

void
ags_play_wave_channel_run_init(AgsPlayWaveChannelRun *play_wave_channel_run)
{
  AGS_RECALL(play_wave_channel_run)->name = "ags-play-wave";
  AGS_RECALL(play_wave_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_channel_run)->xml_type = "ags-play-wave-channel-run";
  AGS_RECALL(play_wave_channel_run)->port = NULL;

  play_wave_channel_run->timestamp = ags_timestamp_new();

  play_wave_channel_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_wave_channel_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_wave_channel_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_wave_channel_run_finalize(GObject *gobject)
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  play_wave_channel_run = AGS_PLAY_WAVE_CHANNEL_RUN(gobject);

  /* timestamp */
  if(play_wave_channel->timestamp != NULL){
    g_object_unref(G_OBJECT(play_wave_channel->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_channel_run_parent_class)->finalize(gobject);
}

void
ags_play_wave_channel_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_play_wave_channel_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_wave_channel_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_wave_channel_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_wave_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  /* read parent */
  ags_play_wave_channel_run_parent_plugin_interface->read(file, node, plugin);
}

xmlNode*
ags_play_wave_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  xmlNode *node;

  /* write parent */
  node = ags_play_wave_channel_run_parent_plugin_interface->write(file, parent, plugin);

  return(node);
}

void
ags_play_wave_channel_run_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsWave *wave;
  AgsPlayWaveAudio *play_wave_audio;
  AgsPlayWaveAudioRun *play_wave_audio_run;
  AgsPlayWaveChannel *play_wave_channel;

  AgsMutexManager *mutex_manager;

  GList *list;

  guint line;
  guint samplerate;
  guint buffer_size;
  guint64 x_offset;
  gboolean do_playback;
  
  GValue do_playback_value = {0,};
  GValue x_offset_value = {0,};

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get AgsPlayWaveAudio */
  play_wave_audio = AGS_PLAY_WAVE_AUDIO(AGS_RECALL_CHANNEL_RUN(play_wave_channel_run)->recall_audio_run->recall_audio);

  /* get AgsPlayWaveAudioRun */
  play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(AGS_RECALL_CHANNEL_RUN(play_wave_channel_run)->recall_audio_run);

  /* get AgsPlayWaveChannel */
  play_wave_channel = AGS_PLAY_WAVE_CHANNEL(play_wave_channel_run->recall_channel_run.recall_channel);

  g_value_init(&do_playback_value, G_TYPE_BOOLEAN);
  ags_port_safe_read(play_wave_channel->do_playback, &do_playback_value);

  do_playback = g_value_get_boolean(&do_playback_value);
  g_value_unset(&do_playback_value);
  
  if(!do_playback){
    return;
  }
  
  g_value_init(&x_offset_value, G_TYPE_UINT64);
  ags_port_safe_read(play_wave_channel->x_offset, &x_offset_value);

  x_offset = g_value_get_boolean(&x_offset_value);
  g_value_unset(&x_offset_value);

  /* get audio */
  audio = AGS_RECALL_AUDIO(play_wave_audio)->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
    
  pthread_mutex_unlock(application_mutex);

  /* get channel */
  channel = AGS_RECALL_CHANNEL(play_wave_channel)->source;

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);
  
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
    
  pthread_mutex_unlock(application_mutex);
  
  /* get some presets */
  pthread_mutex_lock(audio_mutex);

  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;
  
  pthread_mutex_unlock(audio_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  line = channel->line;
  
  pthread_mutex_unlock(channel_mutex);

  /* time stamp offset */
  play_wave_channel_run->timestamp->timer.ags_offset.offset = (guint64) ((64.0 * (double) samplerate) * floor(x_offset / (64.0 * (double) samplerate)));

  /* find wave */
  pthread_mutex_lock(audio_mutex);

  list = ags_wave_find_near_timestamp(audio->wave, line,
				      play_wave_channel_run->timestamp);

  if(list != NULL){
    wave = list->data;
  }

  pthread_mutex_unlock(audio_mutex);

  //TODO:JK: implement me
}

AgsRecall*
ags_play_wave_channel_run_duplicate(AgsRecall *recall,
				    AgsRecallID *recall_id,
				    guint *n_params, GParameter *parameter)
{
  AgsPlayWaveChannelRun *copy_play_wave_channel_run;

  copy_play_wave_channel_run = AGS_RECALL_CLASS(ags_play_wave_channel_run_parent_class)->duplicate(recall,
												   recall_id,
												   n_params, parameter));

  return((AgsRecall *) copy_play_wave_channel_run);
}

/**
 * ags_play_wave_channel_run_new:
 *
 * Creates an #AgsPlayWaveChannelRun
 *
 * Returns: a new #AgsPlayWaveChannelRun
 *
 * Since: 1.5.0
 */
AgsPlayWaveChannelRun*
ags_play_wave_channel_run_new()
{
  AgsPlayWaveChannelRun *play_wave_channel_run;

  play_wave_channel_run = (AgsPlayWaveChannelRun *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL_RUN,
								 NULL);

  return(play_wave_channel_run);
}
