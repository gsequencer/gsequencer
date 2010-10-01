#include "ags_delay.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_recall_id.h"

#include <stdlib.h>

GType ags_delay_get_type();
void ags_delay_class_init(AgsDelayClass *delay);
void ags_delay_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_delay_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_delay_init(AgsDelay *delay);
void ags_delay_connect(AgsConnectable *connectable);
void ags_delay_disconnect(AgsConnectable *connectable);
void ags_delay_run_connect(AgsRunConnectable *run_connectable);
void ags_delay_run_disconnect(AgsRunConnectable *run_connectable);
void ags_delay_finalize(GObject *gobject);

void ags_delay_run_init_pre(AgsRecall *recall, gpointer data);
void ags_delay_run_inter(AgsRecall *recall, gpointer data);
void ags_delay_done(AgsRecall *recall, gpointer data);
void ags_delay_cancel(AgsRecall *recall, gpointer data);
void ags_delay_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_delay_duplicate(AgsRecall *recall, AgsRecallID *recall_id);
void ags_delay_notify_dependency(AgsRecall *recall, guint notify_mode, gint count);

static gpointer ags_delay_parent_class = NULL;
static AgsConnectableInterface *ags_delay_parent_connectable_interface;
static AgsRunConnectableInterface *ags_delay_parent_run_connectable_interface;

GType
ags_delay_get_type()
{
  static GType ags_type_delay = 0;

  if(!ags_type_delay){
    static const GTypeInfo ags_delay_info = {
      sizeof (AgsDelayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDelay),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay = g_type_register_static(AGS_TYPE_RECALL,
					    "AgsDelay\0",
					    &ags_delay_info,
					    0);

    g_type_add_interface_static(ags_type_delay,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_delay,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_delay);
}

void
ags_delay_class_init(AgsDelayClass *delay)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_delay_parent_class = g_type_class_peek_parent(delay);

  gobject = (GObjectClass *) delay;

  gobject->finalize = ags_delay_finalize;

  recall = (AgsRecallClass *) delay;

  recall->duplicate = ags_delay_duplicate;
  recall->notify_dependency = ags_delay_notify_dependency;
}

void
ags_delay_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_delay_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_delay_connect;
  connectable->disconnect = ags_delay_disconnect;
}

void
ags_delay_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_delay_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_delay_run_connect;
  run_connectable->disconnect = ags_delay_run_disconnect;
}

void
ags_delay_init(AgsDelay *delay)
{
  delay->shared_audio = NULL;

  delay->recall_ref = 0;

  delay->hide_ref = 0;
  delay->hide_ref_counter = 0;

  delay->counter = 0;
}

void
ags_delay_connect(AgsConnectable *connectable)
{
  AgsDelay *delay;

  printf("ags_delay_connect\n\0");

  ags_delay_parent_connectable_interface->connect(connectable);

  /* AgsDelay */
  delay = AGS_DELAY(connectable);

  g_signal_connect((GObject *) delay, "run_init_pre\0",
		   G_CALLBACK(ags_delay_run_init_pre), NULL);

  g_signal_connect((GObject *) delay, "run_inter\0",
		   G_CALLBACK(ags_delay_run_inter), NULL);

  g_signal_connect((GObject *) delay, "done\0",
		   G_CALLBACK(ags_delay_done), NULL);

  g_signal_connect((GObject *) delay, "cancel\0",
		   G_CALLBACK(ags_delay_cancel), NULL);

  g_signal_connect((GObject *) delay, "remove\0",
		   G_CALLBACK(ags_delay_remove), NULL);
}

void
ags_delay_disconnect(AgsConnectable *connectable)
{
  ags_delay_parent_connectable_interface->disconnect(connectable);
}

void
ags_delay_run_connect(AgsRunConnectable *run_connectable)
{
  ags_delay_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_delay_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_delay_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_delay_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_parent_class)->finalize(gobject);
}

void
ags_delay_run_init_pre(AgsRecall *recall, gpointer data)
{
  printf("ags_delay_run_init_pre\n\0");

  recall->flags |= AGS_RECALL_PERSISTENT;
}

void
ags_delay_run_inter(AgsRecall *recall, gpointer data)
{
  AgsDelay *delay;

  delay = (AgsDelay *) recall;

  fprintf(stdout, "ags_delay_run_inter - debug\n\0");

  if(delay->hide_ref != 0)
    delay->hide_ref_counter++;

  if(delay->hide_ref_counter != delay->hide_ref)
    return;

  delay->hide_ref_counter = 0;

  if((AGS_RECALL_PERSISTENT & (recall->flags)) == 0 &&
     delay->recall_ref == 0){
    delay->counter = 0;
    ags_recall_done(recall);
  }else{
    if(delay->shared_audio->delay == delay->counter){
      delay->counter = 0;
      fprintf(stdout, "delay->delay == delay->counter\n\0");
    }else{
      delay->counter++;
    }
  }
}

void
ags_delay_done(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_delay_cancel(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_delay_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_delay_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsDelay *delay, *copy;
  
  delay = (AgsDelay *) recall;
  copy = (AgsDelay *) AGS_RECALL_CLASS(ags_delay_parent_class)->duplicate(recall, recall_id);

  copy->shared_audio = delay->shared_audio;

  copy->recall_ref = delay->recall_ref;

  copy->hide_ref = delay->hide_ref;
  copy->hide_ref_counter = delay->hide_ref_counter;

  copy->counter = delay->counter;

  return((AgsRecall *) copy);
}

void
ags_delay_notify_dependency(AgsRecall *recall, guint notify_mode, gint count)
{
  AgsDelay *delay;

  delay = AGS_DELAY(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    delay->hide_ref += count;

    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_SHARED_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    delay->recall_ref += count;

    break;
  default:
    printf("ags_delay.c - ags_delay_notify: unknown notify");
  }
}

AgsDelay*
ags_delay_new(AgsDelaySharedAudio *shared_audio)
{
  AgsDelay *delay;

  delay = (AgsDelay *) g_object_new(AGS_TYPE_DELAY, NULL);

  delay->shared_audio = shared_audio;

  return(delay);
}
