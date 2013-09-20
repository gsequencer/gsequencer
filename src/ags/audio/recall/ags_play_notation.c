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

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

void ags_play_notation_class_init(AgsPlayNotationClass *play_notation);
void ags_play_notation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_notation_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
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
void ags_play_notation_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_notation_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);

void ags_play_notation_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_notation_duplicate(AgsRecall *recall,
				       AgsRecallID *recall_id,
				       guint *n_params, GParameter *parameter);

void ags_play_notation_play_note_done(AgsRecall *recall, AgsPlayNotation *play_notation);
void ags_play_notation_alloc_input_callback(AgsDelayAudioRun *delay,
					    guint nth_run, guint attack,
					    AgsPlayNotation *play_notation);

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_play_notation_parent_class = NULL;
static AgsConnectableInterface* ags_play_notation_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_notation_parent_dynamic_connectable_interface;

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

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_notation = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						    "AgsPlayNotation\0",
						    &ags_play_notation_info,
						    0);

    g_type_add_interface_static(ags_type_play_notation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_notation,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
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
  param_spec = g_param_spec_object("notation\0",
				   "assigned notation\0",
				   "the assigned AgsNotation\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  param_spec = g_param_spec_object("delay_audio_run\0",
				   "assigned AgsDelayAudioRun\0",
				   "the AgsDelayAudioRun which emits notation_alloc_input signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("count_beats_audio_run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "the AgsCount_BeatsAudioRun which just counts\0",
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
ags_play_notation_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_notation_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_notation_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_notation_disconnect_dynamic;
}

void
ags_play_notation_init(AgsPlayNotation *play_notation)
{
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
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = g_value_get_object(value);

      if(g_list_find(play_notation->notation, notation) != NULL || notation == NULL){
	return;
      }

      g_object_ref(notation);
      play_notation->notation = g_list_append(play_notation->notation, notation);
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
					play_notation->notation_alloc_input_handler);
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
	    play_notation->notation_alloc_input_handler =
	      g_signal_connect(G_OBJECT(delay_audio_run), "notation_alloc_input\0",
			       G_CALLBACK(ags_play_notation_alloc_input_callback), play_notation);
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

  if(play_notation->notation != NULL){
    GList *current;

    current = play_notation->notation;

    while(current != NULL){
      g_object_unref(G_OBJECT(current->data));

      current = current->next;
    }

    current = play_notation->notation;
    play_notation->notation = NULL;
    g_list_free(current);
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
  AgsPlayNotation *play_notation;

  /* call parent */
  ags_play_notation_parent_connectable_interface->connect(connectable);

  play_notation = AGS_PLAY_NOTATION(connectable);

  if(play_notation->delay_audio_run != NULL){
    play_notation->notation_alloc_input_handler = g_signal_connect_after(G_OBJECT(play_notation->delay_audio_run), "notation_alloc_input\0",
									 G_CALLBACK(ags_play_notation_alloc_input_callback), play_notation);
  }
}

void
ags_play_notation_disconnect(AgsConnectable *connectable)
{
  AgsPlayNotation *play_notation;

  /* call parent */
  ags_play_notation_parent_connectable_interface->disconnect(connectable);

  play_notation = AGS_PLAY_NOTATION(connectable);

  if(play_notation->delay_audio_run != NULL){
    g_signal_handler_disconnect(G_OBJECT(play_notation->delay_audio_run), play_notation->notation_alloc_input_handler);
  }
}

void
ags_play_notation_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayNotation *play_notation;

  play_notation = AGS_PLAY_NOTATION(dynamic_connectable);

  /* call parent */
  ags_play_notation_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect */
  play_notation->notation_alloc_input_handler =
    g_signal_connect(G_OBJECT(play_notation->delay_audio_run), "notation_alloc_input\0",
		     G_CALLBACK(ags_play_notation_alloc_input_callback), play_notation);
  
}

void
ags_play_notation_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_play_notation_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
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
	       "delay_audio_run\0", delay_audio_run,
	       "count_beats_audio_run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_play_notation_duplicate(AgsRecall *recall,
			    AgsRecallID *recall_id,
			    guint *n_params, GParameter *parameter)
{
  AgsPlayNotation *copy, *play_notation;
  GList *current;

  copy = AGS_PLAY_NOTATION(AGS_RECALL_CLASS(ags_play_notation_parent_class)->duplicate(recall,
										       recall_id,
										       n_params, parameter));

  play_notation = AGS_PLAY_NOTATION(recall);

  current = play_notation->notation;

  while(current != NULL){
    g_object_set(G_OBJECT(copy),
		 "notation\0", current->data,
		 NULL);

    current = current->next;
  }

  return((AgsRecall *) copy);
}

void
ags_play_notation_alloc_input_callback(AgsDelayAudioRun *delay,
				       guint nth_run, guint attack,
				       AgsPlayNotation *play_notation)
{
  AgsNotation *notation;
  AgsAudio *audio;
  AgsChannel *selected_channel, *channel, *next_pad;
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

  //FIXME:JK: this is wrong
  channel = ags_channel_nth(audio->output, nth_run - 1);

  notation = AGS_NOTATION(g_list_nth(audio->notation, channel->audio_channel)->data);

  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    channel = ags_channel_nth(audio->input, channel->audio_channel);
  }

  current_position = notation->notes; // start_loop

  while(current_position != NULL){
    note = AGS_NOTE(current_position->data);
    
    if(current_position != notation->end_loop){
      if(note->x[0] == play_notation->count_beats_audio_run->notation_counter){
	selected_channel = ags_channel_pad_nth(channel, note->y);
	
	recycling = selected_channel->first_recycling;
	
	g_message("playing: %u | %u\n\0", note->x[0], note->y);
	
	while(recycling != selected_channel->last_recycling){
	  audio_signal = ags_audio_signal_new((GObject *) audio->devout,
					      (GObject *) recycling,
					      (GObject *) AGS_RECALL(play_notation)->recall_id);
	  ags_recycling_create_audio_signal_with_frame_count(recycling,
							     audio_signal,
							     note->x[1] - note->x[0],
							     attack);
	  ags_audio_signal_connect(audio_signal);

	  g_message("adding\n\0");

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
