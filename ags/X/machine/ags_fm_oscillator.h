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

#ifndef __AGS_FM_OSCILLATOR_H__
#define __AGS_FM_OSCILLATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_FM_OSCILLATOR                (ags_fm_oscillator_get_type())
#define AGS_FM_OSCILLATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_FM_OSCILLATOR, AgsFMOscillator))
#define AGS_FM_OSCILLATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FM_OSCILLATOR, AgsFMOscillatorClass))
#define AGS_IS_FM_OSCILLATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FM_OSCILLATOR))
#define AGS_IS_FM_OSCILLATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FM_OSCILLATOR))
#define AGS_FM_OSCILLATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FM_OSCILLATOR, AgsFMOscillatorClass))

#define AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT (44100.0 / 27.5)
#define AGS_FM_OSCILLATOR_DEFAULT_SYNC_POINT_COUNT (3)

typedef struct _AgsFMOscillator AgsFMOscillator;
typedef struct _AgsFMOscillatorClass AgsFMOscillatorClass;

typedef enum{
  AGS_FM_OSCILLATOR_CONNECTED   = 1,
}AgsFMOscillatorFlags;

struct _AgsFMOscillator
{
  GtkFrame frame;

  guint flags;

  GtkComboBox *wave;

  GtkSpinButton *attack;
  GtkSpinButton *frame_count;

  GtkSpinButton *frequency;
  GtkSpinButton *phase;

  GtkSpinButton *volume;

  GtkCheckButton *do_sync;

  GtkSpinButton **sync_point;
  guint sync_point_count;

  GtkComboBox *fm_lfo_wave;

  GtkSpinButton *fm_lfo_frequency;
  GtkSpinButton *fm_lfo_depth;
  
  GtkSpinButton *fm_tuning;
};

struct _AgsFMOscillatorClass
{
  GtkFrameClass frame;

  void (*control_changed)(AgsFMOscillator *fm_oscillator);
};

GType ags_fm_oscillator_get_type(void);

void ags_fm_oscillator_control_changed(AgsFMOscillator *fm_oscillator);

AgsFMOscillator* ags_fm_oscillator_new();

#endif /*__AGS_FM_OSCILLATOR_H__*/
