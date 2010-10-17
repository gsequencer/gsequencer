#ifndef __AGS_APPLY_WAVETABLE_H__
#define __AGS_APPLY_WAVETABLE_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_task.h"

#define AGS_TYPE_APPLY_WAVETABLE                (ags_apply_wavetable_get_type())
#define AGS_APPLY_WAVETABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetable))
#define AGS_APPLY_WAVETABLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetableClass))
#define AGS_IS_APPLY_WAVETABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_WAVETABLE))
#define AGS_IS_APPLY_WAVETABLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_WAVETABLE))
#define AGS_APPLY_WAVETABLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetableClass))

typedef struct _AgsApplyWavetable AgsApplyWavetable;
typedef struct _AgsApplyWavetableClass AgsApplyWavetableClass;

struct _AgsApplyWavetable
{
  AgsTask task;
};

struct _AgsApplyWavetableClass
{
  AgsTaskClass task;
};

GType ags_apply_wavetable_get_type();

AgsApplyWavetable* ags_apply_wavetable_new();

#endif /*__AGS_APPLY_WAVETABLE_H__*/
