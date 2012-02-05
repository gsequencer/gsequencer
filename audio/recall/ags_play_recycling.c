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

#include <ags/audio/recall/ags_play_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling);
void ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_recycling_init(AgsPlayRecycling *play_recycling);
void ags_play_recycling_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_play_recycling_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_play_recycling_connect(AgsConnectable *connectable);
void ags_play_recycling_disconnect(AgsConnectable *connectable);
void ags_play_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_play_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_play_recycling_finalize(GObject *gobject);

void ags_play_recycling_done(AgsRecall *recall);
void ags_play_recycling_cancel(AgsRecall *recall);
AgsRecall* ags_play_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint n_params, GParameter *parameter);

void ags_play_recycling_source_add_audio_signal(AgsPlayRecycling *play_recycling,
						AgsAudioSignal *audio_signal);

void ags_play_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsPlayRecycling *play_recycling);
void ags_play_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							    AgsAudioSignal *audio_signal,
							    AgsPlayRecycling *play_recycling);

void ags_play_recycling_play_audio_signal_done(AgsRecall *recall,
					       gpointer data);

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_AUDIO_CHANNEL,
  PROP_SOURCE,
};

static gpointer ags_play_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_play_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_recycling_parent_run_connectable_interface;

GType
ags_play_recycling_get_type()
{
  static GType ags_type_play_recycling = 0;

  if(!ags_type_play_recycling){
    static const GTypeInfo ags_play_recycling_info = {
      sizeof (AgsPlayRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_recycling = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsPlayRecycling\0",
						     &ags_play_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_play_recycling);
}

void
ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_recycling_parent_class = g_type_class_peek_parent(play_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) play_recycling;

  gobject->set_property = ags_play_recycling_set_property;
  gobject->get_property = ags_play_recycling_get_property;

  gobject->finalize = ags_play_recycling_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "assigned AgsDevout\0",
				   "The AgsDevout this recall is assigned with\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_uint("audio_channel\0",
				 "assigned audio Channel\0",
				 "The audio channel this recall does output to\0",
				 0,
				 65536,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  param_spec = g_param_spec_object("source\0",
				   "source AgsRecycling\0",
				   "The AgsRecycling this recall has as source\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_recycling;

  recall->done = ags_play_recycling_done;
  recall->cancel = ags_play_recycling_cancel;
  recall->duplicate = ags_play_recycling_duplicate;
}

void
ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_recycling_connect;
  connectable->disconnect = ags_play_recycling_disconnect;
}

void
ags_play_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_recycling_run_connect;
  run_connectable->disconnect = ags_play_recycling_run_disconnect;
}

void
ags_play_recycling_init(AgsPlayRecycling *play_recycling)
{
  play_recycling->devout = NULL;

  play_recycling->source = NULL;

  play_recycling->audio_channel = 0;
}

void
ags_play_recycling_connect(AgsConnectable *connectable)
{
  ags_play_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_recycling_disconnect(AgsConnectable *connectable)
{
  ags_play_recycling_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsPlayRecycling *play_recycling;
  GObject *gobject;

  ags_play_recycling_parent_run_connectable_interface->connect(run_connectable);

  /* AgsPlayRecycling */
  play_recycling = AGS_PLAY_RECYCLING(run_connectable);

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  play_recycling->source_add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_play_recycling_source_add_audio_signal_callback), play_recycling);

  play_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_play_recycling_source_remove_audio_signal_callback), play_recycling);
}

void
ags_play_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsPlayRecycling *play_recycling;
  GObject *gobject;

  ags_play_recycling_parent_run_connectable_interface->disconnect(run_connectable);

  /* AgsPlayRecycling */
  play_recycling = AGS_PLAY_RECYCLING(run_connectable);

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_handler);

  g_signal_handler_disconnect(gobject, play_recycling->source_remove_audio_signal_handler);
}

void
ags_play_recycling_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayRecycling *play_recycling;
  
  play_recycling = AGS_PLAY_RECYCLING(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(play_recycling->devout == devout)
	return;

      if(play_recycling->devout != NULL)
	g_object_unref(G_OBJECT(play_recycling->devout));

      if(devout != NULL)
	g_object_ref(G_OBJECT(devout));

      play_recycling->devout = devout;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      play_recycling->audio_channel = g_value_get_uint(value);
    }
    break;
  case PROP_SOURCE:
    {
      AgsRecycling *source;

      source = (AgsRecycling *) g_value_get_object(value);

      if(play_recycling->source == source)
	return;

      if(play_recycling->source != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_recycling)->flags)) != 0){
	  gobject = G_OBJECT(play_recycling->source);

	  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_handler);
	  
	  g_signal_handler_disconnect(gobject, play_recycling->source_remove_audio_signal_handler);
	}

	g_object_unref(play_recycling->source);
      }

      if(source != NULL)
	g_object_ref(source);

      play_recycling->source = source;

      if(source != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_recycling)->flags)) != 0){
	  gobject = G_OBJECT(source);
	  
	  play_recycling->source_add_audio_signal_handler =
	    g_signal_connect_after(gobject, "add_audio_signal\0",
				   G_CALLBACK(ags_play_recycling_source_add_audio_signal_callback), play_recycling);
	    
	  play_recycling->source_remove_audio_signal_handler =
	    g_signal_connect(gobject, "remove_audio_signal\0",
			     G_CALLBACK(ags_play_recycling_source_remove_audio_signal_callback), play_recycling);
	}
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_recycling_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, play_recycling->devout);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, play_recycling->audio_channel);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, play_recycling->source);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_recycling_finalize(GObject *gobject)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(gobject);

  if(play_recycling->devout != NULL)
    g_object_unref(G_OBJECT(play_recycling->devout));

  if(play_recycling->source != NULL)
    g_object_unref(G_OBJECT(play_recycling->source));

  /* call parent */
  G_OBJECT_CLASS(ags_play_recycling_parent_class)->finalize(gobject);
}

void 
ags_play_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_play_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_recycling_parent_class)->cancel(recall);

  /* empty */
}

AgsRecall*
ags_play_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint n_params, GParameter *parameter)
{
  AgsPlayRecycling *play_recycling, *copy;

  play_recycling = (AgsPlayRecycling *) recall;
  copy = (AgsPlayRecycling *) AGS_RECALL_CLASS(ags_play_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);

  g_object_set(G_OBJECT(copy),
	       "devout\0", play_recycling->devout,
	       "source\0", play_recycling->source,
	       "audio_channel\0", play_recycling->audio_channel,
	       NULL);

  return((AgsRecall *) copy);
}

void
ags_play_recycling_source_add_audio_signal(AgsPlayRecycling *play_recycling,
					   AgsAudioSignal *audio_signal)
{
  AgsPlayAudioSignal *play_audio_signal;
  AgsAttack *attack;
  GValue attack_value = {0,};

  audio_signal->stream_current = audio_signal->stream_beginning;

  attack = ags_attack_duplicate_from_devout(G_OBJECT(play_recycling->devout));

  play_audio_signal = ags_play_audio_signal_new(audio_signal,
						play_recycling->devout,
						play_recycling->audio_channel,
						attack);

  ags_recall_add_child(AGS_RECALL(play_recycling),
		       AGS_RECALL(play_audio_signal));

  g_signal_connect((GObject *) play_audio_signal, "done\0",
		   G_CALLBACK(ags_play_recycling_play_audio_signal_done), NULL);

}

void
ags_play_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						    AgsAudioSignal *audio_signal,
						    AgsPlayRecycling *play_recycling)
{
  printf("ags_play_recycling_source_add_audio_signal_callback\n\0");

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(play_recycling)->recall_id->group_id)
    ags_play_recycling_source_add_audio_signal(play_recycling,
					       audio_signal);
  else{
    printf("   debug:  retrieved = %u ;  this = %u\n\0", AGS_RECALL_ID(audio_signal->recall_id)->group_id, AGS_RECALL(play_recycling)->recall_id->group_id);
  }
}

void
ags_play_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
						       AgsAudioSignal *audio_signal,
						       AgsPlayRecycling *play_recycling)
{
  AgsDevout *devout;
  AgsRecall *play_recycling_recall;
  AgsCancelRecall *cancel_recall;
  AgsPlayAudioSignal *play_audio_signal;
  GList *list;
  guint audio_channel;

  play_recycling_recall = AGS_RECALL(play_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == play_recycling_recall->recall_id->group_id){
    AgsRecall *recall;

    devout = AGS_DEVOUT(AGS_AUDIO(AGS_CHANNEL(source->channel)->audio)->devout);
    list = ags_recall_get_children(AGS_RECALL(play_recycling_recall));
    audio_channel = AGS_CHANNEL(source->channel)->audio_channel;

    while(list != NULL){
      play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(list->data);
      recall = AGS_RECALL(play_audio_signal);

      if(play_audio_signal->source == audio_signal && (AGS_RECALL_DONE & (recall->flags)) == 0){
	recall->flags |= AGS_RECALL_HIDE;
	cancel_recall = ags_cancel_recall_new(recall,
					      NULL);

	ags_devout_append_task(devout, (AgsTask *) cancel_recall);

	break;
      }

      list = list->next;
    }
  }
}

void
ags_play_recycling_play_audio_signal_done(AgsRecall *recall,
					  gpointer data)
{
  recall->flags |= AGS_RECALL_REMOVE | AGS_RECALL_HIDE;
}

AgsPlayRecycling*
ags_play_recycling_new(AgsRecycling *source,
		       AgsDevout *devout,
		       guint audio_channel)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = (AgsPlayRecycling *) g_object_new(AGS_TYPE_PLAY_RECYCLING,
						     "source\0", source,
						     "devout\0", devout,
						     "audio_channel\0", audio_channel,
						     NULL);

  return(play_recycling);
}
