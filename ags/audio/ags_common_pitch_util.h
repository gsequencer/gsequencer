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

#ifndef __AGS_COMMON_PITCH_UTIL_H__
#define __AGS_COMMON_PITCH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_fast_pitch_util.h>
#include <ags/audio/ags_hq_pitch_util.h>
#include <ags/audio/ags_fluid_interpolate_none_util.h>
#include <ags/audio/ags_fluid_interpolate_linear_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_fluid_interpolate_7th_order_util.h>

G_BEGIN_DECLS

typedef struct _AgsCommonPitchUtil AgsCommonPitchUtil;

struct _AgsCommonPitchUtil
{
  //empty
};

GType ags_common_pitch_util_get_type(void);

gpointer ags_common_pitch_util_get_source(gpointer pitch_util,
					  GType pitch_type);
void ags_common_pitch_util_set_source(gpointer pitch_util,
				      GType pitch_type,
				      gpointer source);

guint ags_common_pitch_util_get_source_stride(gpointer pitch_util,
					      GType pitch_type);
void ags_common_pitch_util_set_source_stride(gpointer pitch_util,
					      GType pitch_type,
					     guint source_stride);

gpointer ags_common_pitch_util_get_destination(gpointer pitch_util,
					      GType pitch_type);
void ags_common_pitch_util_set_destination(gpointer pitch_util,
					   GType pitch_type,
					   gpointer destination);

guint ags_common_pitch_util_get_destination_stride(gpointer pitch_util,
						   GType pitch_type);
void ags_common_pitch_util_set_destination_stride(gpointer pitch_util,
						  GType pitch_type,
						  guint destination_stride);

guint ags_common_pitch_util_get_buffer_length(gpointer pitch_util,
					      GType pitch_type);
void ags_common_pitch_util_set_buffer_length(gpointer pitch_util,
					     GType pitch_type,
					     guint buffer_length);

guint ags_common_pitch_util_get_format(gpointer pitch_util,
				       GType pitch_type);
void ags_common_pitch_util_set_format(gpointer pitch_util,
				      GType pitch_type,
				      guint format);

guint ags_common_pitch_util_get_samplerate(gpointer pitch_util,
					   GType pitch_type);
void ags_common_pitch_util_set_samplerate(gpointer pitch_util,
					  GType pitch_type,
					  guint samplerate);

gdouble ags_common_pitch_util_get_base_key(gpointer pitch_util,
					   GType pitch_type);
void ags_common_pitch_util_set_base_key(gpointer pitch_util,
					GType pitch_type,
					gdouble base_key);

gdouble ags_common_pitch_util_get_tuning(gpointer pitch_util,
					 GType pitch_type);
void ags_common_pitch_util_set_tuning(gpointer pitch_util,
				      GType pitch_type,
				      gdouble tuning);

gboolean ags_common_pitch_util_get_vibrato_enabled(gpointer pitch_util,
						   GType pitch_type);
void ags_common_pitch_util_set_vibrato_enabled(gpointer pitch_util,
					       GType pitch_type,
					       gboolean vibrato_enabled);

gdouble ags_common_pitch_util_get_vibrato_gain(gpointer pitch_util,
					       GType pitch_type);
void ags_common_pitch_util_set_vibrato_gain(gpointer pitch_util,
					    GType pitch_type,
					    gdouble vibrato_gain);

gdouble ags_common_pitch_util_get_vibrato_lfo_depth(gpointer pitch_util,
						    GType pitch_type);
void ags_common_pitch_util_set_vibrato_lfo_depth(gpointer pitch_util,
						 GType pitch_type,
						 gdouble vibrato_lfo_depth);

gdouble ags_common_pitch_util_get_vibrato_lfo_freq(gpointer pitch_util,
						   GType pitch_type);
void ags_common_pitch_util_set_vibrato_lfo_freq(gpointer pitch_util,
						GType pitch_type,
						gdouble vibrato_lfo_freq);

gdouble ags_common_pitch_util_get_vibrato_tuning(gpointer pitch_util,
						 GType pitch_type);
void ags_common_pitch_util_set_vibrato_tuning(gpointer pitch_util,
					      GType pitch_type,
					      gdouble vibrato_tuning);

void ags_common_pitch_util_pitch(gpointer pitch_util,
				 GType pitch_type);

G_END_DECLS

#endif /*__AGS_COMMON_PITCH_UTIL_H__*/
