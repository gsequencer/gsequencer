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

#include <ags/audio/recall/ags_copy_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling);
void ags_copy_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_recycling_init(AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_copy_recycling_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_copy_recycling_connect(AgsConnectable *connectable);
void ags_copy_recycling_disconnect(AgsConnectable *connectable);
void ags_copy_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_recycling_finalize(GObject *gobject);

void ags_copy_recycling_done(AgsRecall *recall);
void ags_copy_recycling_cancel(AgsRecall *recall);
void ags_copy_recycling_remove(AgsRecall *recall);
AgsRecall* ags_copy_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_recycling_source_add_audio_signal(AgsCopyRecycling *copy_recycling,
						AgsAudioSignal *audio_signal);
void ags_copy_recycling_destination_add_audio_signal(AgsCopyRecycling *copy_recycling,
						     AgsAudioSignal *audio_signal);

void ags_copy_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_source_add_audio_signal_with_frame_count_callback(AgsRecycling *source,
									  AgsAudioSignal *audio_signal, guint length,
									  AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							    AgsAudioSignal *audio_signal,
							    AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_add_audio_signal_callback(AgsRecycling *destination,
							      AgsAudioSignal *audio_signal,
							      AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_add_audio_signal_with_frame_count_callback(AgsRecycling *destination,
									       AgsAudioSignal *audio_signal, guint length,
									       AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
								 AgsAudioSignal *audio_signal,
								 AgsCopyRecycling *copy_recycling);

void ags_copy_recycling_copy_audio_signal_done(AgsRecall *recall,
					       gpointer data);

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
  PROP_DEVOUT,
};

static gpointer ags_copy_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_copy_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_recycling_parent_run_connectable_interface;

GType
ags_copy_recycling_get_type()
{
  static GType ags_type_copy_recycling = 0;

  if(!ags_type_copy_recycling){
    static const GTypeInfo ags_copy_recycling_info = {
      sizeof (AgsCopyRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_recycling = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsCopyRecycling\0",
						     &ags_copy_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_copy_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_recycling);
}

void
ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_recycling_parent_class = g_type_class_peek_parent(copy_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_recycling;

  gobject->set_property = ags_copy_recycling_set_property;
  gobject->get_property = ags_copy_recycling_get_property;

  gobject->finalize = ags_copy_recycling_finalize;

  /* properties */
  param_spec = g_param_spec_gtype("devout\0",
				  "assigned AgsDevout\0",
				  "The AgsDevout this recall is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_gtype("destination\0",
				  "destination AgsRecycling\0",
				  "The AgsRecycling this recall has as destination\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_gtype("source\0",
				  "source AgsRecycling\0",
				  "The AgsRecycling this recall has as source\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_recycling;

  recall->done = ags_copy_recycling_done;
  recall->cancel = ags_copy_recycling_cancel;
  recall->remove = ags_copy_recycling_remove;

  recall->duplicate = ags_copy_recycling_duplicate;
}

void
ags_copy_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_recycling_connect;
  connectable->disconnect = ags_copy_recycling_disconnect;
}

void
ags_copy_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_recycling_run_connect;
  run_connectable->disconnect = ags_copy_recycling_run_disconnect;
}

void
ags_copy_recycling_init(AgsCopyRecycling *copy_recycling)
{
  copy_recycling->devout = NULL;

  copy_recycling->destination = NULL;

  copy_recycling->source = NULL;

  copy_recycling->child_destination = NULL;
}

void
ags_copy_recycling_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsCopyRecycling *copy_recycling;
  
  copy_recycling = AGS_COPY_RECYCLING(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(copy_recycling->devout == devout)
	return;

      if(copy_recycling->devout != NULL)
	g_object_unref(G_OBJECT(copy_recycling->devout));

      if(devout != NULL)
	g_object_ref(G_OBJECT(devout));

      copy_recycling->devout = devout;
    }
    break;
  case PROP_DESTINATION:
    {
      AgsRecycling *destination;

      destination = (AgsRecycling *) g_value_get_object(value);

      if(copy_recycling->destination == destination)
	return;

      if(copy_recycling->destination != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_recycling)->flags)) != 0){
	  gobject = G_OBJECT(copy_recycling->destination);

	  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_handler);
	  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_with_frame_count_handler);
	  
	  g_signal_handler_disconnect(gobject, copy_recycling->destination_remove_audio_signal_handler);
	}

	g_object_unref(copy_recycling->destination);
      }

      if(destination != NULL)
	g_object_ref(destination);

      copy_recycling->destination = destination;

      if(destination != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_recycling)->flags)) != 0){
	  gobject = G_OBJECT(destination);
	  
	  copy_recycling->destination_add_audio_signal_handler =
	    g_signal_connect_after(gobject, "add_audio_signal\0",
				   G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_callback), copy_recycling);
	  
	  copy_recycling->destination_add_audio_signal_with_frame_count_handler =
	    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
				   G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_with_frame_count_callback), copy_recycling);
  
	  copy_recycling->destination_remove_audio_signal_handler =
	    g_signal_connect(gobject, "remove_audio_signal\0",
			     G_CALLBACK(ags_copy_recycling_destination_remove_audio_signal_callback), copy_recycling);
	}
      }
    }
    break;
  case PROP_SOURCE:
    {
      AgsRecycling *source;

      source = (AgsRecycling *) g_value_get_object(value);

      if(copy_recycling->source == source)
	return;

      if(copy_recycling->source != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_recycling)->flags)) != 0){
	  gobject = G_OBJECT(copy_recycling->source);

	  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_handler);
	  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_with_frame_count_handler);
	  
	  g_signal_handler_disconnect(gobject, copy_recycling->source_remove_audio_signal_handler);
	}

	g_object_unref(copy_recycling->source);
      }

      if(source != NULL)
	g_object_ref(source);

      copy_recycling->source = source;

      if(source != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_recycling)->flags)) != 0){
	  gobject = G_OBJECT(source);
	  
	  copy_recycling->source_add_audio_signal_handler =
	    g_signal_connect_after(gobject, "add_audio_signal\0",
				   G_CALLBACK(ags_copy_recycling_source_add_audio_signal_callback), copy_recycling);
	  
	  copy_recycling->source_add_audio_signal_with_frame_count_handler =
	    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
				   G_CALLBACK(ags_copy_recycling_source_add_audio_signal_with_frame_count_callback), copy_recycling);
  
	  copy_recycling->source_remove_audio_signal_handler =
	    g_signal_connect(gobject, "remove_audio_signal\0",
			     G_CALLBACK(ags_copy_recycling_source_remove_audio_signal_callback), copy_recycling);
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
ags_copy_recycling_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsCopyRecycling *copy_recycling;
  
  copy_recycling = AGS_COPY_RECYCLING(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, copy_recycling->devout);
    }
    break;
  case PROP_DESTINATION:
    {
      g_value_set_object(value, copy_recycling->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, copy_recycling->source);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_recycling_connect(AgsConnectable *connectable)
{
  AgsCopyRecycling *copy_recycling;

  ags_copy_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_recycling_disconnect(AgsConnectable *connectable)
{
  AgsCopyRecycling *copy_recycling;

  ags_copy_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_copy_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyRecycling *copy_recycling;
  GObject *gobject;

  ags_copy_recycling_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyRecycling */
  copy_recycling = AGS_COPY_RECYCLING(run_connectable);

  /* destination */
  gobject = G_OBJECT(copy_recycling->destination);

  copy_recycling->destination_add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_callback), copy_recycling);
    
  copy_recycling->destination_add_audio_signal_with_frame_count_handler =
    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
			   G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_with_frame_count_callback), copy_recycling);
  
  copy_recycling->destination_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_destination_remove_audio_signal_callback), copy_recycling);

  /* source */
  gobject = G_OBJECT(copy_recycling->source);

  copy_recycling->source_add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_copy_recycling_source_add_audio_signal_callback), copy_recycling);

  copy_recycling->source_add_audio_signal_with_frame_count_handler =
    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
			   G_CALLBACK(ags_copy_recycling_source_add_audio_signal_with_frame_count_callback), copy_recycling);

  copy_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_source_remove_audio_signal_callback), copy_recycling);
}

void
ags_copy_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyRecycling *copy_recycling;
  GObject *gobject;

  ags_copy_recycling_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyRecycling */
  copy_recycling = AGS_COPY_RECYCLING(run_connectable);

  /* destination */
  gobject = G_OBJECT(copy_recycling->destination);

  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_with_frame_count_handler);

  g_signal_handler_disconnect(gobject, copy_recycling->destination_remove_audio_signal_handler);

  /* source */
  gobject = G_OBJECT(copy_recycling->source);

  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_with_frame_count_handler);

  g_signal_handler_disconnect(gobject, copy_recycling->source_remove_audio_signal_handler);
}

void
ags_copy_recycling_finalize(GObject *gobject)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = AGS_COPY_RECYCLING(gobject);

  if(copy_recycling->devout != NULL)
    g_object_unref(G_OBJECT(copy_recycling->devout));

  if(copy_recycling->destination != NULL)
    g_object_unref(G_OBJECT(copy_recycling->destination));

  if(copy_recycling->source != NULL)
    g_object_unref(G_OBJECT(copy_recycling->source));

  if(copy_recycling->child_destination != NULL)
    g_object_unref(copy_recycling->child_destination);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_recycling_parent_class)->finalize(gobject);
}

void 
ags_copy_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_copy_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->cancel(recall);

  /* empty */
}

void 
ags_copy_recycling_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_copy_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyRecycling *copy_recycling, *copy;

  copy_recycling = (AgsCopyRecycling *) recall;
  copy = (AgsCopyRecycling *) AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->duplicate(recall, recall_id);

  copy->devout = copy_recycling->devout;
  copy->destination = copy_recycling->destination;
  copy->source = copy_recycling->source;
  // copy->child_destination = copy_recycling->child_destination;

  return((AgsRecall *) copy);
}

void
ags_copy_recycling_source_add_audio_signal(AgsCopyRecycling *copy_recycling,
					   AgsAudioSignal *audio_signal)
{
  AgsCopyAudioSignal *copy_audio_signal;
  AgsAttack *attack;

  audio_signal->stream_current = audio_signal->stream_beginning;

  attack = ags_attack_duplicate_from_devout(copy_recycling->devout);

  copy_audio_signal = ags_copy_audio_signal_new(copy_recycling->child_destination,
						audio_signal,
						copy_recycling->devout,
						attack);

  ags_recall_add_child(AGS_RECALL(copy_recycling), AGS_RECALL(copy_audio_signal));
  g_signal_connect(G_OBJECT(copy_audio_signal), "done\0",
		   G_CALLBACK(ags_copy_recycling_copy_audio_signal_done), copy_recycling);
}

void
ags_copy_recycling_destination_add_audio_signal(AgsCopyRecycling *copy_recycling,
						AgsAudioSignal *audio_signal)
{
  copy_recycling->child_destination = audio_signal;
}

void
ags_copy_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						    AgsAudioSignal *audio_signal,
						    AgsCopyRecycling *copy_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->group_id)
    ags_copy_recycling_source_add_audio_signal(copy_recycling,
					       audio_signal);
}

void
ags_copy_recycling_source_add_audio_signal_with_frame_count_callback(AgsRecycling *source,
								     AgsAudioSignal *audio_signal, guint length,
								     AgsCopyRecycling *copy_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->group_id)
    ags_copy_recycling_source_add_audio_signal(copy_recycling,
					       audio_signal);
}

void
ags_copy_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
						       AgsAudioSignal *audio_signal,
						       AgsCopyRecycling *copy_recycling)
{
  AgsDevout *devout;
  AgsRecall *copy_recycling_recall;
  AgsCancelRecall *cancel_recall;
  AgsCopyAudioSignal *copy_audio_signal;
  GList *list;

  copy_recycling_recall = AGS_RECALL(copy_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == copy_recycling_recall->recall_id->group_id){
    AgsRecall *recall;

    devout = AGS_DEVOUT(AGS_AUDIO(AGS_CHANNEL(source->channel)->audio)->devout);

    list = ags_recall_get_children(AGS_RECALL(copy_recycling_recall));

    while(list != NULL){
      copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(list->data);
      recall = AGS_RECALL(copy_audio_signal);

      if(copy_audio_signal->source == audio_signal && (AGS_RECALL_DONE & (recall->flags)) == 0){
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
ags_copy_recycling_destination_add_audio_signal_callback(AgsRecycling *destination,
							 AgsAudioSignal *audio_signal,
							 AgsCopyRecycling *copy_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->parent_group_id)
    ags_copy_recycling_destination_add_audio_signal(copy_recycling,
						    audio_signal);
}

void
ags_copy_recycling_destination_add_audio_signal_with_frame_count_callback(AgsRecycling *destination,
									  AgsAudioSignal *audio_signal, guint length,
									  AgsCopyRecycling *copy_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->parent_group_id)
    ags_copy_recycling_destination_add_audio_signal(copy_recycling,
						    audio_signal);
}

void
ags_copy_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
							    AgsAudioSignal *audio_signal,
							    AgsCopyRecycling *copy_recycling)
{
  AgsDevout *devout;
  AgsRecall *copy_recycling_recall;
  AgsCancelRecall *cancel_recall;
  AgsCopyAudioSignal *copy_audio_signal;
  GList *list;
  GValue children_value = {0,};

  copy_recycling_recall = AGS_RECALL(copy_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == copy_recycling_recall->recall_id->group_id){
    AgsRecall *recall;

    devout = AGS_DEVOUT(AGS_AUDIO(AGS_CHANNEL(destination->channel)->audio)->devout);;

    list = ags_recall_get_children(AGS_RECALL(copy_recycling_recall));

    while(list != NULL){
      copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(list->data);
      recall = AGS_RECALL(copy_audio_signal);

      if(copy_audio_signal->destination == audio_signal && (AGS_RECALL_DONE & (recall->flags)) == 0){
	cancel_recall = ags_cancel_recall_new(recall,
					      NULL);

	ags_devout_append_task(devout, (AgsTask *) cancel_recall);
      }

      list = list->next;
    }
  }
}

void
ags_copy_recycling_copy_audio_signal_done(AgsRecall *recall,
					  gpointer data)
{
  fprintf(stdout, "ags_copy_recycling_copy_audio_signal_done\n\0");
  recall->flags |= AGS_RECALL_REMOVE | AGS_RECALL_HIDE;
}

AgsCopyRecycling*
ags_copy_recycling_new(AgsRecycling *destination,
		       AgsRecycling *source,
		       AgsDevout *devout)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = (AgsCopyRecycling *) g_object_new(AGS_TYPE_COPY_RECYCLING,
						     "destination\0", destination,
						     "source\0", source,
						     "devout\0", devout,
						     NULL);

  return(copy_recycling);
}
