#include "ags_play_notation.h"

#include "../ags_recall_id.h"

#include "ags_play_note.h"

GType ags_play_notation_get_type();
void ags_play_notation_class_init(AgsPlayNotationClass *play_notation);
void ags_play_notation_init(AgsPlayNotation *play_notation);
void ags_play_notation_finalize(GObject *gobject);
void ags_play_notation_connect(AgsPlayNotation *play_notation);

void ags_play_notation_pre(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_post(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_notation_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

void ags_play_notation_play_note_done(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

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
    ags_type_play_notation = g_type_register_static(AGS_TYPE_RECALL, "AgsPlayNotation\0", &ags_play_notation_info, 0);
  }
  return (ags_type_play_notation);
}

void
ags_play_notation_class_init(AgsPlayNotationClass *play_notation)
{
  GObjectClass *gobject;

  ags_play_notation_parent_class = g_type_class_peek_parent(play_notation);

  gobject = (GObjectClass *) play_notation;
  gobject->finalize = ags_play_notation_finalize;
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
  ags_recall_connect((AgsRecall *) play_notation);

  g_signal_connect((GObject *) play_notation, "run_pre\0",
		   G_CALLBACK(ags_play_notation_pre), NULL);

  g_signal_connect((GObject *) play_notation, "run_post\0",
		   G_CALLBACK(ags_play_notation_post), NULL);

  g_signal_connect((GObject *) play_notation, "stop\0",
		   G_CALLBACK(ags_play_notation_stop), NULL);

  g_signal_connect((GObject *) play_notation, "cancel\0",
		   G_CALLBACK(ags_play_notation_cancel), NULL);
}

void
ags_play_notation_pre(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsPlayNotation *play_notation;
  AgsPlayNote *play_note;
  GList *current, *next;

  play_notation = (AgsPlayNotation *) recall;

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

AgsPlayNotation*
ags_play_notation_new()
{
  AgsPlayNotation *play_notation;

  play_notation = (AgsPlayNotation *) g_object_new(AGS_TYPE_PLAY_NOTATION, NULL);

  return(play_notation);
}
