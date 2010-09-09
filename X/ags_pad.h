#ifndef __AGS_PAD_H__
#define __AGS_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_line.h"
#include "../audio/ags_channel.h"

#define AGS_TYPE_PAD                (ags_pad_get_type())
#define AGS_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD, AgsPad))
#define AGS_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD, AgsPadClass))
#define AGS_IS_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PAD))
#define AGS_IS_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PAD))
#define AGS_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PAD, AgsPadClass))

typedef struct _AgsPad AgsPad;
typedef struct _AgsPadClass AgsPadClass;

struct _AgsPad
{
  GtkVBox vbox;

  AgsChannel *channel;
  AgsLine *selected_line;

  GtkOptionMenu *option;

  GtkToggleButton *group;
  GtkToggleButton *mute;
  GtkToggleButton *solo;
};

struct _AgsPadClass
{
  GtkVBoxClass vbox;

  void (*resize_lines)(AgsPad *pad, guint audio_channels, guint audio_channels_old);
};

void ags_pad_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old);

AgsPad* ags_pad_new();

#endif /*__AGS_PAD_H__*/
