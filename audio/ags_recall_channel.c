#include "ags_recall_channel.h"

GType ags_recall_channel_get_type();
void ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel);
void ags_recall_channel_init(AgsRecallChannel *recall_channel);
void ags_recall_channel_finalize(GObject *gobject);

static gpointer ags_recall_channel_parent_class = NULL;

GType
ags_recall_channel_get_type()
{
  static GType ags_type_recall_channel = 0;

  if(!ags_type_recall_channel){
    static const GTypeInfo ags_recall_channel_info = {
      sizeof (AgsRecallChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_init,
    };

    ags_type_recall_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsRecallChannel\0",
						     &ags_recall_channel_info,
						     0);
  }

  return(ags_type_recall_channel);
}

void
ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel)
{
  GObjectClass *gobject;

  ags_recall_channel_parent_class = g_type_class_peek_parent(recall_channel);

  gobject = (GObjectClass *) recall_channel;

  gobject->finalize = ags_recall_channel_finalize;
}

void
ags_recall_channel_init(AgsRecallChannel *recall_channel)
{
}

void
ags_recall_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->finalize(gobject);
}

AgsRecallChannel*
ags_recall_channel_new()
{
  AgsRecallChannel *recall_channel;

  recall_channel = (AgsRecallChannel *) g_object_new(AGS_TYPE_RECALL_CHANNEL, NULL);

  return(recall_channel);
}
