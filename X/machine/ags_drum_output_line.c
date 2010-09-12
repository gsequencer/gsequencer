#include "ags_drum_output_line.h"

#include "../ags_window.h"

#include "ags_drum.h"

#include "../../audio/recall/ags_delay.h"
#include "../../audio/recall/ags_play_volume.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_recall_volume.h"
#include "../../audio/recall/ags_copy_pattern.h"

GType ags_drum_output_line_get_type();
void ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line);
void ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line);
void ags_drum_output_line_destroy(GtkObject *object);
void ags_drum_output_line_connect(AgsDrumOutputLine *drum_output_line);

void ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_drum_output_line_parent_class = NULL;

GType
ags_drum_output_line_get_type()
{
  static GType ags_type_drum_output_line = 0;

  if(!ags_type_drum_output_line){
    static const GTypeInfo ags_drum_output_line_info = {
      sizeof(AgsDrumOutputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_output_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumOutputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_output_line_init,
    };

    ags_type_drum_output_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsDrumOutputLine\0", &ags_drum_output_line_info,
						       0);
  }

  return(ags_type_drum_output_line);
}

void
ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line)
{
  AgsLineClass *line;

  ags_drum_output_line_parent_class = g_type_class_peek_parent(drum_output_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_output_line);

  line->set_channel = ags_drum_output_line_set_channel;
}

void
ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line)
{
  GtkVScale *scale;

  drum_output_line->flags = 0;
}

void
ags_drum_output_line_destroy(GtkObject *object)
{
}

void
ags_drum_output_line_connect(AgsDrumOutputLine *drum_output_line)
{
}

void
ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->set_channel(line, channel);
}

void
ags_drum_output_line_map_recall(AgsDrumOutputLine *drum_output_line)
{
}

AgsDrumOutputLine*
ags_drum_output_line_new(AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = (AgsDrumOutputLine *) g_object_new(AGS_TYPE_DRUM_OUTPUT_LINE,
							"channel\0", channel,
							NULL);

  return(drum_output_line);
}
