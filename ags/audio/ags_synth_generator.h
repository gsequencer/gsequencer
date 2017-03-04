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

#ifndef __AGS_SYNTH_GENERATOR_H__
#define __AGS_SYNTH_GENERATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_function.h>

#define AGS_TYPE_SYNTH_GENERATOR                (ags_synth_generator_get_type())
#define AGS_SYNTH_GENERATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGenerator))
#define AGS_SYNTH_GENERATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))
#define AGS_IS_SYNTH_GENERATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_IS_SYNTH_GENERATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_SYNTH_GENERATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))

typedef struct _AgsSynthGenerator AgsSynthGenerator;
typedef struct _AgsSynthGeneratorClass AgsSynthGeneratorClass;

typedef enum{
  AGS_SYNTH_GENERATOR_OSCILLATOR_SIN,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH,
  AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE,
}AgsSynthGeneratorOscillator;

struct _AgsSynthGenerator
{
  AgsFunction function;

  guint flags;
  
  guint samplerate;
  guint buffer_size;
  guint format;

  guint n_frames;
  
  guint oscillator;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;
  
  GObject *timestamp;
};

struct _AgsSynthGeneratorClass
{
  AgsFunctionClass function;
};
  
GType ags_synth_generator_get_type();

AgsSynthGenerator* ags_synth_generator_new();

#endif /*__AGS_SYNTH_GENERATOR_H__*/
