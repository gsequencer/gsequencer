/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/recall/ags_buffer_audio_signal.h>
#include <ags/audio/recall/ags_buffer_recycling.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/task/ags_unref_audio_signal.h>

void ags_buffer_audio_signal_class_init(AgsBufferAudioSignalClass *buffer_audio_signal);
void ags_buffer_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_buffer_audio_signal_init(AgsBufferAudioSignal *buffer_audio_signal);
void ags_buffer_audio_signal_connect(AgsConnectable *connectable);
void ags_buffer_audio_signal_disconnect(AgsConnectable *connectable);
void ags_buffer_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_buffer_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_buffer_audio_signal_finalize(GObject *gobject);

void ags_buffer_audio_signal_run_init_pre(AgsRecall *recall);
void ags_buffer_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_buffer_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

static gpointer ags_buffer_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_buffer_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_buffer_audio_signal_parent_dynamic_connectable_interface;

GType
ags_buffer_audio_signal_get_type()
{
  static GType ags_type_buffer_audio_signal = 0;

  if(!ags_type_buffer_audio_signal){
    static const GTypeInfo ags_buffer_audio_signal_info = {
      sizeof (AgsBufferAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBufferAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_buffer_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_buffer_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsBufferAudioSignal\0",
							  &ags_buffer_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_buffer_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_buffer_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_buffer_audio_signal);
}

void
ags_buffer_audio_signal_class_init(AgsBufferAudioSignalClass *buffer_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_buffer_audio_signal_parent_class = g_type_class_peek_parent(buffer_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) buffer_audio_signal;

  gobject->finalize = ags_buffer_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) buffer_audio_signal;

  recall->run_init_pre = ags_buffer_audio_signal_run_init_pre;
  recall->run_inter = ags_buffer_audio_signal_run_inter;
  recall->duplicate = ags_buffer_audio_signal_duplicate;
}

void
ags_buffer_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_buffer_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_buffer_audio_signal_connect;
  connectable->disconnect = ags_buffer_audio_signal_disconnect;
}

void
ags_buffer_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_buffer_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_buffer_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_buffer_audio_signal_disconnect_dynamic;
}

void
ags_buffer_audio_signal_init(AgsBufferAudioSignal *buffer_audio_signal)
{
  AGS_RECALL(buffer_audio_signal)->name = "ags-buffer\0";
  AGS_RECALL(buffer_audio_signal)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(buffer_audio_signal)->build_id = AGS_BUILD_ID;
  AGS_RECALL(buffer_audio_signal)->xml_type = "ags-buffer-audio-signal\0";
  AGS_RECALL(buffer_audio_signal)->port = NULL;

  AGS_RECALL(buffer_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_buffer_audio_signal_finalize(GObject *gobject)
{
  AgsDevout *devout;
  AgsBufferAudioSignal *buffer_audio_signal;
  AgsRecallAudioSignal *recall_audio_signal;
  AgsUnrefAudioSignal *unref_audio_signal;
  AgsAudioSignal *audio_signal;

  buffer_audio_signal = AGS_BUFFER_AUDIO_SIGNAL(gobject);
  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  audio_signal = recall_audio_signal->source;

  if(AGS_RECALL(recall_audio_signal)->parent != NULL){
    ags_recycling_remove_audio_signal(AGS_RECYCLING(AGS_RECALL_RECYCLING(AGS_RECALL(recall_audio_signal)->parent)->source),
				      recall_audio_signal->source);
  }

  if(recall_audio_signal->source != NULL){
    g_object_unref(G_OBJECT(recall_audio_signal->source));
    /* unref audio signal */
    //    unref_audio_signal = ags_unref_audio_signal_new(audio_signal);
    //    ags_devout_append_task(AGS_DEVOUT(AGS_RECALL(recall_audio_signal)->devout), (AgsTask *) unref_audio_signal);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_buffer_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_buffer_audio_signal_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_buffer_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_buffer_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_buffer_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_buffer_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_buffer_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_buffer_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_buffer_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_buffer_audio_signal_run_init_pre(AgsRecall *recall)
{
  recall->flags &= (~AGS_RECALL_PERSISTENT);

  /* call parent */
  AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->run_init_pre(recall);
}

void
ags_buffer_audio_signal_run_inter(AgsRecall *recall)
{
  AgsBufferAudioSignal *buffer_audio_signal;

  buffer_audio_signal = AGS_BUFFER_AUDIO_SIGNAL(recall);

  //TODO:JK: implement me

  /* call parent */
  AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->run_inter(recall);
}

AgsRecall*
ags_buffer_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsBufferAudioSignal *copy;

  copy = (AgsBufferAudioSignal *) AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  return((AgsRecall *) copy);
}

AgsBufferAudioSignal*
ags_buffer_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsBufferAudioSignal *buffer_audio_signal;

  buffer_audio_signal = (AgsBufferAudioSignal *) g_object_new(AGS_TYPE_BUFFER_AUDIO_SIGNAL,
							      "source\0", audio_signal,
							      NULL);

  return(buffer_audio_signal);
}
