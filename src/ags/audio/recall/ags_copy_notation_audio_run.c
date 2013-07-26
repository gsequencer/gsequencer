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

#include <ags/audio/recall/ags_copy_notation_audio_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_copy_notation_audio.h>

void ags_copy_notation_audio_run_class_init(AgsCopyNotationAudioRunClass *copy_notation_audio_run);
void ags_copy_notation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_notation_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_copy_notation_audio_run_init(AgsCopyNotationAudioRun *copy_notation_audio_run);
void ags_copy_notation_audio_run_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_copy_notation_audio_run_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_copy_notation_audio_run_connect(AgsConnectable *connectable);
void ags_copy_notation_audio_run_disconnect(AgsConnectable *connectable);
void ags_copy_notation_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_notation_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_notation_audio_run_finalize(GObject *gobject);

void ags_copy_notation_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_copy_notation_audio_run_duplicate(AgsRecall *recall,
						 AgsRecallID *recall_id,
						 guint *n_params, GParameter *parameter);
void ags_copy_notation_audio_run_run_init_pre(AgsRecall *recall);

void ags_copy_notation_audio_run_tic_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
							  guint run_order,
							  AgsCopyNotationAudioRun *copy_notation_audio_run);

enum{
  PROP_0,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_copy_notation_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_copy_notation_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_copy_notation_audio_run_parent_dynamic_connectable_interface;

GType
ags_copy_notation_audio_run_get_type()
{
  static GType ags_type_copy_notation_audio_run = 0;

  if(!ags_type_copy_notation_audio_run){
    static const GTypeInfo ags_copy_notation_audio_run_info = {
      sizeof (AgsCopyNotationAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_notation_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyNotationAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_notation_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_notation_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_notation_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_notation_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							      "AgsCopyNotationAudioRun\0",
							      &ags_copy_notation_audio_run_info,
							      0);
    
    g_type_add_interface_static(ags_type_copy_notation_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_copy_notation_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_copy_notation_audio_run);
}

void
ags_copy_notation_audio_run_class_init(AgsCopyNotationAudioRunClass *copy_notation_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_notation_audio_run_parent_class = g_type_class_peek_parent(copy_notation_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_notation_audio_run;

  gobject->set_property = ags_copy_notation_audio_run_set_property;
  gobject->get_property = ags_copy_notation_audio_run_get_property;

  gobject->finalize = ags_copy_notation_audio_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("count_beats_audio_run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "The AgsCountBeatsAudioRun which emits beat signal\0",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);
  
  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_notation_audio_run;

  recall->resolve_dependencies = ags_copy_notation_audio_run_resolve_dependencies;
  recall->duplicate = ags_copy_notation_audio_run_duplicate;
  recall->run_init_pre = ags_copy_notation_audio_run_run_init_pre;
}

void
ags_copy_notation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_notation_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_notation_audio_run_connect;
  connectable->disconnect = ags_copy_notation_audio_run_disconnect;
}

void
ags_copy_notation_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_copy_notation_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_copy_notation_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_copy_notation_audio_run_disconnect_dynamic;
}

void
ags_copy_notation_audio_run_init(AgsCopyNotationAudioRun *copy_notation_audio_run)
{
  copy_notation_audio_run->count_beats_audio_run = NULL;

  copy_notation_audio_run->current_note = NULL;
}

void
ags_copy_notation_audio_run_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(copy_notation_audio_run->count_beats_audio_run == count_beats_audio_run)
	return;

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(copy_notation_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(copy_notation_audio_run),
				       (AgsRecall *) copy_notation_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(copy_notation_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(G_OBJECT(count_beats_audio_run));

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(copy_notation_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      copy_notation_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_notation_audio_run_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;
  
  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, copy_notation_audio_run->count_beats_audio_run);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_notation_audio_run_finalize(GObject *gobject)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(gobject);

  if(copy_notation_audio_run->count_beats_audio_run != NULL)
    g_object_unref(copy_notation_audio_run->count_beats_audio_run);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_notation_audio_run_parent_class)->finalize(gobject);
}

void
ags_copy_notation_audio_run_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_notation_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_copy_notation_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_notation_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_notation_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;
  AgsDelayAudioRun *delay_audio_run;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_copy_notation_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect tic_alloc in AgsDelayAudioRun */
  delay_audio_run = copy_notation_audio_run->count_beats_audio_run->delay_audio_run;

  g_object_ref(G_OBJECT(delay_audio_run));
  copy_notation_audio_run->tic_alloc_input_handler =
    g_signal_connect(G_OBJECT(delay_audio_run), "tic_alloc_input\0",
		     G_CALLBACK(ags_copy_notation_audio_run_tic_alloc_input_callback), copy_notation_audio_run);
}

void
ags_copy_notation_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;
  AgsDelayAudioRun *delay_audio_run;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_copy_notation_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* disconnect tic_alloc in AgsDelayAudioRun */
  delay_audio_run = copy_notation_audio_run->count_beats_audio_run->delay_audio_run;

  g_signal_handler_disconnect(G_OBJECT(delay_audio_run),
			      copy_notation_audio_run->tic_alloc_input_handler);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_copy_notation_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsCopyNotationAudioRun *copy_notation_audio_run;
  AgsRecallDependency *recall_dependency;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_audio_run)->data);

  list = template->dependencies;
  group_id = recall->recall_id->group_id;

  count_beats_audio_run = NULL;
  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "count_beats_audio_run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_copy_notation_audio_run_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, GParameter *parameter)
{
  AgsCopyNotationAudioRun *copy;

  copy = AGS_COPY_NOTATION_AUDIO_RUN(AGS_RECALL_CLASS(ags_copy_notation_audio_run_parent_class)->duplicate(recall,
													   recall_id,
													   n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_copy_notation_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsCopyNotationAudio *copy_notation_audio;
  AgsCopyNotationAudioRun *copy_notation_audio_run;

  copy_notation_audio_run = AGS_COPY_NOTATION_AUDIO_RUN(recall);
  copy_notation_audio = AGS_COPY_NOTATION_AUDIO(AGS_RECALL_AUDIO_RUN(copy_notation_audio_run)->recall_audio);

  copy_notation_audio_run->current_note = copy_notation_audio->notation->notes;
}

void
ags_copy_notation_audio_run_tic_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						     guint run_order,
						     AgsCopyNotationAudioRun *copy_notation_audio_run)
{
  AgsAudio *audio;
  AgsChannel *input, *current_input;
  AgsCopyNotationAudio *copy_notation_audio;

  copy_notation_audio = AGS_COPY_NOTATION_AUDIO(AGS_RECALL_AUDIO_RUN(copy_notation_audio_run)->recall_audio);
  audio = AGS_RECALL_AUDIO(copy_notation_audio)->audio;

  input = ags_channel_nth(audio->input, copy_notation_audio->audio_channel);

  if(input != NULL){
    AgsRecycling *recycling, *last_recycling;
    AgsAudioSignal *audio_signal;
    AgsNote *note;
    GList *note_list;
    guint offset;
    guint length;

    note_list = copy_notation_audio_run->current_note;
    offset = copy_notation_audio_run->count_beats_audio_run->notation_counter;

    while(note_list != NULL &&
	  (note = AGS_NOTE(note_list->data))->x[0] == offset){
      current_input = ags_channel_nth(input, note->y);

      if(current_input != NULL){
	recycling = current_input->first_recycling;
	
	if(recycling == NULL){
	  note_list = note_list->next;
	  continue;
	}

	last_recycling = current_input->last_recycling;

	if((AGS_COPY_NOTATION_AUDIO_FIT_AUDIO_SIGNAL & (copy_notation_audio->flags)) != 0){
	  length = note->x[1] - note->x[0];
	}

	while(recycling != last_recycling->next){
	  if((AGS_COPY_NOTATION_AUDIO_FIT_AUDIO_SIGNAL & (copy_notation_audio->flags)) != 0){
	    audio_signal = ags_audio_signal_new_with_length((GObject *) copy_notation_audio->devout,
							    (GObject *) recycling,
							    (GObject *) AGS_RECALL(copy_notation_audio_run)->recall_id,
							    length);
	  }else{
	    audio_signal = ags_audio_signal_new((GObject *) copy_notation_audio->devout,
						(GObject *) recycling,
						(GObject *) AGS_RECALL(copy_notation_audio_run)->recall_id);
	  }

	  ags_audio_signal_connect(audio_signal);
	
	  ags_recycling_add_audio_signal(recycling,
					 audio_signal);

	  recycling = recycling->next;
	}
      }

      note_list = note_list->next;
    }

    copy_notation_audio_run->current_note = note_list;
  }
}

AgsCopyNotationAudioRun*
ags_copy_notation_audio_run_new(AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCopyNotationAudioRun *copy_notation_audio_run;

  copy_notation_audio_run = (AgsCopyNotationAudioRun *) g_object_new(AGS_TYPE_COPY_NOTATION_AUDIO_RUN,
								     "count_beats_audio_run\0", count_beats_audio_run,
								     NULL);

  return(copy_notation_audio_run);
}
