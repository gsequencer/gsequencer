/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_LINEAR_INTERPOLATE_UTIL_H__
#define __AGS_LINEAR_INTERPOLATE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_LINEAR_INTERPOLATE_UTIL         (ags_linear_interpolate_util_get_type())
#define AGS_LINEAR_INTERPOLATE_UTIL(ptr) ((AgsLinearInterpolateUtil *)(ptr))

#define AGS_LINEAR_UTIL_INITIALIZER ((AgsLinearInterpolateUtil) {	\
      .source = NULL,							\
      .source_stride = 1,						\
      .destination = NULL,						\
      .destination_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .factor = 0.0 })

typedef struct _AgsLinearInterpolateUtil AgsLinearInterpolateUtil;

struct _AgsLinearInterpolateUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  guint format;
  guint samplerate;

  gdouble factor;
};

GType ags_linear_interpolate_util_get_type(void);

AgsLinearInterpolateUtil* ags_linear_interpolate_util_alloc();

gpointer ags_linear_interpolate_util_copy(AgsLinearInterpolateUtil *ptr);
void ags_linear_interpolate_util_free(AgsLinearInterpolateUtil *ptr);

gpointer ags_linear_interpolate_util_get_source(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_source(AgsLinearInterpolateUtil *linear_interpolate_util,
					    gpointer source);

guint ags_linear_interpolate_util_get_source_stride(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_source_stride(AgsLinearInterpolateUtil *linear_interpolate_util,
						   guint source_stride);

gpointer ags_linear_interpolate_util_get_destination(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_destination(AgsLinearInterpolateUtil *linear_interpolate_util,
						 gpointer destination);

guint ags_linear_interpolate_util_get_destination_stride(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_destination_stride(AgsLinearInterpolateUtil *linear_interpolate_util,
							guint destination_stride);

guint ags_linear_interpolate_util_get_buffer_length(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_buffer_length(AgsLinearInterpolateUtil *linear_interpolate_util,
						   guint buffer_length);

guint ags_linear_interpolate_util_get_format(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_format(AgsLinearInterpolateUtil *linear_interpolate_util,
					    guint format);

guint ags_linear_interpolate_util_get_samplerate(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_samplerate(AgsLinearInterpolateUtil *linear_interpolate_util,
						guint samplerate);

gdouble ags_linear_interpolate_util_get_factor(AgsLinearInterpolateUtil *linear_interpolate_util);
void ags_linear_interpolate_util_set_factor(AgsLinearInterpolateUtil *linear_interpolate_util,
					    gdouble factor);

void ags_linear_interpolate_util_pitch(AgsLinearInterpolateUtil *linear_interpolate_util);

G_END_DECLS

#endif /*__AGS_LINEAR_INTERPOLATE_UTIL_H__*/
