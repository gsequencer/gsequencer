/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_FM_SYNTH_H__
#define __AGS_FM_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_FM_SYNTH                (ags_fm_synth_get_type())
#define AGS_FM_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FM_SYNTH, AgsFMSynth))
#define AGS_FM_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FM_SYNTH, AgsFMSynthClass))
#define AGS_IS_FM_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FM_SYNTH))
#define AGS_IS_FM_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FM_SYNTH))
#define AGS_FM_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FM_SYNTH, AgsFMSynthClass))

#define AGS_FM_SYNTH_BASE_NOTE_MAX (72.0)
#define AGS_FM_SYNTH_BASE_NOTE_MIN (-72.0)

typedef struct _AgsFMSynth AgsFMSynth;
typedef struct _AgsFMSynthClass AgsFMSynthClass;

typedef enum{
  AGS_FM_SYNTH_AUTO_UPDATE          = 1,
}AgsFMSynthFlags;

struct _AgsFMSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  GtkVBox *input_pad;

  GtkCheckButton *enable_lfo;
  
  GtkCheckButton *auto_update;
  GtkButton *update;

  GtkSpinButton *lower; // how many channels until to lowest freq

  GtkSpinButton *lfo_freq;
  GtkSpinButton *lfo_phase;

  GtkSpinButton *lfo_depth;
  GtkSpinButton *lfo_tuning;
};

struct _AgsFMSynthClass
{
  AgsMachineClass machine;
};

GType ags_fm_synth_get_type(void);

void ags_fm_synth_update(AgsFMSynth *fm_synth);

AgsFMSynth* ags_fm_synth_new(GObject *soundcard);

#endif /*__AGS_FM_SYNTH_H__*/
