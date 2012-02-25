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

void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_stream_audio_signal_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_stream_audio_signal_connect(AgsConnectable *connectable);
void ags_stream_audio_signal_disconnect(AgsConnectable *connectable);
void ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_post(AgsRecall *recall);
AgsRecall* ags_stream_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);
void ags_stream_audio_signal_remove(AgsRecall *recall);

enum{
  PROP_0,
  PROP_AUDIO_SIGNAL,
};

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

    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
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

  gobject->set_property = ags_stream_audio_signal_set_property;
  gobject->get_property = ags_stream_audio_signal_get_property;

  gobject->finalize = ags_stream_audio_signal_finalize;

  /* properties */
  param_spec = g_param_spec_object("audio_signal\0",
				   "stream AgsAudioSignal\0",
				   "The AgsAudioSignal to stream\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_audio_signal;

  recall->run_post = ags_stream_audio_signal_run_post;
  recall->duplicate = ags_stream_audio_signal_duplicate;
  recall->remove = ags_stream_audio_signal_remove;
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
  stream_audio_signal->audio_signal = NULL;
}

void
ags_stream_audio_signal_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_object(value);

      if(stream_audio_signal->audio_signal == audio_signal)
	return;

      if(stream_audio_signal->audio_signal != NULL)
	g_object_unref(G_OBJECT(stream_audio_signal->audio_signal));

      if(audio_signal != NULL)
	g_object_ref(G_OBJECT(audio_signal));

      stream_audio_signal->audio_signal = audio_signal;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stream_audio_signal_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_object(value, stream_audio_signal->audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(gobject);

  //  if(stream_audio_signal->audio_signal != NULL)
    //    g_object_unref(G_OBJECT(stream_audio_signal->audio_signal));

  G_OBJECT_CLASS(ags_stream_audio_signal_parent_class)->finalize(gobject);
}

void
ags_stream_audio_signal_connect(AgsConnectable *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface->connect(run_connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_post(AgsRecall *recall)
{
  AgsStreamAudioSignal *stream_audio_signal;

  AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_post(recall);

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);

  if(stream_audio_signal->audio_signal->stream_current != NULL){
    stream_audio_signal->audio_signal->stream_current = stream_audio_signal->audio_signal->stream_current->next;
  }else{
    ags_recall_done(recall);
  }
}

AgsRecall*
ags_stream_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsStreamAudioSignal *stream_audio_signal, *copy;

  stream_audio_signal = (AgsStreamAudioSignal *) recall;
  copy = (AgsStreamAudioSignal *) AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  g_object_set(G_OBJECT(copy),
	       "audio_signal\0", stream_audio_signal->audio_signal,
	       NULL);

  return((AgsRecall *) copy);
}

void
ags_stream_audio_signal_remove(AgsRecall *recall)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);

  ags_recycling_remove_audio_signal(AGS_STREAM_RECYCLING(recall->parent)->recycling,
				    stream_audio_signal->audio_signal);

  AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->remove(recall);
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
