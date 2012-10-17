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

#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/recall/ags_stream_recycling.h>

#include <ags/audio/task/ags_unref_audio_signal.h>

void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_connect(AgsConnectable *connectable);
void ags_stream_audio_signal_disconnect(AgsConnectable *connectable);
void ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_init_pre(AgsRecall *recall);
void ags_stream_audio_signal_run_post(AgsRecall *recall);
AgsRecall* ags_stream_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

static gpointer ags_stream_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_stream_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_audio_signal_parent_run_connectable_interface;

GType
ags_stream_audio_signal_get_type()
{
  static GType ags_type_stream_audio_signal = 0;

  if(!ags_type_stream_audio_signal){
    static const GTypeInfo ags_stream_audio_signal_info = {
      sizeof (AgsStreamAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsStreamAudioSignal\0",
							  &ags_stream_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_stream_audio_signal);
}

void
ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_stream_audio_signal_parent_class = g_type_class_peek_parent(stream_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_audio_signal;

  gobject->finalize = ags_stream_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_audio_signal;

  recall->run_init_pre = ags_stream_audio_signal_run_init_pre;
  recall->run_post = ags_stream_audio_signal_run_post;
  recall->duplicate = ags_stream_audio_signal_duplicate;
}

void
ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_audio_signal_connect;
  connectable->disconnect = ags_stream_audio_signal_disconnect;
}

void
ags_stream_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_audio_signal_run_connect;
  run_connectable->disconnect = ags_stream_audio_signal_run_disconnect;
}

void
ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal)
{
  AGS_RECALL(stream_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  AgsDevout *devout;
  AgsStreamAudioSignal *stream_audio_signal;
  AgsRecallAudioSignal *recall_audio_signal;
  AgsUnrefAudioSignal *unref_audio_signal;
  AgsAudioSignal *audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(gobject);
  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  audio_signal = recall_audio_signal->source;

  ags_recycling_remove_audio_signal(AGS_RECYCLING(AGS_RECALL_RECYCLING(AGS_RECALL(recall_audio_signal)->parent)->source),
  				    recall_audio_signal->source);

  /* unref audio signal */
  //  unref_audio_signal = ags_unref_audio_signal_new(audio_signal);
  //  ags_devout_append_task(recall_audio_signal->devout, (AgsTask *) unref_audio_signal);

  /* call parent */
  G_OBJECT_CLASS(ags_stream_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_stream_audio_signal_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_run_connectable_interface->connect(run_connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_init_pre(AgsRecall *recall)
{
  recall->flags &= (~AGS_RECALL_PERSISTENT);

  /* call parent */
  AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_init_pre(recall);
}

void
ags_stream_audio_signal_run_post(AgsRecall *recall)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);

  if(AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current != NULL){
    AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current = AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current->next;
  }else{
    ags_recall_done(recall);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_post(recall);
}

AgsRecall*
ags_stream_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsStreamAudioSignal *copy;

  copy = (AgsStreamAudioSignal *) AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  return((AgsRecall *) copy);
}

AgsStreamAudioSignal*
ags_stream_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = (AgsStreamAudioSignal *) g_object_new(AGS_TYPE_STREAM_AUDIO_SIGNAL,
							      "audio_signal\0", audio_signal,
							      NULL);

  return(stream_audio_signal);
}
