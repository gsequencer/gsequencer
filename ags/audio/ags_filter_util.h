/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_FILTER_UTIL_H__
#define __AGS_FILTER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

void ags_filter_util_highpass_s8(gint8 *buffer,
				 guint buffer_length,
				 guint samplerate);
void ags_filter_util_highpass_s16(gint16 *buffer,
				  guint buffer_length,
				  guint samplerate);
void ags_filter_util_highpass_s24(gint32 *buffer,
				  guint buffer_length,
				  guint samplerate);
void ags_filter_util_highpass_s32(gint32 *buffer,
				  guint buffer_length,
				  guint samplerate);
void ags_filter_util_highpass_s64(gint64 *buffer,
				  guint buffer_length,
				  guint samplerate);
void ags_filter_util_highpass_float(gfloat *buffer,
				    guint buffer_length,
				    guint samplerate);
void ags_filter_util_highpass_double(gdouble *buffer,
				     guint buffer_length,
				     guint samplerate);
void ags_filter_util_highpass_complex(AgsComplex *buffer,
				      guint buffer_length,
				      guint samplerate);

void ags_filter_util_lowpass_s8(gint8 *buffer,
				guint buffer_length,
				guint samplerate);
void ags_filter_util_lowpass_s16(gint16 *buffer,
				 guint buffer_length,
				 guint samplerate);
void ags_filter_util_lowpass_s24(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate);
void ags_filter_util_lowpass_s32(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate);
void ags_filter_util_lowpass_s64(gint64 *buffer,
				 guint buffer_length,
				 guint samplerate);
void ags_filter_util_lowpass_float(gfloat *buffer,
				   guint buffer_length,
				   guint samplerate);
void ags_filter_util_lowpass_double(gdouble *buffer,
				    guint buffer_length,
				    guint samplerate);
void ags_filter_util_lowpass_complex(AgsComplex *buffer,
				     guint buffer_length,
				     guint samplerate);

void ags_filter_util_bandpass_s8(gint8 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble lower, gdouble upper);
void ags_filter_util_bandpass_s16(gint16 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble lower, gdouble upper);
void ags_filter_util_bandpass_s24(gint32 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble lower, gdouble upper);
void ags_filter_util_bandpass_s32(gint32 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble lower, gdouble upper);
void ags_filter_util_bandpass_s64(gint64 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble lower, gdouble upper);
void ags_filter_util_bandpass_float(gfloat *buffer,
				    guint buffer_length,
				    guint samplerate,
				    gdouble lower, gdouble upper);
void ags_filter_util_bandpass_double(gdouble *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble lower, gdouble upper);
void ags_filter_util_bandpass_complex(AgsComplex *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble lower, gdouble upper);

void ags_filter_util_equalizer_s8(gint8 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble frequency, gdouble bandwidth,
				  gdouble gain);
void ags_filter_util_equalizer_s16(gint16 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency, gdouble bandwidth,
				   gdouble gain);
void ags_filter_util_equalizer_s24(gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency, gdouble bandwidth,
				   gdouble gain);
void ags_filter_util_equalizer_s32(gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency, gdouble bandwidth,
				   gdouble gain);
void ags_filter_util_equalizer_s64(gint64 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency, gdouble bandwidth,
				   gdouble gain);
void ags_filter_util_equalizer_float(gfloat *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble frequency, gdouble bandwidth,
				     gdouble gain);
void ags_filter_util_equalizer_double(gdouble *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency, gdouble bandwidth,
				      gdouble gain);
void ags_filter_util_equalizer_complex(AgsComplex *buffer,
				       guint buffer_length,
				       guint samplerate,
				       gdouble frequency, gdouble bandwidth,
				       gdouble gain);

void ags_filter_util_pitch_s8(gint8 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning);
void ags_filter_util_pitch_s16(gint16 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s24(gint32 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s32(gint32 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s64(gint64 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_float(gfloat *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning);
void ags_filter_util_pitch_double(gdouble *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning);
void ags_filter_util_pitch_complex(AgsComplex *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);

#endif /*__AGS_FILTER_UTIL_H__*/
