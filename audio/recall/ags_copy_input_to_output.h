#ifndef __AGS_COPY_INPUT_TO_OUTPUT_H__
#define __AGS_COPY_INPUT_TO_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_signal.h>

#define AGS_TYPE_COPY_INPUT_TO_OUTPUT                (ags_copy_input_to_output_get_type())
#define AGS_COPY_INPUT_TO_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_COPY_INPUT_TO_OUTPUT, AgsCopyInputToOutput))
#define AGS_COPY_INPUT_TO_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_COPY_INPUT_TO_OUTPUT, AgsCopyInputToOutput))

typedef struct _AgsCopyInputToOutput AgsCopyInputToOutput;
typedef struct _AgsCopyInputToOutputClass AgsCopyInputToOutputClass;

struct _AgsCopyInputToOutput{
  AgsRecall recall;

  AgsChannel *output;
  AgsChannel *input;
};

struct _AgsCopyInputToOutputClass{
  AgsRecallClass recall;
};

GType ags_copy_input_to_output_get_type();

void ags_copy_input_to_output(AgsRecall *recall);

AgsCopyInputToOutput* ags_copy_input_to_output_new();

#endif /*__AGS_COPY_INPUT_TO_OUTPUT_H__*/
