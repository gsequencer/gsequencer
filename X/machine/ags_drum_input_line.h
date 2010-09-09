#ifndef __AGS_DRUM_INPUT_LINE_H__
#define __AGS_DRUM_INPUT_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_line.h"

#define AGS_TYPE_DRUM_INPUT_LINE                (ags_drum_input_line_get_type())
#define AGS_DRUM_INPUT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM_INPUT_LINE, AgsDrumInputLine))
#define AGS_DRUM_INPUT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DRUM_INPUT_LINE, AgsDrumInputLineClass))
#define AGS_IS_DRUM_INPUT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM_INPUT_LINE))
#define AGS_IS_DRUM_INPUT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM_INPUT_LINE))
#define AGS_DRUM_INPUT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DRUM_INPUT_LINE, AgsDrumInputLineClass))

typedef struct _AgsDrumInputLine AgsDrumInputLine;
typedef struct _AgsDrumInputLineClass AgsDrumInputLineClass;

typedef enum{
  AGS_DRUM_INPUT_LINE_MAPPED_RECALL  =  1,
}AgsDrumInputLineFlags;

struct _AgsDrumInputLine
{
  AgsLine line;

  guint flags;
};

struct _AgsDrumInputLineClass
{
  AgsLineClass line;
};

void ags_drum_input_line_map_recall(AgsDrumInputLine *drum_input_line,
				    guint output_pad_start);

AgsDrumInputLine* ags_drum_input_line_new(AgsChannel *channel);

#endif /*__AGS_DRUM_INPUT_LINE_H__*/
