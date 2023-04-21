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

#ifndef __AGS_PHASE_SHIFT_UTIL_H__
#define __AGS_PHASE_SHIFT_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_PHASE_SHIFT_UTIL         (ags_phase_shift_util_get_type())

#define AGS_PHASE_SHIFT_UTIL_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

typedef struct _AgsPhaseShiftUtil AgsPhaseShiftUtil;

struct _AgsPhaseShiftUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble frequency;
  gdouble amount;
  gdouble phase;
};

GType ags_phase_shift_util_get_type(void);

AgsPhaseShiftUtil* ags_phase_shift_util_alloc();

gpointer ags_phase_shift_util_copy(AgsPhaseShiftUtil *ptr);
void ags_phase_shift_util_free(AgsPhaseShiftUtil *ptr);

/* getter/setter */
gpointer ags_phase_shift_util_get_destination(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_destination(AgsPhaseShiftUtil *phase_shift_util,
					  gpointer destination);

guint ags_phase_shift_util_get_destination_stride(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_destination_stride(AgsPhaseShiftUtil *phase_shift_util,
						 guint destination_stride);

gpointer ags_phase_shift_util_get_source(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_source(AgsPhaseShiftUtil *phase_shift_util,
				     gpointer source);

guint ags_phase_shift_util_get_source_stride(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_source_stride(AgsPhaseShiftUtil *phase_shift_util,
					    guint source_stride);

guint ags_phase_shift_util_get_buffer_length(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_buffer_length(AgsPhaseShiftUtil *phase_shift_util,
					    guint buffer_length);

AgsSoundcardFormat ags_phase_shift_util_get_format(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_format(AgsPhaseShiftUtil *phase_shift_util,
				     AgsSoundcardFormat format);

gdouble ags_phase_shift_util_get_frequency(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_frequency(AgsPhaseShiftUtil *phase_shift_util,
					gdouble frequency);

gdouble ags_phase_shift_util_get_amount(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_amount(AgsPhaseShiftUtil *phase_shift_util,
				     gdouble amount);

gdouble ags_phase_shift_util_get_phase(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_set_phase(AgsPhaseShiftUtil *phase_shift_util,
				    gdouble phase);

void ags_phase_shift_util_process_s8(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_s16(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_s24(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_s32(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_s64(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_float(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_double(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process_complex(AgsPhaseShiftUtil *phase_shift_util);
void ags_phase_shift_util_process(AgsPhaseShiftUtil *phase_shift_util);

G_END_DECLS

#endif /*__AGS_PHASE_SHIFT_UTIL_H__*/
