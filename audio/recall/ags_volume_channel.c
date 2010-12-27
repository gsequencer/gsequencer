#include <ags/audio/recall/ags_volume_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_volume_recycling.h>

void ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel);
void ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_volume_channel_init(AgsVolumeChannel *volume_channel);
void ags_volume_channel_connect(AgsConnectable *connectable);
void ags_volume_channel_disconnect(AgsConnectable *connectable);
void ags_volume_channel_run_connect(AgsRunConnectable *run_connectable);
void ags_volume_channel_run_disconnect(AgsRunConnectable *run_connectable);
void ags_volume_channel_finalize(GObject *gobject);

void ags_volume_channel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_volume_channel_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_volume_channel_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

AgsRecall* ags_volume_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_volume_channel_map_volume_recycling(AgsVolumeChannel *volume_channel);
void ags_volume_channel_remap_volume_recycling(AgsVolumeChannel *volume_channel,
					       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					       AgsRecycling *new_start_region, AgsRecycling *new_end_region);

void ags_volume_channel_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsVolumeChannel *volume_channel);

static gpointer ags_volume_channel_parent_class = NULL;
static AgsConnectableInterface *ags_volume_channel_parent_connectable_interface;
static AgsRunConnectableInterface *ags_volume_channel_parent_run_connectable_interface;

GType
ags_volume_channel_get_type()
{
  static GType ags_type_volume_channel = 0;

  if(!ags_type_volume_channel){
    static const GTypeInfo ags_volume_channel_info = {
      sizeof (AgsVolumeChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsVolumeChannel\0",
						     &ags_volume_channel_info,
						     0);

    g_type_add_interface_static(ags_type_volume_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_channel,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_volume_channel);
}

void
ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_volume_channel_parent_class = g_type_class_peek_parent(volume_channel);

  gobject = (GObjectClass *) volume_channel;

  gobject->finalize = ags_volume_channel_finalize;

  recall = (AgsRecallClass *) volume_channel;

  recall->duplicate = ags_volume_channel_duplicate;
}

void
ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_channel_connect;
  connectable->disconnect = ags_volume_channel_disconnect;
}

void
ags_volume_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_volume_channel_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_volume_channel_run_connect;
  run_connectable->disconnect = ags_volume_channel_run_disconnect;
}

void
ags_volume_channel_init(AgsVolumeChannel *volume_channel)
{
  volume_channel->channel = NULL;
  volume_channel->volume = NULL;
}

void
ags_volume_channel_connect(AgsConnectable *connectable)
{
  AgsVolumeChannel *volume_channel;

  ags_volume_channel_parent_connectable_interface->connect(connectable);

  volume_channel = AGS_VOLUME_CHANNEL(connectable);
}

void
ags_volume_channel_disconnect(AgsConnectable *connectable)
{
  ags_volume_channel_parent_connectable_interface->disconnect(connectable);
}

void
ags_volume_channel_run_connect(AgsRunConnectable *run_connectable)
{
  AgsVolumeChannel *volume_channel;

  ags_volume_channel_parent_run_connectable_interface->connect(run_connectable);

  volume_channel = AGS_VOLUME_CHANNEL(run_connectable);

  volume_channel->channel_recycling_changed_handler =
    g_signal_connect(G_OBJECT(volume_channel->channel), "recycling_changed\0",
		     G_CALLBACK(ags_volume_channel_recycling_changed_callback), volume_channel);
}

void
ags_volume_channel_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsVolumeChannel *volume_channel;

  ags_volume_channel_parent_run_connectable_interface->disconnect(run_connectable);

  volume_channel = AGS_VOLUME_CHANNEL(run_connectable);

  g_signal_handler_disconnect(G_OBJECT(volume_channel), volume_channel->channel_recycling_changed_handler);
}

void
ags_volume_channel_finalize(GObject *gobject)
{
  AgsVolumeChannel *volume_channel;
}

AgsRecall*
ags_volume_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsVolumeChannel *volume_channel, *copy;

  volume_channel = (AgsVolumeChannel *) recall;  
  copy = (AgsVolumeChannel *) AGS_RECALL_CLASS(ags_volume_channel_parent_class)->duplicate(recall, recall_id);

  copy->channel = volume_channel->channel;

  copy->volume = volume_channel->volume;

  ags_volume_channel_map_volume_recycling(copy);

  return((AgsRecall *) copy);
}

void
ags_volume_channel_map_volume_recycling(AgsVolumeChannel *volume_channel)
{
  AgsRecycling *recycling;
  AgsVolumeRecycling *volume_recycling;
  guint audio_channel;
  
  recycling = volume_channel->channel->first_recycling;

  if(recycling == NULL)
    return;

  audio_channel = volume_channel->channel->audio_channel;

  while(recycling != volume_channel->channel->last_recycling->next){
    volume_recycling = ags_volume_recycling_new(recycling, volume_channel->volume);

    ags_recall_add_child(AGS_RECALL(volume_channel), AGS_RECALL(volume_recycling), audio_channel);

    recycling = recycling->next;
  }
}

void
ags_volume_channel_remap_volume_recycling(AgsVolumeChannel *volume_channel,
					  AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					  AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *recycling;
  AgsVolumeRecycling *volume_recycling;
  GList *list;
  guint audio_channel;

  audio_channel = volume_channel->channel->audio_channel;

  /* remove old */
  if(old_start_region !=  NULL){
    AgsDevout *devout;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(volume_channel->channel->audio)->devout);
    recycling = old_start_region;

    while(recycling != old_end_region->next){
      list = AGS_RECALL(volume_channel)->child;
      
      while(list != NULL){
	if(AGS_VOLUME_RECYCLING(list->data)->recycling == recycling){
	  //	  AGS_RECALL(list->data)->flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;
	  cancel_recall = ags_cancel_recall_new(AGS_RECALL(list->data), audio_channel);

	  ags_devout_append_task(devout, (AgsTask *) cancel_recall);
	}


	list = list->next;
      }

      recycling = recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    recycling = new_start_region;

    while(recycling != new_end_region->next){
      volume_recycling = ags_volume_recycling_new(recycling, volume_channel->volume);
      
      ags_recall_add_child(AGS_RECALL(volume_channel), AGS_RECALL(volume_recycling), audio_channel);
      
      recycling = recycling->next;
    }
  }
}

void
ags_volume_channel_recycling_changed_callback(AgsChannel *channel,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
					      AgsVolumeChannel *volume_channel)
{
  ags_volume_channel_remap_volume_recycling(volume_channel,
					    old_start_region, old_end_region,
					    new_start_region, new_end_region);
}

AgsVolumeChannel*
ags_volume_channel_new(AgsChannel *channel, gdouble *volume)
{
  AgsVolumeChannel *volume_channel;

  volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL, NULL);

  volume_channel->channel = channel;
  
  volume_channel->volume = volume;

  return(volume_channel);
}
