#include "ags_copy_pattern_channel.h"

#include "ags_copy_pattern_audio.h"
#include "ags_copy_pattern_audio_run.h"
#include "ags_copy_pattern_channel_run.h"

GType ags_copy_pattern_channel_get_type();
void ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel);
void ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel);
void ags_copy_pattern_channel_finalize(GObject *gobject);

static gpointer ags_copy_pattern_channel_parent_class = NULL;

GType
ags_copy_pattern_channel_get_type()
{
  static GType ags_type_copy_pattern_channel = 0;

  if(!ags_type_copy_pattern_channel){
    static const GTypeInfo ags_copy_pattern_channel_info = {
      sizeof (AgsCopyPatternChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_channel_init,
    };

    ags_type_copy_pattern_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCopyPatternChannel\0",
							   &ags_copy_pattern_channel_info,
							   0);
  }

  return(ags_type_copy_pattern_channel);
}

void
ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel)
{
  GObjectClass *gobject;

  ags_copy_pattern_channel_parent_class = g_type_class_peek_parent(copy_pattern_channel);

  gobject = (GObjectClass *) copy_pattern_channel;

  gobject->finalize = ags_copy_pattern_channel_finalize;
}

void
ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel)
{
  copy_pattern_channel->destination = NULL;

  copy_pattern_channel->source = NULL;
  copy_pattern_channel->pattern = NULL;
}

void
ags_copy_pattern_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_channel_parent_class)->finalize(gobject);
}

AgsCopyPatternChannel*
ags_copy_pattern_channel_new(AgsChannel *destination,
			     AgsChannel *source, AgsPattern *pattern)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								"recall_audio_type\0", AGS_TYPE_COPY_PATTERN_AUDIO,
								"recall_audio_run_type\0", AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								"recall_channel_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL,
								"recall_channel_run_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
								"channel\0", source,
								NULL);

  copy_pattern_channel->destination = destination;

  copy_pattern_channel->source = source;
  copy_pattern_channel->pattern = pattern;

  return(copy_pattern_channel);
}
