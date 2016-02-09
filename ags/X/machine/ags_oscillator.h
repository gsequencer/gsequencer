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

#ifndef __AGS_OSCILLATOR_H__
#define __AGS_OSCILLATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <libxml/tree.h>

#include <ags/file/ags_file.h>

#define AGS_TYPE_OSCILLATOR                (ags_oscillator_get_type())
#define AGS_OSCILLATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_OSCILLATOR, AgsOscillator))
#define AGS_OSCILLATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSCILLATOR, AgsOscillatorClass))
#define AGS_IS_OSCILLATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OSCILLATOR))
#define AGS_IS_OSCILLATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OSCILLATOR))
#define AGS_OSCILLATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OSCILLATOR, AgsOscillatorClass))

typedef struct _AgsOscillator AgsOscillator;
typedef struct _AgsOscillatorClass AgsOscillatorClass;

struct _AgsOscillator
{
  GtkFrame frame;

  GtkComboBox *wave;
  gulong wave_handler;

  GtkSpinButton *attack;
  gulong attack_handler;

  GtkSpinButton *frame_count;
  gulong frame_count_handler;

  GtkSpinButton *frequency;
  gulong frequency_handler;

  GtkSpinButton *phase;
  gulong phase_handler;

  GtkSpinButton *volume;
  gulong volume_handler;
};

struct _AgsOscillatorClass
{
  GtkFrameClass frame;
};

GType ags_oscillator_get_type(void);

void ags_file_read_oscillator(AgsFile *file, xmlNode *node, AgsOscillator **oscillator);
xmlNode* ags_file_write_oscillator(AgsFile *file, xmlNode *parent, AgsOscillator *oscillator);

AgsOscillator* ags_oscillator_new();

#endif /*__AGS_OSCILLATOR_H__*/
