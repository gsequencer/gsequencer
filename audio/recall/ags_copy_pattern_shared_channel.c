#include "ags_copy_pattern_shared_channel.h"

GType ags_copy_pattern_shared_channel_get_type();
void ags_copy_pattern_shared_channel_class_init(AgsCopyPatternSharedChannelClass *copy_pattern_shared_channel);
void ags_copy_pattern_shared_channel_init(AgsCopyPatternSharedChannel *copy_pattern_shared_channel);
void ags_copy_pattern_shared_channel_finalize(GObject *gobject);

static gpointer ags_copy_pattern_shared_channel_parent_class = NULL;

GType
ags_copy_pattern_shared_channel_get_type()
{
  static GType ags_type_copy_pattern_shared_channel = 0;

  if(!ags_type_copy_pattern_shared_channel){
    static const GTypeInfo ags_copy_pattern_shared_channel_info = {
      sizeof (AgsCopyPatternSharedChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_shared_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternSharedChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_shared_channel_init,
    };

    ags_type_copy_pattern_shared_channel = g_type_register_static(AGS_TYPE_RECALL_SHARED_CHANNEL,
								  "AgsCopyPatternSharedChannel\0",
								  &ags_copy_pattern_shared_channel_info,
								  0);
  }

  return(ags_type_copy_pattern_shared_channel);
}

void
ags_copy_pattern_shared_channel_class_init(AgsCopyPatternSharedChannelClass *copy_pattern_shared_channel)
{
  GObjectClass *gobject;

  ags_copy_pattern_shared_channel_parent_class = g_type_class_peek_parent(copy_pattern_shared_channel);

  gobject = (GObjectClass *) copy_pattern_shared_channel;

  gobject->finalize = ags_copy_pattern_shared_channel_finalize;
}

void
ags_copy_pattern_shared_channel_init(AgsCopyPatternSharedChannel *copy_pattern_shared_channel)
{
  copy_pattern_shared_channel->destination = NULL;

  copy_pattern_shared_channel->source = NULL;
  copy_pattern_shared_channel->pattern = NULL;
}

void
ags_copy_pattern_shared_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_shared_channel_parent_class)->finalize(gobject);
}

AgsCopyPatternSharedChannel*
ags_copy_pattern_shared_channel_new(AgsChannel *destination,
				    AgsChannel *source, AgsPattern *pattern)
{
  AgsCopyPatternSharedChannel *copy_pattern_shared_channel;

  copy_pattern_shared_channel = (AgsCopyPatternSharedChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL, NULL);

  copy_pattern_shared_channel->destination = destination;

  copy_pattern_shared_channel->source = source;
  copy_pattern_shared_channel->pattern = pattern;

  return(copy_pattern_shared_channel);
}
