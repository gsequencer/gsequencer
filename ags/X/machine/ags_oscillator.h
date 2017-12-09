/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_OSCILLATOR_H__
#define __AGS_OSCILLATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <libxml/tree.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_OSCILLATOR                (ags_oscillator_get_type())
#define AGS_OSCILLATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_OSCILLATOR, AgsOscillator))
#define AGS_OSCILLATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSCILLATOR, AgsOscillatorClass))
#define AGS_IS_OSCILLATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OSCILLATOR))
#define AGS_IS_OSCILLATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OSCILLATOR))
#define AGS_OSCILLATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OSCILLATOR, AgsOscillatorClass))

#define AGS_OSCILLATOR_DEFAULT_FRAME_COUNT (44100.0 / 27.5)
#define AGS_OSCILLATOR_DEFAULT_SYNC_POINT_COUNT (3)

typedef struct _AgsOscillator AgsOscillator;
typedef struct _AgsOscillatorClass AgsOscillatorClass;

struct _AgsOscillator
{
  GtkFrame frame;

  GtkComboBox *wave;
  gulong wave_handler;

  GtkSpinButton *attack;
  GtkSpinButton *frame_count;

  GtkSpinButton *frequency;
  GtkSpinButton *phase;

  GtkSpinButton *volume;

  GtkCheckButton *do_sync;

  GtkSpinButton **sync_point;
  guint sync_point_count;
};

struct _AgsOscillatorClass
{
  GtkFrameClass frame;

  void (*control_changed)(AgsOscillator *oscillator);
};

GType ags_oscillator_get_type(void);

void ags_file_read_oscillator(AgsFile *file, xmlNode *node, AgsOscillator **oscillator);
xmlNode* ags_file_write_oscillator(AgsFile *file, xmlNode *parent, AgsOscillator *oscillator);

void ags_oscillator_control_changed(AgsOscillator *oscillator);

AgsOscillator* ags_oscillator_new();

#endif /*__AGS_OSCILLATOR_H__*/
