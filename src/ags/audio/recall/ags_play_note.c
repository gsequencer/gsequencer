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

#include <ags/audio/recall/ags_play_note.h>

#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

void ags_play_note_class_init(AgsPlayNoteClass *play_note);
void ags_play_note_init(AgsPlayNote *play_note);
void ags_play_note_finalize(GObject *gobject);
void ags_play_note_connect(AgsPlayNote *play_note);

void ags_play_note_map_play_audio_signal(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_note(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_note_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_note_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

void ags_play_note_play_audio_signal_done(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

static gpointer ags_play_note_parent_class = NULL;

GType
ags_play_note_get_type()
{
  static GType ags_type_play_note = 0;

  if(!ags_type_play_note){
    static const GTypeInfo ags_play_note_info = {
      sizeof (AgsPlayNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_note_init,
    };
    ags_type_play_note = g_type_register_static(AGS_TYPE_RECALL, "AgsPlayNote\0", &ags_play_note_info, 0);
  }
  return (ags_type_play_note);
}

void
ags_play_note_class_init(AgsPlayNoteClass *play_note)
{
  GObjectClass *gobject;

  ags_play_note_parent_class = g_type_class_peek_parent(play_note);

  gobject = (GObjectClass *) play_note;
  gobject->finalize = ags_play_note_finalize;
}

void
ags_play_note_init(AgsPlayNote *play_note)
{
  play_note->soundcard = NULL;

  play_note->note = NULL;
  play_note->channel = NULL;
}

void
ags_play_note_finalize(GObject *gobject)
{
  /*
  AgsPlayNote *play_note;
  GList *list, *list_next;

  play_note = AGS_PLAY_NOTE(gobject);

  list = play_note->recall.recall;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }
  */
  G_OBJECT_CLASS(ags_play_note_parent_class)->finalize(gobject);
}

void
ags_play_note_connect(AgsPlayNote *play_note)
{
  //  ags_recall_connect(AGS_RECALL(play_note));

  g_signal_connect((GObject *) play_note, "run_inter\0",
		   G_CALLBACK(ags_play_note), NULL);

  g_signal_connect((GObject *) play_note, "stop\0",
		   G_CALLBACK(ags_play_note_stop), NULL);

  g_signal_connect((GObject *) play_note, "cancel\0",
		   G_CALLBACK(ags_play_note_cancel), NULL);
}

void
ags_play_note_map_play_audio_signal(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;
  AgsPlayNote *play_note;
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsPlayAudioSignal *play_audio_signal;
  GList *start_play_audio_signal, *list_play_audio_signal;
  gboolean first_recall;
  /*
  void ags_play_note_map_play_audio_signal_real(){
    audio_signal->flags |= AGS_AUDIO_SIGNAL_PLAY;
    audio_signal->audio_channel = channel->audio_channel;

    play_audio_signal = ags_play_audio_signal_new();
    play_audio_signal->soundcard = play_note->soundcard;
    play_audio_signal->audio_signal = audio_signal;
    AGS_RECALL(play_audio_signal)->parent = (GObject *) play_note;

    ags_play_audio_signal_connect(play_audio_signal);
    g_signal_connect((GObject *) play_audio_signal, "done\0",
		     G_CALLBACK(ags_play_note_play_audio_signal_done), NULL);

    if(first_recall){
      start_play_audio_signal =
	list_play_audio_signal = g_list_alloc();
      
      first_recall = FALSE;
    }else{
      list_play_audio_signal->next = g_list_alloc();
      list_play_audio_signal->next->prev = list_play_audio_signal;
      list_play_audio_signal = list_play_audio_signal->next;
    }
    
    list_play_audio_signal->data = (gpointer) play_audio_signal;
  }

  play_notation_audio_run = (AgsPlayNotationAudioRun *) recall->parent;
  play_note = (AgsPlayNote *) recall;

  channel = ags_channel_nth_pad(play_notation_audio_run->channel, play_note->note->y);

  if(channel != NULL){
    recycling = channel->first_recycling;

    first_recall = TRUE;
    start_play_audio_signal = NULL;

    if((AGS_NOTE_DEFAULT_LENGTH & play_note->note->flags) != 0){
      while(recycling != channel->last_recycling->next){
	audio_signal = ags_recycling_add_audio_signal(recycling);
	ags_play_note_map_play_audio_signal_real();

	recycling = recycling->next;
      }
    }else{
      guint length;
      
      length = ((double) (play_note->note->x[1] - play_note->note->x[0]) * ((double) play_note->soundcard->samplerate * 60.0 / play_notation_audio_run->bpm[0] / 16.0));

      while(recycling != channel->last_recycling->next){
	audio_signal = ags_recycling_add_audio_signal_with_length(recycling, length);
	ags_play_note_map_play_audio_signal_real();

	recycling = recycling->next;
      }
    }

    play_note->recall.recall = g_list_concat(play_note->recall.recall, start_play_audio_signal);
  }else{
    g_signal_emit_by_name((GObject *) recall, "done\0", recall_id);
  }
*/
}

void
ags_play_note(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /*
  GList *list, *list_next;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_static_mutex_lock(&mutex);

  if(recall->recall == NULL){
    g_signal_emit_by_name((GObject *) recall, "done\0", recall_id);
    g_static_mutex_unlock(&mutex);
  }else{
    g_static_mutex_unlock(&mutex);

    list = recall->recall;

    while(list != NULL){
      list_next = list->next;

      g_signal_emit_by_name(G_OBJECT(list->data), "run_inter\0", recall_id);

      list = list_next;
    }
  }
  */
}

void
ags_play_note_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
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
ags_play_note_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

void
ags_play_note_play_audio_signal_done(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /*
  AgsPlayNote *play_note;
  AgsPlayAudioSignal *play_audio_signal;

  play_note = AGS_PLAY_NOTE(recall->parent);
  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);

  AGS_RECALL(play_note)->recall = g_list_remove(AGS_RECALL(play_note)->recall, recall);
  g_object_unref(G_OBJECT(play_audio_signal));

  if(play_note->recall.recall == NULL){
    g_signal_emit_by_name((GObject *) play_note, "done\0", recall_id);
  }
  */
}

AgsPlayNote*
ags_play_note_new()
{
  AgsPlayNote *play_note;

  play_note = (AgsPlayNote *) g_object_new(AGS_TYPE_PLAY_NOTE, NULL);

  return(play_note);
}
