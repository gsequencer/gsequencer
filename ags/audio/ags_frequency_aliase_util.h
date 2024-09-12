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

#ifndef __AGS_FREQUENCY_ALIASE_UTIL_H__
#define __AGS_FREQUENCY_ALIASE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FREQUENCY_ALIASE_UTIL         (ags_frequency_aliase_util_get_type())

#define AGS_FREQUENCY_ALIASE_UTIL_INITIALIZER ((AgsFrequencyAliaseUtil) {	\
      .source = NULL,						\
      .source_stride = 1,					\
      .phase_shifted_source = NULL,				\
      .phase_shifted_source_stride = 1,				\
      .destination = NULL,					\
      .destination_stride = 1,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE  })

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
  AgsSoundcardFormat format;
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

AgsSoundcardFormat ags_frequency_aliase_util_get_format(AgsFrequencyAliaseUtil *frequency_aliase_util);
void ags_frequency_aliase_util_set_format(AgsFrequencyAliaseUtil *frequency_aliase_util,
					  AgsSoundcardFormat format);

void ags_frequency_aliase_util_process(AgsFrequencyAliaseUtil *frequency_aliase_util);

G_END_DECLS

#endif /*__AGS_FREQUENCY_ALIASE_UTIL_H__*/
