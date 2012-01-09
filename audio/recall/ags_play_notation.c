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

#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_play_note.h>

void ags_play_notation_class_init(AgsPlayNotationClass *play_notation);
void ags_play_notation_init(AgsPlayNotation *play_notation);
void ags_play_notation_finalize(GObject *gobject);
void ags_play_notation_connect(AgsPlayNotation *play_notation);

void ags_copy_pattern_channel_run_resolve_dependencies(AgsRecall *recall);
void ags_play_notation_pre(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_post(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

void ags_play_notation_play_note_done(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

void ags_play_notation_delay_tic_count(AgsDelayAudioRun *delay, guint nth_run);

static gpointer ags_play_notation_parent_class = NULL;

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

    ags_type_play_notation = g_type_register_static(AGS_TYPE_RECALL,
						    "AgsPlayNotation\0",
						    &ags_play_notation_info,
						    0);
  }

  return (ags_type_play_notation);
}

void
ags_play_notation_class_init(AgsPlayNotationClass *play_notation)
{
  GObjectClass *gobject;
  AgsRecall *recall;

  ags_play_notation_parent_class = g_type_class_peek_parent(play_notation);

  /* GObjectClass */
  gobject = (GObjectClass *) play_notation;

  gobject->finalize = ags_play_notation_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_notation;

  recall->resolve_dependencies = ags_copy_pattern_channel_run_resolve_dependencies;
  recall->done = ags_copy_pattern_channel_run_done;
  recall->cancel = ags_copy_pattern_channel_run_cancel;
  recall->remove = ags_copy_pattern_channel_run_remove;
}

void
ags_play_notation_init(AgsPlayNotation *play_notation)
{
  play_notation->devout = NULL;
  
  play_notation->channel = NULL;
  play_notation->notation = NULL;

  //  play_notation->bpm = NULL;

  play_notation->current = NULL;
}

void
ags_play_notation_finalize(GObject *gobject)
{
  /*
  AgsPlayNotation *play_notation;
  GList *list, *list_next;

  play_notation = (AgsPlayNotation *) gobject;

  list = play_notation->recall.child;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }
  */

  G_OBJECT_CLASS(ags_play_notation_parent_class)->finalize(gobject);
}

void
ags_play_notation_connect(AgsPlayNotation *play_notation)
{
  //  ags_recall_connect((AgsRecall *) play_notation);

  /*
  g_signal_connect((GObject *) play_notation, "run_pre\0",
		   G_CALLBACK(ags_play_notation_pre), NULL);

  g_signal_connect((GObject *) play_notation, "run_post\0",
		   G_CALLBACK(ags_play_notation_post), NULL);
  */

  g_signal_connect((GObject *) play_notation, "stop\0",
		   G_CALLBACK(ags_play_notation_stop), NULL);

  g_signal_connect((GObject *) play_notation, "cancel\0",
		   G_CALLBACK(ags_play_notation_cancel), NULL);
}

void
ags_play_notation_run_connect(AgsPlayNotation *play_notation)
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = ags;

  g_signal_connect_after(G_OBJECT(delay_audio_run), "tic_count\0",
			 G_CALLBACK(ags_play_notation_delay_tic_count), play_notation);
}

void
ags_copy_pattern_channel_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsPlayNotation *play_notation;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  play_notation = AGS_PLAY_NOTATION(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_audio_run)->data);

  list = template->dependencies;
  group_id = recall->recall_id->group_id;

  delay_audio_run = NULL;
  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay_audio_run\0", delay_audio_run,
	       NULL);
}

void
ags_play_notation_pre(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /* -- deprecated -- use relative counters instead
  if((AGS_DEVOUT_PLAY_NOTE & play_notation->devout->flags) == 0)
    return;

  if(play_notation->current == NULL){
    current = play_notation->notation->note;

    if(AGS_NOTE(current->data)->x[0] == play_notation->devout->note_offset){
      goto ags_play_notation_pre0;
    }else{
      return;
    }
  }else{
    current = play_notation->current;
  }

  while((next = current->next) != NULL && AGS_NOTE(next->data)->x[0] == play_notation->devout->note_offset){
    current = next;
  ags_play_notation_pre0:

    play_note = ags_play_note_new();

    play_note->recall.parent = (GObject *) play_notation;
    play_note->devout = notation->devout;
    ags_play_note_connect(play_note);
    g_signal_connect((GObject *) play_note, "done\0",
		     G_CALLBACK(ags_play_notation_play_note_done), NULL);

    play_note->note = (AgsNote *) current->data;
    ags_play_note_map_play_audio_signal(play_note);

    recall->recall = g_list_append(recall->recall, play_note);
  }

  play_notation->current = current;
  */
}

void
ags_play_notation_post(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /*
  GList *list, *list_next;

  list = recall->recall;

  while(list != NULL){
    list_next = list->next;

    ags_recall_run_inter(AGS_RECALL(list->data), recall_id);

    list = list_next;
  }
  */
}

void
ags_play_notation_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /*
  GList *list, *list_next;

  list = recall->recall;
  recall->recall = NULL;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }
  */
}

void
ags_play_notation_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

void
ags_play_notation_play_note_done(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /*
  AgsPlayNotation *play_notation;

  play_notation = AGS_PLAY_NOTATION(recall->parent);

  play_notation->recall.recall = g_list_remove(play_notation->recall.recall, recall);
  g_object_unref(G_OBJECT(recall));
  */
}

void
ags_play_notation_delay_tic_count(AgsDelayAudioRun *delay, guint nth_run)
{
  AgsNotation *notation;
  AgsPlayNotation *play_notation;
  AgsAudio *audio;
  AgsChannel *selected_channel;
  GList *current_position;
  AgsNote *note;
  AgsRecycling *recycling;

  play_notation = (AgsPlayNotation *) recall;
  notation = play_notation->notation;

  audio = AGS_AUDIO(play_notation->notation->audio);
  
  if((AGS_PLAY_NOTATION_DEFAULT & notation->flags) != 0){
    selected_channel = audio->input;
  }else{
    selected_channel = audio->output;
  }

  current_position = notation->start_loop;
  note = AGS_NOTE(current_position->data);

  if(current_position != notation->end_loop &&
     note->x[0] == notation->offset){
    selected_channel = ags_channel_nth(selected_channel, note->y);

    recycling = selected_channel->first_recycling;

    while(recycling != selected_channel->last_recycling){
      ags_recycling_add_audio_signal_with_frame_count(recycling,
						      ags_audio_signal_get_template(recycling->audio_signal),
						      note->x[1] - note->x[0]);
    }
  }
}

AgsPlayNotation*
ags_play_notation_new()
{
  AgsPlayNotation *play_notation;

  play_notation = (AgsPlayNotation *) g_object_new(AGS_TYPE_PLAY_NOTATION, NULL);

  return(play_notation);
}
