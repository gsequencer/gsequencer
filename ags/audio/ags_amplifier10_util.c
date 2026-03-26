/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/ags_amplifier10_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

static void ags_fil_proc_s8(AgsAmplifier10Util *amplifier10_util,
			    guint nth_sect,
			    gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_s16(AgsAmplifier10Util *amplifier10_util,
			     guint nth_sect,
			     gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_s24(AgsAmplifier10Util *amplifier10_util,
			     guint nth_sect,
			     gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_s32(AgsAmplifier10Util *amplifier10_util,
			     guint nth_sect,
			     gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_s64(AgsAmplifier10Util *amplifier10_util,
			     guint nth_sect,
			     gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_float(AgsAmplifier10Util *amplifier10_util,
			       guint nth_sect,
			       gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_double(AgsAmplifier10Util *amplifier10_util,
				guint nth_sect,
				gint k, gdouble f, gdouble b, gdouble g);
static void ags_fil_proc_complex(AgsAmplifier10Util *amplifier10_util,
				 guint nth_sect,
				 gint k, gdouble f, gdouble b, gdouble g);

static void ags_fil_proc(AgsAmplifier10Util *amplifier10_util,
			 AgsSoundcardFormat format,
			 guint nth_sect,
			 gint k, gdouble f, gdouble b, gdouble g);

static gdouble exp2ap(gdouble x);

/**
 * SECTION:ags_amplifier10_util
 * @short_description: Boxed type of amplifier10 util
 * @title: AgsAmplifier10Util
 * @section_id:
 * @include: ags/audio/ags_amplifier10_util.h
 *
 * Boxed type of amplifier10 util data type.
 */

GType
ags_amplifier10_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_amplifier10_util = 0;

    ags_type_amplifier10_util =
      g_boxed_type_register_static("AgsAmplifier10Util",
				   (GBoxedCopyFunc) ags_amplifier10_util_copy,
				   (GBoxedFreeFunc) ags_amplifier10_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_amplifier10_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_amplifier10_util_alloc:
 *
 * Allocate #AgsAmplifier10Util-struct
 *
 * Returns: a new #AgsAmplifier10Util-struct
 *
 * Since: 8.4.0
 */
AgsAmplifier10Util*
ags_amplifier10_util_alloc()
{
  AgsAmplifier10Util *ptr;

  ptr = (AgsAmplifier10Util *) g_new(AgsAmplifier10Util,
				     1);

  ptr[0] = AGS_AMPLIFIER10_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_amplifier10_util_copy:
 * @ptr: the original #AgsAmplifier10Util-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAmplifier10Util-struct
 *
 * Since: 8.4.0
 */
gpointer
ags_amplifier10_util_copy(AgsAmplifier10Util *ptr)
{
  AgsAmplifier10Util *new_ptr;

  guint i;
  
  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsAmplifier10Util *) g_new(AgsAmplifier10Util,
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
  
  new_ptr->amp_3_frequency = ptr->amp_4_frequency;
  new_ptr->amp_3_bandwidth = ptr->amp_4_bandwidth;
  new_ptr->amp_3_gain = ptr->amp_4_gain;

  new_ptr->amp_5_frequency = ptr->amp_5_frequency;
  new_ptr->amp_5_bandwidth = ptr->amp_5_bandwidth;
  new_ptr->amp_5_gain = ptr->amp_5_gain;

  new_ptr->amp_6_frequency = ptr->amp_6_frequency;
  new_ptr->amp_6_bandwidth = ptr->amp_6_bandwidth;
  new_ptr->amp_6_gain = ptr->amp_6_gain;

  new_ptr->amp_7_frequency = ptr->amp_7_frequency;
  new_ptr->amp_7_bandwidth = ptr->amp_7_bandwidth;
  new_ptr->amp_7_gain = ptr->amp_7_gain;

  new_ptr->amp_8_frequency = ptr->amp_8_frequency;
  new_ptr->amp_8_bandwidth = ptr->amp_8_bandwidth;
  new_ptr->amp_8_gain = ptr->amp_8_gain;
  
  new_ptr->amp_3_frequency = ptr->amp_9_frequency;
  new_ptr->amp_3_bandwidth = ptr->amp_9_bandwidth;
  new_ptr->amp_3_gain = ptr->amp_9_gain;

  new_ptr->filter_gain = ptr->filter_gain;

  for(i = 0; i < AGS_AMPLIFIER10_UTIL_AMP_COUNT; i++){
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
 * ags_amplifier10_util_free:
 * @ptr: the #AgsAmplifier10Util-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_free(AgsAmplifier10Util *ptr)
{
  g_return_if_fail(ptr != NULL);

  //  g_free(ptr->destination);

  //  if(ptr->destination != ptr->source){
  //    g_free(ptr->source);
  //  }
  
  g_free(ptr);
}

/**
 * ags_amplifier10_util_get_destination:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get destination buffer of @amplifier10_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 8.4.0
 */
gpointer
ags_amplifier10_util_get_destination(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(NULL);
  }

  return(amplifier10_util->destination);
}

/**
 * ags_amplifier10_util_set_destination:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @destination: (transfer none) : the destination buffer
 *
 * Set @destination buffer of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_destination(AgsAmplifier10Util *amplifier10_util,
				     gpointer destination)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->destination = destination;
}

/**
 * ags_amplifier10_util_get_destination_stride:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get destination stride of @amplifier10_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 8.4.0
 */
guint
ags_amplifier10_util_get_destination_stride(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0);
  }

  return(amplifier10_util->destination_stride);
}

/**
 * ags_amplifier10_util_set_destination_stride:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_destination_stride(AgsAmplifier10Util *amplifier10_util,
					    guint destination_stride)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->destination_stride = destination_stride;
}

/**
 * ags_amplifier10_util_get_source:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get source buffer of @amplifier10_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 8.4.0
 */
gpointer
ags_amplifier10_util_get_source(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(NULL);
  }

  return(amplifier10_util->source);
}

/**
 * ags_amplifier10_util_set_source:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_source(AgsAmplifier10Util *amplifier10_util,
				gpointer source)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->source = source;
}

/**
 * ags_amplifier10_util_get_source_stride:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get source stride of @amplifier10_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 8.4.0
 */
guint
ags_amplifier10_util_get_source_stride(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0);
  }

  return(amplifier10_util->source_stride);
}

/**
 * ags_amplifier10_util_set_source_stride:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_source_stride(AgsAmplifier10Util *amplifier10_util,
				       guint source_stride)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->source_stride = source_stride;
}

/**
 * ags_amplifier10_util_get_buffer_length:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get buffer length of @amplifier10_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 8.4.0
 */
guint
ags_amplifier10_util_get_buffer_length(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0);
  }

  return(amplifier10_util->buffer_length);
}

/**
 * ags_amplifier10_util_set_buffer_length:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_buffer_length(AgsAmplifier10Util *amplifier10_util,
				       guint buffer_length)
{
  if(amplifier10_util == NULL ||
     amplifier10_util->buffer_length == buffer_length){
    return;
  }

  ags_stream_free(amplifier10_util->mix_buffer);

  amplifier10_util->mix_buffer = NULL;

  if(buffer_length > 0){
    amplifier10_util->mix_buffer = ags_stream_alloc(buffer_length,
						    amplifier10_util->format);
  }
  
  amplifier10_util->buffer_length = buffer_length;
}

/**
 * ags_amplifier10_util_get_format:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get format of @amplifier10_util.
 * 
 * Returns: the format
 * 
 * Since: 8.4.0
 */
AgsSoundcardFormat
ags_amplifier10_util_get_format(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0);
  }

  return(amplifier10_util->format);
}

/**
 * ags_amplifier10_util_set_format:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @format: the format
 *
 * Set @format of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_format(AgsAmplifier10Util *amplifier10_util,
				AgsSoundcardFormat format)
{
  if(amplifier10_util == NULL){
    return;
  }

  if(amplifier10_util->buffer_length > 0){
    if(amplifier10_util->mix_buffer == NULL){
      amplifier10_util->mix_buffer = ags_stream_alloc(amplifier10_util->buffer_length,
						      format);
    }else{
      ags_stream_free(amplifier10_util->mix_buffer);

      amplifier10_util->mix_buffer = ags_stream_alloc(amplifier10_util->buffer_length,
						      format);
    }
  }

  amplifier10_util->format = format;
}

/**
 * ags_amplifier10_util_get_samplerate:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get samplerate of @amplifier10_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 8.4.0
 */
guint
ags_amplifier10_util_get_samplerate(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0);
  }

  return(amplifier10_util->samplerate);
}

/**
 * ags_amplifier10_util_set_samplerate:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_samplerate(AgsAmplifier10Util *amplifier10_util,
				    guint samplerate)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->samplerate = samplerate;
}


/**
 * ags_amplifier10_util_get_amp_0_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_0_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_0_frequency);
}

/**
 * ags_amplifier10_util_set_amp_0_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_0_frequency: the amp 0 frequency
 *
 * Set @amp_0_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_0_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_0_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_0_frequency = amp_0_frequency;
}

/**
 * ags_amplifier10_util_get_amp_0_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_0_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_0_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_0_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_0_bandwidth: the amp 0 bandwidth
 *
 * Set @amp_0_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_0_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_0_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_0_bandwidth = amp_0_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_0_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_0_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_0_gain);
}

/**
 * ags_amplifier10_util_set_amp_0_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_0_gain: the amp 0 gain
 *
 * Set @amp_0_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_0_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_0_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_0_gain = amp_0_gain;
}

/**
 * ags_amplifier10_util_get_amp_1_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_1_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_1_frequency);
}

/**
 * ags_amplifier10_util_set_amp_1_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_1_frequency: the amp 1 frequency
 *
 * Set @amp_1_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_1_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_1_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_1_frequency = amp_1_frequency;
}

/**
 * ags_amplifier10_util_get_amp_1_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_1_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_1_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_1_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_1_bandwidth: the amp 1 bandwidth
 *
 * Set @amp_1_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_1_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_1_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_1_bandwidth = amp_1_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_1_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_1_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_1_gain);
}

/**
 * ags_amplifier10_util_set_amp_1_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_1_gain: the amp 1 gain
 *
 * Set @amp_1_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_1_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_1_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_1_gain = amp_1_gain;
}

/**
 * ags_amplifier10_util_get_amp_2_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_2_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_2_frequency);
}

/**
 * ags_amplifier10_util_set_amp_2_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_2_frequency: the amp 2 frequency
 *
 * Set @amp_2_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_2_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_2_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_2_frequency = amp_2_frequency;
}

/**
 * ags_amplifier10_util_get_amp_2_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_2_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_2_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_2_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_2_bandwidth: the amp 2 bandwidth
 *
 * Set @amp_2_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_2_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_2_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_2_bandwidth = amp_2_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_2_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_2_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_2_gain);
}

/**
 * ags_amplifier10_util_set_amp_2_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_2_gain: the amp 2 gain
 *
 * Set @amp_2_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_2_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_2_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_2_gain = amp_2_gain;
}

/**
 * ags_amplifier10_util_get_amp_3_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_3_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_3_frequency);
}

/**
 * ags_amplifier10_util_set_amp_3_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_3_frequency: the amp 3 frequency
 *
 * Set @amp_3_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_3_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_3_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_3_frequency = amp_3_frequency;
}

/**
 * ags_amplifier10_util_get_amp_3_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_3_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_3_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_3_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_3_bandwidth: the amp 3 bandwidth
 *
 * Set @amp_3_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_3_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_3_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_3_bandwidth = amp_3_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_3_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_3_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_3_gain);
}

/**
 * ags_amplifier10_util_set_amp_3_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_3_gain: the amp 3 gain
 *
 * Set @amp_3_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_3_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_3_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_3_gain = amp_3_gain;
}

/**
 * ags_amplifier10_util_get_amp_4_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_4_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_4_frequency);
}

/**
 * ags_amplifier10_util_set_amp_4_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_4_frequency: the amp 4 frequency
 *
 * Set @amp_4_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_4_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_4_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_4_frequency = amp_4_frequency;
}

/**
 * ags_amplifier10_util_get_amp_4_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_4_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_4_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_4_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_4_bandwidth: the amp 4 bandwidth
 *
 * Set @amp_4_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_4_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_4_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_4_bandwidth = amp_4_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_4_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_4_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_4_gain);
}

/**
 * ags_amplifier10_util_set_amp_4_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_4_gain: the amp 4 gain
 *
 * Set @amp_4_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_4_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_4_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_4_gain = amp_4_gain;
}

/**
 * ags_amplifier10_util_get_amp_5_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_5_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_5_frequency);
}

/**
 * ags_amplifier10_util_set_amp_5_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_5_frequency: the amp 0 frequency
 *
 * Set @amp_5_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_5_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_5_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_5_frequency = amp_5_frequency;
}

/**
 * ags_amplifier10_util_get_amp_5_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_5_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_5_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_5_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_5_bandwidth: the amp 0 bandwidth
 *
 * Set @amp_5_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_5_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_5_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_5_bandwidth = amp_5_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_5_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 0 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 0
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_5_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_5_gain);
}

/**
 * ags_amplifier10_util_set_amp_5_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_5_gain: the amp 0 gain
 *
 * Set @amp_5_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_5_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_5_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_5_gain = amp_5_gain;
}

/**
 * ags_amplifier10_util_get_amp_6_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_6_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_6_frequency);
}

/**
 * ags_amplifier10_util_set_amp_6_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_6_frequency: the amp 1 frequency
 *
 * Set @amp_6_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_6_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_6_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_6_frequency = amp_6_frequency;
}

/**
 * ags_amplifier10_util_get_amp_6_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_6_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_6_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_6_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_6_bandwidth: the amp 1 bandwidth
 *
 * Set @amp_6_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_6_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_6_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_6_bandwidth = amp_6_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_6_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 1 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 1
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_6_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_6_gain);
}

/**
 * ags_amplifier10_util_set_amp_6_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_6_gain: the amp 1 gain
 *
 * Set @amp_6_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_6_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_6_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_6_gain = amp_6_gain;
}

/**
 * ags_amplifier10_util_get_amp_7_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_7_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_7_frequency);
}

/**
 * ags_amplifier10_util_set_amp_7_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_7_frequency: the amp 2 frequency
 *
 * Set @amp_7_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_7_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_7_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_7_frequency = amp_7_frequency;
}

/**
 * ags_amplifier10_util_get_amp_7_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_7_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_7_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_7_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_7_bandwidth: the amp 2 bandwidth
 *
 * Set @amp_7_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_7_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_7_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_7_bandwidth = amp_7_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_7_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 2 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 2
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_7_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_7_gain);
}

/**
 * ags_amplifier10_util_set_amp_7_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_7_gain: the amp 2 gain
 *
 * Set @amp_7_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_7_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_7_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_7_gain = amp_7_gain;
}

/**
 * ags_amplifier10_util_get_amp_8_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_8_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_8_frequency);
}

/**
 * ags_amplifier10_util_set_amp_8_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_8_frequency: the amp 3 frequency
 *
 * Set @amp_8_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_8_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_8_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_8_frequency = amp_8_frequency;
}

/**
 * ags_amplifier10_util_get_amp_8_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_8_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_8_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_8_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_8_bandwidth: the amp 3 bandwidth
 *
 * Set @amp_8_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_8_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_8_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_8_bandwidth = amp_8_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_8_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 3 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 3
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_8_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_8_gain);
}

/**
 * ags_amplifier10_util_set_amp_8_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_8_gain: the amp 3 gain
 *
 * Set @amp_8_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_8_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_8_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_8_gain = amp_8_gain;
}

/**
 * ags_amplifier10_util_get_amp_9_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 frequency of @amplifier10_util.
 * 
 * Returns: the frequency of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_9_frequency(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_9_frequency);
}

/**
 * ags_amplifier10_util_set_amp_9_frequency:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_9_frequency: the amp 4 frequency
 *
 * Set @amp_9_frequency of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_9_frequency(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_9_frequency)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_9_frequency = amp_9_frequency;
}

/**
 * ags_amplifier10_util_get_amp_9_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 bandwidth of @amplifier10_util.
 * 
 * Returns: the bandwidth of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_9_bandwidth(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_9_bandwidth);
}

/**
 * ags_amplifier10_util_set_amp_9_bandwidth:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_9_bandwidth: the amp 4 bandwidth
 *
 * Set @amp_9_bandwidth of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_9_bandwidth(AgsAmplifier10Util *amplifier10_util,
					 gdouble amp_9_bandwidth)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_9_bandwidth = amp_9_bandwidth;
}

/**
 * ags_amplifier10_util_get_amp_9_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get amp 4 gain of @amplifier10_util.
 * 
 * Returns: the gain of amp 4
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_amp_9_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->amp_9_gain);
}

/**
 * ags_amplifier10_util_set_amp_9_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @amp_9_gain: the amp 4 gain
 *
 * Set @amp_9_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_amp_9_gain(AgsAmplifier10Util *amplifier10_util,
				    gdouble amp_9_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->amp_9_gain = amp_9_gain;
}

/**
 * ags_amplifier10_util_get_filter_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Get filter gain of @amplifier10_util.
 * 
 * Returns: the gain of filter
 * 
 * Since: 8.4.0
 */
gdouble
ags_amplifier10_util_get_filter_gain(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL){
    return(0.0);
  }

  return(amplifier10_util->filter_gain);
}

/**
 * ags_amplifier10_util_set_filter_gain:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * @filter_gain: the filter gain
 *
 * Set @filter_gain of @amplifier10_util.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_set_filter_gain(AgsAmplifier10Util *amplifier10_util,
				     gdouble filter_gain)
{
  if(amplifier10_util == NULL){
    return;
  }

  amplifier10_util->filter_gain = filter_gain;
}

static void
ags_fil_proc_s8(AgsAmplifier10Util *amplifier10_util,
		guint nth_sect,
		gint k, gdouble f, gdouble b, gdouble g)
{
  gint8 *mix_buffer;
  gint8 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint8 *) amplifier10_util->mix_buffer;

  source = (gint8 *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint8) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_s16(AgsAmplifier10Util *amplifier10_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint16 *mix_buffer;
  gint16 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint16 *) amplifier10_util->mix_buffer;

  source = (gint16 *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint16) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_s24(AgsAmplifier10Util *amplifier10_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint32 *mix_buffer;
  gint32 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint32 *) amplifier10_util->mix_buffer;

  source = (gint32 *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint32) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_s32(AgsAmplifier10Util *amplifier10_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint32 *mix_buffer;
  gint32 *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint32 *) amplifier10_util->mix_buffer;

  source = (gint32 *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint32) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_s64(AgsAmplifier10Util *amplifier10_util,
		 guint nth_sect,
		 gint k, gdouble f, gdouble b, gdouble g)
{
  gint64 *mix_buffer;
  gint64 *source;

  guint destination_stride, source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gint64 *) amplifier10_util->mix_buffer;

  source = (gint64 *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gint64) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_float(AgsAmplifier10Util *amplifier10_util,
		   guint nth_sect,
		   gint k, gdouble f, gdouble b, gdouble g)
{
  gfloat *mix_buffer;
  gfloat *source;

  guint source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gfloat *) amplifier10_util->mix_buffer;

  source = (gfloat *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gfloat) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_double(AgsAmplifier10Util *amplifier10_util,
		    guint nth_sect,
		    gint k, gdouble f, gdouble b, gdouble g)
{
  gdouble *mix_buffer;
  gdouble *source;

  guint destination_stride, source_stride;
  
  gdouble s1, s2, d1, d2, a, da, x, y;
  gboolean  u2 = FALSE;

  mix_buffer = (gdouble *) amplifier10_util->mix_buffer;

  source = (gdouble *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = (source[0]);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    mix_buffer[0] -= (gdouble) (a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x));

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc_complex(AgsAmplifier10Util *amplifier10_util,
		     guint nth_sect,
		     gint k, gdouble f, gdouble b, gdouble g)
{
  AgsComplex *mix_buffer;
  AgsComplex *source;

  guint source_stride;

  double _Complex x, y;
  gdouble s1, s2, d1, d2, a, da;
  gboolean  u2 = FALSE;

  mix_buffer = (AgsComplex *) amplifier10_util->mix_buffer;

  source = (AgsComplex *) amplifier10_util->source;
  source_stride = amplifier10_util->source_stride;

  s1 = amplifier10_util->proc_sect[nth_sect].s1;
  s2 = amplifier10_util->proc_sect[nth_sect].s2;
  
  a = amplifier10_util->proc_sect[nth_sect].a;

  d1 = 0.0;
  d2 = 0.0;
  da = 0.0;

  if(f != amplifier10_util->proc_sect[nth_sect].f){
    if(f < 0.5f * amplifier10_util->proc_sect[nth_sect].f){
      f = 0.5f * amplifier10_util->proc_sect[nth_sect].f;
    }else if (f > 2.0f * amplifier10_util->proc_sect[nth_sect].f){
      f = 2.0f * amplifier10_util->proc_sect[nth_sect].f;
    }
    
    amplifier10_util->proc_sect[nth_sect].f = f;
    amplifier10_util->proc_sect[nth_sect].s1 = -cosf (6.283185f * f);
    d1 = (amplifier10_util->proc_sect[nth_sect].s1 - s1) / k;           
    u2 = TRUE;
  }             
       
  if(g != amplifier10_util->proc_sect[nth_sect].g){
    if(g < 0.5f * amplifier10_util->proc_sect[nth_sect].g){
      g = 0.5f * amplifier10_util->proc_sect[nth_sect].g;
    }else if (g > 2.0f * amplifier10_util->proc_sect[nth_sect].g){
      g = 2.0f * amplifier10_util->proc_sect[nth_sect].g;
    }
    
    amplifier10_util->proc_sect[nth_sect].g = g;
    amplifier10_util->proc_sect[nth_sect].a = 0.5f * (g - 1.0f);
    da = (amplifier10_util->proc_sect[nth_sect].a - a) / k;
    u2 = TRUE;
  }

  if(b != amplifier10_util->proc_sect[nth_sect].b){
    if(b < 0.5f * amplifier10_util->proc_sect[nth_sect].b){
      b = 0.5f * amplifier10_util->proc_sect[nth_sect].b;
    }else if(b > 2.0f * amplifier10_util->proc_sect[nth_sect].b){
      b = 2.0f * amplifier10_util->proc_sect[nth_sect].b;
    }
      
    amplifier10_util->proc_sect[nth_sect].b = b; 
    u2 = TRUE;
  }

  if(u2){
    b *= 7.0 * f / sqrtf(g);         
    amplifier10_util->proc_sect[nth_sect].s2 = (1 - b) / (1 + b);
    d2 = (amplifier10_util->proc_sect[nth_sect].s2 - s2) / k;        
  }

  while(k--){  
    s1 += d1;
    s2 += d2;
    a += da;

    x = ags_complex_get(source);
    y = x - s2 * amplifier10_util->proc_sect[nth_sect].z2;

    ags_complex_set(mix_buffer,
		    x - a * (amplifier10_util->proc_sect[nth_sect].z2 + s2 * y - x)); 

    y -= s1 * amplifier10_util->proc_sect[nth_sect].z1;
    amplifier10_util->proc_sect[nth_sect].z2 = amplifier10_util->proc_sect[nth_sect].z1 + s1 * y;
    amplifier10_util->proc_sect[nth_sect].z1 = y + 1e-10f;

    source += source_stride;
    mix_buffer++;
  }
}

static void
ags_fil_proc(AgsAmplifier10Util *amplifier10_util,
	     AgsSoundcardFormat format,
	     guint nth_sect,
	     gint k, gdouble f, gdouble b, gdouble g)
{
  if(amplifier10_util == NULL ||
     amplifier10_util->destination == NULL ||
     amplifier10_util->source == NULL){
    return;
  }
  
  switch(amplifier10_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_fil_proc_s8(amplifier10_util,
		      nth_sect,
		      k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_fil_proc_s16(amplifier10_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_fil_proc_s24(amplifier10_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_fil_proc_s32(amplifier10_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_fil_proc_s64(amplifier10_util,
		       nth_sect,
		       k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_fil_proc_float(amplifier10_util,
			 nth_sect,
			 k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_fil_proc_double(amplifier10_util,
			  nth_sect,
			  k, f, b, g);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_fil_proc_complex(amplifier10_util,
			   nth_sect,
			   k, f, b, g);
    }
    break;
  }
}

static gdouble
exp2ap(gdouble x)
{
  int i;

  i = (int)(floor (x));
  x -= i;

  return(ldexp(1 + x * (0.6930f + x * (0.2416f + x * (0.0517f + x * 0.0137f))), i));
}

/**
 * ags_amplifier10_util_process_s8:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of signed 8 bit data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_s8(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;

  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);

  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }
    
    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = (gint8) (g * (gdouble) aip[i * source_stride]);
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_s16:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of signed 16 bit data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_s16(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = (gint16) (g * (gdouble) aip[i * source_stride]);
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = (gint16) (g * (gdouble) sig[i] + (1.0 - g) * (gdouble) aip[i * source_stride]);
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_s24:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of signed 24 bit data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_s24(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_s32:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of signed 32 bit data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_s32(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_s64:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of signed 64 bit data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_s64(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_float:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of floating point data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_float(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_double:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of double precision floating point data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_double(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      sig[i] = g * aip[i * source_stride];
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	aop[i] = g * sig[i] + (1.0 - g) * aip[i * source_stride];
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process_complex:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10 of complex floating point data.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process_complex(AgsAmplifier10Util *amplifier10_util)
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
  gdouble sfreq[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sband[AGS_AMPLIFIER10_UTIL_AMP_COUNT];
  gdouble sgain[AGS_AMPLIFIER10_UTIL_AMP_COUNT];

  aip = amplifier10_util->source;
  aop = amplifier10_util->mix_buffer;
  
  destination = amplifier10_util->destination;
  
  source_stride = amplifier10_util->source_stride;
  destination_stride = amplifier10_util->destination_stride;

  len = 
    buffer_length = amplifier10_util->buffer_length;
  format = amplifier10_util->format;
  
  fgain = exp2ap(0.1661 * amplifier10_util->filter_gain);
  
  t = 0.0;
  
  for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
    switch(j){
    case 0:
      t = amplifier10_util->amp_0_frequency / amplifier10_util->samplerate;

      break;
    case 1:
      t = amplifier10_util->amp_1_frequency / amplifier10_util->samplerate;

      break;
    case 2:
      t = amplifier10_util->amp_2_frequency / amplifier10_util->samplerate;

      break;
    case 3:
      t = amplifier10_util->amp_3_frequency / amplifier10_util->samplerate;

      break;
    case 4:
      t = amplifier10_util->amp_4_frequency / amplifier10_util->samplerate;

      break;
    case 5:
      t = amplifier10_util->amp_5_frequency / amplifier10_util->samplerate;

      break;
    case 6:
      t = amplifier10_util->amp_6_frequency / amplifier10_util->samplerate;

      break;
    case 7:
      t = amplifier10_util->amp_7_frequency / amplifier10_util->samplerate;

      break;
    case 8:
      t = amplifier10_util->amp_8_frequency / amplifier10_util->samplerate;

      break;
    case 9:
      t = amplifier10_util->amp_9_frequency / amplifier10_util->samplerate;

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
      sband[j] = amplifier10_util->amp_0_bandwidth;

      break;
    case 1:
      sband[j] = amplifier10_util->amp_1_bandwidth;

      break;
    case 2:
      sband[j] = amplifier10_util->amp_2_bandwidth;

      break;
    case 3:
      sband[j] = amplifier10_util->amp_3_bandwidth;

      break;
    case 4:
      sband[j] = amplifier10_util->amp_4_bandwidth;

      break;
    case 5:
      sband[j] = amplifier10_util->amp_5_bandwidth;

      break;
    case 6:
      sband[j] = amplifier10_util->amp_6_bandwidth;

      break;
    case 7:
      sband[j] = amplifier10_util->amp_7_bandwidth;

      break;
    case 8:
      sband[j] = amplifier10_util->amp_8_bandwidth;

      break;
    case 9:
      sband[j] = amplifier10_util->amp_9_bandwidth;

      break;
    default:
      g_warning("unknown amp bandwidth");
    }

    switch(j){
    case 0:
      {
	if(amplifier10_util->amp_0_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_0_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 1:
      {
	if(amplifier10_util->amp_1_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_1_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 2:
      {
	if(amplifier10_util->amp_2_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_2_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 3:
      {
	if(amplifier10_util->amp_3_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_3_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 4:
      {
	if(amplifier10_util->amp_4_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_4_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 5:
      {
	if(amplifier10_util->amp_5_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_5_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 6:
      {
	if(amplifier10_util->amp_6_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_6_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 7:
      {
	if(amplifier10_util->amp_7_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_7_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 8:
      {
	if(amplifier10_util->amp_8_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_8_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    case 9:
      {
	if(amplifier10_util->amp_9_enabled > 0.0){
	  sgain[j] = exp2ap (0.1661 * amplifier10_util->amp_9_gain);
	}else{
	  sgain[j] = 1.0;
	}
      }
      break;
    default:
      g_warning("unknown amp gain");
    }
  }

  while(len){
    k = (len > 48) ? 32 : len;
        
    t = fgain;
    g = amplifier10_util->filter_gain;

    if(t > 1.25 * g){
      t = 1.25 * g;
    }else if (t < 0.80 * g){
      t = 0.80 * g;
    }

    amplifier10_util->filter_gain = t;
    d = (t - g) / k;

    for(i = 0; i < k; i++){
      g += d;
      ags_complex_set(sig + i,
		      g * ags_complex_get(aip + (i * source_stride)));
    }

    for(j = 0; j < AGS_AMPLIFIER10_UTIL_AMP_COUNT; j++){
      ags_fil_proc(amplifier10_util,
		   format,
		   j,
		   k, sfreq[j], sband[j], sgain[j]);
    }
                  
    j = amplifier10_util->fade;
    g = j / 16.0;
    p = 0;
    
    if(amplifier10_util->bypass == 0.0){
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
    
    amplifier10_util->fade = j;
    
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
	ags_complex_set(aop + (i),
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
					      amplifier10_util->mix_buffer, 1, 0,
					      buffer_length, ags_audio_buffer_util_get_copy_mode_from_format(NULL,
													     ags_audio_buffer_util_format_from_soundcard(NULL, format),
													     ags_audio_buffer_util_format_from_soundcard(NULL, format)));
}

/**
 * ags_amplifier10_util_process:
 * @amplifier10_util: the #AgsAmplifier10Util-struct
 * 
 * Process amplifier10.
 *
 * Since: 8.4.0
 */
void
ags_amplifier10_util_process(AgsAmplifier10Util *amplifier10_util)
{
  if(amplifier10_util == NULL ||
     amplifier10_util->destination == NULL ||
     amplifier10_util->source == NULL){
    return;
  }

  switch(amplifier10_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_amplifier10_util_process_s8(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_amplifier10_util_process_s16(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_amplifier10_util_process_s24(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_amplifier10_util_process_s32(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_amplifier10_util_process_s64(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_amplifier10_util_process_float(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_amplifier10_util_process_double(amplifier10_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_amplifier10_util_process_complex(amplifier10_util);
    }
    break;
  }
}
