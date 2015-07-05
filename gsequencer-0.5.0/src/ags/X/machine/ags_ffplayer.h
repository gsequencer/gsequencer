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

#ifndef __AGS_FFPLAYER_H__
#define __AGS_FFPLAYER_H__

#include <glib.h>
#include <glib-object.h>
#include <pango/pango.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#include <ags/audio/file/ags_ipatch.h>

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

  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *name;
  gchar *xml_type;

  GtkButton *open;
  GtkWidget *open_dialog;
  
  guint control_width;
  guint control_height;
  
  GtkDrawingArea *drawing_area;
  GtkAdjustment *hadjustment;

  AgsIpatch *ipatch;
  GtkComboBoxText *preset;
  GtkComboBoxText *instrument;
};

struct _AgsFFPlayerClass
{
  AgsMachineClass machine;
};

GType ags_ffplayer_get_type(void);

void ags_ffplayer_paint(AgsFFPlayer *ffplayer);
// char* ags_ffplayer_sound_string();

AgsFFPlayer* ags_ffplayer_new(GObject *devout);

#endif /*__AGS_FFPLAYER_H__*/
