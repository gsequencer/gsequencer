/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SYNTH_GENERATOR                (ags_synth_generator_get_type())
#define AGS_SYNTH_GENERATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGenerator))
#define AGS_SYNTH_GENERATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))
#define AGS_IS_SYNTH_GENERATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_IS_SYNTH_GENERATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_SYNTH_GENERATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))

#define AGS_SYNTH_GENERATOR_GET_OBJ_MUTEX(obj) (&(((AgsSynthGenerator *) obj)->obj_mutex))

#define AGS_SYNTH_GENERATOR_DEFAULT_SAMPLERATE ((guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE)
#define AGS_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE (AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE)
#define AGS_SYNTH_GENERATOR_DEFAULT_FORMAT (AGS_SOUNDCARD_DEFAULT_FORMAT)

#define AGS_SYNTH_GENERATOR_DEFAULT_OSCILLATOR (AGS_SYNTH_GENERATOR_OSCILLATOR_SIN)

#define AGS_SYNTH_GENERATOR_DEFAULT_FREQUENCY (440.0)
#define AGS_SYNTH_GENERATOR_DEFAULT_PHASE (0.0)
#define AGS_SYNTH_GENERATOR_DEFAULT_VOLUME (1.0)

#define AGS_SYNTH_GENERATOR_DEFAULT_LFO_DEPTH (1.0)
#define AGS_SYNTH_GENERATOR_DEFAULT_TUNING (0.0)

#define AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_FREQUENCY (6.0)
#define AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_DEPTH (1.0)
#define AGS_SYNTH_GENERATOR_DEFAULT_FM_TUNING (0.0)

typedef struct _AgsSynthGenerator AgsSynthGenerator;
typedef struct _AgsSynthGeneratorClass AgsSynthGeneratorClass;

/**
 * AgsSynthGeneratorOscillator:
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SIN: sinus
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH: sawtooth
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE: triangle
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE: square
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE: impulse
 * 
 * Enum values to specify oscillator.
 */
typedef enum{
  AGS_SYNTH_GENERATOR_OSCILLATOR_SIN,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH,
  AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE,
  AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE,
}AgsSynthGeneratorOscillator;

struct _AgsSynthGenerator
{
  AgsFunction function;

  guint flags;
  
  GRecMutex obj_mutex;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint frame_count;
  guint loop_start;
  guint loop_end;

  gdouble delay;
  guint attack;

  gboolean do_lfo;  

  guint oscillator;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;

  gdouble lfo_depth;

  gdouble tuning;
  
  gboolean sync_relative;
  AgsComplex **sync_point;
  guint sync_point_count;

  gboolean do_fm_synth;
  
  guint fm_lfo_oscillator;
  
  gdouble fm_lfo_frequency;
  gdouble fm_lfo_depth;

  gdouble fm_tuning;
  
  AgsComplex *damping;
  AgsComplex *vibration;
  
  GObject *timestamp;
};

struct _AgsSynthGeneratorClass
{
  AgsFunctionClass function;
};
  
GType ags_synth_generator_get_type();

GRecMutex* ags_synth_generator_get_obj_mutex(AgsSynthGenerator *synth_generator);

guint ags_synth_generator_get_samplerate(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_samplerate(AgsSynthGenerator *synth_generator, guint samplerate);

guint ags_synth_generator_get_buffer_size(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_buffer_size(AgsSynthGenerator *synth_generator, guint buffer_size);

guint ags_synth_generator_get_format(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_format(AgsSynthGenerator *synth_generator, guint format);

gdouble ags_synth_generator_get_delay(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_delay(AgsSynthGenerator *synth_generator, gdouble delay);

guint ags_synth_generator_get_frame_count(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_frame_count(AgsSynthGenerator *synth_generator, guint frame_count);

guint ags_synth_generator_get_loop_start(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_loop_start(AgsSynthGenerator *synth_generator, guint loop_start);

guint ags_synth_generator_get_loop_end(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_loop_end(AgsSynthGenerator *synth_generator, guint loop_end);

gboolean ags_synth_generator_get_do_lfo(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_do_lfo(AgsSynthGenerator *synth_generator, gboolean do_lfo);

guint ags_synth_generator_get_oscillator(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_oscillator(AgsSynthGenerator *synth_generator, guint oscillator);

gdouble ags_synth_generator_get_frequency(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_frequency(AgsSynthGenerator *synth_generator, gdouble frequency);

gdouble ags_synth_generator_get_phase(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_phase(AgsSynthGenerator *synth_generator, gdouble phase);

gdouble ags_synth_generator_get_volume(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_volume(AgsSynthGenerator *synth_generator, gdouble volume);

gdouble ags_synth_generator_get_lfo_depth(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_lfo_depth(AgsSynthGenerator *synth_generator, gdouble lfo_depth);

gdouble ags_synth_generator_get_tuning(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_tuning(AgsSynthGenerator *synth_generator, gdouble tuning);

gboolean ags_synth_generator_get_do_fm_synth(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_do_fm_synth(AgsSynthGenerator *synth_generator, gboolean do_fm_synth);

guint ags_synth_generator_get_fm_lfo_oscillator(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_fm_lfo_oscillator(AgsSynthGenerator *synth_generator, guint fm_lfo_oscillator);

gdouble ags_synth_generator_get_fm_lfo_frequency(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_fm_lfo_frequency(AgsSynthGenerator *synth_generator, gdouble fm_lfo_frequency);

gdouble ags_synth_generator_get_fm_lfo_depth(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_fm_lfo_depth(AgsSynthGenerator *synth_generator, gdouble fm_lfo_depth);

gdouble ags_synth_generator_get_fm_tuning(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_fm_tuning(AgsSynthGenerator *synth_generator, gdouble fm_tuning);

AgsTimestamp* ags_synth_generator_get_timestamp(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_timestamp(AgsSynthGenerator *synth_generator,
				       AgsTimestamp *timestamp);

void ags_synth_generator_compute(AgsSynthGenerator *synth_generator,
				 GObject *audio_signal,
				 gdouble note);

AgsSynthGenerator* ags_synth_generator_new();

G_END_DECLS

#endif /*__AGS_SYNTH_GENERATOR_H__*/
