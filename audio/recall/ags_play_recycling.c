#include "ags_play_recycling.h"

#include "../ags_audio.h"
#include "../ags_audio_signal.h"
#include "../ags_recycling.h"
#include "../ags_recall_id.h"

#include "ags_play_audio_signal.h"

#include <stdlib.h>
#include <stdio.h>

GType ags_play_recycling_get_type();
void ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling);
void ags_play_recycling_init(AgsPlayRecycling *play_recycling);
void ags_play_recycling_finalize(GObject *gobject);

void ags_play_recycling_run_init_inter(AgsRecall *recall, gpointer data);

void ags_play_recycling_run_inter(AgsRecall *recall, gpointer data);

void ags_play_recycling_done(AgsRecall *recall, gpointer data);
void ags_play_recycling_cancel(AgsRecall *recall, gpointer data);
void ags_play_recycling_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_play_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_play_recycling_source_add_audio_signal(AgsPlayRecycling *play_recycling,
						AgsAudioSignal *audio_signal);

void ags_play_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsPlayRecycling *play_recycling);
void ags_play_recycling_source_add_audio_signal_with_length_callback(AgsRecycling *source,
								     AgsAudioSignal *audio_signal, guint length,
								     AgsPlayRecycling *play_recycling);
void ags_play_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							    AgsAudioSignal *audio_signal,
							    AgsPlayRecycling *play_recycling);

void ags_play_recycling_play_audio_signal_done(AgsRecall *recall,
					       gpointer data);

static gpointer ags_play_recycling_parent_class = NULL;

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

    ags_type_play_recycling = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsPlayRecycling\0",
						     &ags_play_recycling_info,
						     0);
  }

  return(ags_type_play_recycling);
}

void
ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling)
{
  GObjectClass *gobject;

  ags_play_recycling_parent_class = g_type_class_peek_parent(play_recycling);

  gobject = (GObjectClass *) play_recycling;
  gobject->finalize = ags_play_recycling_finalize;
}

void
ags_play_recycling_init(AgsPlayRecycling *play_recycling)
{
  play_recycling->devout = NULL;

  play_recycling->source = NULL;

  play_recycling->audio_channel = 0;
}

void
ags_play_recycling_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_play_recycling_parent_class)->finalize(gobject);
}

void
ags_play_recycling_connect(AgsPlayRecycling *play_recycling)
{
  //  ags_recall_connect(AGS_RECALL(play_recycling));

  g_signal_connect((GObject *) play_recycling, "run_init_inter\0",
		   G_CALLBACK(ags_play_recycling_run_init_inter), NULL);

  g_signal_connect((GObject *) play_recycling, "run_inter\0",
		   G_CALLBACK(ags_play_recycling_run_inter), NULL);

  g_signal_connect((GObject *) play_recycling, "done\0",
		   G_CALLBACK(ags_play_recycling_done), NULL);

  g_signal_connect((GObject *) play_recycling, "remove\0",
		   G_CALLBACK(ags_play_recycling_remove), NULL);

  g_signal_connect((GObject *) play_recycling, "cancel\0",
		   G_CALLBACK(ags_play_recycling_cancel), NULL);
}

void
ags_play_recycling_connect_run_handler(AgsPlayRecycling *play_recycling)
{
  GObject *gobject;  

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  play_recycling->source_add_audio_signal_handler =
    g_signal_connect(gobject, "add_audio_signal\0",
		     G_CALLBACK(ags_play_recycling_source_add_audio_signal_callback), play_recycling);

  play_recycling->source_add_audio_signal_with_length_handler =
    g_signal_connect(gobject, "add_audio_signal_with_length\0",
		     G_CALLBACK(ags_play_recycling_source_add_audio_signal_with_length_callback), play_recycling);

  play_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_play_recycling_source_remove_audio_signal_callback), play_recycling);
}

void
ags_play_recycling_disconnect_run_handler(AgsPlayRecycling *play_recycling)
{
  GObject *gobject;

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_with_length_handler);

  g_signal_handler_disconnect(gobject, play_recycling->source_remove_audio_signal_handler);
}

void
ags_play_recycling_run_init_inter(AgsRecall *recall, gpointer data)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(recall);
  
  ags_play_recycling_connect_run_handler(play_recycling);
}

void
ags_play_recycling_run_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void 
ags_play_recycling_done(AgsRecall *recall, gpointer data)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(recall);

  ags_play_recycling_disconnect_run_handler(play_recycling);
}

void
ags_play_recycling_cancel(AgsRecall *recall, gpointer data)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(recall);

  ags_play_recycling_disconnect_run_handler(play_recycling);
}

void 
ags_play_recycling_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_play_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsPlayRecycling *play_recycling, *copy;

  play_recycling = (AgsPlayRecycling *) recall;
  copy = (AgsPlayRecycling *) AGS_RECALL_CLASS(ags_play_recycling_parent_class)->duplicate(recall, recall_id);

  copy->devout = play_recycling->devout;
  copy->source = play_recycling->source;
  copy->audio_channel = play_recycling->audio_channel;

  return((AgsRecall *) copy);
}

void
ags_play_recycling_source_add_audio_signal(AgsPlayRecycling *play_recycling,
					   AgsAudioSignal *audio_signal)
{
  AgsPlayAudioSignal *play_audio_signal;  

  audio_signal->stream_current = audio_signal->stream_beginning;

  play_audio_signal = ags_play_audio_signal_new(audio_signal, play_recycling->audio_channel,
						play_recycling->devout);

  play_audio_signal->recall.parent = (GObject *) play_recycling;

  ags_play_audio_signal_connect(play_audio_signal);
  g_signal_connect((GObject *) play_audio_signal, "done\0",
		   G_CALLBACK(ags_play_recycling_play_audio_signal_done), NULL);

  play_recycling->recall.child = g_list_prepend(play_recycling->recall.child, play_audio_signal);

  if((AGS_RECALL_RUN_INITIALIZED & (play_recycling->recall.flags)) != 0){
    ags_recall_run_init_pre((AgsRecall *) play_audio_signal);
    ags_recall_run_init_inter((AgsRecall *) play_audio_signal);
    ags_recall_run_init_post((AgsRecall *) play_audio_signal);

    play_audio_signal->recall.flags |= AGS_RECALL_RUN_INITIALIZED;
  }
}

void
ags_play_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						    AgsAudioSignal *audio_signal,
						    AgsPlayRecycling *play_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(play_recycling)->recall_id->group_id)
    ags_play_recycling_source_add_audio_signal(play_recycling,
					       audio_signal);
}

void
ags_play_recycling_source_add_audio_signal_with_length_callback(AgsRecycling *source,
								AgsAudioSignal *audio_signal, guint length,
								AgsPlayRecycling *play_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(play_recycling)->recall_id->group_id)
    ags_play_recycling_source_add_audio_signal(play_recycling, 
					       audio_signal);
}

void
ags_play_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
						       AgsAudioSignal *audio_signal,
						       AgsPlayRecycling *play_recycling)
{
  AgsRecall *play_recycling_recall;
  AgsPlayAudioSignal *play_audio_signal;
  GList *list;

  play_recycling_recall = AGS_RECALL(play_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == play_recycling_recall->recall_id->group_id){
    list = play_recycling_recall->child;

    while(list != NULL){
      play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(list->data);

      if(play_audio_signal->source == audio_signal && (AGS_RECALL_DONE & (AGS_RECALL(play_audio_signal)->flags)) == 0){
	play_audio_signal->recall.flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;
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
  fprintf(stdout, "ags_play_recycling_play_audio_signal_done\n\0");
  recall->flags |= AGS_RECALL_REMOVE;
}

AgsPlayRecycling*
ags_play_recycling_new(AgsRecycling *source, guint audio_channel,
		       AgsDevout *devout)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = (AgsPlayRecycling *) g_object_new(AGS_TYPE_PLAY_RECYCLING, NULL);

  play_recycling->source = source;

  play_recycling->devout = devout;

  return(play_recycling);
}
