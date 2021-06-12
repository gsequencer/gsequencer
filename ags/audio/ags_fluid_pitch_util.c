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

#include <ags/audio/ags_fluid_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_linear_interpolate_util.h>

/**
 * SECTION:ags_fluid_pitch_util
 * @short_description: fluid pitch util
 * @title: AgsFLUIDPitchUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_pitch_util.h
 *
 * Utility functions to pitch.
 */

/**
 * ags_fluid_pitch_util_compute_s8:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_s8(gint8 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_s16:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_s16(gint16 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_s24:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_s24(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning)
{
}

/**
 * ags_fluid_pitch_util_compute_s32:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_s32(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_s64:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_s64(gint64 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_float:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_float(gfloat *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_double:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_double(gdouble *buffer,
				    guint buffer_length,
				    guint samplerate,
				    gdouble base_key,
				    gdouble tuning)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_pitch_util_compute_complex:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_pitch_util_compute_complex(AgsComplex *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning)
{
  //TODO:JK: implement me
}
