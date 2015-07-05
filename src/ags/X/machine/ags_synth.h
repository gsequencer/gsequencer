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

#ifndef __AGS_SYNTH_H__
#define __AGS_SYNTH_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_SYNTH                (ags_synth_get_type())
#define AGS_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH, AgsSynth))
#define AGS_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH, AgsSynthClass))
#define AGS_IS_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH))
#define AGS_IS_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH))
#define AGS_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH, AgsSynthClass))

typedef struct _AgsSynth AgsSynth;
typedef struct _AgsSynthClass AgsSynthClass;

typedef enum{
  AGS_SYNTH_AUTO_UPDATE          = 1,
}AgsSynthFlags;

struct _AgsSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  GtkVBox *input_pad;

  GtkSpinButton *lower; // how many channels until to lowest freq
  GtkSpinButton *loop_start;
  GtkSpinButton *loop_end;

  GtkCheckButton *auto_update;
  GtkButton *update;
};

struct _AgsSynthClass
{
  AgsMachineClass machine;
};

GType ags_synth_get_type(void);

AgsSynth* ags_synth_new(GObject *soundcard);

#endif /*__AGS_SYNTH_H__*/
