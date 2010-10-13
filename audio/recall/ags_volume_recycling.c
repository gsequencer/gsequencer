#include "ags_volume_recycling.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include "ags_volume_channel.h"
#include "ags_volume_audio_signal.h"

void ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling);
void ags_volume_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling);
void ags_volume_recycling_connect(AgsConnectable *connectable);
void ags_volume_recycling_disconnect(AgsConnectable *connectable);
void ags_volume_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_volume_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_volume_recycling_finalize(GObject *gobject);

void ags_volume_recycling_run_init_inter(AgsRecall *recall, gpointer data);

void ags_volume_recycling_done(AgsRecall *recall, gpointer data);
void ags_volume_recycling_cancel(AgsRecall *recall, gpointer data);
void ags_volume_recycling_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_volume_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_volume_recycling_add_audio_signal(AgsVolumeRecycling *volume_recycling,
					   AgsRecycling *recycling,
					   AgsAudioSignal *audio_signal);

void ags_volume_recycling_add_audio_signal_callback(AgsRecycling *recycling,
						    AgsAudioSignal *audio_signal,
						    AgsVolumeRecycling *volume_recycling);
void ags_volume_recycling_add_audio_signal_with_frame_count_callback(AgsRecycling *recycling,
								     AgsAudioSignal *audio_signal, guint frame_count,
								     AgsVolumeRecycling *volume_recycling);

void ags_volume_recycling_volume_audio_signal_done(AgsRecall *recall,
						   gpointer data);

static gpointer ags_volume_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_volume_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_volume_recycling_parent_run_connectable_interface;

GType
ags_volume_recycling_get_type()
{
  static GType ags_type_volume_recycling = 0;

  if(!ags_type_volume_recycling){
    static const GTypeInfo ags_volume_recycling_info = {
      sizeof (AgsVolumeRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsVolumeRecycling\0",
						       &ags_volume_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_volume_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_volume_recycling);
}

void
ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_volume_recycling_parent_class = g_type_class_peek_parent(volume_recycling);

  gobject = (GObjectClass *) volume_recycling;

  gobject->finalize = ags_volume_recycling_finalize;

  recall = (AgsRecallClass *) volume_recycling;

  recall->duplicate = ags_volume_recycling_duplicate;
}

void
ags_volume_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_recycling_connect;
  connectable->disconnect = ags_volume_recycling_disconnect;
}

void
ags_volume_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_volume_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_volume_recycling_run_connect;
  run_connectable->disconnect = ags_volume_recycling_run_disconnect;
}

void
ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling)
{
  volume_recycling->recycling = NULL;
}

void
ags_volume_recycling_connect(AgsConnectable *connectable)
{
  AgsVolumeRecycling *volume_recycling;

  ags_volume_recycling_parent_connectable_interface->connect(connectable);

  volume_recycling = AGS_VOLUME_RECYCLING(connectable);

  g_signal_connect((GObject *) volume_recycling, "run_init_inter\0",
		   G_CALLBACK(ags_volume_recycling_run_init_inter), NULL);

  g_signal_connect((GObject *) volume_recycling, "done\0",
		   G_CALLBACK(ags_volume_recycling_done), NULL);

  g_signal_connect((GObject *) volume_recycling, "cancel\0",
		   G_CALLBACK(ags_volume_recycling_cancel), NULL);

  g_signal_connect((GObject *) volume_recycling, "remove\0",
		   G_CALLBACK(ags_volume_recycling_remove), NULL);
}

void
ags_volume_recycling_disconnect(AgsConnectable *connectable)
{
  ags_volume_recycling_parent_connectable_interface->disconnect(connectable);
}

void
ags_volume_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsVolumeRecycling *volume_recycling;
  GObject *gobject;

  ags_volume_recycling_parent_run_connectable_interface->connect(run_connectable);

  volume_recycling = AGS_VOLUME_RECYCLING(run_connectable);
  gobject = G_OBJECT(volume_recycling->recycling);

  volume_recycling->add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal\0",
			   G_CALLBACK(ags_volume_recycling_add_audio_signal_callback), volume_recycling);

  volume_recycling->add_audio_signal_with_frame_count_handler =
    g_signal_connect_after(gobject, "add_audio_signal_with_frame_count\0",
			   G_CALLBACK(ags_volume_recycling_add_audio_signal_with_frame_count_callback), volume_recycling);
}

void
ags_volume_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsVolumeRecycling *volume_recycling;
  GObject *gobject;

  ags_volume_recycling_parent_run_connectable_interface->disconnect(run_connectable);

  volume_recycling = AGS_VOLUME_RECYCLING(run_connectable);
  gobject = G_OBJECT(volume_recycling);

  g_signal_handler_disconnect(gobject, volume_recycling->add_audio_signal_handler);
  g_signal_handler_disconnect(gobject, volume_recycling->add_audio_signal_with_frame_count_handler);
}

void
ags_volume_recycling_finalize(GObject *gobject)
{
  /* empty */
}

void
ags_volume_recycling_run_init_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void 
ags_volume_recycling_done(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_volume_recycling_cancel(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_volume_recycling_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_volume_recycling_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsVolumeRecycling *volume_recycling, *copy;

  volume_recycling = (AgsVolumeRecycling *) recall;
  copy = (AgsVolumeRecycling *) AGS_RECALL_CLASS(ags_volume_recycling_parent_class)->duplicate(recall, recall_id);

  copy->recycling = volume_recycling->recycling;

  copy->volume = volume_recycling->volume;

  return((AgsRecall *) copy);
}

void
ags_volume_recycling_add_audio_signal(AgsVolumeRecycling *volume_recycling,
				      AgsRecycling *recycling,
				      AgsAudioSignal *audio_signal)
{
  AgsVolumeAudioSignal *volume_audio_signal;
  guint audio_channel;

  volume_audio_signal = ags_volume_audio_signal_new(audio_signal, volume_recycling->volume);

  AGS_RECALL(volume_audio_signal)->flags |= AGS_RECALL_PROPAGATE_DONE;

  audio_channel = AGS_VOLUME_CHANNEL(AGS_RECALL(volume_recycling)->parent)->channel->audio_channel;

  ags_recall_add_child(AGS_RECALL(volume_recycling), AGS_RECALL(volume_audio_signal), audio_channel);
}

void
ags_volume_recycling_add_audio_signal_callback(AgsRecycling *recycling,
					       AgsAudioSignal *audio_signal,
					       AgsVolumeRecycling *volume_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(volume_recycling)->recall_id->group_id)
    ags_volume_recycling_add_audio_signal(volume_recycling,
					  recycling,
					  audio_signal);
}

void
ags_volume_recycling_add_audio_signal_with_frame_count_callback(AgsRecycling *recycling,
								AgsAudioSignal *audio_signal, guint frame_count,
								AgsVolumeRecycling *volume_recycling)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) == 0 &&
     audio_signal->recall_id != NULL &&
     AGS_RECALL_ID(audio_signal->recall_id)->group_id == AGS_RECALL(volume_recycling)->recall_id->group_id)
    ags_volume_recycling_add_audio_signal(volume_recycling,
					  recycling,
					  audio_signal);
}

void
ags_volume_recycling_volume_audio_signal_done(AgsRecall *recall,
					      gpointer data)
{
  fprintf(stdout, "ags_volume_recycling_volume_audio_signal_done\n\0");
  recall->flags |= AGS_RECALL_REMOVE;
}

AgsVolumeRecycling*
ags_volume_recycling_new(AgsRecycling *recycling, gdouble *volume)
{
  AgsVolumeRecycling *volume_recycling;

  volume_recycling = (AgsVolumeRecycling *) g_object_new(AGS_TYPE_VOLUME_RECYCLING, NULL);

  volume_recycling->recycling = recycling;

  volume_recycling->volume = volume;

  return(volume_recycling);
}

