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

/* 
 *  This LADSPA plugin provides mono and stereo filters
 *
 * (c) Fraser Stuart 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <ags/audio/ags_low_pass_filter_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

void ags_invada_check_param_change(guint param, gdouble *control, gdouble *last, gdouble *converted, guint samplerate, gdouble (*ConvertFunction)(guint, gdouble, guint));

gdouble ags_invada_convert_param(guint param, gdouble value, guint samplerate);

#define AGS_INVADA_IFILTER_FREQ (0)
#define AGS_INVADA_IFILTER_GAIN (1)
#define AGS_INVADA_IFILTER_NOCLIP (2)

/**
 * SECTION:ags_low_pass_filter_util
 * @short_description: Boxed type of low-pass filter util
 * @title: AgsLowPassFilterUtil
 * @section_id:
 * @include: ags/audio/ags_low_pass_filter_util.h
 *
 * Boxed type of low-pass filter util data type.
 */

GType
ags_low_pass_filter_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_low_pass_filter_util = 0;

    ags_type_low_pass_filter_util =
      g_boxed_type_register_static("AgsLowPassFilterUtil",
				   (GBoxedCopyFunc) ags_low_pass_filter_util_copy,
				   (GBoxedFreeFunc) ags_low_pass_filter_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_low_pass_filter_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_low_pass_filter_util_alloc:
 *
 * Allocate #AgsLowPassFilterUtil-struct
 *
 * Returns: a new #AgsLowPassFilterUtil-struct
 *
 * Since: 7.7.0
 */
AgsLowPassFilterUtil*
ags_low_pass_filter_util_alloc()
{
  AgsLowPassFilterUtil *ptr;

  ptr = (AgsLowPassFilterUtil *) g_new(AgsLowPassFilterUtil,
				       1);

  ptr[0] = AGS_LOW_PASS_FILTER_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_low_pass_filter_util_copy:
 * @ptr: the original #AgsLowPassFilterUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsLowPassFilterUtil-struct
 *
 * Since: 7.7.0
 */
gpointer
ags_low_pass_filter_util_copy(AgsLowPassFilterUtil *ptr)
{
  AgsLowPassFilterUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsLowPassFilterUtil *) g_new(AgsLowPassFilterUtil,
					   1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->cut_off_frequency = ptr->cut_off_frequency;
  new_ptr->filter_gain = ptr->filter_gain;
  new_ptr->no_clip = ptr->no_clip;
  
  new_ptr->last_freq = ptr->last_freq;
  new_ptr->last_gain = ptr->last_gain;
  new_ptr->last_no_clip = ptr->last_no_clip;

  new_ptr->converted_freq = ptr->converted_freq;
  new_ptr->converted_gain = ptr->converted_gain;
  new_ptr->converted_no_clip = ptr->converted_no_clip;

  return(new_ptr);
}

/**
 * ags_low_pass_filter_util_free:
 * @ptr: the #AgsLowPassFilterUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_free(AgsLowPassFilterUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  //  g_free(ptr->destination);

  //  if(ptr->destination != ptr->source){
  //    g_free(ptr->source);
  //  }
  
  g_free(ptr);
}

/**
 * ags_low_pass_filter_util_get_destination:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get destination buffer of @low_pass_filter_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_low_pass_filter_util_get_destination(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(NULL);
  }

  return(low_pass_filter_util->destination);
}

/**
 * ags_low_pass_filter_util_set_destination:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @destination: (transfer none) : the destination buffer
 *
 * Set @destination buffer of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_destination(AgsLowPassFilterUtil *low_pass_filter_util,
					 gpointer destination)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->destination = destination;
}

/**
 * ags_low_pass_filter_util_get_destination_stride:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get destination stride of @low_pass_filter_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_low_pass_filter_util_get_destination_stride(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0);
  }

  return(low_pass_filter_util->destination_stride);
}

/**
 * ags_low_pass_filter_util_set_destination_stride:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_destination_stride(AgsLowPassFilterUtil *low_pass_filter_util,
						guint destination_stride)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->destination_stride = destination_stride;
}

/**
 * ags_low_pass_filter_util_get_source:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get source buffer of @low_pass_filter_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_low_pass_filter_util_get_source(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(NULL);
  }

  return(low_pass_filter_util->source);
}

/**
 * ags_low_pass_filter_util_set_source:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_source(AgsLowPassFilterUtil *low_pass_filter_util,
				    gpointer source)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->source = source;
}

/**
 * ags_low_pass_filter_util_get_source_stride:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get source stride of @low_pass_filter_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_low_pass_filter_util_get_source_stride(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0);
  }

  return(low_pass_filter_util->source_stride);
}

/**
 * ags_low_pass_filter_util_set_source_stride:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_source_stride(AgsLowPassFilterUtil *low_pass_filter_util,
					   guint source_stride)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->source_stride = source_stride;
}

/**
 * ags_low_pass_filter_util_get_buffer_length:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get buffer length of @low_pass_filter_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.7.0
 */
guint
ags_low_pass_filter_util_get_buffer_length(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0);
  }

  return(low_pass_filter_util->buffer_length);
}

/**
 * ags_low_pass_filter_util_set_buffer_length:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_buffer_length(AgsLowPassFilterUtil *low_pass_filter_util,
					   guint buffer_length)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->buffer_length = buffer_length;
}

/**
 * ags_low_pass_filter_util_get_format:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get format of @low_pass_filter_util.
 * 
 * Returns: the format
 * 
 * Since: 7.7.0
 */
AgsSoundcardFormat
ags_low_pass_filter_util_get_format(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0);
  }

  return(low_pass_filter_util->format);
}

/**
 * ags_low_pass_filter_util_set_format:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @format: the format
 *
 * Set @format of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_format(AgsLowPassFilterUtil *low_pass_filter_util,
				    AgsSoundcardFormat format)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->format = format;
}

/**
 * ags_low_pass_filter_util_get_samplerate:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get samplerate of @low_pass_filter_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 7.7.0
 */
guint
ags_low_pass_filter_util_get_samplerate(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0);
  }

  return(low_pass_filter_util->samplerate);
}

/**
 * ags_low_pass_filter_util_set_samplerate:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_samplerate(AgsLowPassFilterUtil *low_pass_filter_util,
					guint samplerate)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->samplerate = samplerate;
}

/**
 * ags_low_pass_filter_util_get_cut_off_frequency:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get cut off frequency of @low_pass_filter_util.
 * 
 * Returns: the frequency of cut off
 * 
 * Since: 7.7.0
 */
gdouble
ags_low_pass_filter_util_get_cut_off_frequency(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0.0);
  }

  return(low_pass_filter_util->cut_off_frequency);
}

/**
 * ags_low_pass_filter_util_set_cut_off_frequency:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @cut_off_frequency: the cut off frequency
 *
 * Set @cut_off_frequency of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_cut_off_frequency(AgsLowPassFilterUtil *low_pass_filter_util,
					       gdouble cut_off_frequency)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->cut_off_frequency = cut_off_frequency;
}

/**
 * ags_low_pass_filter_util_get_filter_gain:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get filter gain of @low_pass_filter_util.
 * 
 * Returns: the gain of filter
 * 
 * Since: 7.7.0
 */
gdouble
ags_low_pass_filter_util_get_filter_gain(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0.0);
  }

  return(low_pass_filter_util->filter_gain);
}

/**
 * ags_low_pass_filter_util_set_filter_gain:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @filter_gain: the filter gain
 *
 * Set @filter_gain of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_filter_gain(AgsLowPassFilterUtil *low_pass_filter_util,
					 gdouble filter_gain)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->filter_gain = filter_gain;
}

/**
 * ags_low_pass_filter_util_get_no_clip:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Get no clip of @low_pass_filter_util.
 * 
 * Returns: the gain of filter
 * 
 * Since: 7.7.0
 */
gdouble
ags_low_pass_filter_util_get_no_clip(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL){
    return(0.0);
  }

  return(low_pass_filter_util->no_clip);
}

/**
 * ags_low_pass_filter_util_set_no_clip:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * @no_clip: the no clip
 *
 * Set @no_clip of @low_pass_filter_util.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_set_no_clip(AgsLowPassFilterUtil *low_pass_filter_util,
				     gdouble no_clip)
{
  if(low_pass_filter_util == NULL){
    return;
  }

  low_pass_filter_util->no_clip = no_clip;
}

void
ags_invada_check_param_change(guint param, gdouble *control, gdouble *last, gdouble *converted, guint samplerate, gdouble (*ConvertFunction)(guint, gdouble, guint))
{
  if(*control != *last){
    *last = *control;
    *converted = (*ConvertFunction)(param, *control, samplerate);
  }
}

gdouble
ags_invada_ino_clip(gdouble in)
{
  gdouble out;
  
  if(fabs(in) < 0.7){
    out = in;
  }else{
    out = (in > 0.0) ? (0.7 + 0.3 * (1.0 - pow(2.718281828, 3.33333333*(0.7 - in)))): (-0.7 - 0.3 * (1.0 - pow(2.718281828, 3.33333333 * (0.7 + in))));
  }
  
  return(out);
}

gdouble
ags_invada_convert_param(guint param, gdouble value, guint samplerate)
{
  gdouble temp;
  gdouble result;

  switch(param){
  case AGS_INVADA_IFILTER_FREQ:
    {
      temp = value / (gdouble) samplerate;
      
      if(temp <  0.0005){
	result = 1000.0;
      }else if (temp <= 0.5){
	result = 1.0 / (2.0 * temp);
      }else{
	result = 1.0;
      }
    }
    break;
  case AGS_INVADA_IFILTER_GAIN:
    {
      if(value < 0.0){
	result = 1.0;
      }else if(value < 12.0){
	result = pow(10.0, value / 20.0);
      }else{
	result = pow(10.0 ,0.6);
      }
    }
    break;
  case AGS_INVADA_IFILTER_NOCLIP:
    {
      if(value < 0.5){
	result = 0.0; 
      }else{
	result = 1.0;
      }
    }
    break;
  default:
    {
      result = 0.0;
    }
  }
  
  return(result);
}

/**
 * ags_low_pass_filter_util_process_s8:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_s8(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gint8 *destination;
  gint8 *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gint8 *) low_pass_filter_util->destination;
  source = (gint8 *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
  
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gint8) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_s16:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_s16(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gint16 *destination;
  gint16 *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gint16 *) low_pass_filter_util->destination;
  source = (gint16 *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gint16) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_s24:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_s24(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gint32 *destination;
  gint32 *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gint32 *) low_pass_filter_util->destination;
  source = (gint32 *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gint32) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_s32:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_s32(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gint32 *destination;
  gint32 *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gint32 *) low_pass_filter_util->destination;
  source = (gint32 *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gint32) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_s64:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_s64(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gint64 *destination;
  gint64 *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gint64 *) low_pass_filter_util->destination;
  source = (gint64 *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gint64) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_float:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_float(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gfloat *destination;
  gfloat *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gfloat *) low_pass_filter_util->destination;
  source = (gfloat *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gfloat) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_double:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_double(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  gdouble *destination;
  gdouble *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (gdouble *) low_pass_filter_util->destination;
  source = (gdouble *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    audio1_sum = ((samples - 1.0) * audio1_sum + ((gdouble) source[0])) / samples;
    audio1 = audio1_sum * gain;
    
    destination[0] = (gdouble) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process_complex:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low-pass filter of complex floating point data.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process_complex(AgsLowPassFilterUtil *low_pass_filter_util)
{
  gdouble (*param_func)(guint, gdouble, guint) = NULL;
  
  AgsComplex *destination;
  AgsComplex *source;
  
  guint source_stride, destination_stride;
  guint samplerate;
  guint buffer_length;
    
  gdouble samples, gain, no_clip;
  gdouble audio1, audio1_sum;

  guint i, i_stop;

  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  destination = (AgsComplex *) low_pass_filter_util->destination;
  source = (AgsComplex *) low_pass_filter_util->source;
  
  source_stride = low_pass_filter_util->source_stride;
  destination_stride = low_pass_filter_util->destination_stride;

  samplerate = low_pass_filter_util->samplerate;
  buffer_length = low_pass_filter_util->buffer_length;
      
  param_func = &ags_invada_convert_param;
  
  ags_invada_check_param_change(AGS_INVADA_IFILTER_FREQ, &(low_pass_filter_util->cut_off_frequency), &(low_pass_filter_util->last_freq),   &(low_pass_filter_util->converted_freq), samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_GAIN, &(low_pass_filter_util->filter_gain), &(low_pass_filter_util->last_gain),   &(low_pass_filter_util->converted_gain),  samplerate, param_func);
  ags_invada_check_param_change(AGS_INVADA_IFILTER_NOCLIP, &(low_pass_filter_util->no_clip), &(low_pass_filter_util->last_no_clip), &(low_pass_filter_util->converted_no_clip), samplerate, param_func);
    
  i = 0;
  i_stop = low_pass_filter_util->buffer_length - (low_pass_filter_util->buffer_length % 8);

  samples = low_pass_filter_util->converted_freq;
  gain = low_pass_filter_util->converted_gain;
  no_clip = low_pass_filter_util->converted_no_clip;

  audio1_sum = 0.0;
  
  for(; i < buffer_length; i++){
    double _Complex z;

    z = ags_complex_get(source);
    
    audio1_sum = ((samples - 1.0) * audio1_sum + z) / samples;
    audio1 = audio1_sum * gain;

    z = (double _Complex) ((no_clip > 0.0) ? ags_invada_ino_clip(audio1): audio1);
    
    ags_complex_set(destination,
		    z);

    source += source_stride;
    destination += destination_stride;
  }
}

/**
 * ags_low_pass_filter_util_process:
 * @low_pass_filter_util: the #AgsLowPassFilterUtil-struct
 * 
 * Process low_pass_filter.
 *
 * Since: 7.7.0
 */
void
ags_low_pass_filter_util_process(AgsLowPassFilterUtil *low_pass_filter_util)
{
  if(low_pass_filter_util == NULL ||
     low_pass_filter_util->destination == NULL ||
     low_pass_filter_util->source == NULL){
    return;
  }

  switch(low_pass_filter_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_low_pass_filter_util_process_s8(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_low_pass_filter_util_process_s16(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_low_pass_filter_util_process_s24(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_low_pass_filter_util_process_s32(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_low_pass_filter_util_process_s64(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_low_pass_filter_util_process_float(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_low_pass_filter_util_process_double(low_pass_filter_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_low_pass_filter_util_process_complex(low_pass_filter_util);
    }
    break;
  }
}
