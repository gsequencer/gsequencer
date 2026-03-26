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

#ifndef __AGS_AMPLIFIER10_UTIL_H__
#define __AGS_AMPLIFIER10_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_AMPLIFIER10_UTIL         (ags_amplifier10_util_get_type())
#define AGS_AMPLIFIER10_UTIL(ptr)         ((AgsAmplifier10Util *)(ptr))

#define AGS_AMPLIFIER10_UTIL_AMP_COUNT (10)

#define AGS_AMPLIFIER10_UTIL_INITIALIZER ((AgsAmplifier10Util) {	\
      .source = NULL,							\
	.source_stride = 1,						\
	.destination = NULL,						\
	.destination_stride = 1,					\
	.mix_buffer = NULL,						\
	.buffer_length = 0,						\
	.format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
	.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
	.amp_0_enabled = 1.0,						\
	.amp_0_frequency = 28.0,					\
	.amp_0_bandwidth = 0.5,						\
	.amp_0_gain = 0.0,						\
	.amp_1_enabled = 1.0,						\
	.amp_1_frequency =56.0,						\
	.amp_1_bandwidth = 0.5,						\
	.amp_1_gain = 0.0,						\
	.amp_2_enabled = 1.0,						\
	.amp_2_frequency = 112.0,					\
	.amp_2_bandwidth = 0.5,						\
	.amp_2_gain = 0.0,						\
	.amp_3_enabled = 1.0,						\
	.amp_3_frequency = 224.0,					\
	.amp_3_bandwidth = 0.5,						\
	.amp_3_gain = 0.0,						\
	.amp_4_enabled = 1.0,						\
	.amp_4_frequency = 448.0,					\
	.amp_4_bandwidth = 0.5,						\
	.amp_4_gain = 0.0,						\
	.amp_5_enabled = 1.0,						\
	.amp_5_frequency = 896.0,					\
	.amp_5_bandwidth = 0.5,						\
	.amp_5_gain = 0.0,						\
	.amp_6_enabled = 1.0,						\
	.amp_6_frequency = 1792.0,					\
	.amp_6_bandwidth = 0.5,						\
	.amp_6_gain = 0.0,						\
	.amp_7_enabled = 1.0,						\
	.amp_7_frequency = 3584.0,					\
	.amp_7_bandwidth = 0.5,						\
	.amp_7_gain = 0.0,						\
	.amp_8_enabled = 1.0,						\
	.amp_8_frequency = 7168.0,					\
	.amp_8_bandwidth = 0.5,						\
	.amp_8_gain = 0.0,						\
	.amp_9_enabled = 1.0,						\
	.amp_9_frequency = 14366.0,					\
	.amp_9_bandwidth = 0.5,						\
	.amp_9_gain = 0.0,						\
	.bypass = 0.0,							\
	.filter_gain = 0.0,						\
	.fade = 0,							\
	.proc_sect = { 0,} })

typedef struct _AgsAmplifier10UtilProc AgsAmplifier10UtilProc;
typedef struct _AgsAmplifier10Util AgsAmplifier10Util;

struct _AgsAmplifier10UtilProc
{
  gdouble f;
  gdouble b;
  gdouble g;

  gdouble a;

  gdouble s1;
  gdouble s2;

  gdouble z1;
  gdouble z2;
};

struct _AgsAmplifier10Util
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer mix_buffer;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble amp_0_enabled;
  gdouble amp_0_frequency;
  gdouble amp_0_bandwidth;
  gdouble amp_0_gain;

  gdouble amp_1_enabled;
  gdouble amp_1_frequency;
  gdouble amp_1_bandwidth;
  gdouble amp_1_gain;

  gdouble amp_2_enabled;
  gdouble amp_2_frequency;
  gdouble amp_2_bandwidth;
  gdouble amp_2_gain;

  gdouble amp_3_enabled;
  gdouble amp_3_frequency;
  gdouble amp_3_bandwidth;
  gdouble amp_3_gain;
  
  gdouble amp_4_enabled;
  gdouble amp_4_frequency;
  gdouble amp_4_bandwidth;
  gdouble amp_4_gain;
  
  gdouble amp_5_enabled;
  gdouble amp_5_frequency;
  gdouble amp_5_bandwidth;
  gdouble amp_5_gain;

  gdouble amp_6_enabled;
  gdouble amp_6_frequency;
  gdouble amp_6_bandwidth;
  gdouble amp_6_gain;

  gdouble amp_7_enabled;
  gdouble amp_7_frequency;
  gdouble amp_7_bandwidth;
  gdouble amp_7_gain;

  gdouble amp_8_enabled;
  gdouble amp_8_frequency;
  gdouble amp_8_bandwidth;
  gdouble amp_8_gain;
  
  gdouble amp_9_enabled;
  gdouble amp_9_frequency;
  gdouble amp_9_bandwidth;
  gdouble amp_9_gain;

  gdouble bypass;
  gdouble filter_gain;
  gint fade;
  
  AgsAmplifier10UtilProc proc_sect[AGS_AMPLIFIER10_UTIL_AMP_COUNT]; 
};

GType ags_amplifier10_util_get_type(void);

AgsAmplifier10Util* ags_amplifier10_util_alloc();

gpointer ags_amplifier10_util_copy(AgsAmplifier10Util *ptr);
void ags_amplifier10_util_free(AgsAmplifier10Util *ptr);

gpointer ags_amplifier10_util_get_source(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_source(AgsAmplifier10Util *amplifier10_util,
				     gpointer source);

guint ags_amplifier10_util_get_source_stride(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_source_stride(AgsAmplifier10Util *amplifier10_util,
					    guint source_stride);

gpointer ags_amplifier10_util_get_destination(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_destination(AgsAmplifier10Util *amplifier10_util,
					  gpointer destination);

guint ags_amplifier10_util_get_destination_stride(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_destination_stride(AgsAmplifier10Util *amplifier10_util,
						 guint destination_stride);

guint ags_amplifier10_util_get_buffer_length(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_buffer_length(AgsAmplifier10Util *amplifier10_util,
					    guint buffer_length);

AgsSoundcardFormat ags_amplifier10_util_get_format(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_format(AgsAmplifier10Util *amplifier10_util,
				     AgsSoundcardFormat format);

guint ags_amplifier10_util_get_samplerate(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_samplerate(AgsAmplifier10Util *amplifier10_util,
					 guint samplerate);

gdouble ags_amplifier10_util_get_amp_0_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_0_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_0_frequency);

gdouble ags_amplifier10_util_get_amp_0_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_0_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_0_bandwidth);

gdouble ags_amplifier10_util_get_amp_0_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_0_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_0_gain);

gdouble ags_amplifier10_util_get_amp_1_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_1_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_1_frequency);

gdouble ags_amplifier10_util_get_amp_1_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_1_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_1_bandwidth);

gdouble ags_amplifier10_util_get_amp_1_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_1_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_1_gain);

gdouble ags_amplifier10_util_get_amp_2_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_2_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_2_frequency);

gdouble ags_amplifier10_util_get_amp_2_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_2_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_2_bandwidth);

gdouble ags_amplifier10_util_get_amp_2_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_2_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_2_gain);

gdouble ags_amplifier10_util_get_amp_3_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_3_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_3_frequency);

gdouble ags_amplifier10_util_get_amp_3_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_3_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_3_bandwidth);

gdouble ags_amplifier10_util_get_amp_3_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_3_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_3_gain);

gdouble ags_amplifier10_util_get_amp_4_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_4_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_4_frequency);

gdouble ags_amplifier10_util_get_amp_4_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_4_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_4_bandwidth);

gdouble ags_amplifier10_util_get_amp_4_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_4_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_4_gain);

gdouble ags_amplifier10_util_get_amp_5_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_5_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_5_frequency);

gdouble ags_amplifier10_util_get_amp_5_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_5_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_5_bandwidth);

gdouble ags_amplifier10_util_get_amp_5_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_5_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_5_gain);

gdouble ags_amplifier10_util_get_amp_6_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_6_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_6_frequency);

gdouble ags_amplifier10_util_get_amp_6_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_6_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_6_bandwidth);

gdouble ags_amplifier10_util_get_amp_6_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_6_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_6_gain);

gdouble ags_amplifier10_util_get_amp_7_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_7_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_7_frequency);

gdouble ags_amplifier10_util_get_amp_7_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_7_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_7_bandwidth);

gdouble ags_amplifier10_util_get_amp_7_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_7_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_7_gain);

gdouble ags_amplifier10_util_get_amp_8_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_8_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_8_frequency);

gdouble ags_amplifier10_util_get_amp_8_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_8_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_8_bandwidth);

gdouble ags_amplifier10_util_get_amp_8_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_8_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_8_gain);

gdouble ags_amplifier10_util_get_amp_9_frequency(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_9_frequency(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_9_frequency);

gdouble ags_amplifier10_util_get_amp_9_bandwidth(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_9_bandwidth(AgsAmplifier10Util *amplifier10_util,
					      gdouble amp_9_bandwidth);

gdouble ags_amplifier10_util_get_amp_9_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_amp_9_gain(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_9_gain);

gdouble ags_amplifier10_util_get_filter_gain(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_set_filter_gain(AgsAmplifier10Util *amplifier10_util,
					  gdouble filter_gain);

void ags_amplifier10_util_process_s8(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_s16(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_s24(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_s32(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_s64(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_float(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_double(AgsAmplifier10Util *amplifier10_util);
void ags_amplifier10_util_process_complex(AgsAmplifier10Util *amplifier10_util);

void ags_amplifier10_util_process(AgsAmplifier10Util *amplifier10_util);

G_END_DECLS

#endif /*__AGS_AMPLIFIER10_UTIL_H__*/
