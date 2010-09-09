#include "ags_copy_recycling.h"

#include "../ags_recall_id.h"

#include "ags_copy_audio_signal.h"

#include <stdlib.h>
#include <stdio.h>

GType ags_copy_recycling_get_type();
void ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling);
void ags_copy_recycling_init(AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_finalize(GObject *gobject);

void ags_copy_recycling_run_init_inter(AgsRecall *recall, gpointer data);

void ags_copy_recycling_run_inter(AgsRecall *recall, gpointer data);

void ags_copy_recycling_done(AgsRecall *recall, gpointer data);
void ags_copy_recycling_cancel(AgsRecall *recall, gpointer data);
void ags_copy_recycling_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_copy_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_recycling_source_add_audio_signal(AgsCopyRecycling *copy_recycling,
						AgsAudioSignal *audio_signal);
void ags_copy_recycling_destination_add_audio_signal(AgsCopyRecycling *copy_recycling,
						     AgsAudioSignal *audio_signal);

void ags_copy_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_source_add_audio_signal_with_length_callback(AgsRecycling *source,
								     AgsAudioSignal *audio_signal, guint length,
								     AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							    AgsAudioSignal *audio_signal,
							    AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_add_audio_signal_callback(AgsRecycling *destination,
							      AgsAudioSignal *audio_signal,
							      AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_add_audio_signal_with_length_callback(AgsRecycling *destination,
									  AgsAudioSignal *audio_signal, guint length,
									  AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
								 AgsAudioSignal *audio_signal,
								 AgsCopyRecycling *copy_recycling);

void ags_copy_recycling_copy_audio_signal_done(AgsRecall *recall,
					       gpointer data);

static gpointer ags_copy_recycling_parent_class = NULL;

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

    ags_type_copy_recycling = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsCopyRecycling\0",
						     &ags_copy_recycling_info,
						     0);
  }

  return(ags_type_copy_recycling);
}

void
ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_recycling_parent_class = g_type_class_peek_parent(copy_recycling);

  gobject = (GObjectClass *) copy_recycling;

  gobject->finalize = ags_copy_recycling_finalize;

  recall = (AgsRecallClass *) copy_recycling;

  recall->duplicate = ags_copy_recycling_duplicate;
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
ags_copy_recycling_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_recycling_parent_class)->finalize(gobject);
}

void
ags_copy_recycling_connect(AgsCopyRecycling *copy_recycling)
{
  ags_recall_connect(AGS_RECALL(copy_recycling));

  g_signal_connect((GObject *) copy_recycling, "run_init_inter\0",
		   G_CALLBACK(ags_copy_recycling_run_init_inter), NULL);

  g_signal_connect((GObject *) copy_recycling, "run_inter\0",
		   G_CALLBACK(ags_copy_recycling_run_inter), NULL);

  g_signal_connect((GObject *) copy_recycling, "done\0",
		   G_CALLBACK(ags_copy_recycling_done), NULL);

  g_signal_connect((GObject *) copy_recycling, "remove\0",
		   G_CALLBACK(ags_copy_recycling_remove), NULL);

  g_signal_connect((GObject *) copy_recycling, "cancel\0",
		   G_CALLBACK(ags_copy_recycling_cancel), NULL);
}

void
ags_copy_recycling_connect_run_handler(AgsCopyRecycling *copy_recycling)
{
  GObject *gobject;  

  /* destination */
  gobject = G_OBJECT(copy_recycling->destination);

  copy_recycling->destination_add_audio_signal_handler =
    g_signal_connect(gobject, "add_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_callback), copy_recycling);
    
  copy_recycling->destination_add_audio_signal_with_length_handler =
    g_signal_connect(gobject, "add_audio_signal_with_length\0",
		     G_CALLBACK(ags_copy_recycling_destination_add_audio_signal_with_length_callback), copy_recycling);
  
  copy_recycling->destination_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_destination_remove_audio_signal_callback), copy_recycling);

  /* source */
  gobject = G_OBJECT(copy_recycling->source);

  copy_recycling->source_add_audio_signal_handler =
    g_signal_connect(gobject, "add_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_source_add_audio_signal_callback), copy_recycling);

  copy_recycling->source_add_audio_signal_with_length_handler =
    g_signal_connect(gobject, "add_audio_signal_with_length\0",
		     G_CALLBACK(ags_copy_recycling_source_add_audio_signal_with_length_callback), copy_recycling);

  copy_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_copy_recycling_source_remove_audio_signal_callback), copy_recycling);
}

void
ags_copy_recycling_disconnect_run_handler(AgsCopyRecycling *copy_recycling)
{
  GObject *gobject;

  /* destination */
  gobject = G_OBJECT(copy_recycling->destination);

  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, copy_recycling->destination_add_audio_signal_with_length_handler);

  g_signal_handler_disconnect(gobject, copy_recycling->destination_remove_audio_signal_handler);

  /* source */
  gobject = G_OBJECT(copy_recycling->source);

  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, copy_recycling->source_add_audio_signal_with_length_handler);

  g_signal_handler_disconnect(gobject, copy_recycling->source_remove_audio_signal_handler);
}

void
ags_copy_recycling_run_init_inter(AgsRecall *recall, gpointer data)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = AGS_COPY_RECYCLING(recall);
  
  ags_copy_recycling_connect_run_handler(copy_recycling);
}

void
ags_copy_recycling_run_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void 
ags_copy_recycling_done(AgsRecall *recall, gpointer data)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = AGS_COPY_RECYCLING(recall);

  ags_copy_recycling_disconnect_run_handler(copy_recycling);
}

void
ags_copy_recycling_cancel(AgsRecall *recall, gpointer data)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = AGS_COPY_RECYCLING(recall);

  ags_copy_recycling_disconnect_run_handler(copy_recycling);
}

void 
ags_copy_recycling_remove(AgsRecall *recall, gpointer data)
{
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

  audio_signal->stream_current = audio_signal->stream_beginning;

  copy_audio_signal = ags_copy_audio_signal_new(copy_recycling->child_destination,
						audio_signal,
						copy_recycling->devout);

  copy_audio_signal->recall.parent = (GObject *) copy_recycling;

  ags_copy_audio_signal_connect(copy_audio_signal);
  g_signal_connect((GObject *) copy_audio_signal, "done\0",
		   G_CALLBACK(ags_copy_recycling_copy_audio_signal_done), NULL);

  copy_recycling->recall.child = g_list_prepend(copy_recycling->recall.child, copy_audio_signal);

  if((AGS_RECALL_RUN_INITIALIZED & (copy_recycling->recall.flags)) != 0){
    ags_recall_run_init_pre((AgsRecall *) copy_audio_signal);
    ags_recall_run_init_inter((AgsRecall *) copy_audio_signal);
    ags_recall_run_init_post((AgsRecall *) copy_audio_signal);

    copy_audio_signal->recall.flags |= AGS_RECALL_RUN_INITIALIZED;
  }
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
ags_copy_recycling_source_add_audio_signal_with_length_callback(AgsRecycling *source,
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
  AgsRecall *copy_recycling_recall;
  AgsCopyAudioSignal *copy_audio_signal;
  GList *list;

  copy_recycling_recall = AGS_RECALL(copy_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == copy_recycling_recall->recall_id->group_id){
    list = copy_recycling_recall->child;

    while(list != NULL){
      copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(list->data);

      if(copy_audio_signal->source == audio_signal && (AGS_RECALL_DONE & (AGS_RECALL(copy_audio_signal)->flags)) == 0){
	copy_audio_signal->recall.flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;
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
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->group_id)
    ags_copy_recycling_destination_add_audio_signal(copy_recycling,
						    audio_signal);
}

void
ags_copy_recycling_destination_add_audio_signal_with_length_callback(AgsRecycling *destination,
								     AgsAudioSignal *audio_signal, guint length,
								     AgsCopyRecycling *copy_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(copy_recycling)->recall_id->group_id)
    ags_copy_recycling_destination_add_audio_signal(copy_recycling,
						    audio_signal);
}

void
ags_copy_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
							    AgsAudioSignal *audio_signal,
							    AgsCopyRecycling *copy_recycling)
{
  AgsRecall *copy_recycling_recall;
  AgsCopyAudioSignal *copy_audio_signal;
  GList *list;

  copy_recycling_recall = AGS_RECALL(copy_recycling);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == copy_recycling_recall->recall_id->group_id){
    list = copy_recycling_recall->child;

    while(list != NULL){
      copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(list->data);

      if(copy_audio_signal->destination == audio_signal && (AGS_RECALL_DONE & (AGS_RECALL(copy_audio_signal)->flags)) == 0){
	copy_audio_signal->recall.flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;
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
  recall->flags |= AGS_RECALL_REMOVE;
}

AgsCopyRecycling*
ags_copy_recycling_new(AgsRecycling *destination,
		       AgsRecycling *source,
		       AgsDevout *devout)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = (AgsCopyRecycling *) g_object_new(AGS_TYPE_COPY_RECYCLING, NULL);

  copy_recycling->destination = destination;

  copy_recycling->source = source;

  copy_recycling->devout = devout;

  return(copy_recycling);
}
