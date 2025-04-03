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
 * Copyright (C) 2004-2009 Fons Adriaensen <fons@kokkinizita.net>
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
*/

#include <ags/audio/ags_amplifier_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

void ags_fil_proc_s8(AgsAmplifierUtil *amplifier_util,
		     guint nth_sect,
		     gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_s16(AgsAmplifierUtil *amplifier_util,
		      guint nth_sect,
		      gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_s24(AgsAmplifierUtil *amplifier_util,
		      guint nth_sect,
		      gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_s32(AgsAmplifierUtil *amplifier_util,
		      guint nth_sect,
		      gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_s64(AgsAmplifierUtil *amplifier_util,
		      guint nth_sect,
		      gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_float(AgsAmplifierUtil *amplifier_util,
			guint nth_sect,
			gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_double(AgsAmplifierUtil *amplifier_util,
			 guint nth_sect,
			 gint k, gdouble f, gdouble b, gdouble g);
void ags_fil_proc_complex(AgsAmplifierUtil *amplifier_util,
			  guint nth_sect,
			  gint k, gdouble f, gdouble b, gdouble g);

void ags_fil_proc(AgsAmplifierUtil *amplifier_util,
		  AgsSoundcardFormat format,
		  guint nth_sect,
		  gint k, gdouble f, gdouble b, gdouble g);

gdouble exp2ap(gdouble x);

/**
 * SECTION:ags_amplifier_util
 * @short_description: Boxed type of amplifier util
 * @title: AgsAmplifierUtil
 * @section_id:
 * @include: ags/audio/ags_amplifier_util.h
 *
 * Boxed type of amplifier util data type.
 */

GType
ags_amplifier_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_amplifier_util = 0;

    ags_type_amplifier_util =
      g_boxed_type_register_static("AgsAmplifierUtil",
				   (GBoxedCopyFunc) ags_amplifier_util_copy,
				   (GBoxedFreeFunc) ags_amplifier_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_amplifier_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_amplifier_util_alloc:
 *
 * Allocate #AgsAmplifierUtil-struct
 *
 * Returns: a new #AgsAmplifierUtil-struct
 *
 * Since: 7.7.0
 */
AgsAmplifierUtil*
ags_amplifier_util_alloc()
{
  AgsAmplifierUtil *ptr;

  ptr = (AgsAmplifierUtil *) g_new(AgsAmplifierUtil,
				   1);

  ptr[0] = AGS_AMPLIFIER_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_amplifier_util_copy:
 * @ptr: the original #AgsAmplifierUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAmplifierUtil-struct
 *
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_copy(AgsAmplifierUtil *ptr)
{
  AgsAmplifierUtil *new_ptr;

  guint i;
  
  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsAmplifierUtil *) g_new(AgsAmplifierUtil,
				       1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->amp_0_frequency = ptr->amp_0_frequency;
  new_ptr->amp_0_bandwidth = ptr->amp_0_bandwidth;
  new_ptr->amp_0_gain = ptr->amp_0_gain;

  new_ptr->amp_1_frequency = ptr->amp_1_frequency;
  new_ptr->amp_1_bandwidth = ptr->amp_1_bandwidth;
  new_ptr->amp_1_gain = ptr->amp_1_gain;

  new_ptr->amp_2_frequency = ptr->amp_2_frequency;
  new_ptr->amp_2_bandwidth = ptr->amp_2_bandwidth;
  new_ptr->amp_2_gain = ptr->amp_2_gain;

  new_ptr->amp_3_frequency = ptr->amp_3_frequency;
  new_ptr->amp_3_bandwidth = ptr->amp_3_bandwidth;
  new_ptr->amp_3_gain = ptr->amp_3_gain;

  new_ptr->filter_gain = ptr->filter_gain;

  for(i = 0; i < AGS_AMPLIFIER_UTIL_AMP_COUNT; i++){
    new_ptr->proc_sect[i].f = ptr->proc_sect[i].f;
    new_ptr->proc_sect[i].b = ptr->proc_sect[i].b;
    new_ptr->proc_sect[i].g = ptr->proc_sect[i].g;
  
    new_ptr->proc_sect[i].a = ptr->proc_sect[i].a;

    new_ptr->proc_sect[i].s1 = ptr->proc_sect[i].s1;
    new_ptr->proc_sect[i].s2 = ptr->proc_sect[i].s2;

    new_ptr->proc_sect[i].z1 = ptr->proc_sect[i].z1;
    new_ptr->proc_sect[i].z2 = ptr->proc_sect[i].z2;
  }
  
  return(new_ptr);
}

/**
 * ags_amplifier_util_free:
 * @ptr: the #AgsAmplifierUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_free(AgsAmplifierUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  //  g_free(ptr->destination);

  //  if(ptr->destination != ptr->source){
  //    g_free(ptr->source);
  //  }
  
  g_free(ptr);
}

/**
 * ags_amplifier_util_get_destination:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get destination buffer of @amplifier_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_get_destination(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(NULL);
  }

  return(amplifier_util->destination);
}

/**
 * ags_amplifier_util_set_destination:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @destination: (transfer none) : the destination buffer
 *
 * Set @destination buffer of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_destination(AgsAmplifierUtil *amplifier_util,
				   gpointer destination)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->destination = destination;
}

/**
 * ags_amplifier_util_get_destination_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get destination stride of @amplifier_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_destination_stride(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->destination_stride);
}

/**
 * ags_amplifier_util_set_destination_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_destination_stride(AgsAmplifierUtil *amplifier_util,
					  guint destination_stride)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->destination_stride = destination_stride;
}

/**
 * ags_amplifier_util_get_source:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get source buffer of @amplifier_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_get_source(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(NULL);
  }

  return(amplifier_util->source);
}

/**
 * ags_amplifier_util_set_source:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_source(AgsAmplifierUtil *amplifier_util,
			      gpointer source)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->source = source;
}

/**
 * ags_amplifier_util_get_source_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get source stride of @amplifier_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_source_stride(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->source_stride);
}

/**
 * ags_amplifier_util_set_source_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_source_stride(AgsAmplifierUtil *amplifier_util,
				     guint source_stride)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->source_stride = source_stride;
}

/**
 * ags_amplifier_util_get_buffer_length:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get buffer length of @amplifier_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_buffer_length(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->buffer_length);
}

/**
 * ags_amplifier_util_set_buffer_length:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_buffer_length(AgsAmplifierUtil *amplifier_util,
				     guint buffer_length)
{
  if(amplifier_util == NULL){
    return;
  }

  if(buffer_length == 0){
    ags_stream_free(amplifier_util->mix_buffer);

    amplifier_util->mix_buffer = NULL;
  }

  if(buffer_length > 0){
    if(amplifier_util->mix_buffer == NULL){
      amplifier_util->mix_buffer = ags_stream_alloc(buffer_length,
						    amplifier_util->format);
    }else{
      ags_stream_free(amplifier_util->mix_buffer);

      amplifier_util->mix_buffer = ags_stream_alloc(buffer_length,
						    amplifier_util->format);
    }
  }
  
  amplifier_util->buffer_length = buffer_length;
}

/**
 * ags_amplifier_util_get_format:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get format of @amplifier_util.
 * 
 * Returns: the format
 * 
 * Since: 7.7.0
 */
AgsSoundcardFormat
ags_amplifier_util_get_format(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->format);
}

/**
 * ags_amplifier_util_set_format:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @format: the format
 *
 * Set @format of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_format(AgsAmplifierUtil *amplifier_util,
			      AgsSoundcardFormat format)
{
  if(amplifier_util == NULL){
    return;
  }

  if(amplifier_util->buffer_length > 0){
    if(amplifier_util->mix_buffer == NULL){
      amplifier_util->mix_buffer = ags_stream_alloc(amplifier_util->buffer_length,
						    format);
    }else{
      ags_stream_free(amplifier_util->mix_buffer);

      amplifier_util->mix_buffer = ags_stream_alloc(amplifier_util->buffer_length,
						    format);
    }
  }

  amplifier_util->format = format;
}

/**
 * ags_amplifier_util_get_samplerate:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get samplerate of @amplifier_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_samplerate(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->samplerate);
}

/**
 * ags_amplifier_util_set_samplerate:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_samplerate(AgsAmplifierUtil *amplifier_util,
				  guint samplerate)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->samplerate = samplerate;
}

/**
 * ags_amplifier_util_get_amp_0_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_frequency);
}

/**
 * ags_amplifier_util_set_amp_0_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_frequency: the amp 0 frequency
 *
 * Set @amp_0_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_0_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_frequency = amp_0_frequency;
}

/**
 * ags_amplifier_util_get_amp_0_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_0_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_bandwidth: the amp 0 bandwidth
 *
 * Set @amp_0_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_0_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_bandwidth = amp_0_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_0_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_gain);
}

/**
 * ags_amplifier_util_set_amp_0_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_gain: the amp 0 gain
 *
 * Set @amp_0_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_0_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_gain = amp_0_gain;
}

/**
 * ags_amplifier_util_get_amp_1_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_frequency);
}

/**
 * ags_amplifier_util_set_amp_1_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_frequency: the amp 1 frequency
 *
 * Set @amp_1_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_1_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_frequency = amp_1_frequency;
}

/**
 * ags_amplifier_util_get_amp_1_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_1_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_bandwidth: the amp 1 bandwidth
 *
 * Set @amp_1_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_1_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_bandwidth = amp_1_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_1_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_gain);
}

/**
 * ags_amplifier_util_set_amp_1_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_gain: the amp 1 gain
 *
 * Set @amp_1_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_1_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_gain = amp_1_gain;
}

/**
 * ags_amplifier_util_get_amp_2_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_frequency);
}

/**
 * ags_amplifier_util_set_amp_2_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_frequency: the amp 2 frequency
 *
 * Set @amp_2_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_2_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_frequency = amp_2_frequency;
}

/**
 * ags_amplifier_util_get_amp_2_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_2_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_bandwidth: the amp 2 bandwidth
 *
 * Set @amp_2_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_2_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_bandwidth = amp_2_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_2_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_gain);
}

/**
 * ags_amplifier_util_set_amp_2_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_gain: the amp 2 gain
 *
 * Set @amp_2_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_2_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_gain = amp_2_gain;
}

/**
 * ags_amplifier_util_get_amp_3_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_frequency);
}

/**
 * ags_amplifier_util_set_amp_3_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_frequency: the amp 3 frequency
 *
 * Set @amp_3_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_3_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_frequency = amp_3_frequency;
}

/**
 * ags_amplifier_util_get_amp_3_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_3_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_bandwidth: the amp 3 bandwidth
 *
 * Set @amp_3_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_3_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_bandwidth = amp_3_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_3_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_gain);
}

/**
 * ags_amplifier_util_set_amp_3_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_gain: the amp 3 gain
 *
 * Set @amp_3_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_3_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_gain = amp_3_gain;
}

/**
 * ags_amplifier_util_get_filter_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get filter gain of @amplifier_util.
 * 
 * Returns: the gain of filter
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_filter_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->filter_gain);
}

/**
 * ags_amplifier_util_set_filter_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @filter_gain: the filter gain
 *
 * Set @filter_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_filter_gain(AgsAmplifierUtil *amplifier_util,
				   gdouble filter_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->filter_gain = filter_gain;
}

void
ags_fil_proc_s8(AgsAmplifierUtil *amplifier_util,
		guint nth_sect,
		gint k, gdouble f, gdouble b, gdouble g)
{
  gint8 *mix_buffer;
  gint8 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint8 *) amplifier_util->mix_buffer;

  source = (gint8 *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint8) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_s16(AgsAmplifierUtil *amplifier_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint16 *mix_buffer;
  gint16 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint16 *) amplifier_util->mix_buffer;

  source = (gint16 *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint16) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_s24(AgsAmplifierUtil *amplifier_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint32 *mix_buffer;
  gint32 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint32 *) amplifier_util->mix_buffer;

  source = (gint32 *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint32) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_s32(AgsAmplifierUtil *amplifier_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint32 *mix_buffer;
  gint32 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint32 *) amplifier_util->mix_buffer;

  source = (gint32 *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint32) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_s64(AgsAmplifierUtil *amplifier_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint64 *mix_buffer;
  gint64 *source;

  guint destination_stride, source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint64 *) amplifier_util->mix_buffer;

  source = (gint64 *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint64) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_float(AgsAmplifierUtil *amplifier_util,
		    guint nth_sect,
		    gint k, gdouble f, gdouble b, gdouble g)
{
  gfloat *mix_buffer;
  gfloat *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gfloat *) amplifier_util->mix_buffer;

  source = (gfloat *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gfloat) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_double(AgsAmplifierUtil *amplifier_util,
		    guint nth_sect,
		    gint k, gdouble f, gdouble b, gdouble g)
{
  gdouble *mix_buffer;
  gdouble *source;

  guint destination_stride, source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gdouble *) amplifier_util->mix_buffer;

  source = (gdouble *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gdouble) (a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc_complex(AgsAmplifierUtil *amplifier_util,
		     guint nth_sect,
		     gint k, gdouble f, gdouble b, gdouble g)
{
  AgsComplex *mix_buffer;
  AgsComplex *source;

  guint source_stride;

  double _Complex x, y;
  gdouble s1, s2, d1, d2, a, da;
  gboolean  u2 = FALSE;

  mix_buffer = (AgsComplex *) amplifier_util->mix_buffer;

  source = (AgsComplex *) amplifier_util->source;
  source_stride = amplifier_util->source_stride;

  s1 = amplifier_util->proc_sect[nth_sect].s1;
  s2 = amplifier_util->proc_sect[nth_sect].s2;
  
  a = amplifier_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier_util->proc_sect[nth_sect].f;
    }
    
    amplifier_util->proc_sect[nth_sect].f = f;
    amplifier_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier_util->proc_sect[nth_sect].g;
    }
    
    amplifier_util->proc_sect[nth_sect].g = g;
    amplifier_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier_util->proc_sect[nth_sect].b;
    }
      
    amplifier_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = ags_complex_get(source);
    y = x - s2 * amplifier_util->proc_sect[nth_sect].z2;

    ags_complex_set(mix_buffer,
		    x - a * (amplifier_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier_util->proc_sect[nth_sect].z1;
    amplifier_util->proc_sect[nth_sect].z2 = amplifier_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

void
ags_fil_proc(AgsAmplifierUtil *amplifier_util,
	     AgsSoundcardFormat format,
	     guint nth_sect,
	     gint k, gdouble f, gdouble b, gdouble g)
{
  if(amplifier_util == NULL ||
     amplifier_util->destination == NULL ||
     amplifier_util->source == NULL){
    return;
  }
  
  switch(amplifier_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_fil_proc_s8(amplifier_util,
		      nth_sect,
		      k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_fil_proc_s16(amplifier_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_fil_proc_s24(amplifier_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_fil_proc_s32(amplifier_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_fil_proc_s64(amplifier_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_fil_proc_float(amplifier_util,
			 nth_sect,
			 k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_fil_proc_double(amplifier_util,
			  nth_sect,
			  k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_fil_proc_complex(amplifier_util,
			   nth_sect,
			   k, f, b, g);
    }
    break;
  }
}

gdouble
exp2ap(gdouble x)
{
  int i;

  i = (int)(floor (x));
  x -= i;

  return(ldexp(1 + x * (0.6930f + x * (0.2416f + x * (0.0517f + x * 0.0137f))), i));
}

/**
 * ags_amplifier_util_process_s8:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s8(AgsAmplifierUtil *amplifier_util)
{
  gint8 *destination;
  gint8 *aip;
  gint8 *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gint8 *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;

  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = (gint8) (g * (gdouble) aip[i * source_stride]);
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i] = (gint8) (g * (gdouble) sig[i] + (1.0 - g) * (gdouble) aip[i * source_stride]);
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_s16:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s16(AgsAmplifierUtil *amplifier_util)
{
  gint16 *destination;
  gint16 *aip;
  gint16 *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gint16 *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = (gint16) (g * (gdouble) aip[i * source_stride]);
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = (gint16) (g * (gdouble) sig[i] + (1.0 - g) * (gdouble) aip[i * source_stride]);
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_s24:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s24(AgsAmplifierUtil *amplifier_util)
{
  gint32 *destination;
  gint32 *aip;
  gint32 *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gint32 *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_s32:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s32(AgsAmplifierUtil *amplifier_util)
{
  gint32 *destination;
  gint32 *aip;
  gint32 *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gint32 *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_s64:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s64(AgsAmplifierUtil *amplifier_util)
{
  gint64 *destination;
  gint64 *aip;
  gint64 *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gint64 *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_float:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_float(AgsAmplifierUtil *amplifier_util)
{
  gfloat *destination;
  gfloat *aip;
  gfloat *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  gfloat *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_double:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_double(AgsAmplifierUtil *amplifier_util)
{
  double *destination;
  double *aip;
  double *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  double *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	aop[i * destination_stride] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process_complex:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of complex floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_complex(AgsAmplifierUtil *amplifier_util)
{
  AgsComplex *destination;
  AgsComplex *aip;
  AgsComplex *aop;

  guint source_stride, destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint len;
  gint i, j, k;
  AgsComplex *p, sig[48];
  gdouble t, g, d;
  gdouble fgain;
  gdouble sfreq[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER_UTIL_AMP_COUNT];

  aip = amplifier_util->source;
  aop = amplifier_util->mix_buffer;
  
  destination = amplifier_util->destination;
  
  source_stride = amplifier_util->source_stride;
  destination_stride = amplifier_util->destination_stride;

  len = 
    buffer_length = amplifier_util->buffer_length;
  format = amplifier_util->format;
  
  fgain = exp2ap(0.1661 * amplifier_util->filter_gain);
  
  for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier_util->amp_0_frequency / amplifier_util->samplerate;

      break;
    case 1:
      t = amplifier_util->amp_1_frequency / amplifier_util->samplerate;

      break;
    case 2:
      t = amplifier_util->amp_2_frequency / amplifier_util->samplerate;

      break;
    case 3:
      t = amplifier_util->amp_3_frequency / amplifier_util->samplerate;

      break;
    default:
      g_warning("unknown amp freq");
    }
      
    if(t < 0.0002){
      t = 0.0002;
    }

    if(t > 0.4998){
      t = 0.4998;
    }

    sfreq [j] = t;        

    switch(j){
    case 0:
      sband[j] = amplifier_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier_util->amp_3_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    if(amplifier_util->amp_0_frequency > 0.0){
      switch(j){
      case 0:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_0_gain);

	break;
      case 1:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_1_gain);

	break;
      case 2:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_2_gain);

	break;
      case 3:
	sgain[j] = exp2ap (0.1661 * amplifier_util->amp_3_gain);

	break;
      default:
	g_warning("unknown amp gain");
      }

    }else{
      sgain [j] = 1.0;
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      ags_complex_set(sig + i,
		      g * ags_complex_get(aip + (i * source_stride)));
    }

    for(j = 0; j < AGS_AMPLIFIER_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier_util->amp_0_frequency > 0.0){
      if(j == 16){
	p = sig;
      }else ++j;
    }else{
      if(j == 0){
	p = aip;
      }else{
	--j;
      }
    }
    
    amplifier_util->fade = j;
    
    if(p){
      ags_audio_buffer_util_clear_buffer(NULL,
					 aop, 1,
					 k, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  aop, 1, 0,
						  p, source_stride, 0,
						  k, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												     ags_audio_buffer_util_format_from_soundcard(NULL, format),
												     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
      
      //      memcpy(aop, p, k * sizeof (float));
    }else{
      d = (j / 16.0 - g) / k;
      
      for (i = 0; i < k; i++){
	g += d;
	ags_complex_set(aop + (i * destination_stride),
			g * ags_complex_get(sig + i) + (1.0 - g) * ags_complex_get(aip + (i * source_stride)));
      }
    }
    
    aip += (k * source_stride);
    aop += (k);
    len -= k;
  }

  ags_audio_buffer_util_clear_buffer(NULL,
				     destination, destination_stride,
				     buffer_length, ags_audio_buffer_util_format_from_soundcard(NULL, format));
  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
					      destination, destination_stride, 0,
					      aop, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
												 ags_audio_buffer_util_format_from_soundcard(NULL, format),
												 ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier_util_process:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL ||
     amplifier_util->destination == NULL ||
     amplifier_util->source == NULL){
    return;
  }

  switch(amplifier_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_amplifier_util_process_s8(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_amplifier_util_process_s16(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_amplifier_util_process_s24(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_amplifier_util_process_s32(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_amplifier_util_process_s64(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_amplifier_util_process_float(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_amplifier_util_process_double(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_amplifier_util_process_complex(amplifier_util);
    }
    break;
  }
}
