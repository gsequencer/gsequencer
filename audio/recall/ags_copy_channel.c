#include "ags_copy_channel.h"

#include "../ags_recycling.h"
#include "../ags_recall_id.h"

#include "ags_copy_recycling.h"

#include <stdlib.h>
#include <stdio.h>

GType ags_copy_channel_get_type();
void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_run_init_pre(AgsRecall *recall, gpointer data);
void ags_copy_channel_run_init_inter(AgsRecall *recall, gpointer data);

void ags_copy_channel_run_inter(AgsRecall *recall, gpointer data);

void ags_copy_channel_done(AgsRecall *recall, gpointer data);
void ags_copy_channel_remove(AgsRecall *recall, gpointer data);
void ags_copy_channel_cancel(AgsRecall *recall, gpointer data);

AgsRecall* ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel);
void ags_copy_channel_remap_child_destination(AgsCopyChannel *copy_channel,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_copy_channel_remap_child_source(AgsCopyChannel *copy_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region);

void ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsCopyChannel *copy_channel);

void ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							     AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							     AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							     AgsCopyChannel *copy_channel);

static gpointer ags_copy_channel_parent_class = NULL;

GType
ags_copy_channel_get_type()
{
  static GType ags_type_copy_channel = 0;

  if(!ags_type_copy_channel){
    static const GTypeInfo ags_copy_channel_info = {
      sizeof (AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };

    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsCopyChannel\0",
						   &ags_copy_channel_info,
						   0);
  }

  return(ags_type_copy_channel);
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  gobject = (GObjectClass *) copy_channel;

  gobject->finalize = ags_copy_channel_finalize;

  recall = (AgsRecallClass *) copy_channel;

  recall->duplicate = ags_copy_channel_duplicate;
}

void
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  copy_channel->devout = NULL;

  copy_channel->destination = NULL;

  copy_channel->source = NULL;
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_connect(AgsCopyChannel *copy_channel)
{
  //  ags_recall_connect(AGS_RECALL(copy_channel));

  g_signal_connect((GObject *) copy_channel, "run_init_pre\0",
		   G_CALLBACK(ags_copy_channel_run_init_pre), NULL);

  g_signal_connect((GObject *) copy_channel, "run_init_inter\0",
		   G_CALLBACK(ags_copy_channel_run_init_inter), NULL);

  g_signal_connect((GObject *) copy_channel, "run_inter\0",
		   G_CALLBACK(ags_copy_channel_run_inter), NULL);

  g_signal_connect((GObject *) copy_channel, "done\0",
		   G_CALLBACK(ags_copy_channel_done), NULL);

  g_signal_connect((GObject *) copy_channel, "remove\0",
		   G_CALLBACK(ags_copy_channel_remove), NULL);

  g_signal_connect((GObject *) copy_channel, "cancel\0",
		   G_CALLBACK(ags_copy_channel_cancel), NULL);
}

void
ags_copy_channel_connect_run_handler(AgsCopyChannel *copy_channel)
{
  GObject *gobject;

  /* destination */
  gobject = (GObject *) copy_channel->destination;

  copy_channel->destination_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_copy_channel_destination_recycling_changed_callback), copy_channel);

  /* source */
  gobject = (GObject *) copy_channel->source;

  copy_channel->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_copy_channel_source_recycling_changed_callback), copy_channel);
}

void
ags_copy_channel_disconnect_run_handler(AgsCopyChannel *copy_channel)
{
  GObject *gobject;

  /* destination */
  gobject = G_OBJECT(copy_channel->destination);
  
  g_signal_handler_disconnect(gobject, copy_channel->destination_recycling_changed_handler);

  /* source */
  gobject = G_OBJECT(copy_channel->source);

  g_signal_handler_disconnect(gobject, copy_channel->source_recycling_changed_handler);
}

void
ags_copy_channel_run_init_pre(AgsRecall *recall, gpointer data)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) recall;

  ags_copy_channel_map_copy_recycling(copy_channel);
}

void
ags_copy_channel_run_init_inter(AgsRecall *recall, gpointer data)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) recall;

  ags_copy_channel_connect_run_handler(copy_channel);
}

void
ags_copy_channel_run_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_copy_channel_done(AgsRecall *recall, gpointer data)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(recall);

  ags_copy_channel_disconnect_run_handler(copy_channel);
}

void
ags_copy_channel_cancel(AgsRecall *recall, gpointer data)
{
  ags_copy_channel_disconnect_run_handler((AgsCopyChannel *) recall);
}

void 
ags_copy_channel_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyChannel *copy_channel, *copy;

  copy_channel = (AgsCopyChannel *) recall;
  copy = (AgsCopyChannel *) AGS_RECALL_CLASS(ags_copy_channel_parent_class)->duplicate(recall, recall_id);

  copy->devout = copy_channel->devout;
  copy->destination = copy_channel->destination;
  copy->source = copy_channel->source;

  return((AgsRecall *) copy);
}

void
ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;

  destination_recycling = copy_channel->destination->first_recycling;

  if(destination_recycling == NULL || copy_channel->source->first_recycling == NULL)
    return;

  while(destination_recycling != copy_channel->destination->last_recycling->next){
    source_recycling = copy_channel->source->first_recycling;

    while(source_recycling != copy_channel->source->last_recycling->next){
      copy_recycling = ags_copy_recycling_new(destination_recycling,
					      source_recycling,
					      copy_channel->devout);

      copy_recycling->recall.parent = (GObject *) copy_channel;

      ags_copy_recycling_connect(copy_recycling);

      copy_channel->recall.child = g_list_prepend(copy_channel->recall.child, copy_recycling);

      source_recycling = source_recycling->next;
    }

    destination_recycling = destination_recycling->next;
  }
}

void
ags_copy_channel_remap_child_destination(AgsCopyChannel *copy_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;
  GList *list;

  /* remove old */
  if(old_start_region != NULL){
    destination_recycling = old_start_region;

    while(destination_recycling != old_end_region->next){
      list = copy_channel->recall.child;

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->destination == destination_recycling)
	  AGS_RECALL(list->data)->flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;

	list = list->next;
      }

      destination_recycling = destination_recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    if(copy_channel->source->first_recycling == NULL)
      return;

    destination_recycling = new_start_region;

    while(destination_recycling != new_end_region->next){
      source_recycling = copy_channel->source->first_recycling;

      while(source_recycling != copy_channel->source->last_recycling->next){
	copy_recycling = ags_copy_recycling_new(destination_recycling,
						source_recycling,
						copy_channel->devout);

	copy_recycling->recall.parent = (GObject *) copy_channel;

	ags_copy_recycling_connect(copy_recycling);

	copy_channel->recall.child = g_list_prepend(copy_channel->recall.child, copy_recycling);

	if((AGS_RECALL_RUN_INITIALIZED & (copy_channel->recall.flags)) != 0){
	  ags_recall_run_init_pre((AgsRecall *) copy_recycling);
	  ags_recall_run_init_inter((AgsRecall *) copy_recycling);
	  ags_recall_run_init_post((AgsRecall *) copy_recycling);
	}

	source_recycling = source_recycling->next;
      }
      
      destination_recycling = destination_recycling->next;
    }
  }
}

void
ags_copy_channel_remap_child_source(AgsCopyChannel *copy_channel,
				    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				    AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;
  GList *list;

  /* remove old */
  if(old_start_region != NULL){
    source_recycling = old_start_region;

    while(source_recycling != old_end_region->next){
      list = copy_channel->recall.child;

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->source == source_recycling)
	  AGS_RECALL(list->data)->flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;

	list = list->next;
      }

      source_recycling = source_recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    if(copy_channel->destination->first_recycling == NULL)
      return;

    destination_recycling = copy_channel->destination->first_recycling;

    while(destination_recycling != copy_channel->destination->last_recycling->next){
      source_recycling = new_start_region;

      while(source_recycling != new_end_region->next){
	copy_recycling = ags_copy_recycling_new(destination_recycling,
						source_recycling,
						copy_channel->devout);

	copy_recycling->recall.parent = (GObject *) copy_channel;

	ags_copy_recycling_connect(copy_recycling);

	copy_channel->recall.child = g_list_prepend(copy_channel->recall.child, copy_recycling);

	if((AGS_RECALL_RUN_INITIALIZED & (copy_channel->recall.flags)) != 0){
	  ags_recall_run_init_pre((AgsRecall *) copy_recycling);
	  ags_recall_run_init_inter((AgsRecall *) copy_recycling);
	  ags_recall_run_init_post((AgsRecall *) copy_recycling);

	  copy_recycling->recall.flags |= AGS_RECALL_RUN_INITIALIZED;
	}

	source_recycling = source_recycling->next;
      }
      
      destination_recycling = destination_recycling->next;
    }
  }
}

void
ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsCopyChannel *copy_channel)
{
  ags_copy_channel_remap_child_source(copy_channel,
				      old_start_region, old_end_region,
				      new_start_region, new_end_region);
}

void
ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsCopyChannel *copy_channel)
{
  ags_copy_channel_remap_child_destination(copy_channel,
					   old_start_region, old_end_region,
					   new_start_region, new_end_region);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source,
		     AgsDevout *devout)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL, NULL);

  copy_channel->destination = destination;

  copy_channel->source = source;

  copy_channel->devout = devout;

  return(copy_channel);
}
