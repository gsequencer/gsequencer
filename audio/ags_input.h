#ifndef __AGS_INPUT_H__
#define __AGS_INPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "ags_channel.h"

#include "file/ags_audio_file.h"

#define AGS_TYPE_INPUT                (ags_input_get_type())
#define AGS_INPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INPUT, AgsInput))
#define AGS_INPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INPUT, AgsInputClass))
#define AGS_IS_INPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INPUT))
#define AGS_IS_INPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_INPUT))
#define AGS_INPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INPUT, AgsInputClass))

typedef struct _AgsInput AgsInput;
typedef struct _AgsInputClass AgsInputClass;

struct _AgsInput
{
  AgsChannel channel;

  AgsFile *file;
};

struct _AgsInputClass
{
  AgsChannelClass channel;
};

GType ags_input_get_type();

void ags_input_connect(AgsInput *input);

AgsInput* ags_input_new();

#endif /*__AGS_INPUT_H__*/
