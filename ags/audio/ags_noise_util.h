/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_NOISE_UTIL_H__
#define __AGS_NOISE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOISE_UTIL         (ags_noise_util_get_type())

#define AGS_NOISE_UTIL_DEFAULT_FREQUENCY (440.0)

typedef struct _AgsNoiseUtil AgsNoiseUtil;

/**
 * AgsNoiseuUtilMode:
 * @AGS_NOISE_UTIL_WHITE_NOISE: white noise
 * @AGS_NOISE_UTIL_PINK_NOISE: pink noise
 * 
 * Enum values to specify noise mode.
 */
typedef enum{
  AGS_NOISE_UTIL_WHITE_NOISE,
  AGS_NOISE_UTIL_PINK_NOISE,
}AgsNoiseuUtilMode;

struct _AgsNoiseUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  guint format;
  guint samplerate;

  guint mode;
  
  gdouble volume;
  
  gdouble frequency;
};

GType ags_noise_util_get_type(void);

AgsNoiseUtil* ags_noise_util_alloc();

gpointer ags_noise_util_copy(AgsNoiseUtil *ptr);
void ags_noise_util_free(AgsNoiseUtil *ptr);

gpointer ags_noise_util_get_source(AgsNoiseUtil *noise_util);
void ags_noise_util_set_source(AgsNoiseUtil *noise_util,
			       gpointer source);

guint ags_noise_util_get_source_stride(AgsNoiseUtil *noise_util);
void ags_noise_util_set_source_stride(AgsNoiseUtil *noise_util,
				      guint source_stride);

gpointer ags_noise_util_get_destination(AgsNoiseUtil *noise_util);
void ags_noise_util_set_destination(AgsNoiseUtil *noise_util,
				    gpointer destination);

guint ags_noise_util_get_destination_stride(AgsNoiseUtil *noise_util);
void ags_noise_util_set_destination_stride(AgsNoiseUtil *noise_util,
					   guint destination_stride);

guint ags_noise_util_get_buffer_length(AgsNoiseUtil *noise_util);
void ags_noise_util_set_buffer_length(AgsNoiseUtil *noise_util,
				      guint buffer_length);

guint ags_noise_util_get_format(AgsNoiseUtil *noise_util);
void ags_noise_util_set_format(AgsNoiseUtil *noise_util,
			       guint format);

guint ags_noise_util_get_samplerate(AgsNoiseUtil *noise_util);
void ags_noise_util_set_samplerate(AgsNoiseUtil *noise_util,
				   guint samplerate);

gdouble ags_noise_util_get_volume(AgsNoiseUtil *noise_util);
void ags_noise_util_set_volume(AgsNoiseUtil *noise_util,
			       gdouble volume);

gdouble ags_noise_util_get_frequency(AgsNoiseUtil *noise_util);
void ags_noise_util_set_frequency(AgsNoiseUtil *noise_util,
				  gdouble frequency);

void ags_noise_util_compute_s8(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_s16(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_s24(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_s32(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_s64(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_float(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_double(AgsNoiseUtil *noise_util);
void ags_noise_util_compute_complex(AgsNoiseUtil *noise_util);
void ags_noise_util_compute(AgsNoiseUtil *noise_util);

G_END_DECLS

#endif /*__AGS_NOISE_UTIL_H__*/
