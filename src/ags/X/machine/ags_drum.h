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

#ifndef __AGS_DRUM_H__
#define __AGS_DRUM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/machine/ags_pattern_box.h>

#define AGS_TYPE_DRUM                (ags_drum_get_type())
#define AGS_DRUM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM, AgsDrum))
#define AGS_DRUM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM, AgsDrumClass))
#define AGS_IS_DRUM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM))
#define AGS_IS_DRUM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM))
#define AGS_DRUM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DRUM, AgsDrumClass))

typedef struct _AgsDrum AgsDrum;
typedef struct _AgsDrumClass AgsDrumClass;

typedef enum{
  /* empty */
}AgsDrumFlags;

struct _AgsDrum
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  GtkButton *open;
  GtkWidget *open_dialog;
  
  GtkCheckButton *loop_button;
  GtkSpinButton *length_spin;

  GtkToggleButton *run;

  GtkVBox *vbox;

  GtkHBox *input_pad;
  GtkVBox *output_pad;

  GtkToggleButton *selected_edit_button;

  AgsDrumInputPad *selected_pad;
  AgsLine *selected_line;

  GtkToggleButton *index0[4];
  GtkToggleButton *selected0;

  GtkToggleButton *index1[12];
  GtkToggleButton *selected1;

  AgsPatternBox *pattern_box;
};

struct _AgsDrumClass
{
  AgsMachineClass machine;
};

GType ags_drum_get_type(void);

AgsDrum* ags_drum_new(GObject *devout);

#endif /*__AGS_DRUM_H__*/
