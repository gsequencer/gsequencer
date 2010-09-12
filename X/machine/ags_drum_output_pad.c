#include "ags_drum_output_pad.h"
#include "ags_drum_output_pad_callbacks.h"

#include "ags_drum.h"

#include "../ags_line.h"

GType ags_drum_output_pad_get_type();
void ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad);
void ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad);
void ags_drum_output_pad_destroy(GtkObject *object);
void ags_drum_output_pad_connect(AgsDrumOutputPad *drum_output_pad);

void ags_drum_output_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);

static gpointer ags_drum_output_pad_parent_class = NULL;

GType
ags_drum_output_pad_get_type()
{
  static GType drum_output_pad_type = 0;

  if (!drum_output_pad_type){
    static const GtkTypeInfo drum_output_pad_info = {
      "AgsDrumOutputPad\0",
      sizeof(AgsDrumOutputPad), /* base_init */
      sizeof(AgsDrumOutputPadClass), /* base_finalize */
      (GtkClassInitFunc) ags_drum_output_pad_class_init,
      (GtkObjectInitFunc) ags_drum_output_pad_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };
    drum_output_pad_type = gtk_type_unique (AGS_TYPE_PAD, &drum_output_pad_info);
  }

  return (drum_output_pad_type);
}

void
ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad)
{
  GObjectClass *gobject;
  AgsPadClass *pad;

  ags_drum_output_pad_parent_class = g_type_class_peek_parent(drum_output_pad);

  /* AgsPadClass */
  pad = (AgsPadClass *) drum_output_pad;

  pad->set_channel = ags_drum_output_pad_set_channel;
  pad->resize_lines = ags_drum_output_pad_resize_lines;
}

void
ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad)
{
  drum_output_pad->flags = 0;
}

void
ags_drum_output_pad_destroy(GtkObject *object)
{
}

void
ags_drum_output_pad_connect(AgsDrumOutputPad *drum_output_pad)
{
}

void
ags_drum_output_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_drum_output_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_drum_output_pad_parent_class)->resize_lines(pad, line_type,
								audio_channels, audio_channels_old);

  /* empty */
}

AgsDrumOutputPad*
ags_drum_output_pad_new(AgsChannel *channel)
{
  AgsDrumOutputPad *drum_output_pad;

  drum_output_pad = (AgsDrumOutputPad *) g_object_new(AGS_TYPE_DRUM_OUTPUT_PAD,
						      "channel\0", channel,
						      NULL);

  return(drum_output_pad);
}
