/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_DESK_H__
#define __AGS_DESK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/machine/ags_desk_pad.h>

G_BEGIN_DECLS

#define AGS_TYPE_DESK                (ags_desk_get_type())
#define AGS_DESK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DESK, AgsDesk))
#define AGS_DESK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DESK, AgsDeskClass))
#define AGS_IS_DESK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DESK))
#define AGS_IS_DESK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DESK))
#define AGS_DESK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_DESK, AgsDeskClass))

typedef struct _AgsDesk AgsDesk;
typedef struct _AgsDeskClass AgsDeskClass;

struct _AgsDesk
{
  AgsMachine machine;

  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *name;
  gchar *xml_type;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  GtkVBox *vbox;
  
  AgsDeskPad *left_pad;

  GtkVBox *console;
  
  GtkButton *move_left;
  GtkScale *balance;
  GtkButton *move_right;
  
  AgsDeskPad *right_pad;
  
  GtkFileChooserWidget *file_chooser;
};

struct _AgsDeskClass
{
  AgsMachineClass machine;
};

GType ags_desk_get_type(void);

AgsDesk* ags_desk_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_DESK_H__*/
