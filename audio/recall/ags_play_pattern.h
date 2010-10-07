#ifndef __AGS_PLAY_PATTERN_H__
#define __AGS_PLAY_PATTERN_H__

#include <glib-object.h>

#include "../ags_recall.h"

//#include "ags_delay.h"

#include "../ags_channel.h"
#include "../ags_pattern.h"

#define AGS_TYPE_PLAY_PATTERN             (ags_play_pattern_get_type())
#define AGS_PLAY_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PLAY_PATTERN, AgsPlayPattern))
#define AGS_PLAY_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAY_PATTERN, AgsPlayPattern))

typedef struct _AgsPlayPattern AgsPlayPattern;
typedef struct _AgsPlayPatternClass AgsPlayPatternClass;

struct _AgsPlayPattern{
  AgsRecall recall;

  //  AgsDelay *delay;

  AgsChannel *channel;
  AgsPattern *pattern;

  guint i;
  guint j;
  guint bit;
  guint length;

  gboolean loop;
};

struct _AgsPlayPatternClass{
  AgsRecallClass recall;
};

void ags_play_pattern_connect(AgsPlayPattern *play_pattern);

//AgsPlayPattern* ags_play_pattern_new(AgsChannel *channel, AgsDelay *delay);

#endif /*__AGS_PLAY_PATTERN_H__*/
