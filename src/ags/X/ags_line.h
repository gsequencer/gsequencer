/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_LINE_H__
#define __AGS_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_LINE                (ags_line_get_type())
#define AGS_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE, AgsLine))
#define AGS_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE, AgsLineClass))
#define AGS_IS_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE))
#define AGS_IS_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE))
#define AGS_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE, AgsLineClass))

typedef struct _AgsLine AgsLine;
typedef struct _AgsLineClass AgsLineClass;
typedef struct _AgsLineMember AgsLineMember;

typedef enum{
  AGS_LINE_CONNECTED       = 1,
}AgsLineFlags;

struct _AgsLine
{
  GtkMenuItem item;

  guint flags;

  GtkWidget *pad;

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

  GList *control;

  gulong done_handler;
};

struct _AgsLineControl
{
  GType control_type;

  GtkAdjustment *adjustment;
};

GType ags_line_get_type(void);

void ags_line_set_channel(AgsLine *line, AgsChannel *channel);

AgsLine* ags_line_new(GtkWidget *pad, AgsChannel *channel);

#endif /*__AGS_LINE_H__*/
