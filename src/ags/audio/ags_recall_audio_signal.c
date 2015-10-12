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

#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_recall_recycling.h>

#include <ags/lib/ags_list.h>
#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>

void ags_recall_audio_signal_class_init(AgsRecallAudioSignalClass *recall_audio_signal);
void ags_recall_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_audio_signal_init(AgsRecallAudioSignal *recall_audio_signal);
void ags_recall_audio_signal_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_recall_audio_signal_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_recall_audio_signal_connect(AgsConnectable *connectable);
void ags_recall_audio_signal_disconnect(AgsConnectable *connectable);
void ags_recall_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_audio_signal_finalize(GObject *gobject);

void ags_recall_audio_signal_run_init_pre(AgsRecall *recall);
void ags_recall_audio_signal_run_init_inter(AgsRecall *recall);
void ags_recall_audio_signal_run_init_post(AgsRecall *recall);

void ags_recall_audio_signal_run_pre(AgsRecall *recall);
void ags_recall_audio_signal_run_inter(AgsRecall *recall);
void ags_recall_audio_signal_run_post(AgsRecall *recall);
void ags_recall_audio_signal_done(AgsRecall *recall);

AgsRecall* ags_recall_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

void ags_recall_audio_signal_notify_devout(AgsRecallAudioSignal *recall_audio_signal, GParamSpec *param,
					   gpointer data);
void ags_recall_audio_signal_notify_devout_after(AgsRecallAudioSignal *recall_audio_signal, GParamSpec *param,
						 gpointer data);

/**
 * SECTION:ags_recall_audio_signal
 * @Short_description: audio signal context of recall
 * @Title: AgsRecallAudioSignal
 *
 * #AgsRecallAudioSignal acts as audio signal recall.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_recall_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_audio_signal_parent_dynamic_connectable_interface;

GType
ags_recall_audio_signal_get_type()
{
  static GType ags_type_recall_audio_signal = 0;

  if(!ags_type_recall_audio_signal){
    static const GTypeInfo ags_recall_audio_signal_info = {
      sizeof (AgsRecallAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							"AgsRecallAudioSignal\0",
							&ags_recall_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_recall_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_recall_audio_signal);
}

void
ags_recall_audio_signal_class_init(AgsRecallAudioSignalClass *recall_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_signal_parent_class = g_type_class_peek_parent(recall_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_signal;

  gobject->set_property = ags_recall_audio_signal_set_property;
  gobject->get_property = ags_recall_audio_signal_get_property;

  gobject->finalize = ags_recall_audio_signal_finalize;

  /* for debugging purpose */
  recall = (AgsRecallClass *) recall_audio_signal;

  recall->run_init_pre = ags_recall_audio_signal_run_init_pre;
  recall->run_init_inter = ags_recall_audio_signal_run_init_inter;
  recall->run_init_post = ags_recall_audio_signal_run_init_post;

  recall->run_pre = ags_recall_audio_signal_run_pre;
  recall->run_inter = ags_recall_audio_signal_run_inter;
  recall->run_post = ags_recall_audio_signal_run_post;

  /* properties */
  /**
   * AgsRecallAudioSignal:audio-channel:
   *
   * The audio channel to write use.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("audio-channel\0",
				 "output to audio channel\0",
				 "The audio channel to which it should write\0",
				 0,
				 65536,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsRecallAudioSignal:destination:
   *
   * The destination audio signal
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("destination\0",
				   "destination of output\0",
				   "The destination where this recall will write the audio signal to\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallAudioSignal:source:
   *
   * The source audio signal
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("source\0",
				   "source of input\0",
				   "The source where this recall will take the audio signal from\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);
  

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_signal;

  recall->done = ags_recall_audio_signal_done;  

  recall->duplicate = ags_recall_audio_signal_duplicate;
}

void
ags_recall_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_audio_signal_connect;
  connectable->disconnect = ags_recall_audio_signal_disconnect;
}

void
ags_recall_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_audio_signal_disconnect_dynamic;
}

void
ags_recall_audio_signal_init(AgsRecallAudioSignal *recall_audio_signal)
{
  //  g_signal_connect(G_OBJECT(recall_audio_signal), "notify::devout\0",
  //		   G_CALLBACK(ags_recall_audio_signal_notify_devout), NULL);
  //  g_signal_connect_after(G_OBJECT(recall_audio_signal), "notify::devout\0",
  //			 G_CALLBACK(ags_recall_audio_signal_notify_devout_after), NULL);

  recall_audio_signal->flags = AGS_RECALL_INITIAL_RUN;
  recall_audio_signal->audio_channel = 0;

  recall_audio_signal->source = NULL;
  recall_audio_signal->destination = NULL;
}

void
ags_recall_audio_signal_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = (guint) g_value_get_uint(value);

      recall_audio_signal->audio_channel = audio_channel;
    }
    break;
  case PROP_DESTINATION:
    {
      AgsAudioSignal *destination;

      destination = (AgsAudioSignal *) g_value_get_object(value);

      if(recall_audio_signal->destination == destination)
	return;

      if(recall_audio_signal->destination != NULL)
	g_object_unref(recall_audio_signal->destination);

      if(destination != NULL)
	g_object_ref(G_OBJECT(destination));

      recall_audio_signal->destination = destination;
    }
    break;
  case PROP_SOURCE:
    {
      AgsAudioSignal *source;

      source = (AgsAudioSignal *) g_value_get_object(value);

      if(recall_audio_signal->source == source)
	return;

      if(recall_audio_signal->source != NULL)
	g_object_unref(recall_audio_signal->source);

      if(source != NULL)
	g_object_ref(G_OBJECT(source));

      recall_audio_signal->source = source;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_signal_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, recall_audio_signal->audio_channel);
    }
    break;
  case PROP_DESTINATION:
    {
      g_value_set_object(value, recall_audio_signal->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, recall_audio_signal->source);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_signal_connect(AgsConnectable *connectable)
{
  AgsRecallAudioSignal *recall_audio_signal;

  ags_recall_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_signal_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_recall_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_recall_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_recall_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

void
ags_recall_audio_signal_finalize(GObject *gobject)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  if(recall_audio_signal->destination != NULL){
    g_object_unref(recall_audio_signal->destination);
    recall_audio_signal->destination = NULL;
  }

  if(recall_audio_signal->source != NULL){
    g_object_unref(recall_audio_signal->source);
    recall_audio_signal->source = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_signal_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsRecallAudioSignal *recall_audio_signal, *copy;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall);
  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "devout\0", AGS_RECALL(recall_audio_signal)->devout,
				 "audio_channel\0", recall_audio_signal->audio_channel,
				 "destination\0", recall_audio_signal->destination,
				 "source\0", recall_audio_signal->source,
				 NULL);

  copy = (AgsRecallAudioSignal *) AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  return((AgsRecall *) copy);
}

void
ags_recall_audio_signal_run_init_pre(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_init_pre(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_pre\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_pre\n\0");
}

void
ags_recall_audio_signal_run_init_inter(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_init_inter(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_inter\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_inter\n\0");
}

void
ags_recall_audio_signal_run_init_post(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_init_post(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_post\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_init_post\n\0");

}

void
ags_recall_audio_signal_run_pre(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_pre(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_pre\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_pre\n\0");

}

void
ags_recall_audio_signal_run_inter(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_inter(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_inter\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_inter\n\0");

}

void
ags_recall_audio_signal_run_post(AgsRecall *recall){
  AgsAudioSignal *source, *destination;

  source = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  destination = AGS_RECALL_AUDIO_SIGNAL(recall)->destination;

  AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->run_post(recall);

  if(source != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (source->flags)) != 0)
    g_warning("AgsRecallAudioSignal@source - AGS_AUDIO_SIGNAL_TEMPLATE: run_post\n\0");

  if(destination != NULL && (AGS_AUDIO_SIGNAL_TEMPLATE & (destination->flags)) != 0)
    g_warning("AgsRecallAudioSignal@destination - AGS_AUDIO_SIGNAL_TEMPLATE: run_post\n\0");

}

void
ags_recall_audio_signal_done(AgsRecall *recall)
{
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_audio_signal_notify_devout(AgsRecallAudioSignal *recall_audio_signal, GParamSpec *param,
				      gpointer data)
{
  //TODO:JK: implement me
}

void
ags_recall_audio_signal_notify_devout_after(AgsRecallAudioSignal *recall_audio_signal, GParamSpec *param,
					    gpointer data)
{
  //TODO:JK: implement me
}

/**
 * ags_recall_audio_signal_new:
 * @destination: destination #AgsAudioSignal
 * @source: source #AgsAudioSignal
 * @devout: default sink #AgsDevout
 *
 * Creates an #AgsRecallAudioSignal.
 *
 * Returns: a new #AgsRecallAudioSignal.
 *
 * Since: 0.4
 */
AgsRecallAudioSignal*
ags_recall_audio_signal_new(AgsAudioSignal *destination,
			    AgsAudioSignal *source,
			    AgsDevout *devout)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = (AgsRecallAudioSignal *) g_object_new(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							      "devout\0", devout,
							      "destination\0", destination,
							      "source\0", source, 
							      NULL);

  return(recall_audio_signal);
}
