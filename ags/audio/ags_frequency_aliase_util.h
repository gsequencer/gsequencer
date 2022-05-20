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

#ifndef __AGS_FREQUENCY_ALIASE_UTIL_H__
#define __AGS_FREQUENCY_ALIASE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FREQUENCY_ALIASE_UTIL         (ags_frequency_aliase_util_get_type())

#define AGS_FREQUENCY_ALIASE_UTIL_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

typedef struct _AgsFrequencyAliaseUtil AgsFrequencyAliaseUtil;

struct _AgsFrequencyAliaseUtil
{
  gpointer source;
  guint source_stride;

  gpointer phase_shifted_source;
  guint phase_shifted_source_stride;
  
  gpointer destination;
  guint destination_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;
};

GType ags_frequency_aliase_util_get_type(void);

AgsFrequencyAliaseUtil* ags_frequency_aliase_util_alloc();

gpointer ags_frequency_aliase_util_copy(AgsFrequencyAliaseUtil *ptr);
void ags_frequency_aliase_util_free(AgsFrequencyAliaseUtil *ptr);

/* getter/setter */
gpointer ags_frequency_aliase_util_get_destination(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_destination(AgsFrequencyAliaseUtil *frequency_aliase_util,
					       gpointer destination);

guint ags_frequency_aliase_util_get_destination_stride(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_destination_stride(AgsFrequencyAliaseUtil *frequency_aliase_util,
						      guint destination_stride);

gpointer ags_frequency_aliase_util_get_source(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_source(AgsFrequencyAliaseUtil *frequency_aliase_util,
					  gpointer source);

guint ags_frequency_aliase_util_get_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util,
						 guint source_stride);

gpointer ags_frequency_aliase_util_get_phase_shifted_source(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_phase_shifted_source(AgsFrequencyAliaseUtil *frequency_aliase_util,
							gpointer phase_shifted_source);

guint ags_frequency_aliase_util_get_phase_shifted_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_phase_shifted_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util,
							       guint phase_shifted_source_stride);

guint ags_frequency_aliase_util_get_buffer_length(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_buffer_length(AgsFrequencyAliaseUtil *frequency_aliase_util,
						 guint buffer_length);

guint ags_frequency_aliase_util_get_format(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_format(AgsFrequencyAliaseUtil *frequency_aliase_util,
					  guint format);

void ags_frequency_aliase_util_compute_s8(gint8 *destination,
					  gint8 *source,
					  gint8 *phase_shifted_source,
					  guint buffer_length);
void ags_frequency_aliase_util_compute_s16(gint16 *destination,
					   gint16 *source,
					   gint16 *phase_shifted_source,
					   guint buffer_length);
void ags_frequency_aliase_util_compute_s24(gint32 *destination,
					   gint32 *source,
					   gint32 *phase_shifted_source,
					   guint buffer_length);
void ags_frequency_aliase_util_compute_s32(gint32 *destination,
					   gint32 *source,
					   gint32 *phase_shifted_source,
					   guint buffer_length);
void ags_frequency_aliase_util_compute_s64(gint64 *destination,
					   gint64 *source,
					   gint64 *phase_shifted_source,
					   guint buffer_length);
void ags_frequency_aliase_util_compute_float(gfloat *destination,
					     gfloat *source,
					     gfloat *phase_shifted_source,
					     guint buffer_length);
void ags_frequency_aliase_util_compute_double(gdouble *destination,
					      gdouble *source,
					      gdouble *phase_shifted_source,
					      guint buffer_length);
void ags_frequency_aliase_util_compute_complex(AgsComplex *destination,
					       AgsComplex *source,
					       AgsComplex *phase_shifted_source,
					       guint buffer_length);

G_END_DECLS

#endif /*__AGS_FREQUENCY_ALIASE_UTIL_H__*/
