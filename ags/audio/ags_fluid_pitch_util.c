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
#include <ags/audio/ags_fluid_interpolate_none_util.h>
#include <ags/audio/ags_fluid_interpolate_linear_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_fluid_interpolate_7th_order_util.h>

#include <math.h>
#include <string.h>

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
  gint8 *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;
  
  mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_SIGNED_8_BIT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_s8(mix_buffer,
					    buffer,
					    buffer_length,
					    phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_s8(mix_buffer,
					      buffer,
					      buffer_length,
					      phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_s8(mix_buffer,
						 buffer,
						 buffer_length,
						 phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_s8(mix_buffer,
						 buffer,
						 buffer_length,
						 phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gint16 *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;
  
  mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_SIGNED_16_BIT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_s16(mix_buffer,
					     buffer,
					     buffer_length,
					     phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_s16(mix_buffer,
					       buffer,
					       buffer_length,
					       phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_s16(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_s16(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gint32 *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;

  mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_SIGNED_24_BIT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_s24(mix_buffer,
					     buffer,
					     buffer_length,
					     phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_s24(mix_buffer,
					       buffer,
					       buffer_length,
					       phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_s24(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_s24(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gint32 *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;
  
  mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_SIGNED_32_BIT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_s32(mix_buffer,
					     buffer,
					     buffer_length,
					     phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_s32(mix_buffer,
					       buffer,
					       buffer_length,
					       phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_s32(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_s32(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gint64 *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;

  mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_SIGNED_64_BIT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_s64(mix_buffer,
					     buffer,
					     buffer_length,
					     phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_s64(mix_buffer,
					       buffer,
					       buffer_length,
					       phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_s64(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_s64(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gfloat *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;

  mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_FLOAT);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_float(mix_buffer,
					       buffer,
					       buffer_length,
					       phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_float(mix_buffer,
						 buffer,
						 buffer_length,
						 phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_float(mix_buffer,
						    buffer,
						    buffer_length,
						    phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_float(mix_buffer,
						    buffer,
						    buffer_length,
						    phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  gdouble *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;

  mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
					    AGS_SOUNDCARD_DOUBLE);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_double(mix_buffer,
						buffer,
						buffer_length,
						phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_double(mix_buffer,
						  buffer,
						  buffer_length,
						  phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_double(mix_buffer,
						     buffer,
						     buffer_length,
						     phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_double(mix_buffer,
						     buffer,
						     buffer_length,
						     phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
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
  AgsComplex *mix_buffer;
  
  gdouble root_pitch_hz;
  gdouble phase_incr;

  guint interp_method;

  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);

  /* pitch */
  root_pitch_hz = ags_fluid_ct2hz_real(100.0 * base_key);
  
  phase_incr = ags_fluid_ct2hz_real(tuning) / root_pitch_hz;

  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  interp_method = 4;

  switch(interp_method){
  case 0:
  {
    ags_fluid_interpolate_none_util_fill_complex(mix_buffer,
						 buffer,
						 buffer_length,
						 phase_incr);
  }
  break;
  case 1:
  {
    ags_fluid_interpolate_linear_util_fill_complex(mix_buffer,
						   buffer,
						   buffer_length,
						   phase_incr);
  }
  break;
  case 4:
  {
    ags_fluid_interpolate_4th_order_util_fill_complex(mix_buffer,
						      buffer,
						      buffer_length,
						      phase_incr);
  }
  break;
  case 7:
  {
    ags_fluid_interpolate_7th_order_util_fill_complex(mix_buffer,
						      buffer,
						      buffer_length,
						      phase_incr);
  }
  break;
  default:
    g_warning("invalid interpolation method");
  }

  memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

  ags_stream_free(mix_buffer);
}
