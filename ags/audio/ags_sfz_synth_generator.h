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

#ifndef __AGS_SFZ_SYNTH_GENERATOR_H__
#define __AGS_SFZ_SYNTH_GENERATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_SYNTH_GENERATOR                (ags_sfz_synth_generator_get_type())
#define AGS_SFZ_SYNTH_GENERATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_SYNTH_GENERATOR, AgsSFZSynthGenerator))
#define AGS_SFZ_SYNTH_GENERATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_SYNTH_GENERATOR, AgsSFZSynthGeneratorClass))
#define AGS_IS_SFZ_SYNTH_GENERATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SFZ_SYNTH_GENERATOR))
#define AGS_IS_SFZ_SYNTH_GENERATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SFZ_SYNTH_GENERATOR))
#define AGS_SFZ_SYNTH_GENERATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SFZ_SYNTH_GENERATOR, AgsSFZSynthGeneratorClass))

#define AGS_SFZ_SYNTH_GENERATOR_GET_OBJ_MUTEX(obj) (&(((AgsSFZSynthGenerator *) obj)->obj_mutex))

#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_SAMPLERATE ((guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE)
#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE (AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE)
#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_FORMAT (AGS_SOUNDCARD_DEFAULT_FORMAT)

#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_VOLUME (1.0)

#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_BASE_KEY (-48.0)
#define AGS_SFZ_SYNTH_GENERATOR_DEFAULT_TUNING (0.0)

typedef struct _AgsSFZSynthGenerator AgsSFZSynthGenerator;
typedef struct _AgsSFZSynthGeneratorClass AgsSFZSynthGeneratorClass;

struct _AgsSFZSynthGenerator
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

  gdouble volume;

  gdouble base_key;
  gdouble tuning;
  
  GObject *timestamp;
};

struct _AgsSFZSynthGeneratorClass
{
  AgsFunctionClass function;
};
  
GType ags_sfz_synth_generator_get_type();

GRecMutex* ags_sfz_synth_generator_get_obj_mutex(AgsSFZSynthGenerator *sfz_synth_generator);

gchar* ags_sfz_synth_generator_get_filename(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_filename(AgsSFZSynthGenerator *sfz_synth_generator, gchar *filename);

guint ags_sfz_synth_generator_get_samplerate(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_samplerate(AgsSFZSynthGenerator *sfz_synth_generator, guint samplerate);

guint ags_sfz_synth_generator_get_buffer_size(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_buffer_size(AgsSFZSynthGenerator *sfz_synth_generator, guint buffer_size);

guint ags_sfz_synth_generator_get_format(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_format(AgsSFZSynthGenerator *sfz_synth_generator, guint format);

gdouble ags_sfz_synth_generator_get_delay(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_delay(AgsSFZSynthGenerator *sfz_synth_generator, gdouble delay);

guint ags_sfz_synth_generator_get_frame_count(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_frame_count(AgsSFZSynthGenerator *sfz_synth_generator, guint frame_count);

guint ags_sfz_synth_generator_get_loop_start(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_loop_start(AgsSFZSynthGenerator *sfz_synth_generator, guint loop_start);

guint ags_sfz_synth_generator_get_loop_end(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_loop_end(AgsSFZSynthGenerator *sfz_synth_generator, guint loop_end);

gdouble ags_sfz_synth_generator_get_base_key(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_base_key(AgsSFZSynthGenerator *sfz_synth_generator, gdouble base_key);

gdouble ags_sfz_synth_generator_get_tuning(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_tuning(AgsSFZSynthGenerator *sfz_synth_generator, gdouble tuning);

AgsTimestamp* ags_sfz_synth_generator_get_timestamp(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_timestamp(AgsSFZSynthGenerator *sfz_synth_generator,
					   AgsTimestamp *timestamp);

void ags_sfz_synth_generator_compute(AgsSFZSynthGenerator *sfz_synth_generator,
				     GObject *audio_signal,
				     gdouble note);

AgsSFZSynthGenerator* ags_sfz_synth_generator_new();

G_END_DECLS

#endif /*__AGS_SFZ_SYNTH_GENERATOR_H__*/
