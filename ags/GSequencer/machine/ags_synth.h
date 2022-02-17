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

#ifndef __AGS_SYNTH_H__
#define __AGS_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_SYNTH                (ags_synth_get_type())
#define AGS_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH, AgsSynth))
#define AGS_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH, AgsSynthClass))
#define AGS_IS_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH))
#define AGS_IS_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH))
#define AGS_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH, AgsSynthClass))

#define AGS_SYNTH_BASE_NOTE_MAX (72.0)
#define AGS_SYNTH_BASE_NOTE_MIN (-72.0)

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

  GtkBox *input_pad;

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

gboolean ags_synth_test_flags(AgsSynth *synth, guint flags);
void ags_synth_set_flags(AgsSynth *synth, guint flags);
void ags_synth_unset_flags(AgsSynth *synth, guint flags);

void ags_synth_update(AgsSynth *synth);

AgsSynth* ags_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_SYNTH_H__*/
