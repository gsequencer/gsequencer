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

#define AGS_OSCILLATOR_DEFAULT_STEPS (15.0)
#define AGS_OSCILLATOR_BASE_FREQUENCY (440.0 / (AGS_OSCILLATOR_DEFAULT_STEPS * exp((1.0 / 12.0) * log(2.0))))

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
