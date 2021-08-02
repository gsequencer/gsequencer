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

#include <ags/audio/ags_generic_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fast_pitch_util.h>
#include <ags/audio/ags_hq_pitch_util.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_interpolate_none_util.h>
#include <ags/audio/ags_fluid_interpolate_linear_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_fluid_interpolate_7th_order_util.h>

/**
 * SECTION:ags_generic_pitch_util
 * @short_description: generic pitch util
 * @title: AgsGenericPitchUtil
 * @section_id:
 * @include: ags/audio/ags_generic_pitch_util.h
 *
 * Utility functions to pitch.
 */

GType
ags_generic_pitch_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_generic_pitch_util = 0;

    ags_type_generic_pitch_util =
      g_boxed_type_register_static("AgsGenericPitchUtil",
				   (GBoxedCopyFunc) ags_generic_pitch_util_copy,
				   (GBoxedFreeFunc) ags_generic_pitch_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_generic_pitch_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_generic_pitch_util_alloc:
 * 
 * Allocate #AgsGenericPitchUtil-struct.
 * 
 * Returns: the newly allocated #AgsGenericPitchUtil-struct
 * 
 * Since: 3.9.6
 */
AgsGenericPitchUtil*
ags_generic_pitch_util_alloc()
{
  AgsGenericPitchUtil *ptr;
  
  ptr = (AgsGenericPitchUtil *) g_new(AgsGenericPitchUtil,
				      1);

  ptr->pitch_type = AGS_FLUID_4TH_ORDER_INTERPOLATE;

  ptr->fast_pitch_util = ags_fast_pitch_util_alloc();
  ptr->hq_pitch_util = ags_hq_pitch_util_alloc();
  ptr->fluid_interpolate_none_util = ags_fluid_interpolate_none_util_alloc();
  ptr->fluid_interpolate_linear_util = ags_fluid_interpolate_linear_util_alloc();
  ptr->fluid_interpolate_4th_order_util = ags_fluid_interpolate_4th_order_util_alloc();
  ptr->fluid_interpolate_7th_order_util = ags_fluid_interpolate_7th_order_util_alloc();
  
  return(ptr);
}

/**
 * ags_generic_pitch_util_boxed_copy:
 * @ptr: the #AgsGenericPitchUtil-struct
 * 
 * Copy #AgsGenericPitchUtil-struct.
 * 
 * Returns: the newly allocated #AgsGenericPitchUtil-struct
 * 
 * Since: 3.9.6
 */
gpointer
ags_generic_pitch_util_copy(AgsGenericPitchUtil *ptr)
{
  AgsGenericPitchUtil *new_ptr;
  
  new_ptr = (AgsGenericPitchUtil *) g_new(AgsGenericPitchUtil,
					  1);

  new_ptr->pitch_type = ptr->pitch_type;

  new_ptr->fast_pitch_util = ags_fast_pitch_util_copy(ptr->fast_pitch_util);
  new_ptr->hq_pitch_util = ags_hq_pitch_util_copy(ptr->hq_pitch_util);
  new_ptr->fluid_interpolate_none_util = ags_fluid_interpolate_none_util_copy(ptr->fluid_interpolate_none_util);
  new_ptr->fluid_interpolate_linear_util = ags_fluid_interpolate_linear_util_copy(ptr->fluid_interpolate_linear_util);
  new_ptr->fluid_interpolate_4th_order_util = ags_fluid_interpolate_4th_order_util_copy(ptr->fluid_interpolate_4th_order_util);
  new_ptr->fluid_interpolate_7th_order_util = ags_fluid_interpolate_7th_order_util_copy(ptr->fluid_interpolate_7th_order_util);
      
  return(new_ptr);
}

/**
 * ags_generic_pitch_util_free:
 * @ptr: the #AgsGenericPitchUtil-struct
 * 
 * Free #AgsGenericPitchUtil-struct.
 * 
 * Since: 3.9.6
 */
void
ags_generic_pitch_util_free(AgsGenericPitchUtil *ptr)
{  
  ags_fast_pitch_util_free(ptr->fast_pitch_util);
  ags_hq_pitch_util_free(ptr->hq_pitch_util);
  ags_fluid_interpolate_none_util_free(ptr->fluid_interpolate_none_util);
  ags_fluid_interpolate_linear_util_free(ptr->fluid_interpolate_linear_util);
  ags_fluid_interpolate_4th_order_util_free(ptr->fluid_interpolate_4th_order_util);
  ags_fluid_interpolate_7th_order_util_free(ptr->fluid_interpolate_7th_order_util);
  
  g_free(ptr);
}

/**
 * ags_generic_pitch_util_get_pitch_type:
 * @generic_pitch_util: the #AgsGenericPitchUtil-struct
 * 
 * Get pitch type of @generic_pitch_util.
 * 
 * Returns: the pitch type
 * 
 * Since: 3.9.6
 */
guint
ags_generic_pitch_util_get_pitch_type(AgsGenericPitchUtil *generic_pitch_util)
{
  if(generic_pitch_util == NULL){
    return(AGS_FLUID_4TH_ORDER_INTERPOLATE);
  }

  return(generic_pitch_util->pitch_type);
}

/**
 * ags_generic_pitch_util_set_pitch_type:
 * @generic_pitch_util: the #AgsGenericPitchUtil-struct
 * @pitch_type: the pitch type
 * 
 * Set @pitch_type of @generic_pitch_util.
 * 
 * Since: 3.9.6
 */
void
ags_generic_pitch_util_set_pitch_type(AgsGenericPitchUtil *generic_pitch_util,
				      guint pitch_type)
{
  if(generic_pitch_util == NULL){
    return;
  }

  generic_pitch_util->pitch_type = pitch_type;
}

/**
 * ags_generic_pitch_util_pitch:
 * @generic_pitch_util: the #AgsGenericPitchUtil-struct
 * 
 * Pitch @generic_pitch_util.
 * 
 * Since: 3.9.6
 */
void
ags_generic_pitch_util_pitch(AgsGenericPitchUtil *generic_pitch_util)
{
  if(generic_pitch_util == NULL){
    return;
  }
  
  switch(generic_pitch_util->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_pitch(generic_pitch_util->fast_pitch_util);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_pitch(generic_pitch_util->hq_pitch_util);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    ags_fluid_interpolate_none_util_pitch(generic_pitch_util->fluid_interpolate_none_util);
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    ags_fluid_interpolate_linear_util_pitch(generic_pitch_util->fluid_interpolate_linear_util);
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    ags_fluid_interpolate_4th_order_util_pitch(generic_pitch_util->fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    ags_fluid_interpolate_7th_order_util_pitch(generic_pitch_util->fluid_interpolate_7th_order_util);
  }
  break;
  default:
    g_warning("unknown pitch type");
  }
}

/**
 * ags_generic_pitch_util_compute_s8:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_s8(AgsGenericPitch *generic_pitch,
				  gint8 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;
  
  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_s8(buffer,
				   buffer_length,
				   samplerate,
				   base_key,
				   tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_s8(buffer,
				 buffer_length,
				 samplerate,
				 base_key,
				 tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    gint8 *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					    AGS_SOUNDCARD_SIGNED_8_BIT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gint8));

    ags_stream_free(mix_buffer);
  }
  
}

/**
 * ags_generic_pitch_util_compute_s16:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_s16(AgsGenericPitch *generic_pitch,
				   gint16 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_s16(buffer,
				    buffer_length,
				    samplerate,
				    base_key,
				    tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_s16(buffer,
				  buffer_length,
				  samplerate,
				  base_key,
				  tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }
  
  if(fluid_success){
    gint16 *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_SIGNED_16_BIT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }
    
    memcpy(buffer, mix_buffer, buffer_length * sizeof(gint16));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_s24:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_s24(AgsGenericPitch *generic_pitch,
				   gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_s24(buffer,
				    buffer_length,
				    samplerate,
				    base_key,
				    tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_s24(buffer,
				  buffer_length,
				  samplerate,
				  base_key,
				  tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }
  
  if(fluid_success){
    gint32 *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_SIGNED_24_BIT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gint32));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_s32:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_s32(AgsGenericPitch *generic_pitch,
				   gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_s32(buffer,
				    buffer_length,
				    samplerate,
				    base_key,
				    tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_s32(buffer,
				  buffer_length,
				  samplerate,
				  base_key,
				  tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    gint32 *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_SIGNED_32_BIT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gint32));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_s64:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_s64(AgsGenericPitch *generic_pitch,
				   gint64 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_s64(buffer,
				    buffer_length,
				    samplerate,
				    base_key,
				    tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_s64(buffer,
				  buffer_length,
				  samplerate,
				  base_key,
				  tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    gint64 *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_SIGNED_64_BIT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gint64));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_float:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_float(AgsGenericPitch *generic_pitch,
				     gfloat *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_float(buffer,
				      buffer_length,
				      samplerate,
				      base_key,
				      tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_float(buffer,
				    buffer_length,
				    samplerate,
				    base_key,
				    tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    gfloat *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_FLOAT);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gfloat));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_double:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_double(AgsGenericPitch *generic_pitch,
				      gdouble *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble base_key,
				      gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_double(buffer,
				       buffer_length,
				       samplerate,
				       base_key,
				       tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_double(buffer,
				     buffer_length,
				     samplerate,
				     base_key,
				     tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    gdouble *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_DOUBLE);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(gdouble));

    ags_stream_free(mix_buffer);
  }
}

/**
 * ags_generic_pitch_util_compute_complex:
 * @generic_pitch: the #AgsGenericPitch-struct
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.9.0
 */
void
ags_generic_pitch_util_compute_complex(AgsGenericPitch *generic_pitch,
				       AgsComplex *buffer,
				       guint buffer_length,
				       guint samplerate,
				       gdouble base_key,
				       gdouble tuning)
{
  guint fluid_interp_method;
  gboolean fluid_success;
  
  if(generic_pitch == NULL ||
     buffer == NULL){
    return;
  }

  fluid_interp_method = 4;
  fluid_success = FALSE;

  switch(generic_pitch->pitch_type){
  case AGS_FAST_PITCH:
  {
    ags_fast_pitch_util_compute_complex(buffer,
					buffer_length,
					samplerate,
					base_key,
					tuning);
  }
  break;
  case AGS_HQ_PITCH:
  {
    ags_hq_pitch_util_compute_complex(buffer,
				      buffer_length,
				      samplerate,
				      base_key,
				      tuning);
  }
  break;
  case AGS_FLUID_NO_INTERPOLATE:
  {
    fluid_interp_method = 0;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_LINEAR_INTERPOLATE:
  {
    fluid_interp_method = 1;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_4TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 4;

    fluid_success = TRUE;
  }
  break;
  case AGS_FLUID_7TH_ORDER_INTERPOLATE:
  {
    fluid_interp_method = 7;

    fluid_success = TRUE;
  }
  break;
  default:
    g_warning("unknown pitch type");
  }

  if(fluid_success){
    AgsComplex *mix_buffer;
  
    gdouble root_pitch_hz;
    gdouble phase_incr;

    mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						 AGS_SOUNDCARD_COMPLEX);

    /* pitch */
    root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
    phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    switch(fluid_interp_method){
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
      g_warning("invalid fluid interpolation method");
    }

    memcpy(buffer, mix_buffer, buffer_length * sizeof(AgsComplex));

    ags_stream_free(mix_buffer);
  }
}
