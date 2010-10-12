#include "ags_copy_pattern_channel_run.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_audio.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include "ags_copy_pattern_audio.h"
#include "ags_copy_pattern_audio_run.h"
#include "ags_copy_pattern_channel.h"

#include <stdlib.h>

void ags_copy_pattern_channel_run_class_init(AgsCopyPatternChannelRunClass *copy_pattern_channel_run);
void ags_copy_pattern_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_pattern_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_pattern_channel_run_init(AgsCopyPatternChannelRun *copy_pattern_channel_run);
void ags_copy_pattern_channel_run_connect(AgsConnectable *connectable);
void ags_copy_pattern_channel_run_disconnect(AgsConnectable *connectable);
void ags_copy_pattern_channel_run_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_channel_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_channel_run_finalize(GObject *gobject);

void ags_copy_pattern_channel_run_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data);

void ags_copy_pattern_channel_run_done(AgsRecall *recall, gpointer data);
void ags_copy_pattern_channel_run_cancel(AgsRecall *recall, gpointer data);
void ags_copy_pattern_channel_run_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_copy_pattern_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_pattern_channel_run_tic_callback(AgsDelayAudioRun *delay_audio_run, guint audio_channel,
					       AgsCopyPatternChannelRun *copy_pattern_channel_run);

static gpointer ags_copy_pattern_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_copy_pattern_channel_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_pattern_channel_run_parent_run_connectable_interface;

GType
ags_copy_pattern_channel_run_get_type()
{
  static GType ags_type_copy_pattern_channel_run = 0;

  if(!ags_type_copy_pattern_channel_run){
    static const GTypeInfo ags_copy_pattern_channel_run_info = {
      sizeof (AgsCopyPatternChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_pattern_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							       "AgsCopyPatternChannelRun\0",
							       &ags_copy_pattern_channel_run_info,
							       0);
    
    g_type_add_interface_static(ags_type_copy_pattern_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_copy_pattern_channel_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_pattern_channel_run);
}

void
ags_copy_pattern_channel_run_class_init(AgsCopyPatternChannelRunClass *copy_pattern_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_pattern_channel_run_parent_class = g_type_class_peek_parent(copy_pattern_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_channel_run;

  gobject->finalize = ags_copy_pattern_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_pattern_channel_run;

  recall->duplicate = ags_copy_pattern_channel_run_duplicate;
}

void
ags_copy_pattern_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_copy_pattern_channel_run_connectable_parent_interface;

  ags_copy_pattern_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_pattern_channel_run_connect;
  connectable->disconnect = ags_copy_pattern_channel_run_disconnect;
}

void
ags_copy_pattern_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_pattern_channel_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_pattern_channel_run_run_connect;
  run_connectable->disconnect = ags_copy_pattern_channel_run_run_disconnect;
}

void
ags_copy_pattern_channel_run_init(AgsCopyPatternChannelRun *copy_pattern_channel_run)
{
  /* empty */
}

void
ags_copy_pattern_channel_run_connect(AgsConnectable *connectable)
{
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  ags_copy_pattern_channel_run_parent_connectable_interface->connect(connectable);

  /* AgsCopyPatternChannelRun */
  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(connectable);

  g_signal_connect((GObject *) copy_pattern_channel_run, "run_init_pre\0",
		   G_CALLBACK(ags_copy_pattern_channel_run_run_init_pre), NULL);

  g_signal_connect((GObject *) copy_pattern_channel_run, "remove\0",
		   G_CALLBACK(ags_copy_pattern_channel_run_remove), NULL);

  g_signal_connect((GObject *) copy_pattern_channel_run, "cancel\0",
		   G_CALLBACK(ags_copy_pattern_channel_run_cancel), NULL);
}

void
ags_copy_pattern_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_copy_pattern_channel_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_pattern_channel_run_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  ags_copy_pattern_channel_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyPatternChannelRun */
  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(run_connectable);
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL(copy_pattern_channel_run)->recall_audio_run);

  if((AGS_COPY_PATTERN_AUDIO_RUN_RUN_CONNECTED & (copy_pattern_audio_run->flags)) == 0){
    ags_run_connectable_connect(AGS_RUN_CONNECTABLE(copy_pattern_audio_run));
  }

  copy_pattern_channel_run->tic_handler =
    g_signal_connect(G_OBJECT(copy_pattern_audio_run->delay_audio_run), "tic\0",
		     G_CALLBACK(ags_copy_pattern_channel_run_tic_callback), copy_pattern_channel_run);
}

void
ags_copy_pattern_channel_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  ags_copy_pattern_channel_run_parent_run_connectable_interface->disconnect(run_connectable);

  /* AgsCopyPatternChannelRun */
  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(run_connectable);
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL(copy_pattern_channel_run)->recall_audio_run);

  g_signal_handler_disconnect(G_OBJECT(copy_pattern_audio_run->delay_audio_run), copy_pattern_channel_run->tic_handler);
}

void
ags_copy_pattern_channel_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_channel_run_parent_class)->finalize(gobject);
}

void
ags_copy_pattern_channel_run_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannel *copy_pattern_channel;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(recall);

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall->recall_audio_run);
  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(recall->recall_channel);

  if(copy_pattern_audio_run->delay_audio_run == NULL){
    AgsAudio *audio;
    AgsRecallID *parent_recall_id;
    GList *delay_list;

    audio = AGS_AUDIO(copy_pattern_channel->source->audio);

    parent_recall_id = ags_recall_id_find_group_id(copy_pattern_channel->destination->recall_id,
						   AGS_RECALL(copy_pattern_channel_run)->recall_id->parent_group_id);

    if(parent_recall_id->parent_group_id == 0)
      delay_list = ags_recall_find_type_with_group_id(audio->play, AGS_TYPE_DELAY_AUDIO_RUN, parent_recall_id->group_id);
    else
      delay_list = ags_recall_find_type_with_group_id(audio->recall, AGS_TYPE_DELAY_AUDIO_RUN, parent_recall_id->group_id);
    
    copy_pattern_audio_run->delay_audio_run = ((delay_list != NULL) ? AGS_DELAY_AUDIO_RUN(delay_list->data): NULL);
  }

  ags_recall_notify_dependency(AGS_RECALL(copy_pattern_audio_run->delay_audio_run),
			       AGS_RECALL_NOTIFY_CHANNEL_RUN, 1);
  copy_pattern_audio_run->recall_ref++;
  fprintf(stdout, "ags_copy_pattern_channel_run_run_init_pre - line: %u\n\0", copy_pattern_channel->source->line);
}

void
ags_copy_pattern_channel_run_done(AgsRecall *recall, gpointer data)
{
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(recall);

  //  copy_pattern_channel_run->shared_audio_run->delay->recall_ref--;
  //  copy_pattern_channel_run->shared_audio_run->recall_ref--;
}

void
ags_copy_pattern_channel_run_cancel(AgsRecall *recall, gpointer data)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(recall);
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL(copy_pattern_channel_run)->recall_audio_run);

  ags_recall_notify_dependency(AGS_RECALL(copy_pattern_audio_run->delay_audio_run),
			       AGS_RECALL_NOTIFY_CHANNEL_RUN, -1);
  //  copy_pattern_channel_run->shared_audio_run->recall_ref--;
}

void
ags_copy_pattern_channel_run_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_copy_pattern_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyPatternChannelRun *copy;

  copy = AGS_COPY_PATTERN_CHANNEL_RUN(AGS_RECALL_CLASS(ags_copy_pattern_channel_run_parent_class)->duplicate(recall, recall_id));

  /* empty */

  return((AgsRecall *) copy);
}

void
ags_copy_pattern_channel_run_tic_callback(AgsDelayAudioRun *delay_audio_run, guint audio_channel, AgsCopyPatternChannelRun *copy_pattern_channel_run)
{
  AgsChannel *output;
  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannel *copy_pattern_channel;
  //  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(AGS_RECALL(copy_pattern_channel_run)->recall_channel);

  //  pthread_mutex_lock(&mutex);
  if(copy_pattern_channel->source->audio_channel != audio_channel)
    return;

  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(AGS_RECALL(copy_pattern_channel_run)->recall_audio);
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL(copy_pattern_channel_run)->recall_audio_run);

  /*
  if(copy_pattern_audio_run->bit == copy_pattern_audio->length){
    //      pthread_mutex_unlock(&mutex);
    if(copy_pattern_audio->loop){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	recycling = copy_pattern_channel->destination->first_recycling;

	if(recycling != NULL){
	  while(recycling != copy_pattern_channel->destination->last_recycling->next){
	    audio_signal = ags_audio_signal_new((GObject *) recycling,
						(GObject *) AGS_RECALL(copy_pattern_channel_run)->recall_id);
	    audio_signal->devout = (GObject *) copy_pattern_audio->devout;
	    ags_audio_signal_connect(audio_signal);
	    
	    ags_recycling_add_audio_signal(recycling,
					   audio_signal);
	    
	    recycling = recycling->next;
	  }
	}

	//	copy_pattern->shared_audio_run->bit = 0;
	goto ags_copy_pattern_pre0;
    }else{
      if((AGS_RECALL_PERSISTENT & (AGS_RECALL(delay_audio_run)->flags)) != 0)
	AGS_RECALL(delay_audio_run)->flags &= (~AGS_RECALL_PERSISTENT);

      if(AGS_RECALL(copy_pattern_channel_run)->child == NULL){
	//	fprintf(stdout, "copy_pattern->recall.recall == NULL\n\0");
	ags_recall_done((AgsRecall *) copy_pattern_channel_run);

	ags_recall_notify_dependency(AGS_RECALL(delay_audio_run),
				     AGS_RECALL_NOTIFY_CHANNEL_RUN, -1);
	copy_pattern_audio_run->recall_ref--;
	//	copy_pattern->shared_audio_run->bit = 0;
      }
      //      pthread_mutex_unlock(&mutex);
    }
  }else{
    //    pthread_mutex_unlock(&mutex);
    ags_copy_pattern_pre0:
  */
    //    fprintf(stdout, "ags_copy_pattern - line: %u\n\0", copy_pattern->shared_channel->source->line);

  if(ags_pattern_get_bit((AgsPattern *) AGS_COPY_PATTERN_CHANNEL(AGS_RECALL(copy_pattern_channel_run)->recall_channel)->pattern,
			 copy_pattern_audio->i, copy_pattern_audio->j,
			 copy_pattern_audio_run->bit)){
    AgsRecycling *recycling;
    AgsAudioSignal *audio_signal;
    
    fprintf(stdout, "ags_copy_pattern - playing: bit = %u; line = %u\n\0", copy_pattern_audio_run->bit, copy_pattern_channel->source->line);
    
    recycling = copy_pattern_channel->source->first_recycling;
	
    if(recycling != NULL){
      while(recycling != copy_pattern_channel->source->last_recycling->next){
	audio_signal = ags_audio_signal_new((GObject *) recycling,
					    (GObject *) AGS_RECALL(copy_pattern_channel_run)->recall_id);
	audio_signal->devout = (GObject *) copy_pattern_audio->devout;
	ags_audio_signal_connect(audio_signal);
	
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);
	
	recycling = recycling->next;
      }
    }
  }
  
      //      printf("%u\n\0", copy_pattern->shared_audio_run->bit);
      //      copy_pattern->shared_audio_run->bit++;
  //  }
}

AgsCopyPatternChannelRun*
ags_copy_pattern_channel_run_new(AgsRecallAudio *recall_audio,
				 AgsRecallAudioRun *recall_audio_run,
				 AgsRecallChannel *recall_channel)
{
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
								       "recall_audio_type\0", AGS_TYPE_COPY_PATTERN_AUDIO,
								       "recall_audio\0", recall_audio,
								       "recall_audio_run_type\0", AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								       "recall_audio_run\0", recall_audio_run,
								       "recall_channel_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL,
								       "recall_channel\0", recall_channel,
								       "recall_channel_run_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
								       NULL);

  return(copy_pattern_channel_run);
}
