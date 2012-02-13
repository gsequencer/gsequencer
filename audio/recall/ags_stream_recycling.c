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

#include <ags/audio/recall/ags_stream_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>

void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_stream_recycling_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_stream_recycling_connect(AgsConnectable *connectable);
void ags_stream_recycling_disconnect(AgsConnectable *connectable);
void ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_recycling_finalize(GObject *gobject);

AgsRecall* ags_stream_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

void ags_stream_recycling_add_audio_signal(AgsStreamRecycling *stream_recycling,
					   AgsRecycling *recycling,
					   AgsAudioSignal *audio_signal);

void ags_stream_recycling_add_audio_signal_callback(AgsRecycling *recycling,
						    AgsAudioSignal *audio_signal,
						    AgsStreamRecycling *stream_recycling);

enum{
  PROP_0,
  PROP_RECYCLING,
};

static gpointer ags_stream_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_stream_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_recycling_parent_run_connectable_interface;

GType
ags_stream_recycling_get_type()
{
  static GType ags_type_stream_recycling = 0;

  if(!ags_type_stream_recycling){
    static const GTypeInfo ags_stream_recycling_info = {
      sizeof (AgsStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsStreamRecycling\0",
						       &ags_stream_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_stream_recycling);
}

void
ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_stream_recycling_parent_class = g_type_class_peek_parent(stream_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_recycling;

  gobject->set_property = ags_stream_recycling_set_property;
  gobject->get_property = ags_stream_recycling_get_property;

  gobject->finalize = ags_stream_recycling_finalize;

  /* properties */
  param_spec = g_param_spec_object("recycling\0",
				   "assigned recycling\0",
				   "The recycling to stream\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_recycling;

  recall->duplicate = ags_stream_recycling_duplicate;
}

void
ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_recycling_connect;
  connectable->disconnect = ags_stream_recycling_disconnect;
}

void
ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_recycling_run_connect;
  run_connectable->disconnect = ags_stream_recycling_run_disconnect;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  stream_recycling->recycling = NULL;
}

void
ags_stream_recycling_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(stream_recycling->recycling == recycling)
	return;

      if(stream_recycling->recycling != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(stream_recycling)->flags)) != 0){
	  g_signal_handler_disconnect(gobject,
				      stream_recycling->add_audio_signal_handler);
	}

	g_object_unref(G_OBJECT(stream_recycling->recycling));
      }

      if(recycling != NULL){
	g_object_ref(G_OBJECT(recycling));

	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(stream_recycling)->flags)) != 0){
	  stream_recycling->add_audio_signal_handler =
	    g_signal_connect_after(gobject, "add_audio_signal\0",
				   G_CALLBACK(ags_stream_recycling_add_audio_signal_callback), stream_recycling);
	}
      }

      stream_recycling->recycling = recycling;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stream_recycling_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      g_value_set_object(value, stream_recycling->recycling);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(gobject);

  if(stream_recycling->recycling != NULL)
    g_object_unref(G_OBJECT(stream_recycling->recycling));

  /* call parent */
  G_OBJECT_CLASS(ags_stream_recycling_parent_class)->finalize(gobject);
}

void
ags_stream_recycling_connect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_recycling_disconnect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsStreamRecycling *stream_recycling;
  GObject *gobject;

  ags_stream_recycling_parent_run_connectable_interface->connect(run_connectable);

  stream_recycling = AGS_STREAM_RECYCLING(run_connectable);
  gobject = G_OBJECT(stream_recycling->recycling);

  stream_recycling->add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_stream_recycling_add_audio_signal_callback), stream_recycling);
}

void
ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsStreamRecycling *stream_recycling;
  GObject *gobject;

  ags_stream_recycling_parent_run_connectable_interface->disconnect(run_connectable);

  stream_recycling = AGS_STREAM_RECYCLING(run_connectable);
  gobject = G_OBJECT(stream_recycling);

  g_signal_handler_disconnect(gobject,
			      stream_recycling->add_audio_signal_handler);
}

AgsRecall*
ags_stream_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsStreamRecycling *stream_recycling, *copy;

  stream_recycling = (AgsStreamRecycling *) recall;
  copy = (AgsStreamRecycling *) AGS_RECALL_CLASS(ags_stream_recycling_parent_class)->duplicate(recall,
											       recall_id,
											       n_params, parameter);

  g_object_set(G_OBJECT(copy),
	       "recycling\0", stream_recycling->recycling,
	       NULL);

  return((AgsRecall *) copy);
}

void
ags_stream_recycling_add_audio_signal(AgsStreamRecycling *stream_recycling,
				      AgsRecycling *recycling,
				      AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = ags_stream_audio_signal_new(audio_signal);

  if((AGS_RECALL_PLAYBACK & (AGS_RECALL(stream_recycling)->flags)) != 0)
    AGS_RECALL(stream_audio_signal)->flags |= AGS_RECALL_PROPAGATE_DONE;

  ags_recall_add_child(AGS_RECALL(stream_recycling), AGS_RECALL(stream_audio_signal));
}

void
ags_stream_recycling_add_audio_signal_callback(AgsRecycling *recycling,
					       AgsAudioSignal *audio_signal,
					       AgsStreamRecycling *stream_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(stream_recycling)->recall_id->group_id)
    ags_stream_recycling_add_audio_signal(stream_recycling,
					  recycling,
					  audio_signal);
}

AgsStreamRecycling*
ags_stream_recycling_new(AgsRecycling *recycling)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = (AgsStreamRecycling *) g_object_new(AGS_TYPE_STREAM_RECYCLING,
							 "recycling\0", recycling,
							 NULL);

  return(stream_recycling);
}
