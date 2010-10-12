#include "ags_delay_audio_run.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_recall_id.h"

#include "ags_delay_audio.h"

#include <stdlib.h>

void ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay);
void ags_delay_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_delay_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_delay_audio_run_init(AgsDelayAudioRun *delay);
void ags_delay_audio_run_connect(AgsConnectable *connectable);
void ags_delay_audio_run_disconnect(AgsConnectable *connectable);
void ags_delay_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_delay_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_delay_audio_run_finalize(GObject *gobject);

void ags_delay_audio_run_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data);
void ags_delay_audio_run_run_inter(AgsRecall *recall, guint audio_channel, gpointer data);
void ags_delay_audio_run_done(AgsRecall *recall, gpointer data);
void ags_delay_audio_run_cancel(AgsRecall *recall, gpointer data);
void ags_delay_audio_run_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_delay_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);
void ags_delay_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count);

enum{
  TIC,
  LAST_SIGNAL,
};

static gpointer ags_delay_audio_run_parent_class = NULL;
static AgsConnectableInterface *ags_delay_audio_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_delay_audio_run_parent_run_connectable_interface;
static guint delay_signals[LAST_SIGNAL];

GType
ags_delay_audio_run_get_type()
{
  static GType ags_type_delay_audio_run = 0;

  if(!ags_type_delay_audio_run){
    static const GTypeInfo ags_delay_audio_run_info = {
      sizeof (AgsDelayAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDelayAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						      "AgsDelayAudioRun\0",
						      &ags_delay_audio_run_info,
						      0);

    g_type_add_interface_static(ags_type_delay_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_delay_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_delay_audio_run);
}

void
ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_delay_audio_run_parent_class = g_type_class_peek_parent(delay_audio_run);

  gobject = (GObjectClass *) delay_audio_run;

  gobject->finalize = ags_delay_audio_run_finalize;

  recall = (AgsRecallClass *) delay_audio_run;

  recall->duplicate = ags_delay_audio_run_duplicate;
  recall->notify_dependency = ags_delay_audio_run_notify_dependency;

  delay_audio_run->tic = NULL;

  delay_signals[TIC] =
    g_signal_new("tic\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
}

void
ags_delay_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_delay_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_delay_audio_run_connect;
  connectable->disconnect = ags_delay_audio_run_disconnect;
}

void
ags_delay_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_delay_audio_run_run_connect;
  run_connectable->disconnect = ags_delay_audio_run_run_disconnect;
}

void
ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run)
{
  delay_audio_run->recall_ref = 0;

  delay_audio_run->hide_ref = 0;
  delay_audio_run->hide_ref_counter = 0;

  delay_audio_run->counter = 0;
}

void
ags_delay_audio_run_connect(AgsConnectable *connectable)
{
  AgsDelayAudioRun *delay_audio_run;

  printf("ags_delay_audio_run_connect\n\0");

  ags_delay_audio_run_parent_connectable_interface->connect(connectable);

  /* AgsDelayAudioRun */
  delay_audio_run = AGS_DELAY_AUDIO_RUN(connectable);

  g_signal_connect((GObject *) delay_audio_run, "run_init_pre\0",
		   G_CALLBACK(ags_delay_audio_run_run_init_pre), NULL);

  g_signal_connect((GObject *) delay_audio_run, "run_inter\0",
		   G_CALLBACK(ags_delay_audio_run_run_inter), NULL);

  g_signal_connect((GObject *) delay_audio_run, "done\0",
		   G_CALLBACK(ags_delay_audio_run_done), NULL);

  g_signal_connect((GObject *) delay_audio_run, "cancel\0",
		   G_CALLBACK(ags_delay_audio_run_cancel), NULL);

  g_signal_connect((GObject *) delay_audio_run, "remove\0",
		   G_CALLBACK(ags_delay_audio_run_remove), NULL);
}

void
ags_delay_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_delay_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_delay_audio_run_run_connect(AgsRunConnectable *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_delay_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_delay_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_audio_run_parent_class)->finalize(gobject);
}

void
ags_delay_audio_run_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data)
{
  printf("ags_delay_audio_run_run_init_pre\n\0");

  recall->flags |= AGS_RECALL_PERSISTENT;
}

void
ags_delay_audio_run_run_inter(AgsRecall *recall, guint audio_channel, gpointer data)
{
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  if((AGS_RECALL_PERSISTENT & (recall->flags)) == 0 &&
     delay_audio_run->recall_ref == 0){
    delay_audio_run->counter = 0;
    ags_recall_done(recall);
  }else{
    delay_audio = AGS_DELAY_AUDIO(recall->recall_audio);
    
    if(delay_audio->delay == delay_audio_run->counter){
      delay_audio_run->counter = 0;
      ags_delay_audio_run_tic(delay_audio_run, audio_channel);
    }else{
      if(delay_audio_run->hide_ref != 0)
	delay_audio_run->hide_ref_counter++;

      if(delay_audio_run->hide_ref_counter != delay_audio_run->hide_ref)
	return;

      delay_audio_run->hide_ref_counter = 0;

      delay_audio_run->counter++;
    }
  }
}

void
ags_delay_audio_run_done(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_delay_audio_run_cancel(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_delay_audio_run_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_delay_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsDelayAudioRun *delay_audio_run, *copy;
  
  delay_audio_run = (AgsDelayAudioRun *) recall;
  copy = (AgsDelayAudioRun *) AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->duplicate(recall, recall_id);

  copy->recall_ref = delay_audio_run->recall_ref;

  copy->hide_ref = delay_audio_run->hide_ref;
  copy->hide_ref_counter = delay_audio_run->hide_ref_counter;

  copy->counter = delay_audio_run->counter;

  return((AgsRecall *) copy);
}

void
ags_delay_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count)
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    delay_audio_run->hide_ref += count;

    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_SHARED_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    delay_audio_run->recall_ref += count;

    break;
  default:
    printf("ags_delay_audio_run.c - ags_delay_audio_run_notify: unknown notify");
  }
}

void
ags_delay_audio_run_tic(AgsDelayAudioRun *delay_audio_run, guint audio_channel)
{
  //  printf("%u\n\0", audio_channel);
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_signals[TIC], 0,
		audio_channel);
  g_object_unref(G_OBJECT(delay_audio_run));
}

AgsDelayAudioRun*
ags_delay_audio_run_new(AgsRecallAudio *recall_audio)
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
						      "recall_audio_type\0", AGS_TYPE_DELAY_AUDIO,
						      "recall_audio\0", recall_audio,
						      "recall_audio_run_type\0", AGS_TYPE_DELAY_AUDIO_RUN,
						      NULL);

  return(delay_audio_run);
}
