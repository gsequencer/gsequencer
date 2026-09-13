/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_ABYSS_SYNTH_UTIL_H__
#define __AGS_ABYSS_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_ABYSS_SYNTH_UTIL         (ags_abyss_synth_util_get_type())
#define AGS_ABYSS_SYNTH_UTIL(ptr) ((AgsAbyssSynthUtil *)(ptr))

#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_OSCILLATOR (AGS_SYNTH_OSCILLATOR_SIN)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_FREQUENCY (440.0)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_PHASE (0.0)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_VOLUME (1.0)

#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_PITCH_TUNING (0.0)

#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_VOLUME (1.0)

#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_OSCILLATOR (AGS_SYNTH_OSCILLATOR_SIN)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (6.0)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING (0.0)

#define AGS_ABYSS_SYNTH_SENDS_COUNT (8)

#define AGS_ABYSS_SYNTH_UTIL_INITIALIZER ((AgsAbyssSynthUtil) {		\
      .source = NULL,							\
	.source_stride = 1,						\
	.buffer_length = 0,						\
	.format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
	.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
	.osc_0_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_OSCILLATOR, \
	.osc_0_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_FREQUENCY,	\
	.osc_0_phase = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_PHASE,		\
	.osc_0_volume = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_VOLUME,	\
	.osc_1_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_OSCILLATOR, \
	.osc_1_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_FREQUENCY,	\
	.osc_1_phase = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_PHASE,		\
	.osc_1_volume = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_VOLUME,	\
	.osc_2_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_OSCILLATOR, \
	.osc_2_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_FREQUENCY,	\
	.osc_2_phase = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_PHASE,		\
	.osc_2_volume = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_VOLUME,	\
	.osc_3_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_OSCILLATOR, \
	.osc_3_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_FREQUENCY,	\
	.osc_3_phase = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_PHASE,		\
	.osc_3_volume = AGS_ABYSS_SYNTH_UTIL_DEFAULT_OSC_VOLUME,	\
	.ring_0_pitch_tuning = 0.0,					\
	.ring_0_drive = 1.0,						\
	.ring_0_mix = 0.0,						\
	.ring_0_gain = 1.0,						\
	.ring_1_pitch_tuning = 0.0,					\
	.ring_1_drive = 1.0,						\
	.ring_1_mix = 0.0,						\
	.ring_1_gain = 1.0,						\
	.pitch_util = NULL,						\
	.pitch_buffer = NULL,						\
	.pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL,	\
	.pitch_base_key = 440.0,					\
	.pitch_tuning = AGS_ABYSS_SYNTH_UTIL_DEFAULT_PITCH_TUNING,	\
	.volume = AGS_ABYSS_SYNTH_UTIL_DEFAULT_VOLUME,			\
	.env_0_util = NULL,						\
	.env_0_attack = 1.0,						\
	.env_0_decay = 1.0,						\
	.env_0_sustain = 1.0,						\
	.env_0_release = 1.0,						\
	.env_0_gain = 1.0,						\
	.env_0_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.env_0_sends = {0,},						\
	.env_0_buffer = NULL,						\
	.env_1_util = NULL,						\
	.env_1_attack = 1.0,						\
	.env_1_decay = 1.0,						\
	.env_1_sustain = 1.0,						\
	.env_1_release = 1.0,						\
	.env_1_gain = 1.0,						\
	.env_1_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.env_1_sends = {0,},						\
	.env_1_buffer = NULL,						\
	.env_2_util = NULL,						\
	.env_2_attack = 1.0,						\
	.env_2_decay = 1.0,						\
	.env_2_sustain = 1.0,						\
	.env_2_release = 1.0,						\
	.env_2_gain = 1.0,						\
	.env_2_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.env_2_sends = {0,},						\
	.env_3_buffer = NULL,						\
	.env_3_util = NULL,						\
	.env_3_attack = 1.0,						\
	.env_3_decay = 1.0,						\
	.env_3_sustain = 1.0,						\
	.env_3_release = 1.0,						\
	.env_3_gain = 1.0,						\
	.env_3_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.env_3_sends = {0,},						\
	.env_3_buffer = NULL,						\
	.lfo_0_util = NULL,						\
	.lfo_0_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_OSCILLATOR, \
	.lfo_0_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.lfo_0_depth = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
	.lfo_0_tuning = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING,	\
	.lfo_0_sends = {0,},						\
	.lfo_0_buffer = NULL,						\
	.lfo_1_util = NULL,						\
	.lfo_1_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_OSCILLATOR, \
	.lfo_1_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.lfo_1_depth = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
	.lfo_1_tuning = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING,	\
	.lfo_1_sends = {0,},						\
	.lfo_1_buffer = NULL,						\
	.lfo_2_util = NULL,						\
	.lfo_2_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_OSCILLATOR, \
	.lfo_2_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.lfo_2_depth = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
	.lfo_2_tuning = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING,	\
	.lfo_2_sends = {0,},						\
	.lfo_2_buffer = NULL,						\
	.lfo_3_util = NULL,						\
	.lfo_3_oscillator = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_OSCILLATOR, \
	.lfo_3_frequency = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
	.lfo_3_depth = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
	.lfo_3_tuning = AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING,	\
	.lfo_3_sends = {0,},						\
	.lfo_3_buffer = NULL,						\
	.noise_0_util = NULL,						\
	.noise_0_frequency = 220.0,					\
	.noise_0_gain = 1.0,						\
	.noise_0_sends = {0,},						\
	.noise_0_buffer = NULL,						\
	.noise_1_util = NULL,						\
	.noise_1_frequency = 220.0,					\
	.noise_1_gain = 1.0,						\
	.noise_1_sends = {0,},						\
	.noise_1_buffer = NULL,						\
	.frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),	\
	.offset = 0,							\
	.note_256th_mode = TRUE,					\
	.offset_256th = 0 })

typedef struct _AgsAbyssSynthUtil AgsAbyssSynthUtil;

typedef enum{
  AGS_ABYSS_SYNTH_SENDS_OSC_0_FREQUENCY         = 1,
  AGS_ABYSS_SYNTH_SENDS_OSC_0_PHASE             = 1 <<  1,
  AGS_ABYSS_SYNTH_SENDS_OSC_0_VOLUME            = 1 <<  2,
  AGS_ABYSS_SYNTH_SENDS_OSC_1_FREQUENCY         = 1 <<  3,
  AGS_ABYSS_SYNTH_SENDS_OSC_1_PHASE             = 1 <<  4,
  AGS_ABYSS_SYNTH_SENDS_OSC_1_VOLUME            = 1 <<  5,
  AGS_ABYSS_SYNTH_SENDS_OSC_2_FREQUENCY         = 1 <<  6,
  AGS_ABYSS_SYNTH_SENDS_OSC_2_PHASE             = 1 <<  7,
  AGS_ABYSS_SYNTH_SENDS_OSC_2_VOLUME            = 1 <<  8,
  AGS_ABYSS_SYNTH_SENDS_OSC_3_FREQUENCY         = 1 <<  9,
  AGS_ABYSS_SYNTH_SENDS_OSC_3_PHASE             = 1 << 10,
  AGS_ABYSS_SYNTH_SENDS_OSC_3_VOLUME            = 1 << 11,
  AGS_ABYSS_SYNTH_SENDS_RING_0_PITCH_TUNING     = 1 << 12,
  AGS_ABYSS_SYNTH_SENDS_RING_0_DRIVE            = 1 << 13,
  AGS_ABYSS_SYNTH_SENDS_RING_0_MIX              = 1 << 14,
  AGS_ABYSS_SYNTH_SENDS_RING_0_GAIN             = 1 << 15,
  AGS_ABYSS_SYNTH_SENDS_RING_1_PITCH_TUNING     = 1 << 16,
  AGS_ABYSS_SYNTH_SENDS_RING_1_DRIVE            = 1 << 17,
  AGS_ABYSS_SYNTH_SENDS_RING_1_MIX              = 1 << 18,
  AGS_ABYSS_SYNTH_SENDS_RING_1_GAIN             = 1 << 19,
  AGS_ABYSS_SYNTH_SENDS_PITCH_TUNING            = 1 << 20,
  AGS_ABYSS_SYNTH_SENDS_VOLUME                  = 1 << 21,
}AgsAbyssSynthSends;

struct _AgsAbyssSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  AgsSynthOscillatorMode osc_0_oscillator;
  
  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  AgsSynthOscillatorMode osc_1_oscillator;
  
  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  AgsSynthOscillatorMode osc_2_oscillator;
  
  gdouble osc_2_frequency;
  gdouble osc_2_phase;
  gdouble osc_2_volume;

  AgsSynthOscillatorMode osc_3_oscillator;
  
  gdouble osc_3_frequency;
  gdouble osc_3_phase;
  gdouble osc_3_volume;

  gdouble ring_0_pitch_tuning;
  gdouble ring_0_drive;
  gdouble ring_0_mix;
  gdouble ring_0_gain;

  gdouble ring_1_pitch_tuning;
  gdouble ring_1_drive;
  gdouble ring_1_mix;
  gdouble ring_1_gain;
  
  gpointer pitch_util;
  
  gpointer pitch_buffer;

  GType pitch_type;
  
  gdouble pitch_base_key;
  gdouble pitch_tuning;
  
  gdouble volume;

  gpointer env_0_util;

  gdouble env_0_attack;
  gdouble env_0_decay;
  gdouble env_0_sustain;
  gdouble env_0_release;
  gdouble env_0_gain;
  gdouble env_0_frequency;

  gint64 env_0_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];

  gpointer env_0_buffer;
  
  gpointer env_1_util;

  gdouble env_1_attack;
  gdouble env_1_decay;
  gdouble env_1_sustain;
  gdouble env_1_release;
  gdouble env_1_gain;
  gdouble env_1_frequency;

  gint64 env_1_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];

  gpointer env_1_buffer;
  
  gpointer env_2_util;

  gdouble env_2_attack;
  gdouble env_2_decay;
  gdouble env_2_sustain;
  gdouble env_2_release;
  gdouble env_2_gain;
  gdouble env_2_frequency;

  gint64 env_2_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];

  gpointer env_2_buffer;
  
  gpointer env_3_util;

  gdouble env_3_attack;
  gdouble env_3_decay;
  gdouble env_3_sustain;
  gdouble env_3_release;
  gdouble env_3_gain;
  gdouble env_3_frequency;

  gint64 env_3_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];

  gpointer env_3_buffer;
  
  gpointer lfo_0_util;

  AgsSynthOscillatorMode lfo_0_oscillator;
  gdouble lfo_0_frequency;
  gdouble lfo_0_depth;
  gdouble lfo_0_tuning;

  gint64 lfo_0_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer lfo_0_buffer;

  gpointer lfo_1_util;

  AgsSynthOscillatorMode lfo_1_oscillator;
  gdouble lfo_1_frequency;
  gdouble lfo_1_depth;
  gdouble lfo_1_tuning;

  gint64 lfo_1_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer lfo_1_buffer;
  
  gpointer lfo_2_util;

  AgsSynthOscillatorMode lfo_2_oscillator;
  gdouble lfo_2_frequency;
  gdouble lfo_2_depth;
  gdouble lfo_2_tuning;

  gint64 lfo_2_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer lfo_2_buffer;
  
  gpointer lfo_3_util;

  AgsSynthOscillatorMode lfo_3_oscillator;
  gdouble lfo_3_frequency;
  gdouble lfo_3_depth;
  gdouble lfo_3_tuning;

  gint64 lfo_3_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer lfo_3_buffer;
  
  gpointer noise_0_util;
  
  gdouble noise_0_frequency;
  gdouble noise_0_gain;

  gint64 noise_0_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer noise_0_buffer;
  
  gpointer noise_1_util;
  
  gdouble noise_1_frequency;
  gdouble noise_1_gain;

  gint64 noise_1_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];
  
  gpointer noise_1_buffer;

  guint frame_count;
  guint offset;

  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_abyss_synth_util_get_type(void);

AgsAbyssSynthUtil* ags_abyss_synth_util_alloc();

gpointer ags_abyss_synth_util_copy(AgsAbyssSynthUtil *ptr);
void ags_abyss_synth_util_free(AgsAbyssSynthUtil *ptr);

gpointer ags_abyss_synth_util_get_source(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_source(AgsAbyssSynthUtil *abyss_synth_util,
				     gpointer source);

guint ags_abyss_synth_util_get_source_stride(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_source_stride(AgsAbyssSynthUtil *abyss_synth_util,
					    guint source_stride);

guint ags_abyss_synth_util_get_buffer_length(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_buffer_length(AgsAbyssSynthUtil *abyss_synth_util,
					    guint buffer_length);

AgsSoundcardFormat ags_abyss_synth_util_get_format(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_format(AgsAbyssSynthUtil *abyss_synth_util,
				     AgsSoundcardFormat format);

guint ags_abyss_synth_util_get_samplerate(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_samplerate(AgsAbyssSynthUtil *abyss_synth_util,
					 guint samplerate);

AgsSynthOscillatorMode ags_abyss_synth_util_get_osc_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					       AgsSynthOscillatorMode osc_0_oscillator);

gdouble ags_abyss_synth_util_get_osc_0_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble osc_0_frequency);

gdouble ags_abyss_synth_util_get_osc_0_phase(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_0_phase(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble osc_0_phase);

gdouble ags_abyss_synth_util_get_osc_0_volume(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_0_volume(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble osc_0_volume);

AgsSynthOscillatorMode ags_abyss_synth_util_get_osc_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					       AgsSynthOscillatorMode osc_1_oscillator);

gdouble ags_abyss_synth_util_get_osc_1_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble osc_1_frequency);

gdouble ags_abyss_synth_util_get_osc_1_phase(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_1_phase(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble osc_1_phase);

gdouble ags_abyss_synth_util_get_osc_1_volume(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_osc_1_volume(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble osc_1_volume);

gdouble ags_abyss_synth_util_get_pitch_tuning(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_pitch_tuning(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble pitch_tuning);

gdouble ags_abyss_synth_util_get_volume(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_volume(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble volume);

gdouble ags_abyss_synth_util_get_env_0_attack(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_attack(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble env_0_attack);

gdouble ags_abyss_synth_util_get_env_0_sustain(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_sustain(AgsAbyssSynthUtil *abyss_synth_util,
					    gdouble env_0_sustain);

gdouble ags_abyss_synth_util_get_env_0_decay(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_decay(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble env_0_decay);

gdouble ags_abyss_synth_util_get_env_0_release(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_release(AgsAbyssSynthUtil *abyss_synth_util,
					    gdouble env_0_release);

gdouble ags_abyss_synth_util_get_env_0_gain(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_gain(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_0_gain);

gdouble ags_abyss_synth_util_get_env_0_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble env_0_frequency);

gint64* ags_abyss_synth_util_get_env_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
					     guint *env_0_sends_count);
void ags_abyss_synth_util_set_env_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
					  gint64 *env_0_sends,
					  guint env_0_sends_count);

gdouble ags_abyss_synth_util_get_env_1_attack(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_attack(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble env_1_attack);

gdouble ags_abyss_synth_util_get_env_1_sustain(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_sustain(AgsAbyssSynthUtil *abyss_synth_util,
					    gdouble env_1_sustain);

gdouble ags_abyss_synth_util_get_env_1_decay(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_decay(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble env_1_decay);

gdouble ags_abyss_synth_util_get_env_1_release(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_release(AgsAbyssSynthUtil *abyss_synth_util,
					    gdouble env_1_release);

gdouble ags_abyss_synth_util_get_env_1_gain(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_gain(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_1_gain);

gdouble ags_abyss_synth_util_get_env_1_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_env_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble env_1_frequency);

gint64* ags_abyss_synth_util_get_env_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
					     guint *env_1_sends_count);
void ags_abyss_synth_util_set_env_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
					  gint64 *env_1_sends,
					  guint env_1_sends_count);

AgsSynthOscillatorMode ags_abyss_synth_util_get_lfo_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					       AgsSynthOscillatorMode lfo_0_oscillator);

gdouble ags_abyss_synth_util_get_lfo_0_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble lfo_0_frequency);

gdouble ags_abyss_synth_util_get_lfo_0_depth(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_0_depth(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble lfo_0_depth);

gdouble ags_abyss_synth_util_get_lfo_0_tuning(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_0_tuning(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble lfo_0_tuning);

gint64* ags_abyss_synth_util_get_lfo_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
					     guint *lfo_0_sends_count);
void ags_abyss_synth_util_set_lfo_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
					  gint64 *lfo_0_sends,
					  guint lfo_0_sends_count);

AgsSynthOscillatorMode ags_abyss_synth_util_get_lfo_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					       AgsSynthOscillatorMode lfo_1_oscillator);

gdouble ags_abyss_synth_util_get_lfo_1_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble lfo_1_frequency);

gdouble ags_abyss_synth_util_get_lfo_1_depth(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_1_depth(AgsAbyssSynthUtil *abyss_synth_util,
					  gdouble lfo_1_depth);

gdouble ags_abyss_synth_util_get_lfo_1_tuning(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_lfo_1_tuning(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble lfo_1_tuning);

gint64* ags_abyss_synth_util_get_lfo_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
					     guint *lfo_1_sends_count);
void ags_abyss_synth_util_set_lfo_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
					  gint64 *lfo_1_sends,
					  guint lfo_1_sends_count);

gdouble ags_abyss_synth_util_get_noise_frequency(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_noise_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					      gdouble noise_frequency);

gdouble ags_abyss_synth_util_get_noise_gain(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_noise_gain(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble noise_gain);

gint64* ags_abyss_synth_util_get_noise_sends(AgsAbyssSynthUtil *abyss_synth_util,
					     guint *noise_sends_count);
void ags_abyss_synth_util_set_noise_sends(AgsAbyssSynthUtil *abyss_synth_util,
					  gint64 *noise_sends,
					  guint noise_sends_count);

guint ags_abyss_synth_util_get_frame_count(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_frame_count(AgsAbyssSynthUtil *abyss_synth_util,
					  guint frame_count);

guint ags_abyss_synth_util_get_offset(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_offset(AgsAbyssSynthUtil *abyss_synth_util,
				     guint offset);

gboolean ags_abyss_synth_util_get_note_256th_mode(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_note_256th_mode(AgsAbyssSynthUtil *abyss_synth_util,
					      gboolean note_256th_mode);

guint ags_abyss_synth_util_get_offset_256th(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_set_offset_256th(AgsAbyssSynthUtil *abyss_synth_util,
					   guint offset_256th);

/* compute */
void ags_abyss_synth_util_compute_s8(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_s16(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_s24(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_s32(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_s64(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_float(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_double(AgsAbyssSynthUtil *abyss_synth_util);
void ags_abyss_synth_util_compute_complex(AgsAbyssSynthUtil *abyss_synth_util);

void ags_abyss_synth_util_compute(AgsAbyssSynthUtil *abyss_synth_util);

G_END_DECLS

#endif /*__AGS_ABYSS_SYNTH_UTIL_H__*/
