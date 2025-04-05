/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_LOW_PASS_FILTER_UTIL_H__
#define __AGS_LOW_PASS_FILTER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_LOW_PASS_FILTER_UTIL         (ags_low_pass_filter_util_get_type())
#define AGS_LOW_PASS_FILTER_UTIL(ptr)         ((AgsLowPassFilterUtil *)(ptr))

#define AGS_LOW_PASS_FILTER_UTIL_INITIALIZER ((AgsLowPassFilterUtil) {	\
      .source = NULL,							\
      .source_stride = 1,						\
      .destination = NULL,						\
      .destination_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .cut_off_frequency = 2000.0,					\
      .filter_gain = 1.0,						\
      .no_clip = 0.0,							\
      .last_freq = 0.015811388,						\
      .last_gain = 0.0,							\
      .last_no_clip = 0.0,						\
      .converted_freq = 0.0,						\
      .converted_gain = 0.0,						\
      .converted_no_clip = 0.0 })

typedef struct _AgsLowPassFilterUtil AgsLowPassFilterUtil;

struct _AgsLowPassFilterUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
  
  gdouble cut_off_frequency;
  gdouble filter_gain;
  gdouble no_clip;

  gdouble last_freq;         
  gdouble last_gain;
  gdouble last_no_clip;

  gdouble converted_freq;         
  gdouble converted_gain;
  gdouble converted_no_clip;
};

GType ags_low_pass_filter_util_get_type(void);

AgsLowPassFilterUtil* ags_low_pass_filter_util_alloc();

gpointer ags_low_pass_filter_util_copy(AgsLowPassFilterUtil *ptr);
void ags_low_pass_filter_util_free(AgsLowPassFilterUtil *ptr);

gpointer ags_low_pass_filter_util_get_source(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_source(AgsLowPassFilterUtil *low_pass_filter_util,
					 gpointer source);

guint ags_low_pass_filter_util_get_source_stride(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_source_stride(AgsLowPassFilterUtil *low_pass_filter_util,
						guint source_stride);

gpointer ags_low_pass_filter_util_get_destination(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_destination(AgsLowPassFilterUtil *low_pass_filter_util,
					      gpointer destination);

guint ags_low_pass_filter_util_get_destination_stride(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_destination_stride(AgsLowPassFilterUtil *low_pass_filter_util,
						     guint destination_stride);

guint ags_low_pass_filter_util_get_buffer_length(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_buffer_length(AgsLowPassFilterUtil *low_pass_filter_util,
						guint buffer_length);

AgsSoundcardFormat ags_low_pass_filter_util_get_format(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_format(AgsLowPassFilterUtil *low_pass_filter_util,
					 AgsSoundcardFormat format);

guint ags_low_pass_filter_util_get_samplerate(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_samplerate(AgsLowPassFilterUtil *low_pass_filter_util,
					     guint samplerate);

gdouble ags_low_pass_filter_util_get_cut_off_frequency(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_cut_off_frequency(AgsLowPassFilterUtil *low_pass_filter_util,
						    gdouble cut_off_frequency);

gdouble ags_low_pass_filter_util_get_filter_gain(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_filter_gain(AgsLowPassFilterUtil *low_pass_filter_util,
					      gdouble filter_gain);

gdouble ags_low_pass_filter_util_get_no_clip(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_set_no_clip(AgsLowPassFilterUtil *low_pass_filter_util,
					  gdouble no_clip);

void ags_low_pass_filter_util_process_s8(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_s16(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_s24(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_s32(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_s64(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_float(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_double(AgsLowPassFilterUtil *low_pass_filter_util);
void ags_low_pass_filter_util_process_complex(AgsLowPassFilterUtil *low_pass_filter_util);

void ags_low_pass_filter_util_process(AgsLowPassFilterUtil *low_pass_filter_util);

G_END_DECLS

#endif /*__AGS_LOW_PASS_FILTER_UTIL_H__*/
