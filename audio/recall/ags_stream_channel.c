#include "ags_stream_channel.h"

#include "../ags_recycling.h"
#include "../ags_recall_id.h"

#include "ags_stream_recycling.h"

GType ags_stream_channel_get_type();
void ags_stream_channel_class_init(AgsStreamChannelClass *stream_channel);
void ags_stream_channel_init(AgsStreamChannel *stream_channel);
void ags_stream_channel_finalize(GObject *gobject);

void ags_stream_channel_run_init_pre(AgsRecall *recall, gpointer data);
void ags_stream_channel_run_init_inter(AgsRecall *recall, gpointer data);

void ags_stream_channel_done(AgsRecall *recall, gpointer data);
void ags_stream_channel_cancel(AgsRecall *recall, gpointer data);
void ags_stream_channel_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_stream_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_stream_channel_map_stream_recycling(AgsStreamChannel *stream_channel);
void ags_stream_channel_remap_stream_recycling(AgsStreamChannel *stream_channel,
					       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					       AgsRecycling *new_start_region, AgsRecycling *new_end_region);

void ags_stream_channel_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsStreamChannel *stream_channel);

static gpointer ags_stream_channel_parent_class = NULL;

GType
ags_stream_channel_get_type()
{
  static GType ags_type_stream_channel = 0;

  if(!ags_type_stream_channel){
    static const GTypeInfo ags_stream_channel_info = {
      sizeof (AgsStreamChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_channel_init,
    };
    ags_type_stream_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsStreamChannel\0",
						     &ags_stream_channel_info,
						     0);
  }
  return (ags_type_stream_channel);
}

void
ags_stream_channel_class_init(AgsStreamChannelClass *stream_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_channel_parent_class = g_type_class_peek_parent(stream_channel);

  gobject = (GObjectClass *) stream_channel;

  gobject->finalize = ags_stream_channel_finalize;

  recall = (AgsRecallClass *) stream_channel;

  recall->duplicate = ags_stream_channel_duplicate;
}

void
ags_stream_channel_init(AgsStreamChannel *stream_channel)
{
  stream_channel->channel = NULL;
}

void
ags_stream_channel_finalize(GObject *gobject)
{
  AgsStreamChannel *stream_channel;
}

void
ags_stream_channel_connect(AgsStreamChannel *stream_channel)
{
  ags_recall_connect(AGS_RECALL(stream_channel));

  g_signal_connect((GObject *) stream_channel, "run_init_pre\0",
		   G_CALLBACK(ags_stream_channel_run_init_pre), NULL);

  g_signal_connect((GObject *) stream_channel, "run_init_inter\0",
		   G_CALLBACK(ags_stream_channel_run_init_inter), NULL);

  g_signal_connect((GObject *) stream_channel, "done\0",
		   G_CALLBACK(ags_stream_channel_done), NULL);

  g_signal_connect((GObject *) stream_channel, "cancel\0",
		   G_CALLBACK(ags_stream_channel_cancel), NULL);

  g_signal_connect((GObject *) stream_channel, "remove\0",
		   G_CALLBACK(ags_stream_channel_remove), NULL);
}

void
ags_stream_channel_connect_run_handler(AgsStreamChannel *stream_channel)
{
  stream_channel->channel_recycling_changed_handler =
    g_signal_connect(G_OBJECT(stream_channel->channel), "recycling_changed\0",
		     G_CALLBACK(ags_stream_channel_recycling_changed_callback), stream_channel);
}

void
ags_stream_channel_disconnect_run_handler(AgsStreamChannel *stream_channel)
{
  g_signal_handler_disconnect(G_OBJECT(stream_channel), stream_channel->channel_recycling_changed_handler);
}

void
ags_stream_channel_run_init_pre(AgsRecall *recall, gpointer data)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel * ) recall;

  ags_stream_channel_map_stream_recycling(stream_channel);
}

void
ags_stream_channel_run_init_inter(AgsRecall *recall, gpointer data)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel * ) recall;

  ags_stream_channel_connect_run_handler(stream_channel);
}

void
ags_stream_channel_done(AgsRecall *recall, gpointer data)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel * ) recall;

  ags_stream_channel_disconnect_run_handler(stream_channel);
}

void
ags_stream_channel_cancel(AgsRecall *recall, gpointer data)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel * ) recall;

  ags_stream_channel_disconnect_run_handler(stream_channel);
}

void
ags_stream_channel_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_stream_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsStreamChannel *stream_channel, *copy;

  stream_channel = (AgsStreamChannel *) recall;  
  copy = (AgsStreamChannel *) AGS_RECALL_CLASS(ags_stream_channel_parent_class)->duplicate(recall, recall_id);

  copy->channel = stream_channel->channel;

  return((AgsRecall *) copy);
}

void
ags_stream_channel_map_stream_recycling(AgsStreamChannel *stream_channel)
{
  AgsRecycling *recycling;
  AgsStreamRecycling *stream_recycling;

  recycling = stream_channel->channel->first_recycling;

  if(recycling == NULL)
    return;

  while(recycling != stream_channel->channel->last_recycling->next){
    stream_recycling = ags_stream_recycling_new(recycling);

    stream_recycling->recall.parent = (GObject *) stream_channel;

    ags_stream_recycling_connect(stream_recycling);

    stream_channel->recall.child = g_list_prepend(stream_channel->recall.child, stream_recycling);
    
    recycling = recycling->next;
  }
}

void
ags_stream_channel_remap_stream_recycling(AgsStreamChannel *stream_channel,
					  AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					  AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *recycling;
  AgsStreamRecycling *stream_recycling;
  GList *list;

  /* remove old */
  if(old_start_region !=  NULL){
    recycling = old_start_region;

    while(recycling != old_end_region->next){
      list = stream_channel->recall.child;
      
      while(list != NULL){
	if(AGS_STREAM_RECYCLING(list->data)->recycling == recycling)
	  AGS_RECALL(list->data)->flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;

	list = list->next;
      }

      recycling = recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    recycling = new_start_region;

    while(recycling != new_end_region->next){
      stream_recycling = ags_stream_recycling_new(recycling);
      
      stream_recycling->recall.parent = (GObject *) stream_channel;

      ags_stream_recycling_connect(stream_recycling);

      stream_channel->recall.child = g_list_prepend(stream_channel->recall.child, stream_recycling);

      if((AGS_RECALL_RUN_INITIALIZED & (stream_channel->recall.flags)) != 0){
	ags_recall_run_init_pre((AgsRecall *) stream_recycling);
	ags_recall_run_init_inter((AgsRecall *) stream_recycling);
	ags_recall_run_init_post((AgsRecall *) stream_recycling);
	
	stream_recycling->recall.flags |= AGS_RECALL_RUN_INITIALIZED;
      }
      
      recycling = recycling->next;
    }
  }
}

void
ags_stream_channel_recycling_changed_callback(AgsChannel *channel,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
					      AgsStreamChannel *stream_channel)
{
  ags_stream_channel_remap_stream_recycling(stream_channel,
					    old_start_region, old_end_region,
					    new_start_region, new_end_region);
}

AgsStreamChannel*
ags_stream_channel_new(AgsChannel *channel)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL, NULL);

  stream_channel->channel = channel;

  return(stream_channel);
}
