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

#ifndef __AGS_DRUM_H__
#define __AGS_DRUM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/X/machine/ags_drum_input_pad.h>

#define AGS_TYPE_DRUM                (ags_drum_get_type())
#define AGS_DRUM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM, AgsDrum))
#define AGS_DRUM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM, AgsDrumClass))
#define AGS_IS_DRUM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM))
#define AGS_IS_DRUM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM))
#define AGS_DRUM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DRUM, AgsDrumClass))

#define AGS_DRUM_PATTERN_CONTROLS (16)

typedef struct _AgsDrum AgsDrum;
typedef struct _AgsDrumClass AgsDrumClass;

typedef enum{
  AGS_DRUM_BLOCK_PATTERN    =  1,
}AgsDrumFlags;

struct _AgsDrum
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  GtkButton *open;

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

  guint active_led;
  GtkHBox *led;
  GtkHBox *pattern;

  GtkVBox *offset;
};

struct _AgsDrumClass
{
  AgsMachineClass machine;
};

GType ags_drum_get_type(void);

void ags_drum_set_pattern(AgsDrum *drum);

AgsDrum* ags_drum_new();

#endif /*__AGS_DRUM_H__*/
