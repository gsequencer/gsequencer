#ifndef __AGS_LINE_H__
#define __AGS_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../audio/ags_channel.h"

#define AGS_TYPE_LINE                (ags_line_get_type())
#define AGS_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE, AgsLine))
#define AGS_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE, AgsLineClass))
#define AGS_IS_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE))
#define AGS_IS_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE))
#define AGS_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE, AgsLineClass))

typedef struct _AgsLine AgsLine;
typedef struct _AgsLineClass AgsLineClass;
typedef struct _AgsLineMember AgsLineMember;

struct _AgsLine
{
  GtkMenuItem item;

  GtkTable *table;
  GtkLabel *label;

  AgsChannel *channel;
};

struct _AgsLineClass
{
  GtkMenuItemClass item;

  void (*set_channel)(AgsLine *line, AgsChannel *channel);
};

struct _AgsLineMember
{
  AgsRecall *recall;
  guint control;
};

void ags_line_set_channel(AgsLine *line, AgsChannel *channel);

AgsLine* ags_line_new(AgsChannel *channel);

#endif /*__AGS_LINE_H__*/
