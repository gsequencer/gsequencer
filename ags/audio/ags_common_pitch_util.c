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

#include <ags/audio/ags_common_pitch_util.h>

gpointer ags_common_pitch_util_copy(AgsCommonPitchUtil *ptr);
void ags_common_pitch_util_free(AgsCommonPitchUtil *ptr);

GType
ags_common_pitch_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_common_pitch_util = 0;

    ags_type_common_pitch_util =
      g_boxed_type_register_static("AgsCommonPitchUtil",
				   (GBoxedCopyFunc) ags_common_pitch_util_copy,
				   (GBoxedFreeFunc) ags_common_pitch_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_common_pitch_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_common_pitch_util_copy(AgsCommonPitchUtil *ptr)
{
  AgsCommonPitchUtil *new_ptr;

  new_ptr = g_new0(AgsCommonPitchUtil,
		   1);
  
  return(new_ptr);
}

void
ags_common_pitch_util_free(AgsCommonPitchUtil *ptr)
{
  g_free(ptr);
}

/**
 * ags_common_pitch_util_get_source:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get source of @pitch_util.
 * 
 * Returns: the source of @pitch_util
 * 
 * Since: 4.0.0
 */
gpointer
ags_common_pitch_util_get_source(gpointer pitch_util,
				 GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_source(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_source(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_source(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_source(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_source(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_source(pitch_util));
  }

  return(NULL);
}

/**
 * ags_common_pitch_util_set_source:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @source: the source
 * 
 * Set @source of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_source(gpointer pitch_util,
				 GType pitch_type,
				 gpointer source)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_source(pitch_util,
				   source);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_source(pitch_util,
				 source);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_source(pitch_util,
					       source);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_source(pitch_util,
						 source);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_source(pitch_util,
						    source);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_source(pitch_util,
						    source);
  }
}

/**
 * ags_common_pitch_util_get_source_stride:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get source stride of @pitch_util.
 * 
 * Returns: the source stride of @pitch_util
 * 
 * Since: 4.0.0
 */
guint
ags_common_pitch_util_get_source_stride(gpointer pitch_util,
					GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_source_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_source_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_source_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_source_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_source_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_source_stride(pitch_util));
  }

  return(0);
}

/**
 * ags_common_pitch_util_set_source_stride:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @source_stride: the source stride
 * 
 * Set @source_stride of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_source_stride(gpointer pitch_util,
					GType pitch_type,
					guint source_stride)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_source_stride(pitch_util,
					  source_stride);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_source_stride(pitch_util,
					source_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_source_stride(pitch_util,
						      source_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_source_stride(pitch_util,
							source_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_source_stride(pitch_util,
							   source_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_source_stride(pitch_util,
							   source_stride);
  }
}

/**
 * ags_common_pitch_util_get_destination:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get destination of @pitch_util.
 * 
 * Returns: the destination of @pitch_util
 * 
 * Since: 4.0.0
 */
gpointer
ags_common_pitch_util_get_destination(gpointer pitch_util,
				      GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_destination(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_destination(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_destination(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_destination(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_destination(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_destination(pitch_util));
  }else{
    g_warning("unknown pitch type");
  }

  return(NULL);
}

/**
 * ags_common_pitch_util_set_destination:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @destination: the destination
 * 
 * Set @destination of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_destination(gpointer pitch_util,
				      GType pitch_type,
				      gpointer destination)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_destination(pitch_util,
					destination);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_destination(pitch_util,
				      destination);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_destination(pitch_util,
						    destination);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_destination(pitch_util,
						      destination);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_destination(pitch_util,
							 destination);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_destination(pitch_util,
							 destination);
  }
}

/**
 * ags_common_pitch_util_get_destination_stride:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get destination stride of @pitch_util.
 * 
 * Returns: the destination stride of @pitch_util
 * 
 * Since: 4.0.0
 */
guint
ags_common_pitch_util_get_destination_stride(gpointer pitch_util,
					     GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_destination_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_destination_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_destination_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_destination_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_destination_stride(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_destination_stride(pitch_util));
  }

  return(0);
}

/**
 * ags_common_pitch_util_set_destination_stride:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @destination_stride: the destination stride
 * 
 * Set @destination_stride of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_destination_stride(gpointer pitch_util,
					     GType pitch_type,
					     guint destination_stride)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_destination_stride(pitch_util,
					       destination_stride);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_destination_stride(pitch_util,
					     destination_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_destination_stride(pitch_util,
							   destination_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_destination_stride(pitch_util,
							     destination_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_destination_stride(pitch_util,
								destination_stride);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_destination_stride(pitch_util,
								destination_stride);
  }
}

/**
 * ags_common_pitch_util_get_buffer_length:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get buffer length of @pitch_util.
 * 
 * Returns: the buffer length of @pitch_util
 * 
 * Since: 4.0.0
 */
guint
ags_common_pitch_util_get_buffer_length(gpointer pitch_util,
					GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_buffer_length(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_buffer_length(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_buffer_length(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_buffer_length(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_buffer_length(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_buffer_length(pitch_util));
  }

  return(0);
}

/**
 * ags_common_pitch_util_set_buffer_length:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @buffer_length: the buffer length
 * 
 * Set @buffer_length of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_buffer_length(gpointer pitch_util,
					GType pitch_type,
					guint buffer_length)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_buffer_length(pitch_util,
					  buffer_length);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_buffer_length(pitch_util,
					buffer_length);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_buffer_length(pitch_util,
						      buffer_length);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_buffer_length(pitch_util,
							buffer_length);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_buffer_length(pitch_util,
							   buffer_length);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_buffer_length(pitch_util,
							   buffer_length);
  }
}

/**
 * ags_common_pitch_util_get_format:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get format of @pitch_util.
 * 
 * Returns: the format of @pitch_util
 * 
 * Since: 4.0.0
 */
guint
ags_common_pitch_util_get_format(gpointer pitch_util,
				 GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_format(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_format(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_format(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_format(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_format(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_format(pitch_util));
  }

  return(0);
}

/**
 * ags_common_pitch_util_set_format:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @format: the format
 * 
 * Set @format of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_format(gpointer pitch_util,
				 GType pitch_type,
				 guint format)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_format(pitch_util,
				   format);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_format(pitch_util,
				 format);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_format(pitch_util,
					       format);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_format(pitch_util,
						 format);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_format(pitch_util,
						    format);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_format(pitch_util,
						    format);
  }
}

/**
 * ags_common_pitch_util_get_samplerate:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get samplerate of @pitch_util.
 * 
 * Returns: the samplerate of @pitch_util
 * 
 * Since: 4.0.0
 */
guint
ags_common_pitch_util_get_samplerate(gpointer pitch_util,
				     GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_samplerate(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_samplerate(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_samplerate(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_samplerate(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_samplerate(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_samplerate(pitch_util));
  }

  return(0);
}

/**
 * ags_common_pitch_util_set_samplerate:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @samplerate: the samplerate
 * 
 * Set @samplerate of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_samplerate(gpointer pitch_util,
				     GType pitch_type,
				     guint samplerate)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_samplerate(pitch_util,
				       samplerate);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_samplerate(pitch_util,
				     samplerate);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_samplerate(pitch_util,
						   samplerate);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_samplerate(pitch_util,
						     samplerate);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_samplerate(pitch_util,
							samplerate);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_samplerate(pitch_util,
							samplerate);
  }
}

/**
 * ags_common_pitch_util_get_base_key:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get base key of @pitch_util.
 * 
 * Returns: the base key of @pitch_util
 * 
 * Since: 4.0.0
 */
gdouble
ags_common_pitch_util_get_base_key(gpointer pitch_util,
				   GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_base_key(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_base_key(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_base_key(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_base_key(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_base_key(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_base_key(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_base_key:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @base_key: the base key
 * 
 * Set @base_key of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_base_key(gpointer pitch_util,
				   GType pitch_type,
				   gdouble base_key)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_base_key(pitch_util,
				     base_key);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_base_key(pitch_util,
				   base_key);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_base_key(pitch_util,
						 base_key);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_base_key(pitch_util,
						   base_key);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_base_key(pitch_util,
						      base_key);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_base_key(pitch_util,
						      base_key);
  }
}

/**
 * ags_common_pitch_util_get_tuning:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get tuning of @pitch_util.
 * 
 * Returns: the tuning of @pitch_util
 * 
 * Since: 4.0.0
 */
gdouble
ags_common_pitch_util_get_tuning(gpointer pitch_util,
				 GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_tuning(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_tuning:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @tuning: the tuning
 * 
 * Set @tuning of @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_set_tuning(gpointer pitch_util,
				 GType pitch_type,
 				 gdouble tuning)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_tuning(pitch_util,
				   tuning);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_tuning(pitch_util,
				 tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_tuning(pitch_util,
					       tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_tuning(pitch_util,
						 tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_tuning(pitch_util,
						    tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_tuning(pitch_util,
						    tuning);
  }
}

/**
 * ags_common_pitch_util_get_vibrato_enabled:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get vibrato enabled of @pitch_util.
 * 
 * Returns: the vibrato enabled of @pitch_util
 * 
 * Since: 5.2.0
 */
gboolean
ags_common_pitch_util_get_vibrato_enabled(gpointer pitch_util,
					  GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_vibrato_enabled(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_vibrato_enabled(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_vibrato_enabled(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_vibrato_enabled(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_vibrato_enabled(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_vibrato_enabled(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_vibrato_enabled:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @vibrato_enabled: the vibrato enabled
 * 
 * Set @vibrato_enabled of @pitch_util.
 * 
 * Since: 5.2.0
 */
void
ags_common_pitch_util_set_vibrato_enabled(gpointer pitch_util,
					  GType pitch_type,
					  gboolean vibrato_enabled)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_vibrato_enabled(pitch_util,
					    vibrato_enabled);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_vibrato_enabled(pitch_util,
					  vibrato_enabled);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_vibrato_enabled(pitch_util,
							vibrato_enabled);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_vibrato_enabled(pitch_util,
							  vibrato_enabled);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_vibrato_enabled(pitch_util,
							     vibrato_enabled);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_vibrato_enabled(pitch_util,
							     vibrato_enabled);
  }
}

/**
 * ags_common_pitch_util_get_vibrato_gain:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get vibrato_gain of @pitch_util.
 * 
 * Returns: the vibrato_gain of @pitch_util
 * 
 * Since: 5.2.0
 */
gdouble
ags_common_pitch_util_get_vibrato_gain(gpointer pitch_util,
				       GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_vibrato_gain(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_vibrato_gain(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_vibrato_gain(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_vibrato_gain(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_vibrato_gain(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_vibrato_gain(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_vibrato_gain:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @vibrato_gain: the vibrato_gain
 * 
 * Set @vibrato_gain of @pitch_util.
 * 
 * Since: 5.2.0
 */
void
ags_common_pitch_util_set_vibrato_gain(gpointer pitch_util,
				       GType pitch_type,
				       gdouble vibrato_gain)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_vibrato_gain(pitch_util,
					 vibrato_gain);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_vibrato_gain(pitch_util,
				       vibrato_gain);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_vibrato_gain(pitch_util,
						     vibrato_gain);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_vibrato_gain(pitch_util,
						       vibrato_gain);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_vibrato_gain(pitch_util,
							  vibrato_gain);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_vibrato_gain(pitch_util,
							  vibrato_gain);
  }
}

/**
 * ags_common_pitch_util_get_vibrato_lfo_depth:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get vibrato LFO depth of @pitch_util.
 * 
 * Returns: the vibrato LFO depth of @pitch_util
 * 
 * Since: 5.2.0
 */
gdouble
ags_common_pitch_util_get_vibrato_lfo_depth(gpointer pitch_util,
					    GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_vibrato_lfo_depth(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_vibrato_lfo_depth(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_vibrato_lfo_depth(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_vibrato_lfo_depth(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_depth(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_vibrato_lfo_depth(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_vibrato_lfo_depth:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @vibrato_lfo_depth: the vibrato LFO depth
 * 
 * Set @vibrato_lfo_depth of @pitch_util.
 * 
 * Since: 5.2.0
 */
void
ags_common_pitch_util_set_vibrato_lfo_depth(gpointer pitch_util,
					    GType pitch_type,
					    gdouble vibrato_lfo_depth)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_vibrato_lfo_depth(pitch_util,
					      vibrato_lfo_depth);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_vibrato_lfo_depth(pitch_util,
					    vibrato_lfo_depth);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_vibrato_lfo_depth(pitch_util,
							  vibrato_lfo_depth);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_vibrato_lfo_depth(pitch_util,
							    vibrato_lfo_depth);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_depth(pitch_util,
							       vibrato_lfo_depth);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_vibrato_lfo_depth(pitch_util,
							       vibrato_lfo_depth);
  }
}

/**
 * ags_common_pitch_util_get_vibrato_lfo_freq:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get vibrato LFO frequency of @pitch_util.
 * 
 * Returns: the vibrato LFO frequency of @pitch_util
 * 
 * Since: 5.2.0
 */
gdouble
ags_common_pitch_util_get_vibrato_lfo_freq(gpointer pitch_util,
					   GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_vibrato_lfo_freq(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_vibrato_lfo_freq(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_vibrato_lfo_freq(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_vibrato_lfo_freq(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_freq(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_vibrato_lfo_freq(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_vibrato_lfo_freq:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @vibrato_lfo_freq: the vibrato LFO frequency
 * 
 * Set @vibrato_lfo_freq of @pitch_util.
 * 
 * Since: 5.2.0
 */
void
ags_common_pitch_util_set_vibrato_lfo_freq(gpointer pitch_util,
					   GType pitch_type,
					   gdouble vibrato_lfo_freq)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_vibrato_lfo_freq(pitch_util,
					     vibrato_lfo_freq);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_vibrato_lfo_freq(pitch_util,
					   vibrato_lfo_freq);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_vibrato_lfo_freq(pitch_util,
							 vibrato_lfo_freq);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_vibrato_lfo_freq(pitch_util,
							   vibrato_lfo_freq);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_freq(pitch_util,
							      vibrato_lfo_freq);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_vibrato_lfo_freq(pitch_util,
							      vibrato_lfo_freq);
  }
}

/**
 * ags_common_pitch_util_get_vibrato_tuning:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Get vibrato tuning of @pitch_util.
 * 
 * Returns: the vibrato tuning of @pitch_util
 * 
 * Since: 5.2.0
 */
gdouble
ags_common_pitch_util_get_vibrato_tuning(gpointer pitch_util,
					 GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    return(ags_fast_pitch_util_get_vibrato_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    return(ags_hq_pitch_util_get_vibrato_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    return(ags_fluid_interpolate_none_util_get_vibrato_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    return(ags_fluid_interpolate_linear_util_get_vibrato_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    return(ags_fluid_interpolate_4th_order_util_get_vibrato_tuning(pitch_util));
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    return(ags_fluid_interpolate_7th_order_util_get_vibrato_tuning(pitch_util));
  }

  return(0.0);
}

/**
 * ags_common_pitch_util_set_vibrato_tuning:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * @vibrato_tuning: the vibrato tuning
 * 
 * Set @vibrato_tuning of @pitch_util.
 * 
 * Since: 5.2.0
 */
void
ags_common_pitch_util_set_vibrato_tuning(gpointer pitch_util,
					 GType pitch_type,
					 gdouble vibrato_tuning)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_set_vibrato_tuning(pitch_util,
					   vibrato_tuning);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_set_vibrato_tuning(pitch_util,
					 vibrato_tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_set_vibrato_tuning(pitch_util,
						       vibrato_tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_set_vibrato_tuning(pitch_util,
							 vibrato_tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_set_vibrato_tuning(pitch_util,
							    vibrato_tuning);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_set_vibrato_tuning(pitch_util,
							    vibrato_tuning);
  }
}

/**
 * ags_common_pitch_util_pitch:
 * @pitch_util: the pitch util
 * @pitch_type: the pitch type
 * 
 * Pitch @pitch_util.
 * 
 * Since: 4.0.0
 */
void
ags_common_pitch_util_pitch(gpointer pitch_util,
			    GType pitch_type)
{
  if(pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_pitch(pitch_util);
  }else if(pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_pitch(pitch_util);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_pitch(pitch_util);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_pitch(pitch_util);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_pitch(pitch_util);
  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_pitch(pitch_util);
  }
}
