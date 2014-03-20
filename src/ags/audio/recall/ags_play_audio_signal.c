/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_play_channel.h>

#include <stdlib.h>

void ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal);
void ags_play_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal);
void ags_play_audio_signal_connect(AgsConnectable *connectable);
void ags_play_audio_signal_disconnect(AgsConnectable *connectable);
void ags_play_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_audio_signal_finalize(GObject *gobject);

void ags_play_audio_signal_run_init_pre(AgsRecall *recall);
void ags_play_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_play_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

static gpointer ags_play_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_play_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_audio_signal_parent_dynamic_connectable_interface;

GType
ags_play_audio_signal_get_type()
{
  static GType ags_type_play_audio_signal = 0;

  if(!ags_type_play_audio_signal){
    static const GTypeInfo ags_play_audio_signal_info = {
      sizeof (AgsPlayAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsPlayAudioSignal\0",
							&ags_play_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_play_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_play_audio_signal);
}

void
ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_audio_signal_parent_class = g_type_class_peek_parent(play_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) play_audio_signal;

  gobject->finalize = ags_play_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_audio_signal;

  recall->run_init_pre = ags_play_audio_signal_run_init_pre;
  recall->run_inter = ags_play_audio_signal_run_inter;
  recall->duplicate = ags_play_audio_signal_duplicate;
}

void
ags_play_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_audio_signal_connect;
  connectable->disconnect = ags_play_audio_signal_disconnect;
}

void
ags_play_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_audio_signal_disconnect_dynamic;
}

void
ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal)
{
  AGS_RECALL(play_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_play_audio_signal_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_signal_parent_class)->finalize(gobject);
}

void
ags_play_audio_signal_connect(AgsConnectable *connectable)
{
  ags_play_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_play_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_play_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_play_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_play_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_play_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_play_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsAudioSignal *audio_signal;
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);
  //  AGS_RECALL(play_audio_signal)->flags &= (~AGS_RECALL_PERSISTENT);

  //  audio_signal = AGS_AUDIO_SIGNAL(AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->source);

  //  g_message("=======================\n\nplaying AgsAudioSignal#%llx on AgsDevout[%d]\n\n=======================\0",
  //	    (long long unsigned int) audio_signal,
  //	    AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->audio_channel);
  
  /* call parent */
  AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->run_init_pre(recall);
}

void
ags_play_audio_signal_run_inter(AgsRecall *recall)
{
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *source;
  AgsPlayChannel *play_channel;
  AgsPlayAudioSignal *play_audio_signal;
  GList *stream;
  signed short *buffer0, *buffer1;
  guint audio_channel;
  gboolean muted;
  GValue muted_value = {0,};
  GValue audio_channel_value = {0,};

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);

  devout = AGS_DEVOUT(AGS_RECALL(play_audio_signal)->devout);
  source = AGS_AUDIO_SIGNAL(AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->source);
  stream = source->stream_current;

  if(devout == NULL){
    g_warning("no devout\0");
    return;
  }

  if(stream == NULL){
    ags_recall_done(recall);

    return;
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    buffer0 = devout->buffer[1];
    buffer1 = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    buffer0 = devout->buffer[2];
    buffer1 = devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    buffer0 = devout->buffer[3];
    buffer1 = devout->buffer[0];
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    buffer0 = devout->buffer[0];
    buffer1 = devout->buffer[1];
  }else{
    g_warning("no output buffer\0");
    return;
  }

  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  g_value_init(&muted_value, G_TYPE_BOOLEAN);
  ags_port_safe_read(play_channel->muted,
		     &muted_value);

  muted = g_value_get_boolean(&muted_value);

  if(muted){
    return;
  }

  g_value_init(&audio_channel_value, G_TYPE_UINT);
  ags_port_safe_read(play_channel->audio_channel,
		     &audio_channel_value);

  audio_channel = g_value_get_uint(&audio_channel_value);

  if((AGS_RECALL_INITIAL_RUN & (AGS_RECALL_AUDIO_SIGNAL(recall)->flags)) != 0){
    AGS_RECALL_AUDIO_SIGNAL(recall)->flags &= (~AGS_RECALL_INITIAL_RUN);
    ags_audio_signal_copy_buffer_to_buffer(&(buffer0[audio_channel + source->attack * devout->pcm_channels]),
					   devout->pcm_channels,
					   (signed short *) stream->data, 1,
					   AGS_DEVOUT_DEFAULT_BUFFER_SIZE - source->attack);
  }else{
    ags_audio_signal_copy_buffer_to_buffer(&(buffer0[audio_channel]), devout->pcm_channels,
					   (signed short *) stream->data, 1,
					   devout->buffer_size);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->run_inter(recall);
}

AgsRecall*
ags_play_audio_signal_duplicate(AgsRecall *recall,
				AgsRecallID *recall_id,
				guint *n_params, GParameter *parameter)
{
  AgsPlayAudioSignal *copy;

  copy = (AgsPlayAudioSignal *) AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->duplicate(recall, recall_id,
												n_params, parameter);

  return((AgsRecall *) copy);
}

AgsPlayAudioSignal*
ags_play_audio_signal_new(AgsAudioSignal *source,
			  AgsDevout *devout,
			  guint audio_channel)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = (AgsPlayAudioSignal *) g_object_new(AGS_TYPE_PLAY_AUDIO_SIGNAL,
							  "source\0", source,
							  "devout\0", devout,
							  "audio_channel\0", audio_channel,
							  NULL);

  return(play_audio_signal);
}
