#ifndef __AGS_OUTPUT_H__
#define __AGS_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_OUTPUT                (ags_output_get_type())
#define AGS_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OUTPUT, AgsOutput))
#define AGS_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OUTPUT, AgsOutputClass))
#define AGS_IS_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OUTPUT))
#define AGS_IS_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OUTPUT))
#define AGS_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OUTPUT, AgsOutputClass))

typedef struct _AgsOutput AgsOutput;
typedef struct _AgsOutputClass AgsOutputClass;

struct _AgsOutput
{
  AgsChannel channel;
};

struct _AgsOutputClass
{
  AgsChannelClass channel;
};

GType ags_output_get_type();

void ags_output_connect(AgsOutput *output);

AgsOutput* ags_output_new();

#endif /*__AGS_OUTPUT_H__*/
