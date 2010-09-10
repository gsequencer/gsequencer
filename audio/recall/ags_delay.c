#include "ags_delay.h"

#include "../ags_recall_id.h"

#include <stdlib.h>

GType ags_delay_get_type();
void ags_delay_class_init(AgsDelayClass *delay);
void ags_delay_init(AgsDelay *delay);
void ags_delay_finalize(GObject *gobject);

void ags_delay_run_inter(AgsRecall *recall, gpointer data);
void ags_delay_done(AgsRecall *recall, gpointer data);
void ags_delay_cancel(AgsRecall *recall, gpointer data);
void ags_delay_remove(AgsRecall *recall, gpointer data);
AgsRecall* ags_delay_duplicate(AgsRecall *recall, AgsRecallID *recall_id);
void ags_delay_notify_run(AgsRecall *recall);

static gpointer ags_delay_parent_class = NULL;

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

    ags_type_delay = g_type_register_static(AGS_TYPE_RECALL,
					    "AgsDelay\0",
					    &ags_delay_info,
					    0);
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
ags_delay_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_parent_class)->finalize(gobject);
}

void
ags_delay_connect(AgsDelay *delay)
{
  //  ags_recall_connect((AgsRecall *) delay);

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
ags_delay_run_inter(AgsRecall *recall, gpointer data)
{
  AgsDelay *delay;

  delay = (AgsDelay *) recall;

  if(delay->hide_ref != 0)
    delay->hide_ref_counter++;

  //  fprintf(stdout, "ags_delay_run_inter - debug\n\0");

  if(delay->hide_ref_counter != delay->hide_ref)
    return;

  delay->hide_ref_counter = 0;

  if(delay->recall_ref == 0){
    delay->counter = 0;
    ags_recall_done(recall);
  }else{
    if(delay->shared_audio->delay == delay->counter){
      delay->counter = 0;
      //      fprintf(stdout, "delay->delay == delay->counter\n\0");
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
ags_delay_notify_run(AgsRecall *recall)
{
  AGS_DELAY(recall)->hide_ref++;
}

AgsDelay*
ags_delay_new(AgsDelaySharedAudio *shared_audio)
{
  AgsDelay *delay;

  delay = (AgsDelay *) g_object_new(AGS_TYPE_DELAY, NULL);

  delay->shared_audio = shared_audio;

  return(delay);
}
