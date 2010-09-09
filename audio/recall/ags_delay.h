#ifndef __AGS_DELAY_H__
#define __AGS_DELAY_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"
#include "../ags_audio.h"

#include "ags_delay_shared_audio.h"

#define AGS_TYPE_DELAY                (ags_delay_get_type())
#define AGS_DELAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY, AgsDelay))
#define AGS_DELAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY, AgsDelay))
#define AGS_IS_DELAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DELAY))
#define AGS_IS_DELAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DELAY))
#define AGS_DELAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DELAY, AgsDelayClass))

typedef struct _AgsDelay AgsDelay;
typedef struct _AgsDelayClass AgsDelayClass;

struct _AgsDelay
{
  AgsRecall recall;

  AgsDelaySharedAudio *shared_audio;

  guint recall_ref;

  guint hide_ref;
  guint hide_ref_counter;

  guint counter;
};

struct _AgsDelayClass
{
  AgsRecallClass recall;
};

AgsDelay* ags_delay_new(AgsDelaySharedAudio *shared_audio);

#endif /*__AGS_DELAY_H__*/
