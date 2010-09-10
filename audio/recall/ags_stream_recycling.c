#include "ags_stream_recycling.h"

#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include "ags_stream_audio_signal.h"

GType ags_stream_recycling_get_type();
void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_finalize(GObject *gobject);

void ags_stream_recycling_run_init_inter(AgsRecall *recall, gpointer data);

void ags_stream_recycling_done(AgsRecall *recall, gpointer data);
void ags_stream_recycling_cancel(AgsRecall *recall, gpointer data);
void ags_stream_recycling_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_stream_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_stream_recycling_add_audio_signal(AgsStreamRecycling *stream_recycling,
					   AgsRecycling *recycling,
					   AgsAudioSignal *audio_signal);

void ags_stream_recycling_add_audio_signal_callback(AgsRecycling *recycling,
						    AgsAudioSignal *audio_signal,
						    AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_add_audio_signal_with_length_callback(AgsRecycling *recycling,
								AgsAudioSignal *audio_signal, guint length,
								AgsStreamRecycling *stream_recycling);

void ags_stream_recycling_stream_audio_signal_done(AgsRecall *recall,
						   gpointer data);

static gpointer ags_stream_recycling_parent_class = NULL;

GType
ags_stream_recycling_get_type()
{
  static GType ags_type_stream_recycling = 0;

  if(!ags_type_stream_recycling){
    static const GTypeInfo ags_stream_recycling_info = {
      sizeof (AgsStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_recycling_init,
    };
    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsStreamRecycling\0",
						       &ags_stream_recycling_info,
						       0);
  }
  return (ags_type_stream_recycling);
}

void
ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_recycling_parent_class = g_type_class_peek_parent(stream_recycling);

  gobject = (GObjectClass *) stream_recycling;

  gobject->finalize = ags_stream_recycling_finalize;

  recall = (AgsRecallClass *) stream_recycling;

  recall->duplicate = ags_stream_recycling_duplicate;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  stream_recycling->recycling = NULL;
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  /* empty */
}

void
ags_stream_recycling_connect(AgsStreamRecycling *stream_recycling)
{
  //  ags_recall_connect(AGS_RECALL(stream_recycling));

  g_signal_connect((GObject *) stream_recycling, "run_init_inter\0",
		   G_CALLBACK(ags_stream_recycling_run_init_inter), NULL);

  g_signal_connect((GObject *) stream_recycling, "done\0",
		   G_CALLBACK(ags_stream_recycling_done), NULL);

  g_signal_connect((GObject *) stream_recycling, "cancel\0",
		   G_CALLBACK(ags_stream_recycling_cancel), NULL);

  g_signal_connect((GObject *) stream_recycling, "remove\0",
		   G_CALLBACK(ags_stream_recycling_remove), NULL);
}

void
ags_stream_recycling_connect_run_handler(AgsStreamRecycling *stream_recycling)
{
  GObject *gobject;

  gobject = G_OBJECT(stream_recycling);

  stream_recycling->add_audio_signal_handler =
    g_signal_connect(gobject, "add_audio_signal\0",
		     G_CALLBACK(ags_stream_recycling_add_audio_signal_callback), stream_recycling);

  stream_recycling->add_audio_signal_with_length_handler =
    g_signal_connect(gobject, "add_audio_signal_with_length\0",
		     G_CALLBACK(ags_stream_recycling_add_audio_signal_with_length_callback), stream_recycling);
}

void
ags_stream_recycling_disconnect_run_handler(AgsStreamRecycling *stream_recycling)
{
  GObject *gobject;

  gobject = G_OBJECT(stream_recycling);

  g_signal_handler_disconnect(gobject, stream_recycling->add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, stream_recycling->add_audio_signal_with_length_handler);
}

void
ags_stream_recycling_run_init_inter(AgsRecall *recall, gpointer data)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(recall);

  ags_stream_recycling_connect_run_handler(stream_recycling);
}

void 
ags_stream_recycling_done(AgsRecall *recall, gpointer data)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(recall);

  ags_stream_recycling_disconnect_run_handler(stream_recycling);
}

void
ags_stream_recycling_cancel(AgsRecall *recall, gpointer data)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(recall);

  ags_stream_recycling_disconnect_run_handler(stream_recycling);
}

void
ags_stream_recycling_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_stream_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsStreamRecycling *stream_recycling, *copy;

  stream_recycling = (AgsStreamRecycling *) recall;
  copy = (AgsStreamRecycling *) AGS_RECALL_CLASS(ags_stream_recycling_parent_class)->duplicate(recall, recall_id);

  copy->recycling = stream_recycling->recycling;

  return((AgsRecall *) copy);
}

void
ags_stream_recycling_add_audio_signal(AgsStreamRecycling *stream_recycling,
				      AgsRecycling *recycling,
				      AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = ags_stream_audio_signal_new(audio_signal);

  stream_audio_signal->recall.flags |= AGS_RECALL_PROPAGATE_DONE;
  stream_audio_signal->recall.parent = (GObject *) stream_recycling;

  ags_stream_audio_signal_connect(stream_audio_signal);
  g_signal_connect((GObject *) stream_audio_signal, "done\0",
		   G_CALLBACK(ags_stream_recycling_stream_audio_signal_done), NULL);

  stream_recycling->recall.child = g_list_prepend(stream_recycling->recall.child, stream_audio_signal);
}

void
ags_stream_recycling_add_audio_signal_callback(AgsRecycling *recycling,
					       AgsAudioSignal *audio_signal,
					       AgsStreamRecycling *stream_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(stream_recycling)->recall_id->group_id)
    ags_stream_recycling_add_audio_signal(stream_recycling,
					  recycling,
					  audio_signal);
}

void
ags_stream_recycling_add_audio_signal_with_length_callback(AgsRecycling *recycling,
							   AgsAudioSignal *audio_signal, guint length,
							   AgsStreamRecycling *stream_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(stream_recycling)->recall_id->group_id)
    ags_stream_recycling_add_audio_signal(stream_recycling,
					  recycling,
					  audio_signal);
}

void
ags_stream_recycling_stream_audio_signal_done(AgsRecall *recall,
					      gpointer data)
{
  fprintf(stdout, "ags_stream_recycling_stream_audio_signal_done\n\0");
  recall->flags |= AGS_RECALL_REMOVE;
}

AgsStreamRecycling*
ags_stream_recycling_new(AgsRecycling *recycling)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = (AgsStreamRecycling *) g_object_new(AGS_TYPE_STREAM_RECYCLING, NULL);

  stream_recycling->recycling = recycling;

  return(stream_recycling);
}
