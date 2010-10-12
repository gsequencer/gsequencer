#include "ags_stream_recycling.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include "ags_stream_channel.h"
#include "ags_stream_audio_signal.h"

void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_connect(AgsConnectable *connectable);
void ags_stream_recycling_disconnect(AgsConnectable *connectable);
void ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable);
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
void ags_stream_recycling_add_audio_signal_with_frame_count_callback(AgsRecycling *recycling,
								     AgsAudioSignal *audio_signal, guint frame_count,
								     AgsStreamRecycling *stream_recycling);

void ags_stream_recycling_stream_audio_signal_done(AgsRecall *recall,
						   gpointer data);

static gpointer ags_stream_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_stream_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_recycling_parent_run_connectable_interface;

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsStreamRecycling\0",
						       &ags_stream_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
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
ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_recycling_connect;
  connectable->disconnect = ags_stream_recycling_disconnect;
}

void
ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_recycling_run_connect;
  run_connectable->disconnect = ags_stream_recycling_run_disconnect;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  stream_recycling->recycling = NULL;
}

void
ags_stream_recycling_connect(AgsConnectable *connectable)
{
  AgsStreamRecycling *stream_recycling;

  ags_stream_recycling_parent_connectable_interface->connect(connectable);

  stream_recycling = AGS_STREAM_RECYCLING(connectable);

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
ags_stream_recycling_disconnect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->disconnect(connectable);
}

void
ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsStreamRecycling *stream_recycling;
  GObject *gobject;

  ags_stream_recycling_parent_run_connectable_interface->connect(run_connectable);

  stream_recycling = AGS_STREAM_RECYCLING(run_connectable);
  gobject = G_OBJECT(stream_recycling);

  stream_recycling->add_audio_signal_handler =
    g_signal_connect(gobject, "add_audio_signal\0",
		     G_CALLBACK(ags_stream_recycling_add_audio_signal_callback), stream_recycling);

  stream_recycling->add_audio_signal_with_frame_count_handler =
    g_signal_connect(gobject, "add_audio_signal_with_frame_count\0",
		     G_CALLBACK(ags_stream_recycling_add_audio_signal_with_frame_count_callback), stream_recycling);
}

void
ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsStreamRecycling *stream_recycling;
  GObject *gobject;

  ags_stream_recycling_parent_run_connectable_interface->disconnect(run_connectable);

  stream_recycling = AGS_STREAM_RECYCLING(run_connectable);
  gobject = G_OBJECT(stream_recycling);

  g_signal_handler_disconnect(gobject, stream_recycling->add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, stream_recycling->add_audio_signal_with_frame_count_handler);
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  /* empty */
}

void
ags_stream_recycling_run_init_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void 
ags_stream_recycling_done(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_stream_recycling_cancel(AgsRecall *recall, gpointer data)
{
  /* empty */
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
  guint audio_channel;

  stream_audio_signal = ags_stream_audio_signal_new(audio_signal);

  AGS_RECALL(stream_audio_signal)->flags |= AGS_RECALL_PROPAGATE_DONE;

  audio_channel = AGS_STREAM_CHANNEL(AGS_RECALL(stream_recycling)->parent)->channel->audio_channel;

  ags_recall_add_child(AGS_RECALL(stream_recycling), AGS_RECALL(stream_audio_signal), audio_channel);
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
ags_stream_recycling_add_audio_signal_with_frame_count_callback(AgsRecycling *recycling,
								AgsAudioSignal *audio_signal, guint frame_count,
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
