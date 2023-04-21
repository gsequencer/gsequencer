/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_TIME_STRETCH_UTIL_H__
#define __AGS_TIME_STRETCH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_TIME_STRETCH_UTIL         (ags_time_stretch_util_get_type())
#define AGS_TIME_STRETCH_UTIL(ptr) ((AgsTimeStretchUtil *)(ptr))

#define AGS_TIME_STRETCH_UTIL_DEFAULT_FREQUENCY (440.0)

typedef struct _AgsTimeStretchUtil AgsTimeStretchUtil;

struct _AgsTimeStretchUtil
{
  gpointer source;
  guint source_stride;
  guint source_buffer_length;

  gpointer destination;
  guint destination_stride;
  guint destination_buffer_length;
  
  guint buffer_size;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble frequency;
  
  gdouble orig_bpm;
  gdouble new_bpm;  
};

GType ags_time_stretch_util_get_type(void);

AgsTimeStretchUtil* ags_time_stretch_util_alloc();

gpointer ags_time_stretch_util_copy(AgsTimeStretchUtil *ptr);
void ags_time_stretch_util_free(AgsTimeStretchUtil *ptr);

gpointer ags_time_stretch_util_get_source(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_source(AgsTimeStretchUtil *time_stretch_util,
				      gpointer source);

guint ags_time_stretch_util_get_source_stride(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_source_stride(AgsTimeStretchUtil *time_stretch_util,
					     guint source_stride);

guint ags_time_stretch_util_get_source_buffer_length(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_source_buffer_length(AgsTimeStretchUtil *time_stretch_util,
						    guint source_buffer_length);

gpointer ags_time_stretch_util_get_destination(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_destination(AgsTimeStretchUtil *time_stretch_util,
					   gpointer destination);

guint ags_time_stretch_util_get_destination_stride(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_destination_stride(AgsTimeStretchUtil *time_stretch_util,
						  guint destination_stride);

guint ags_time_stretch_util_get_destination_buffer_length(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_destination_buffer_length(AgsTimeStretchUtil *time_stretch_util,
							 guint destination_buffer_length);

guint ags_time_stretch_util_get_buffer_size(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_buffer_size(AgsTimeStretchUtil *time_stretch_util,
					   guint buffer_size);

AgsSoundcardFormat ags_time_stretch_util_get_format(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_format(AgsTimeStretchUtil *time_stretch_util,
				      AgsSoundcardFormat format);

guint ags_time_stretch_util_get_samplerate(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_samplerate(AgsTimeStretchUtil *time_stretch_util,
					  guint samplerate);

gdouble ags_time_stretch_util_get_frequency(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_frequency(AgsTimeStretchUtil *time_stretch_util,
					 gdouble frequency);

gdouble ags_time_stretch_util_get_orig_bpm(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_orig_bpm(AgsTimeStretchUtil *time_stretch_util,
					gdouble orig_bpm);

gdouble ags_time_stretch_util_get_new_bpm(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_set_new_bpm(AgsTimeStretchUtil *time_stretch_util,
				       gdouble new_bpm);

void ags_time_stretch_util_stretch_s8(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_s16(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_s24(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_s32(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_s64(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_float(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_double(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch_complex(AgsTimeStretchUtil *time_stretch_util);
void ags_time_stretch_util_stretch(AgsTimeStretchUtil *time_stretch_util);

G_END_DECLS

#endif /*__AGS_TIME_STRETCH_UTIL_H__*/
