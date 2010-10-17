#ifndef __AGS_APPLY_SYNTH_H__
#define __AGS_APPLY_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_task.h"

#define AGS_TYPE_APPLY_SYNTH                (ags_apply_synth_get_type())
#define AGS_APPLY_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SYNTH, AgsApplySynth))
#define AGS_APPLY_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SYNTH, AgsApplySynthClass))
#define AGS_IS_APPLY_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SYNTH))
#define AGS_IS_APPLY_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SYNTH))
#define AGS_APPLY_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SYNTH, AgsApplySynthClass))

typedef struct _AgsApplySynth AgsApplySynth;
typedef struct _AgsApplySynthClass AgsApplySynthClass;

struct _AgsApplySynth
{
  AgsTask task;
};

struct _AgsApplySynthClass
{
  AgsTaskClass task;
};

GType ags_apply_synth_get_type();

AgsApplySynth* ags_apply_synth_new();

#endif /*__AGS_APPLY_SYNTH_H__*/
