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

#ifndef __AGS_SF2_SYNTH_GENERATOR_H__
#define __AGS_SF2_SYNTH_GENERATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SF2_SYNTH_GENERATOR                (ags_sf2_synth_generator_get_type())
#define AGS_SF2_SYNTH_GENERATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SF2_SYNTH_GENERATOR, AgsSF2SynthGenerator))
#define AGS_SF2_SYNTH_GENERATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SF2_SYNTH_GENERATOR, AgsSF2SynthGeneratorClass))
#define AGS_IS_SF2_SYNTH_GENERATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SF2_SYNTH_GENERATOR))
#define AGS_IS_SF2_SYNTH_GENERATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SF2_SYNTH_GENERATOR))
#define AGS_SF2_SYNTH_GENERATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SF2_SYNTH_GENERATOR, AgsSF2SynthGeneratorClass))

#define AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(obj) (&(((AgsSF2SynthGenerator *) obj)->obj_mutex))

#define AGS_SF2_SYNTH_GENERATOR_DEFAULT_SAMPLERATE ((guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE)
#define AGS_SF2_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE (AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE)
#define AGS_SF2_SYNTH_GENERATOR_DEFAULT_FORMAT (AGS_SOUNDCARD_DEFAULT_FORMAT)

#define AGS_SF2_SYNTH_GENERATOR_DEFAULT_BASE_KEY (-48.0)
#define AGS_SF2_SYNTH_GENERATOR_DEFAULT_TUNING (0.0)

typedef struct _AgsSF2SynthGenerator AgsSF2SynthGenerator;
typedef struct _AgsSF2SynthGeneratorClass AgsSF2SynthGeneratorClass;

struct _AgsSF2SynthGenerator
{
  AgsFunction function;

  guint flags;
  
  GRecMutex obj_mutex;

  gchar *filename;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint frame_count;
  guint loop_start;
  guint loop_end;

  gdouble delay;
  guint attack;

  gdouble base_key;
  gdouble tuning;
  
  GObject *timestamp;
};

struct _AgsSF2SynthGeneratorClass
{
  AgsFunctionClass function;
};
  
GType ags_sf2_synth_generator_get_type();

GRecMutex* ags_sf2_synth_generator_get_obj_mutex(AgsSF2SynthGenerator *sf2_synth_generator);

gchar* ags_sf2_synth_generator_get_filename(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_filename(AgsSF2SynthGenerator *sf2_synth_generator, gchar *filename);

guint ags_sf2_synth_generator_get_samplerate(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_samplerate(AgsSF2SynthGenerator *sf2_synth_generator, guint samplerate);

guint ags_sf2_synth_generator_get_buffer_size(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_buffer_size(AgsSF2SynthGenerator *sf2_synth_generator, guint buffer_size);

guint ags_sf2_synth_generator_get_format(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_format(AgsSF2SynthGenerator *sf2_synth_generator, guint format);

gdouble ags_sf2_synth_generator_get_delay(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_delay(AgsSF2SynthGenerator *sf2_synth_generator, gdouble delay);

guint ags_sf2_synth_generator_get_frame_count(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_frame_count(AgsSF2SynthGenerator *sf2_synth_generator, guint frame_count);

guint ags_sf2_synth_generator_get_loop_start(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_loop_start(AgsSF2SynthGenerator *sf2_synth_generator, guint loop_start);

guint ags_sf2_synth_generator_get_loop_end(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_loop_end(AgsSF2SynthGenerator *sf2_synth_generator, guint loop_end);

gdouble ags_sf2_synth_generator_get_base_key(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_base_key(AgsSF2SynthGenerator *sf2_synth_generator, gdouble base_key);

gdouble ags_sf2_synth_generator_get_tuning(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_tuning(AgsSF2SynthGenerator *sf2_synth_generator, gdouble tuning);

AgsTimestamp* ags_sf2_synth_generator_get_timestamp(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_timestamp(AgsSF2SynthGenerator *sf2_synth_generator,
					   AgsTimestamp *timestamp);

void ags_sf2_synth_generator_compute(AgsSF2SynthGenerator *sf2_synth_generator,
				     GObject *audio_signal,
				     gdouble note);

AgsSF2SynthGenerator* ags_sf2_synth_generator_new();

G_END_DECLS

#endif /*__AGS_SF2_SYNTH_GENERATOR_H__*/
