/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

typedef struct _AgsFrequencyAliaseUtil AgsFrequencyAliaseUtil;

struct _AgsFrequencyAliaseUtil
{
  //empty
};

GType ags_frequency_aliase_util_get_type(void);

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
