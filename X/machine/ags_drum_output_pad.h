#ifndef __AGS_DRUM_OUTPUT_PAD_H__
#define __AGS_DRUM_OUTPUT_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_pad.h"

#define AGS_TYPE_DRUM_OUTPUT_PAD                (ags_drum_output_pad_get_type())
#define AGS_DRUM_OUTPUT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM_OUTPUT_PAD, AgsDrumOutputPad))
#define AGS_DRUM_OUTPUT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM_OUTPUT_PAD, AgsDrumOutputPadClass))
#define AGS_IS_DRUM_OUTPUT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM_OUTPUT_PAD))
#define AGS_IS_DRUM_OUTPUT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM_OUTPUT_PAD))
#define AGS_DRUM_OUTPUT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DRUM_OUTPUT_PAD, AgsDrumOutputPadClass))

typedef struct _AgsDrumOutputPad AgsDrumOutputPad;
typedef struct _AgsDrumOutputPadClass AgsDrumOutputPadClass;

typedef enum{
  AGS_DRUM_OUTPUT_PAD_PLAY_DONE  = 1,
}AgsDrumOutputPadFlags;

struct _AgsDrumOutputPad
{
  AgsPad pad;

  guint flags;
};

struct _AgsDrumOutputPadClass
{
  AgsPadClass pad;
};

GType ags_drum_output_pad_get_type();

AgsDrumOutputPad* ags_drum_output_pad_new(AgsChannel *channel);

#endif /*__AGS_DRUM_OUTPUT_PAD_H__*/
