#ifndef __AGS_FFPLAYER_H__
#define __AGS_FFPLAYER_H__

#include <glib.h>
#include <glib-object.h>
#include <pango/pango.h>
#include <gtk/gtk.h>

#include "../ags_machine.h"

#define AGS_TYPE_FFPLAYER                (ags_ffplayer_get_type())
#define AGS_FFPLAYER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER, AgsFFPlayer))
#define AGS_FFPLAYER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER, AgsFFPlayerClass))
#define AGS_IS_FFPLAYER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER))
#define AGS_IS_FFPLAYER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FFPLAYER))
#define AGS_FFPLAYER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_FFPLAYER, AgsFFPlayerClass))

typedef struct _AgsFFPlayer AgsFFPlayer;
typedef struct _AgsFFPlayerClass AgsFFPlayerClass;

struct _AgsFFPlayer
{
  AgsMachine machine;

  GtkButton *open;

  guint control_width;
  guint control_height;
  
  GtkDrawingArea *drawing_area;
  GtkAdjustment *hadjustment;
};

struct _AgsFFPlayerClass
{
  AgsMachineClass machine;
};

void ags_ffplayer_paint(AgsFFPlayer *ffplayer);
// char* ags_ffplayer_sound_string();

AgsFFPlayer* ags_ffplayer_new();

#endif /*__AGS_FFPLAYER_H__*/
