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

#include <ags/audio/recall/ags_play_notation.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

void ags_play_notation_class_init(AgsPlayNotationClass *play_notation);
void ags_play_notation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_notation_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_notation_init(AgsPlayNotation *play_notation);
void ags_play_notation_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_play_notation_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_play_notation_finalize(GObject *gobject);
void ags_play_notation_connect(AgsConnectable *connectable);
void ags_play_notation_disconnect(AgsConnectable *connectable);
void ags_play_notation_run_connect(AgsRunConnectable *run_connectable);
void ags_play_notation_run_disconnect(AgsRunConnectable *run_connectable);

void ags_play_notation_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_notation_duplicate(AgsRecall *recall,
				       AgsRecallID *recall_id,
				       guint *n_params, GParameter *parameter);

void ags_play_notation_play_note_done(AgsRecall *recall, AgsPlayNotation *play_notation);
void ags_play_notation_delay_tic_alloc_input_callback(AgsDelayAudioRun *delay,
						      guint nth_run, guint attack,
						      AgsPlayNotation *play_notation);

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_play_notation_parent_class = NULL;
static AgsConnectableInterface* ags_play_notation_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_notation_parent_run_connectable_interface;

GType
ags_play_notation_get_type()
{
  static GType ags_type_play_notation = 0;

  if(!ags_type_play_notation){
    static const GTypeInfo ags_play_notation_info = {
      sizeof (AgsPlayNotationClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_notation_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayNotation),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_notation_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_notation = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						    "AgsPlayNotation",
						    &ags_play_notation_info,
						    0);

    g_type_add_interface_static(ags_type_play_notation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_notation,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_play_notation);
}

void
ags_play_notation_class_init(AgsPlayNotationClass *play_notation)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_notation_parent_class = g_type_class_peek_parent(play_notation);

  /* GObjectClass */
  gobject = (GObjectClass *) play_notation;

  gobject->set_property = ags_play_notation_set_property;
  gobject->get_property = ags_play_notation_get_property;

  gobject->finalize = ags_play_notation_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout",
				   "assigned AgsDevout",
				   "the AgsDevout",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_object("delay_audio_run",
				   "assigned AgsDelayAudioRun",
				   "the AgsDelayAudioRun which emits tic_alloc_input signal",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("count_beats_audio_run",
				   "assigned AgsCountBeatsAudioRun",
				   "the AgsCount_BeatsAudioRun which just counts",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_notation;

  recall->resolve_dependencies = ags_play_notation_resolve_dependencies;
  recall->duplicate = ags_play_notation_duplicate;
}

void
ags_play_notation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_notation_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_notation_connect;
  connectable->disconnect = ags_play_notation_disconnect;
}

void
ags_play_notation_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_notation_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_notation_run_connect;
  run_connectable->disconnect = ags_play_notation_run_disconnect;
}

void
ags_play_notation_init(AgsPlayNotation *play_notation)
{
  play_notation->devout = NULL;
  
  play_notation->notation = NULL;

  play_notation->delay_audio_run = NULL;

  play_notation->count_beats_audio_run = NULL;
}

void
ags_play_notation_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsPlayNotation *play_notation;

  play_notation = AGS_PLAY_NOTATION(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = g_value_get_object(value);

      if(devout == play_notation->devout){
	return;
      }

      if(play_notation->devout != NULL){
	g_object_unref(G_OBJECT(play_notation->devout));
      }

      if(devout != NULL){
	g_object_ref(devout);
      }

      play_notation->devout = devout;
    }
    break;
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = g_value_get_object(value);

      if(delay_audio_run == play_notation->delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_notation->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_notation),
				       (AgsRecall *) play_notation->delay_audio_run);
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_notation)->flags)) != 0){
	    g_signal_handler_disconnect(G_OBJECT(play_notation),
					play_notation->tic_alloc_input_handler);
	  }
	}

	g_object_unref(G_OBJECT(play_notation->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_notation),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_notation)->flags)) != 0){
	    play_notation->tic_alloc_input_handler =
	      g_signal_connect(G_OBJECT(delay_audio_run), "tic_alloc_input",
			       G_CALLBACK(ags_play_notation_delay_tic_alloc_input_callback), play_notation);
	  }
	}
      }

      play_notation->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);

      if(count_beats_audio_run == play_notation->count_beats_audio_run){
	return;
      }

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_notation->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_notation),
				       (AgsRecall *) play_notation->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(play_notation->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_notation),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      play_notation->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_notation_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsPlayNotation *play_notation;
  
  play_notation = AGS_PLAY_NOTATION(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, G_OBJECT(play_notation->devout));
    }
    break;
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_notation->delay_audio_run));
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_notation->count_beats_audio_run));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_notation_finalize(GObject *gobject)
{
  AgsPlayNotation *play_notation;

  play_notation = AGS_PLAY_NOTATION(gobject);

  if(play_notation->devout != NULL){
    g_object_unref(G_OBJECT(play_notation->devout));
  }

  if(play_notation->notation != NULL){
    g_object_unref(G_OBJECT(play_notation->notation));
  }

  if(play_notation->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation->delay_audio_run));
  }

  if(play_notation->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation->count_beats_audio_run));
  }

  G_OBJECT_CLASS(ags_play_notation_parent_class)->finalize(gobject);
}

void
ags_play_notation_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_play_notation_parent_connectable_interface->connect(connectable);
}

void
ags_play_notation_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_play_notation_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_notation_run_connect(AgsRunConnectable *run_connectable)
{
  AgsPlayNotation *play_notation;

  /* call parent */
  ags_play_notation_parent_run_connectable_interface->connect(run_connectable);

  play_notation = AGS_PLAY_NOTATION(run_connectable);

  if(play_notation->delay_audio_run != NULL){
    play_notation->tic_alloc_input_handler = g_signal_connect_after(G_OBJECT(play_notation->delay_audio_run), "tic_alloc_input",
								    G_CALLBACK(ags_play_notation_delay_tic_alloc_input_callback), play_notation);
  }
}

void
ags_play_notation_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsPlayNotation *play_notation;

  /* call parent */
  ags_play_notation_parent_run_connectable_interface->disconnect(run_connectable);

  play_notation = AGS_PLAY_NOTATION(run_connectable);

  if(play_notation->delay_audio_run != NULL){
    g_signal_handler_disconnect(G_OBJECT(play_notation->delay_audio_run), play_notation->tic_alloc_input_handler);
  }
}

void
ags_play_notation_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsPlayNotation *play_notation;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  play_notation = AGS_PLAY_NOTATION(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_audio_run)->data);

  list = template->dependencies;
  group_id = recall->recall_id->group_id;

  delay_audio_run = NULL;
  count_beats_audio_run = NULL;
  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay_audio_run", delay_audio_run,
	       "count_beats_audio_run", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_play_notation_duplicate(AgsRecall *recall,
			    AgsRecallID *recall_id,
			    guint *n_params, GParameter *parameter)
{
  AgsPlayNotation *copy, *play_notation;

  copy = AGS_PLAY_NOTATION(AGS_RECALL_CLASS(ags_play_notation_parent_class)->duplicate(recall,
										       recall_id,
										       n_params, parameter));

  play_notation = AGS_PLAY_NOTATION(recall);

  g_object_set(G_OBJECT(play_notation),
	       "devout", play_notation->devout,
	       NULL);

  //FIXME:JK: write properties
  play_notation = AGS_PLAY_NOTATION(recall);
  copy->notation = play_notation->notation;

  return((AgsRecall *) copy);
}

void
ags_play_notation_delay_tic_alloc_input_callback(AgsDelayAudioRun *delay,
						 guint nth_run, guint attack,
						 AgsPlayNotation *play_notation)
{
  AgsNotation *notation;
  AgsAudio *audio;
  AgsChannel *selected_channel, *channel;
  AgsAudioSignal *audio_signal;
  AgsRunOrder *run_order;
  GList *current_position;
  AgsNote *note;
  AgsRecycling *recycling;
  GList *list;
  guint i;

  list = play_notation->notation;

  if(list == NULL)
    return;

  audio = AGS_RECALL_AUDIO_RUN(play_notation)->recall_audio->audio;

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
    run_order = ags_run_order_find_group_id(audio->run_order,
					    AGS_RECALL(play_notation)->recall_id->child_group_id);
  }else{
    run_order = ags_run_order_find_group_id(audio->run_order,
					    AGS_RECALL(play_notation)->recall_id->group_id);
  }

  channel = AGS_CHANNEL(g_list_nth(run_order->run_order, nth_run - 1)->data);
  channel = ags_channel_nth(audio->input, channel->audio_channel);

  notation = AGS_NOTATION(g_list_nth(list, channel->audio_channel)->data);
  current_position = notation->notes; // start_loop

  while(current_position != NULL){
    note = AGS_NOTE(current_position->data);
    
    if(current_position != notation->end_loop){
      if(note->x[0] == play_notation->count_beats_audio_run->notation_counter){
	selected_channel = ags_channel_pad_nth(channel, note->y);
	
	recycling = selected_channel->first_recycling;
	
	g_message("playing: %u | %u\n", note->x[0], note->y);
	
	while(recycling != selected_channel->last_recycling){
	  audio_signal = ags_audio_signal_new((GObject *) audio->devout,
					      (GObject *) recycling,
					      (GObject *) AGS_RECALL(play_notation)->recall_id);
	  ags_recycling_create_audio_signal_with_frame_count(recycling,
							     audio_signal,
							     note->x[1] - note->x[0],
							     attack);
	  ags_audio_signal_connect(audio_signal);

	  g_message("adding\n");

	  ags_recycling_add_audio_signal(recycling,
					 audio_signal);
	  
	  recycling = recycling->next;
	}
      }else if(note->x[0] > play_notation->count_beats_audio_run->notation_counter){
	break;
      }
    }
    
    current_position = current_position->next;
  }
}

AgsPlayNotation*
ags_play_notation_new()
{
  AgsPlayNotation *play_notation;

  play_notation = (AgsPlayNotation *) g_object_new(AGS_TYPE_PLAY_NOTATION, NULL);

  return(play_notation);
}
