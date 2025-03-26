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

#ifndef __AGS_AMPLIFIER_UTIL_H__
#define __AGS_AMPLIFIER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_AMPLIFIER_UTIL         (ags_amplifier_util_get_type())
#define AGS_AMPLIFIER_UTIL(ptr)         ((AgsAmplifierUtil *)(ptr))

#define AGS_AMPLIFIER_UTIL_AMP_COUNT (4)

#define AGS_AMPLIFIER_UTIL_INITIALIZER ((AgsAmplifierUtil) {	\
      .source = NULL,						\
      .source_stride = 1,					\
      .destination = NULL,					\
      .destination_stride = 1,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,		\
      .amp_0_frequency = 160.0,					\
      .amp_0_bandwidth = 1.0,					\
      .amp_0_gain = 1.0,					\
      .amp_1_frequency = 386.0,					\
      .amp_1_bandwidth = 1.0,					\
      .amp_1_gain = 1.0,					\
      .amp_2_frequency = 1250.0,				\
      .amp_2_bandwidth = 1.0,					\
      .amp_2_gain = 1.0,					\
      .amp_3_frequency = 4975.0,				\
      .amp_3_bandwidth = 1.0,					\
      .amp_3_gain = 1.0,					\
      .filter_gain = 1.0,					\
      .fade = 0,						\
      .proc_sect = { 0,} })

typedef struct _AgsAmplifierUtilProc AgsAmplifierUtilProc;
typedef struct _AgsAmplifierUtil AgsAmplifierUtil;

struct _AgsAmplifierUtilProc
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

struct _AgsAmplifierUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble amp_0_frequency;
  gdouble amp_0_bandwidth;
  gdouble amp_0_gain;

  gdouble amp_1_frequency;
  gdouble amp_1_bandwidth;
  gdouble amp_1_gain;

  gdouble amp_2_frequency;
  gdouble amp_2_bandwidth;
  gdouble amp_2_gain;

  gdouble amp_3_frequency;
  gdouble amp_3_bandwidth;
  gdouble amp_3_gain;

  gdouble filter_gain;
  gint fade;
  
  AgsAmplifierUtilProc proc_sect[AGS_AMPLIFIER_UTIL_AMP_COUNT]; 
};

GType ags_amplifier_util_get_type(void);

AgsAmplifierUtil* ags_amplifier_util_alloc();

gpointer ags_amplifier_util_copy(AgsAmplifierUtil *ptr);
void ags_amplifier_util_free(AgsAmplifierUtil *ptr);

gpointer ags_amplifier_util_get_source(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_source(AgsAmplifierUtil *amplifier_util,
				   gpointer source);

guint ags_amplifier_util_get_source_stride(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_source_stride(AgsAmplifierUtil *amplifier_util,
					  guint source_stride);

gpointer ags_amplifier_util_get_destination(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_destination(AgsAmplifierUtil *amplifier_util,
					gpointer destination);

guint ags_amplifier_util_get_destination_stride(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_destination_stride(AgsAmplifierUtil *amplifier_util,
					       guint destination_stride);

guint ags_amplifier_util_get_buffer_length(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_buffer_length(AgsAmplifierUtil *amplifier_util,
					  guint buffer_length);

AgsSoundcardFormat ags_amplifier_util_get_format(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_format(AgsAmplifierUtil *amplifier_util,
				   AgsSoundcardFormat format);

guint ags_amplifier_util_get_samplerate(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_samplerate(AgsAmplifierUtil *amplifier_util,
				       guint samplerate);

gdouble ags_amplifier_util_get_amp_0_frequency(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_0_frequency(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_0_frequency);

gdouble ags_amplifier_util_get_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_0_bandwidth);

gdouble ags_amplifier_util_get_amp_0_gain(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_0_gain(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_0_gain);

gdouble ags_amplifier_util_get_amp_1_frequency(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_1_frequency(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_1_frequency);

gdouble ags_amplifier_util_get_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_1_bandwidth);

gdouble ags_amplifier_util_get_amp_1_gain(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_1_gain(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_1_gain);

gdouble ags_amplifier_util_get_amp_2_frequency(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_2_frequency(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_2_frequency);

gdouble ags_amplifier_util_get_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_2_bandwidth);

gdouble ags_amplifier_util_get_amp_2_gain(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_2_gain(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_2_gain);

gdouble ags_amplifier_util_get_amp_3_frequency(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_3_frequency(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_3_frequency);

gdouble ags_amplifier_util_get_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util,
					    gdouble amp_3_bandwidth);

gdouble ags_amplifier_util_get_amp_3_gain(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_amp_3_gain(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_3_gain);

gdouble ags_amplifier_util_get_filter_gain(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_set_filter_gain(AgsAmplifierUtil *amplifier_util,
					gdouble filter_gain);

void ags_amplifier_util_process_s8(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_s16(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_s24(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_s32(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_s64(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_float(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_double(AgsAmplifierUtil *amplifier_util);
void ags_amplifier_util_process_complex(AgsAmplifierUtil *amplifier_util);

void ags_amplifier_util_process(AgsAmplifierUtil *amplifier_util);

G_END_DECLS

#endif /*__AGS_AMPLIFIER_UTIL_H__*/
