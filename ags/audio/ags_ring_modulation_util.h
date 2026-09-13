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

#ifndef __AGS_RING_MODULATION_UTIL_H__
#define __AGS_RING_MODULATION_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_RING_MODULATION_UTIL         (ags_ring_modulation_util_get_type())
#define AGS_RING_MODULATION_UTIL(ptr) ((AgsRingModulationUtil *)(ptr))

#define AGS_RING_MODULATION_UTIL_INITIALIZER ((AgsRingModulationUtil) { \
      .modulator_source = NULL,						\
	.modulator_source_stride = 1,					\
	.carrier_source = NULL,						\
	.carrier_source_stride = 1,					\
	.destination = NULL,						\
	.destination_stride = 1,					\
	.buffer_length = 0,						\
	.format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
	.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
	.pitch_util = NULL,						\
	.pitch_buffer = NULL,						\
	.pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL,	\
	.pitch_base_key = 440.0,					\
	.pitch_tuning = 0.0,						\
	.drive = 1.0,							\
	.mix = 0.0,							\
	.gain = 1.0,							\
	.audio_buffer_util = NULL })

typedef struct _AgsRingModulationUtil AgsRingModulationUtil;

struct _AgsRingModulationUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer modulator_source;
  guint modulator_source_stride;

  gpointer carrier_source;
  guint carrier_source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gpointer pitch_util;
  
  gpointer pitch_buffer;

  GType pitch_type;
  
  gdouble pitch_base_key;
  gdouble pitch_tuning;
  
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  AgsAudioBufferUtil *audio_buffer_util;
};

GType ags_ring_modulation_util_get_type(void);

AgsRingModulationUtil* ags_ring_modulation_util_alloc();

gpointer ags_ring_modulation_util_copy(AgsRingModulationUtil *ptr);
void ags_ring_modulation_util_free(AgsRingModulationUtil *ptr);

/* getter/setter */
gpointer ags_ring_modulation_util_get_destination(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_destination(AgsRingModulationUtil *ring_modulation_util,
					      gpointer destination);

guint ags_ring_modulation_util_get_destination_stride(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_destination_stride(AgsRingModulationUtil *ring_modulation_util,
						     guint destination_stride);

gpointer ags_ring_modulation_util_get_modulator_source(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_modulator_source(AgsRingModulationUtil *ring_modulation_util,
						   gpointer modulator_source);

guint ags_ring_modulation_util_get_modulator_source_stride(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_modulator_source_stride(AgsRingModulationUtil *ring_modulation_util,
							  guint modulator_source_stride);

gpointer ags_ring_modulation_util_get_carrier_source(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_carrier_source(AgsRingModulationUtil *ring_modulation_util,
						 gpointer carrier_source);

guint ags_ring_modulation_util_get_carrier_source_stride(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_carrier_source_stride(AgsRingModulationUtil *ring_modulation_util,
							guint carrier_source_stride);

guint ags_ring_modulation_util_get_buffer_length(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_buffer_length(AgsRingModulationUtil *ring_modulation_util,
						guint buffer_length);

AgsSoundcardFormat ags_ring_modulation_util_get_format(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_format(AgsRingModulationUtil *ring_modulation_util,
					 AgsSoundcardFormat format);

gdouble ags_ring_modulation_util_get_pitch_tuning(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_pitch_tuning(AgsRingModulationUtil *ring_modulation_util,
					       gdouble pitch_tuning);

gdouble ags_ring_modulation_util_get_drive(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_drive(AgsRingModulationUtil *ring_modulation_util,
					gdouble drive);

gdouble ags_ring_modulation_util_get_mix(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_mix(AgsRingModulationUtil *ring_modulation_util,
				      gdouble mix);

gdouble ags_ring_modulation_util_get_gain(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_set_gain(AgsRingModulationUtil *ring_modulation_util,
				       gdouble gain);

/* compute */
void ags_ring_modulation_util_compute_s8(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_s16(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_s24(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_s32(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_s64(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_float(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_double(AgsRingModulationUtil *ring_modulation_util);
void ags_ring_modulation_util_compute_complex(AgsRingModulationUtil *ring_modulation_util);

void ags_ring_modulation_util_compute(AgsRingModulationUtil *ring_modulation_util);

G_END_DECLS

#endif /*__AGS_RING_MODULATION_UTIL_H__*/
