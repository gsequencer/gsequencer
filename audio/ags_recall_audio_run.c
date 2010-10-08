#include "ags_recall_audio_run.h"

#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"

#include "ags_audio.h"
#include "ags_channel.h"

#include "ags_recall_audio.h"

GType ags_recall_audio_run_get_type();
void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_runconnectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run);
void ags_recall_audio_run_connect(AgsConnectable *connectable);
void ags_recall_audio_run_disconnect(AgsConnectable *connectable);
void ags_recall_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_audio_run_finalize(GObject *gobject);

AgsRecall* ags_recall_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_recall_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_recall_audio_run_parent_run_connectable_interface;

GType
ags_recall_audio_run_get_type()
{
  static GType ags_type_recall_audio_run = 0;

  if(!ags_type_recall_audio_run){
    static const GTypeInfo ags_recall_audio_run_info = {
      sizeof (AgsRecallAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_runconnectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_run = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallAudioRun\0",
						       &ags_recall_audio_run_info,
						       0);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall_audio_run);
}

void
ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_recall_audio_run_parent_class = g_type_class_peek_parent(recall_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_run;

  gobject->finalize = ags_recall_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_run;

  recall->duplicate = ags_recall_audio_run_duplicate;
}

void
ags_recall_audio_runconnectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_audio_connectable_parent_interface;

  ags_recall_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_audio_run_connect;
  connectable->disconnect = ags_recall_audio_run_disconnect;
}

void
ags_recall_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_audio_run_run_connect;
  run_connectable->disconnect = ags_recall_audio_run_run_disconnect;
}

void
ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run)
{
}

void
ags_recall_audio_run_connect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_run_run_connect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_run_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallAudioRun *recall_audio_run, *copy;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);
  copy = AGS_RECALL_AUDIO_RUN(AGS_RECALL_CLASS(ags_recall_audio_run_parent_class)->duplicate(recall, recall_id));

  audio = AGS_RECALL_AUDIO(recall->recall_audio)->audio;

  if(audio != NULL){
    AgsRecallAudio *recall_audio;
    
    recall_audio = AGS_RECALL_AUDIO(recall->recall_audio);

    AGS_RECALL(copy)->recall_audio = AGS_RECALL(recall_audio);

    AGS_RECALL(recall_audio)->recall_audio_run = g_list_prepend(AGS_RECALL(recall_audio)->recall_audio_run,
								copy);
  }

  return((AgsRecall *) copy);
}

AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN, NULL);

  return(recall_audio_run);
}
