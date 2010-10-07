#include "ags_recall_channel_run.h"

GType ags_recall_channel_run_get_type();
void ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run);
void ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_finalize(GObject *gobject);

static gpointer ags_recall_channel_run_parent_class = NULL;

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

    ags_type_recall_channel_run = g_type_register_static(AGS_TYPE_RECALL,
							 "AgsRecallChannelRun\0",
							 &ags_recall_channel_run_info,
							 0);
  }

  return(ags_type_recall_channel_run);
}

void
ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run)
{
  GObjectClass *gobject;

  ags_recall_channel_run_parent_class = g_type_class_peek_parent(recall_channel_run);

  gobject = (GObjectClass *) recall_channel_run;

  gobject->finalize = ags_recall_channel_run_finalize;
}

void
ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run)
{
}

void
ags_recall_channel_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->finalize(gobject);
}

AgsRecallChannelRun*
ags_recall_channel_run_new()
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = (AgsRecallChannelRun *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN, NULL);

  return(recall_channel_run);
}
