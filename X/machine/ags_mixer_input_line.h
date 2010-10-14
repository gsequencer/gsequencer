#ifndef __AGS_MIXER_INPUT_LINE_H__
#define __AGS_MIXER_INPUT_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_line.h"

#define AGS_TYPE_MIXER_INPUT_LINE                (ags_mixer_input_line_get_type())
#define AGS_MIXER_INPUT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIXER_INPUT_LINE, AgsMixerInputLine))
#define AGS_MIXER_INPUT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIXER_INPUT_LINE, AgsMixerInputLineClass))
#define AGS_IS_MIXER_INPUT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIXER_INPUT_LINE))
#define AGS_IS_MIXER_INPUT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIXER_INPUT_LINE))
#define AGS_MIXER_INPUT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MIXER_INPUT_LINE, AgsMixerInputLineClass))

typedef struct _AgsMixerInputLine AgsMixerInputLine;
typedef struct _AgsMixerInputLineClass AgsMixerInputLineClass;

typedef enum{
  AGS_MIXER_INPUT_LINE_MAPPED_RECALL  =  1,
}AgsMixerInputLineFlags;

struct _AgsMixerInputLine
{
  AgsLine line;

  guint flags;

  GtkVScale *volume;
};

struct _AgsMixerInputLineClass
{
  AgsLineClass line;
};

GType ags_mixer_input_line_get_type();

void ags_mixer_input_line_map_recall(AgsMixerInputLine *mixer_input_line);

AgsMixerInputLine* ags_mixer_input_line_new(AgsChannel *channel);

#endif /*__AGS_MIXER_INPUT_LINE_H__*/

