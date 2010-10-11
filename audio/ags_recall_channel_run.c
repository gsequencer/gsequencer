#include "ags_recall_channel_run.h"

#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"

#include "ags_audio.h"
#include "ags_channel.h"
#include "ags_input.h"

#include "ags_recall_audio.h"
#include "ags_recall_audio_run.h"
#include "ags_recall_channel.h"

GType ags_recall_channel_run_get_type();
void ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run);
void ags_recall_channel_runconnectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_connect(AgsConnectable *connectable);
void ags_recall_channel_run_disconnect(AgsConnectable *connectable);
void ags_recall_channel_run_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_channel_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_channel_run_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_recall_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_recall_channel_run_parent_run_connectable_interface;

GType
ags_recall_channel_run_get_type()
{
  static GType ags_type_recall_channel_run = 0;

  if(!ags_type_recall_channel_run){
    static const GTypeInfo ags_recall_channel_run_info = {
      sizeof (AgsRecallChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_runconnectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel_run = g_type_register_static(AGS_TYPE_RECALL,
							 "AgsRecallChannelRun\0",
							 &ags_recall_channel_run_info,
							 0);
    
    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall_channel_run);
}

void
ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_recall_channel_run_parent_class = g_type_class_peek_parent(recall_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel_run;

  gobject->finalize = ags_recall_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel_run;

  recall->duplicate = ags_recall_channel_run_duplicate;
}

void
ags_recall_channel_runconnectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_channel_run_connectable_parent_interface;

  ags_recall_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_channel_run_connect;
  connectable->disconnect = ags_recall_channel_run_disconnect;
}

void
ags_recall_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_channel_run_run_connect;
  run_connectable->disconnect = ags_recall_channel_run_run_disconnect;
}

void
ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run)
{
  /* empty */
}

void
ags_recall_channel_run_connect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_channel_run_run_connect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_run_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_channel_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecallChannelRun *recall_channel_run, *copy;
  GList *list_start, *list;
  guint group_id;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);
  copy = AGS_RECALL_CHANNEL_RUN(AGS_RECALL_CLASS(ags_recall_channel_run_parent_class)->duplicate(recall, recall_id));

  printf("duplicate recall_channel_run\n\0");
  channel = AGS_RECALL_CHANNEL(recall->recall_channel)->channel;

  if(AGS_RECALL_CHANNEL(recall->recall_channel) != NULL){
    AgsRecallChannel *recall_channel;

    recall_channel = AGS_RECALL_CHANNEL(recall->recall_channel);

    AGS_RECALL(copy)->recall_channel = recall_channel;

    //    AGS_RECALL(recall_channel)->recall_channel_run = g_list_prepend(AGS_RECALL(recall_channel)->recall_channel_run,
    //								    copy);
  }

  if(channel != NULL && AGS_IS_INPUT(channel)){
    AgsRecallAudio *recall_audio;
    AgsRecallAudioRun *recall_audio_run;
    guint group_id;

    /* check for AgsRecallAudio */
    if(recall->recall_audio != NULL){
      printf("ags_recall_channel_run_duplicate --- success with AgsRecallAudio\n\0");
      recall_audio = AGS_RECALL_AUDIO(recall->recall_audio);

      AGS_RECALL(copy)->recall_audio = AGS_RECALL(recall_audio);

      //      AGS_RECALL(recall_audio)->recall_channel_run = g_list_prepend(AGS_RECALL(recall_audio)->recall_channel_run,
      //								    copy);
    }else{
      recall_audio = NULL;
    }

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    
    if(audio == NULL)
      return;

    /* check for AgsRecallAudioRun */
    if((AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL & (recall_id->flags)) == 0)
      list_start = audio->play;
    else
      list_start = audio->recall;

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      group_id = recall_id->group_id;
    else
      group_id = recall_id->parent_group_id;

    list = ags_recall_find_type_with_group_id(list_start,
					      AGS_RECALL(copy)->recall_audio_run_type,
					      group_id);

    printf("ags_recall_channel_run_duplicate --- debug 0\n\0");

    if(list != NULL){
      printf("ags_recall_channel_run_duplicate --- success with AgsRecallAudioRun\n\0");
      recall_audio_run = AGS_RECALL_AUDIO_RUN(list->data);

      AGS_RECALL(copy)->recall_audio_run = recall_audio_run;

      AGS_RECALL(recall_audio_run)->recall_channel_run = g_list_prepend(AGS_RECALL(recall_audio_run)->recall_channel_run,
									copy);
    }    
  }

  return((AgsRecall *) copy);
}

AgsRecallChannelRun*
ags_recall_channel_run_new()
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = (AgsRecallChannelRun *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN, NULL);

  return(recall_channel_run);
}
