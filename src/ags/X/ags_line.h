/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_LINE_H__
#define __AGS_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/widget/ags_expander.h>

#define AGS_TYPE_LINE                (ags_line_get_type())
#define AGS_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE, AgsLine))
#define AGS_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE, AgsLineClass))
#define AGS_IS_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE))
#define AGS_IS_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE))
#define AGS_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE, AgsLineClass))

#define AGS_LINE_DEFAULT_VERSION "0.4.2\0"
#define AGS_LINE_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

typedef struct _AgsLine AgsLine;
typedef struct _AgsLineClass AgsLineClass;

typedef enum{
  AGS_LINE_CONNECTED         = 1,
  AGS_LINE_GROUPED           = 1 << 1,
  AGS_LINE_MAPPED_RECALL     = 1 << 2,
  AGS_LINE_PREMAPPED_RECALL  = 1 << 3,
}AgsLineFlags;

struct _AgsLine
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  gchar *name;

  AgsChannel *channel;

  GtkWidget *pad;

  GtkLabel *label;
  GtkToggleButton *group;

  AgsExpander *expander;
};

struct _AgsLineClass
{
  GtkVBoxClass vbox;

  void (*set_channel)(AgsLine *line, AgsChannel *channel);

  void (*group_changed)(AgsLine *line);

  void (*map_recall)(AgsLine *line,
		     guint output_pad_start);
  GList* (*find_port)(AgsLine *line);
};

GType ags_line_get_type(void);

void ags_line_set_channel(AgsLine *line, AgsChannel *channel);

void ags_line_group_changed(AgsLine *line);

GList* ags_line_find_next_grouped(GList *line);

void ags_line_map_recall(AgsLine *line,
			 guint output_pad_start);
GList* ags_line_find_port(AgsLine *line);

AgsLine* ags_line_new(GtkWidget *pad, AgsChannel *channel);

#endif /*__AGS_LINE_H__*/
