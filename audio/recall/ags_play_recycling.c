#include <ags/audio/recall/ags_play_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling);
void ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_recycling_init(AgsPlayRecycling *play_recycling);
void ags_play_recycling_connect(AgsConnectable *connectable);
void ags_play_recycling_disconnect(AgsConnectable *connectable);
void ags_play_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_play_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_play_recycling_finalize(GObject *gobject);

void ags_play_recycling_done(AgsRecall *recall, gpointer data);
void ags_play_recycling_cancel(AgsRecall *recall, gpointer data);
void ags_play_recycling_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_play_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_play_recycling_source_add_audio_signal(AgsPlayRecycling *play_recycling,
						AgsAudioSignal *audio_signal);

void ags_play_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsPlayRecycling *play_recycling);
void ags_play_recycling_source_add_audio_signal_with_frame_count_callback(AgsRecycling *source,
								     AgsAudioSignal *audio_signal, guint frame_count,
								     AgsPlayRecycling *play_recycling);
void ags_play_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							    AgsAudioSignal *audio_signal,
							    AgsPlayRecycling *play_recycling);

void ags_play_recycling_play_audio_signal_done(AgsRecall *recall,
					       gpointer data);

static gpointer ags_play_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_play_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_recycling_parent_run_connectable_interface;

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_recycling = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsPlayRecycling\0",
						     &ags_play_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
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
ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_recycling_connect;
  connectable->disconnect = ags_play_recycling_disconnect;
}

void
ags_play_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_recycling_run_connect;
  run_connectable->disconnect = ags_play_recycling_run_disconnect;
}

void
ags_play_recycling_init(AgsPlayRecycling *play_recycling)
{
  play_recycling->devout = NULL;

  play_recycling->source = NULL;

  play_recycling->audio_channel = 0;
}

void
ags_play_recycling_connect(AgsConnectable *connectable)
{
  AgsPlayRecycling *play_recycling;

  ags_play_recycling_parent_connectable_interface->connect(connectable);

  /* AgsPlayRecycling */
  play_recycling = AGS_PLAY_RECYCLING(connectable);

  g_signal_connect((GObject *) play_recycling, "done\0",
		   G_CALLBACK(ags_play_recycling_done), NULL);

  g_signal_connect((GObject *) play_recycling, "remove\0",
		   G_CALLBACK(ags_play_recycling_remove), NULL);

  g_signal_connect((GObject *) play_recycling, "cancel\0",
		   G_CALLBACK(ags_play_recycling_cancel), NULL);
}

void
ags_play_recycling_disconnect(AgsConnectable *connectable)
{
  ags_play_recycling_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsPlayRecycling *play_recycling;
  GObject *gobject;

  ags_play_recycling_parent_run_connectable_interface->connect(run_connectable);

  /* AgsPlayRecycling */
  play_recycling = AGS_PLAY_RECYCLING(run_connectable);

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  play_recycling->source_add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_play_recycling_source_add_audio_signal_callback), play_recycling);

  play_recycling->source_add_audio_signal_with_frame_count_handler =
    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
			   G_CALLBACK(ags_play_recycling_source_add_audio_signal_with_frame_count_callback), play_recycling);

  play_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal\0",
		     G_CALLBACK(ags_play_recycling_source_remove_audio_signal_callback), play_recycling);
}

void
ags_play_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsPlayRecycling *play_recycling;
  GObject *gobject;

  ags_play_recycling_parent_run_connectable_interface->disconnect(run_connectable);

  /* AgsPlayRecycling */
  play_recycling = AGS_PLAY_RECYCLING(run_connectable);

  /* source */
  gobject = G_OBJECT(play_recycling->source);

  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, play_recycling->source_add_audio_signal_with_frame_count_handler);

  g_signal_handler_disconnect(gobject, play_recycling->source_remove_audio_signal_handler);
}

void
ags_play_recycling_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_play_recycling_parent_class)->finalize(gobject);
}

void 
ags_play_recycling_done(AgsRecall *recall, gpointer data)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(recall);

  //  ags_play_recycling_disconnect_run_handler(play_recycling);
}

void
ags_play_recycling_cancel(AgsRecall *recall, gpointer data)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(recall);

  //  ags_play_recycling_disconnect_run_handler(play_recycling);
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
  guint audio_channel;

  audio_signal->stream_current = audio_signal->stream_beginning;

  play_audio_signal = ags_play_audio_signal_new(audio_signal, play_recycling->audio_channel,
						play_recycling->devout);

  audio_channel = AGS_PLAY_CHANNEL(AGS_RECALL(play_recycling)->parent)->source->audio_channel;
  ags_recall_add_child(AGS_RECALL(play_recycling), AGS_RECALL(play_audio_signal), audio_channel);

  g_signal_connect((GObject *) play_audio_signal, "done\0",
		   G_CALLBACK(ags_play_recycling_play_audio_signal_done), NULL);

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
ags_play_recycling_source_add_audio_signal_with_frame_count_callback(AgsRecycling *source,
								AgsAudioSignal *audio_signal, guint frame_count,
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
  recall->flags |= AGS_RECALL_REMOVE | AGS_RECALL_HIDE;
}

AgsPlayRecycling*
ags_play_recycling_new(AgsRecycling *source, guint audio_channel,
		       AgsDevout *devout)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = (AgsPlayRecycling *) g_object_new(AGS_TYPE_PLAY_RECYCLING, NULL);

  play_recycling->source = source;
  play_recycling->audio_channel = audio_channel;
  play_recycling->devout = devout;

  return(play_recycling);
}
