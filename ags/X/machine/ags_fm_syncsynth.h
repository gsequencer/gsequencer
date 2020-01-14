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

#ifndef __AGS_FM_SYNCSYNTH_H__
#define __AGS_FM_SYNCSYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/machine/ags_fm_oscillator.h>

G_BEGIN_DECLS

#define AGS_TYPE_FM_SYNCSYNTH                (ags_fm_syncsynth_get_type())
#define AGS_FM_SYNCSYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FM_SYNCSYNTH, AgsFMSyncsynth))
#define AGS_FM_SYNCSYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FM_SYNCSYNTH, AgsFMSyncsynthClass))
#define AGS_IS_FM_SYNCSYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FM_SYNCSYNTH))
#define AGS_IS_FM_SYNCSYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FM_SYNCSYNTH))
#define AGS_FM_SYNCSYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FM_SYNCSYNTH, AgsFMSyncsynthClass))

#define AGS_FM_SYNCSYNTH_BASE_NOTE_MAX (72.0)
#define AGS_FM_SYNCSYNTH_BASE_NOTE_MIN (-72.0)

typedef struct _AgsFMSyncsynth AgsFMSyncsynth;
typedef struct _AgsFMSyncsynthClass AgsFMSyncsynthClass;

typedef enum{
  AGS_FM_SYNCSYNTH_AUTO_UPDATE          = 1,
}AgsFMSyncsynthFlags;

struct _AgsFMSyncsynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  GtkVBox *fm_oscillator;
  GtkButton *add;
  GtkButton *remove;

  GtkCheckButton *auto_update;
  GtkButton *update;

  GtkCheckButton *enable_lfo;
  
  GtkSpinButton *lfo_freq;
  GtkSpinButton *lfo_phase;

  GtkSpinButton *lfo_depth;
  GtkSpinButton *lfo_tuning;
  
  GtkSpinButton *lower; // how many channels until to lowest freq
  GtkSpinButton *loop_start;
  GtkSpinButton *loop_end;
};

struct _AgsFMSyncsynthClass
{
  AgsMachineClass machine;
};

GType ags_fm_syncsynth_get_type(void);

void ags_fm_syncsynth_add_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
					AgsFMOscillator *fm_oscillator);
void ags_fm_syncsynth_remove_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
					   guint nth);

void ags_fm_syncsynth_reset_loop(AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_update(AgsFMSyncsynth *fm_syncsynth);

AgsFMSyncsynth* ags_fm_syncsynth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_FM_SYNCSYNTH_H__*/
