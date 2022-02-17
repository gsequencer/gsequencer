/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_DESK_PAD_H__
#define __AGS_DESK_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_DESK_PAD                (ags_desk_pad_get_type())
#define AGS_DESK_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DESK_PAD, AgsDeskPad))
#define AGS_DESK_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DESK_PAD, AgsDeskPadClass))
#define AGS_IS_DESK_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DESK_PAD))
#define AGS_IS_DESK_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DESK_PAD))
#define AGS_DESK_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DESK_PAD, AgsDeskPadClass))

typedef struct _AgsDeskPad AgsDeskPad;
typedef struct _AgsDeskPadClass AgsDeskPadClass;

typedef enum{
  AGS_DESK_PAD_CONNECTED           = 1,
}AgsDeskPadFlags;

struct _AgsDeskPad
{
  GtkBox box;

  guint flags;

  gchar *name;
  gchar *xml_type;

  AgsChannel *channel;

  GtkLabel *position_time;
  GtkScale *position;
  
  GtkToggleButton *play;
  GtkEntry *filename;
  GtkButton *grab_filename;
  
  AgsIndicator *indicator;
  GtkScale *volume;

  GtkTreeView *playlist;

  GtkButton *move_up;
  GtkButton *move_down;
  GtkButton *add;
  GtkButton *remove;
};

struct _AgsDeskPadClass
{
  GtkBoxClass box;
};

GType ags_desk_pad_get_type(void);

void ags_desk_pad_add_filename(AgsDeskPad *desk_pad,
			       gchar *filename);
void ags_desk_pad_remove_nth_filename(AgsDeskPad *desk_pad,
				      guint nth_filename);

AgsDeskPad* ags_desk_pad_new();

G_END_DECLS

#endif /*__AGS_DESK_PAD_H__*/
