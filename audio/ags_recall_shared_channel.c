#include "ags_recall_shared_channel.h"

GType ags_recall_shared_channel_get_type();
void ags_recall_shared_channel_class_init(AgsRecallSharedChannelClass *recall_shared_channel);
void ags_recall_shared_channel_init(AgsRecallSharedChannel *recall_shared_channel);
void ags_recall_shared_channel_finalize(GObject *gobject);

static gpointer ags_recall_shared_channel_parent_class = NULL;

GType
ags_recall_shared_channel_get_type()
{
  static GType ags_type_recall_shared_channel = 0;

  if(!ags_type_recall_shared_channel){
    static const GTypeInfo ags_recall_shared_channel_info = {
      sizeof (AgsRecallSharedChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_shared_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallSharedChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_shared_channel_init,
    };

    ags_type_recall_shared_channel = g_type_register_static(AGS_TYPE_RECALL_SHARED,
							    "AgsRecallSharedChannel\0",
							    &ags_recall_shared_channel_info,
							    0);
  }

  return(ags_type_recall_shared_channel);
}

void
ags_recall_shared_channel_class_init(AgsRecallSharedChannelClass *recall_shared_channel)
{
  GObjectClass *gobject;

  ags_recall_shared_channel_parent_class = g_type_class_peek_parent(recall_shared_channel);

  gobject = (GObjectClass *) recall_shared_channel;

  gobject->finalize = ags_recall_shared_channel_finalize;
}

void
ags_recall_shared_channel_init(AgsRecallSharedChannel *recall_shared_channel)
{
}

void
ags_recall_shared_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_shared_channel_parent_class)->finalize(gobject);
}

AgsRecallSharedChannel*
ags_recall_shared_channel_new()
{
  AgsRecallSharedChannel *recall_shared_channel;

  recall_shared_channel = (AgsRecallSharedChannel *) g_object_new(AGS_TYPE_RECALL_SHARED_CHANNEL, NULL);

  return(recall_shared_channel);
}
