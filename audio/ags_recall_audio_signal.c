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

#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>

void ags_recall_audio_signal_class_init(AgsRecallAudioSignalClass *recall_audio_signal);
void ags_recall_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
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
void ags_recall_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_audio_signal_finalize(GObject *gobject);

void ags_recall_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_recall_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
  PROP_DEVOUT,
  PROP_ATTACK,
};

static gpointer ags_recall_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_recall_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_recall_audio_signal_parent_run_connectable_interface;

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

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_signal_run_connectable_interface_init,
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
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
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

  /* properties */
  param_spec = g_param_spec_object("destination\0",
				   "destination of output\0",
				   "The destination where this recall will write the audio signal to\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  param_spec = g_param_spec_object("source\0",
				   "source of input\0",
				   "The source where this recall will take the audio signal from\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);
  
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout this recall is assigned to\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);
  
  param_spec = g_param_spec_pointer("attack\0",
				    "assigned attack\0",
				    "The attack that determines to which frame to recall\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_signal;
  
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
ags_recall_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_audio_signal_run_connect;
  run_connectable->disconnect = ags_recall_audio_signal_run_disconnect;
}

void
ags_recall_audio_signal_init(AgsRecallAudioSignal *recall_audio_signal)
{
  recall_audio_signal->devout = NULL;

  recall_audio_signal->source = NULL;
  recall_audio_signal->destination = NULL;
  recall_audio_signal->attack = NULL;
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
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(recall_audio_signal->devout == devout)
	return;

      if(recall_audio_signal->devout != NULL){
	g_object_unref(recall_audio_signal->devout);
	free(recall_audio_signal->attack);
	recall_audio_signal->attack = NULL;
      }

      if(devout != NULL){
	g_object_ref(G_OBJECT(devout));
	recall_audio_signal->attack = ags_attack_duplicate_from_devout((GObject *) devout);
      }

      recall_audio_signal->devout = devout;
    }
    break;
  case PROP_ATTACK:
    {
      AgsAttack *attack;

      attack = (AgsAttack *) g_value_get_pointer(value);

      if(recall_audio_signal->attack == attack)
	return;

      if(recall_audio_signal->attack != NULL)
	free(recall_audio_signal->attack);

      recall_audio_signal->attack = attack;
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
  case PROP_DEVOUT:
    {
      g_value_set_object(value, recall_audio_signal->devout);
    }
    break;
  default:
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, recall_audio_signal->attack);
    }
    break;
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
ags_recall_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_recall_audio_signal_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_recall_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_recall_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_recall_audio_signal_finalize(GObject *gobject)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(gobject);

  if(recall_audio_signal->devout != NULL)
    g_object_unref(recall_audio_signal->devout);

  if(recall_audio_signal->destination != NULL)
    g_object_unref(recall_audio_signal->destination);

  if(recall_audio_signal->attack != NULL)
    free(recall_audio_signal->attack);

  if(recall_audio_signal->source != NULL)
    g_object_unref(recall_audio_signal->source);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_signal_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsRecallAudioSignal *recall_audio_signal, *copy;

  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "devout\0", recall_audio_signal->devout,
				 "destination\0", recall_audio_signal->destination,
				 "source\0", recall_audio_signal->source,
				 NULL);

  recall_audio_signal = (AgsRecallAudioSignal *) recall;
  copy = (AgsRecallAudioSignal *) AGS_RECALL_CLASS(ags_recall_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  return((AgsRecall *) recall);
}

AgsRecallAudioSignal*
ags_recall_audio_signal_new(AgsAudioSignal *destination,
			  AgsAudioSignal *source,
			  AgsDevout *devout,
			  AgsAttack *attack)
{
  AgsRecallAudioSignal *recall_audio_signal;

  recall_audio_signal = (AgsRecallAudioSignal *) g_object_new(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "destination\0", destination,
							  "source\0", source,
							  "devout\0", devout,
							  "attack\0", attack,
							  NULL);

  return(recall_audio_signal);
}
