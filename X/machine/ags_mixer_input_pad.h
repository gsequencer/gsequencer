#ifndef __AGS_MIXER_INPUT_PAD_H__
#define __AGS_MIXER_INPUT_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_pad.h"

#define AGS_TYPE_MIXER_INPUT_PAD                (ags_mixer_input_pad_get_type())
#define AGS_MIXER_INPUT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIXER_INPUT_PAD, AgsMixerInputPad))
#define AGS_MIXER_INPUT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIXER_INPUT_PAD, AgsMixerInputPadClass))
#define AGS_IS_MIXER_INPUT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIXER_INPUT_PAD))
#define AGS_IS_MIXER_INPUT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIXER_INPUT_PAD))
#define AGS_MIXER_INPUT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MIXER_INPUT_PAD, AgsMixerInputPadClass))

typedef struct _AgsMixerInputPad AgsMixerInputPad;
typedef struct _AgsMixerInputPadClass AgsMixerInputPadClass;

struct _AgsMixerInputPad
{
  AgsPad pad;
};

struct _AgsMixerInputPadClass
{
  AgsPadClass pad;
};

GType ags_mixer_input_pad_get_type();

AgsMixerInputPad* ags_mixer_input_pad_new(AgsChannel *channel);

#endif /*__AGS_MIXER_INPUT_PAD_H__*/
