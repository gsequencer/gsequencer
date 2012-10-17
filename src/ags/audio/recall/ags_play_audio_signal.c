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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_play_channel_run.h>

#include <stdlib.h>
#include <pthread.h>

void ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal);
void ags_play_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal);
void ags_play_audio_signal_connect(AgsConnectable *connectable);
void ags_play_audio_signal_disconnect(AgsConnectable *connectable);
void ags_play_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_play_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_play_audio_signal_finalize(GObject *gobject);

void ags_play_audio_signal_run_init_pre(AgsRecall *recall);
void ags_play_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_play_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

static gpointer ags_play_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_play_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_audio_signal_parent_run_connectable_interface;

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

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_signal_run_connectable_interface_init,
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
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
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
ags_play_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_audio_signal_run_connect;
  run_connectable->disconnect = ags_play_audio_signal_run_disconnect;
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
ags_play_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface->connect(run_connectable);

  /* empty */
}

void
ags_play_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);

  /* empty */
}

void
ags_play_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);
  //  AGS_RECALL(play_audio_signal)->flags &= (~AGS_RECALL_PERSISTENT);

  g_message("playing AgsAudioSignal on AgsDevout[%d]\0", AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->audio_channel);

  /* call parent */
  AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->run_init_pre(recall);
}

void
ags_play_audio_signal_run_inter(AgsRecall *recall)
{
  AgsPlayAudioSignal *play_audio_signal;
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *source;
  GList *stream;
  short *buffer0, *buffer1;
  AgsAttack *attack;
  guint audio_channel;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);

  devout = AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->devout;
  source = AGS_AUDIO_SIGNAL(AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->source);
  stream = source->stream_current;

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
  }

  //  attack = play_audio_signal->attack;
  audio_channel = AGS_RECALL_AUDIO_SIGNAL(play_audio_signal)->audio_channel;

  ags_audio_signal_copy_buffer_to_buffer(&(buffer0[audio_channel]), devout->pcm_channels,
					 (short *) stream->data, 1,
					 devout->buffer_size);

//  if(attack->first_start != 0){
//    ags_audio_signal_copy_buffer_to_buffer(&(buffer1[audio_channel]), devout->pcm_channels,
//					   &(((short *) stream->data)[attack->first_length]), 1,
//					   attack->first_start);
//  }
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
			  guint audio_channel,
			  AgsAttack *attack)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = (AgsPlayAudioSignal *) g_object_new(AGS_TYPE_PLAY_AUDIO_SIGNAL,
							  "source\0", source,
							  "devout\0", devout,
							  "audio_channel\0", audio_channel,
							  "attack\0", attack,
							  NULL);

  return(play_audio_signal);
}
