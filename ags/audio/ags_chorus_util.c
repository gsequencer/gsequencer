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

#include <ags/audio/ags_chorus_util.h>

/**
 * SECTION:ags_chorus_util
 * @short_description: chorus util
 * @title: AgsChorusUtil
 * @section_id:
 * @include: ags/audio/ags_chorus_util.h
 *
 * Utility functions to chorus.
 */

GType
ags_chorus_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_chorus_util = 0;

    ags_type_chorus_util =
      g_boxed_type_register_static("AgsChorusUtil",
				   (GBoxedCopyFunc) ags_chorus_util_copy,
				   (GBoxedFreeFunc) ags_chorus_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_chorus_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_chorus_util_alloc:
 * 
 * Allocate #AgsChorusUtil-struct.
 * 
 * Returns: the newly allocated #AgsChorusUtil-struct
 * 
 * Since: 3.13.4
 */
AgsChorusUtil*
ags_chorus_util_alloc()
{
  AgsChorusUtil *ptr;
  
  ptr = (AgsChorusUtil *) g_new(AgsChorusUtil,
				1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->pitch_mix_buffer = NULL;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->offset = 0;

  ptr->base_key = 0.0;

  ptr->input_volume = 1.0;
  ptr->output_volume = 1.0;

  ptr->lfo_oscillator = AGS_CHORUS_UTIL_DEFAULT_LFO_OSCILLATOR;
  ptr->lfo_frequency = AGS_CHORUS_UTIL_DEFAULT_LFO_FREQUENCY;

  ptr->depth = AGS_CHORUS_UTIL_DEFAULT_DEPTH;
  ptr->mix = AGS_CHORUS_UTIL_DEFAULT_MIX;
  ptr->delay = AGS_CHORUS_UTIL_DEFAULT_DELAY;

  ptr->hq_pitch_util = NULL;

  return(ptr);
}

/**
 * ags_chorus_util_copy:
 * @ptr: the original #AgsChorusUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsChorusUtil-struct
 *
 * Since: 3.13.4
 */
gpointer
ags_chorus_util_copy(AgsChorusUtil *ptr)
{
  AgsChorusUtil *new_ptr;
  
  new_ptr = (AgsChorusUtil *) g_new(AgsChorusUtil,
				    1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->pitch_mix_buffer = NULL;

  if(ptr->buffer_length > 0){
    new_ptr->pitch_mix_buffer = ags_stream_alloc(ptr->buffer_length,
						 ptr->format);    
  }
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->base_key = ptr->base_key;
  
  new_ptr->input_volume = ptr->input_volume;
  new_ptr->output_volume = ptr->output_volume;

  new_ptr->lfo_oscillator = ptr->lfo_oscillator;
  new_ptr->lfo_frequency = ptr->lfo_frequency;

  new_ptr->depth = ptr->depth;
  new_ptr->mix = ptr->mix;
  new_ptr->delay = ptr->delay;
  
  return(new_ptr);
}

/**
 * ags_chorus_util_free:
 * @ptr: the #AgsChorusUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_free(AgsChorusUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  g_free(ptr->pitch_mix_buffer);
  
  g_free(ptr);
}

/**
 * ags_chorus_util_get_destination:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get destination buffer of @chorus_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.13.4
 */
gpointer
ags_chorus_util_get_destination(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(NULL);
  }

  return(chorus_util->destination);
}

/**
 * ags_chorus_util_set_destination:
 * @chorus_util: the #AgsChorusUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_destination(AgsChorusUtil *chorus_util,
				gpointer destination)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->destination = destination;
}

/**
 * ags_chorus_util_get_destination_stride:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get destination stride of @chorus_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_destination_stride(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->destination_stride);
}

/**
 * ags_chorus_util_set_destination_stride:
 * @chorus_util: the #AgsChorusUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_destination_stride(AgsChorusUtil *chorus_util,
				       guint destination_stride)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->destination_stride = destination_stride;
}

/**
 * ags_chorus_util_get_source:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get source buffer of @chorus_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.13.4
 */
gpointer
ags_chorus_util_get_source(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(NULL);
  }

  return(chorus_util->source);
}

/**
 * ags_chorus_util_set_source:
 * @chorus_util: the #AgsChorusUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_source(AgsChorusUtil *chorus_util,
			   gpointer source)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->source = source;
}

/**
 * ags_chorus_util_get_source_stride:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get source stride of @chorus_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_source_stride(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->source_stride);
}

/**
 * ags_chorus_util_set_source_stride:
 * @chorus_util: the #AgsChorusUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_source_stride(AgsChorusUtil *chorus_util,
				  guint source_stride)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->source_stride = source_stride;
}

/**
 * ags_chorus_util_get_buffer_length:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get buffer length of @chorus_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_buffer_length(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->buffer_length);
}

/**
 * ags_chorus_util_set_buffer_length:
 * @chorus_util: the #AgsChorusUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_buffer_length(AgsChorusUtil *chorus_util,
				  guint buffer_length)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->buffer_length = buffer_length;

  ags_stream_free(chorus_util->pitch_mix_buffer);

  chorus_util->pitch_mix_buffer = NULL;

  if(buffer_length > 0){
    chorus_util->pitch_mix_buffer = ags_stream_alloc(buffer_length,
						     chorus_util->format);
  }
}

/**
 * ags_chorus_util_get_format:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get format of @chorus_util.
 * 
 * Returns: the format
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_format(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->format);
}

/**
 * ags_chorus_util_set_format:
 * @chorus_util: the #AgsChorusUtil-struct
 * @format: the format
 *
 * Set @format of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_format(AgsChorusUtil *chorus_util,
			   guint format)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->format = format;

  ags_stream_free(chorus_util->pitch_mix_buffer);

  chorus_util->pitch_mix_buffer = NULL;

  if(chorus_util->buffer_length > 0){
    chorus_util->pitch_mix_buffer = ags_stream_alloc(chorus_util->buffer_length,
						     format);
  }
}

/**
 * ags_chorus_util_get_samplerate:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get samplerate of @chorus_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_samplerate(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->samplerate);
}

/**
 * ags_chorus_util_set_samplerate:
 * @chorus_util: the #AgsChorusUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_samplerate(AgsChorusUtil *chorus_util,
			       guint samplerate)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->samplerate = samplerate;
}

/**
 * ags_chorus_util_get_base_key:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get base key of @chorus_util.
 * 
 * Returns: the base key
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_base_key(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0.0);
  }

  return(chorus_util->base_key);
}

/**
 * ags_chorus_util_set_base_key:
 * @chorus_util: the #AgsChorusUtil-struct
 * @base_key: the base key
 *
 * Set @base_key of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_base_key(AgsChorusUtil *chorus_util,
			     gdouble base_key)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->base_key = base_key;
}

/**
 * ags_chorus_util_get_input_volume:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get input volume of @chorus_util.
 * 
 * Returns: the input volume
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_input_volume(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(1.0);
  }

  return(chorus_util->input_volume);
}

/**
 * ags_chorus_util_set_input_volume:
 * @chorus_util: the #AgsChorusUtil-struct
 * @input_volume: the input volume
 *
 * Set @input_volume of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_input_volume(AgsChorusUtil *chorus_util,
				 gdouble input_volume)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->input_volume = input_volume;
}

/**
 * ags_chorus_util_get_output_volume:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get output volume of @chorus_util.
 * 
 * Returns: the output volume
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_output_volume(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(1.0);
  }

  return(chorus_util->output_volume);
}

/**
 * ags_chorus_util_set_output_volume:
 * @chorus_util: the #AgsChorusUtil-struct
 * @output_volume: the output volume
 *
 * Set @output_volume of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_output_volume(AgsChorusUtil *chorus_util,
				  gdouble output_volume)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->output_volume = output_volume;
}

/**
 * ags_chorus_util_get_lfo_oscillator:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get LFO oscillator of @chorus_util.
 * 
 * Returns: the LFO oscillator of #AgsSynthOscillatorMode-enum
 * 
 * Since: 3.13.4
 */
guint
ags_chorus_util_get_lfo_oscillator(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0);
  }

  return(chorus_util->lfo_oscillator);
}

/**
 * ags_chorus_util_set_lfo_oscillator:
 * @chorus_util: the #AgsChorusUtil-struct
 * @lfo_oscillator: the LFO oscillator of #AgsSynthOscillatorMode-enum
 *
 * Set @lfo_oscillator of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_lfo_oscillator(AgsChorusUtil *chorus_util,
				   guint lfo_oscillator)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->lfo_oscillator = lfo_oscillator;
}

/**
 * ags_chorus_util_get_lfo_frequency:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get LFO frequency of @chorus_util.
 * 
 * Returns: the LFO frequency
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_lfo_frequency(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(1.0);
  }

  return(chorus_util->lfo_frequency);
}

/**
 * ags_chorus_util_set_lfo_frequency:
 * @chorus_util: the #AgsChorusUtil-struct
 * @lfo_frequency: the LFO frequency
 *
 * Set @lfo_frequency of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_lfo_frequency(AgsChorusUtil *chorus_util,
				  gdouble lfo_frequency)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->lfo_frequency = lfo_frequency;
}

/**
 * ags_chorus_util_get_depth:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get depth of @chorus_util.
 * 
 * Returns: the depth
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_depth(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0.0);
  }

  return(chorus_util->depth);
}

/**
 * ags_chorus_util_set_depth:
 * @chorus_util: the #AgsChorusUtil-struct
 * @depth: the depth
 *
 * Set @depth of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_depth(AgsChorusUtil *chorus_util,
			  gdouble depth)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->depth = depth;
}

/**
 * ags_chorus_util_get_mix:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get mix of @chorus_util.
 * 
 * Returns: the mix
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_mix(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0.0);
  }

  return(chorus_util->mix);
}

/**
 * ags_chorus_util_set_mix:
 * @chorus_util: the #AgsChorusUtil-struct
 * @mix: the mix
 *
 * Set @mix of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_mix(AgsChorusUtil *chorus_util,
			gdouble mix)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->mix = mix;
}


/**
 * ags_chorus_util_get_delay:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Get delay of @chorus_util.
 * 
 * Returns: the delay
 * 
 * Since: 3.13.4
 */
gdouble
ags_chorus_util_get_delay(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL){
    return(0.0);
  }

  return(chorus_util->delay);
}

/**
 * ags_chorus_util_set_delay:
 * @chorus_util: the #AgsChorusUtil-struct
 * @delay: the delay
 *
 * Set @delay of @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_set_delay(AgsChorusUtil *chorus_util,
			  gdouble delay)
{
  if(chorus_util == NULL){
    return;
  }

  chorus_util->delay = delay;
}

/**
 * ags_chorus_util_compute_s8:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of signed 8 bit data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_s8(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gint8 *destination, *source;
  gint8 *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gint8 *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gint8 new_z;
    gint position;
    
    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      position = i + (guint) floor(delay * ((sin((gdouble) ((offset + i)) * 2.0 * M_PI * lfo_frequency / (gdouble) samplerate) >= 0.0 ? 1.0: -1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      position = i + (guint) floor(delay * ((sin((gdouble) ((offset + i)) * 2.0 * M_PI * lfo_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_s16:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of signed 16 bit data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_s16(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gint16 *destination, *source;
  gint16 *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gint16 *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gint16 new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_s24:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of signed 24 bit data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_s24(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gint32 *destination, *source;
  gint32 *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gint32 *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gint32 new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_s32:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of signed 32 bit data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_s32(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gint32 *destination, *source;
  gint32 *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gint32 *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gint32 new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_s64:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of signed 64 bit data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_s64(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gint64 *destination, *source;
  gint64 *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gint64 *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gint64 new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_float:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of floating point data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_float(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gfloat *destination, *source;
  gfloat *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gfloat *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_FLOAT);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gfloat new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_double:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of double precision floating point data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_double(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  gdouble *destination, *source;
  gdouble *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (gdouble *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_DOUBLE);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    gdouble new_z;
    gint position;

    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * source[i * source_stride]) + mix_b * (input_volume * pitch_mix_buffer[position]));
      }
    }
    break;
    }
    
    destination[i * destination_stride] = new_z;
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute_complex:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util of complex data.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute_complex(AgsChorusUtil *chorus_util)
{
  AgsHQPitchUtil *hq_pitch_util;

  AgsComplex *destination, *source;
  AgsComplex *pitch_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble mix;
  gdouble delay;
  guint pitch_mix_buffer_length;  
  gdouble base_freq, pitch_freq;
  gdouble freq_period, pitch_freq_period;
  gdouble tuning;
  gdouble mix_a, mix_b;
  gdouble ratio;
  guint64 offset;
  guint i;

  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  hq_pitch_util = chorus_util->hq_pitch_util;

  destination = chorus_util->destination;
  destination_stride = chorus_util->destination_stride;

  source = chorus_util->source;
  source_stride = chorus_util->source_stride;

  buffer_length = chorus_util->buffer_length;
  samplerate = chorus_util->samplerate;

  offset = chorus_util->offset;
  
  input_volume = chorus_util->input_volume;
  output_volume = chorus_util->output_volume;
  
  lfo_oscillator = chorus_util->lfo_oscillator;
  lfo_frequency = chorus_util->lfo_frequency;

  ratio = lfo_frequency / samplerate;

  mix = chorus_util->mix;
  delay = chorus_util->delay;

  /* frequency */
  tuning = chorus_util->depth * 0.25;
  
  base_freq = exp2((chorus_util->base_key) / 12.0) * 440.0;

  pitch_freq = exp2((chorus_util->base_key - tuning) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(pitch_freq <= 0.0){
    g_warning("rejecting pitch pitch freq %f <= 0.0", pitch_freq);
    
    return;
  }
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  pitch_freq_period = samplerate / pitch_freq;

  pitch_mix_buffer_length = (freq_period / pitch_freq_period) * buffer_length;

  pitch_mix_buffer = (AgsComplex *) chorus_util->pitch_mix_buffer;

  ags_hq_pitch_util_set_destination(hq_pitch_util,
				    pitch_mix_buffer);
  
  ags_hq_pitch_util_set_source(hq_pitch_util,
			       source);

  ags_hq_pitch_util_set_buffer_length(hq_pitch_util,
				      pitch_mix_buffer_length);

  ags_hq_pitch_util_set_format(hq_pitch_util,
			       AGS_SOUNDCARD_COMPLEX);

  ags_hq_pitch_util_set_base_key(hq_pitch_util,
				 chorus_util->base_key);

  ags_hq_pitch_util_set_tuning(hq_pitch_util,
			       tuning);

  ags_hq_pitch_util_pitch(hq_pitch_util);

  /* mix pitch buffer */
  mix_a = mix;
  mix_b = mix - 0.5;

  if(mix_a > 0.5){
    mix_a = 0.5 - (-1.0 * (0.5 - mix_a));
  }

  if(mix_b < 0.0){
    mix_b = 0.5 - (-1.0 * mix_b);
  }

  mix_a *= 2.0;
  mix_b *= 2.0;
  
  for(i = 0; i < buffer_length; i++){
    double _Complex new_z;
    gint position;
    
    new_z = 0;
    
    switch(lfo_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      position = i + (guint) floor(delay * (sin((offset + i) * 2.0 * M_PI * lfo_frequency / samplerate)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * ags_complex_get(source + (i * source_stride)))) + mix_b * (input_volume * ags_complex_get(pitch_mix_buffer + position));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      position = i + (guint) floor(delay * (((fmod(((gdouble) (offset + i)), ratio) * 2.0 * ratio) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * ags_complex_get(source + (i * source_stride))) + mix_b * (input_volume * ags_complex_get(pitch_mix_buffer + position)));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      position = i + (guint) floor(delay * (((((offset + i)) * ratio * 2.0) - (((double) ((((offset + i)) * ratio)) / 2.0) * 2.0) - 1.0)));
      
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * ags_complex_get(source + (i * source_stride))) + mix_b * (input_volume * ags_complex_get(pitch_mix_buffer + position)));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * ags_complex_get(source + (i * source_stride))) + mix_b * (input_volume * ags_complex_get(pitch_mix_buffer + position)));
      }
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      if(position >= 0 && position < pitch_mix_buffer_length){
	new_z = output_volume * (mix_a * (input_volume * ags_complex_get(source + (i * source_stride))) + mix_b * (input_volume * ags_complex_get(pitch_mix_buffer + position)));
      }
    }
    break;
    }
    
    ags_complex_set(destination + (i * destination_stride),
		    new_z);
  }

  chorus_util->offset += buffer_length;
}

/**
 * ags_chorus_util_compute:
 * @chorus_util: the #AgsChorusUtil-struct
 * 
 * Choralize @chorus_util.
 *
 * Since: 3.13.4
 */
void
ags_chorus_util_compute(AgsChorusUtil *chorus_util)
{
  if(chorus_util == NULL ||
     chorus_util->destination == NULL ||
     chorus_util->source == NULL){
    return;
  }

  switch(chorus_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_chorus_util_compute_s8(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_chorus_util_compute_s16(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_chorus_util_compute_s24(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_chorus_util_compute_s32(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_chorus_util_compute_s64(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_chorus_util_compute_float(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_chorus_util_compute_double(chorus_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_chorus_util_compute_complex(chorus_util);
  }
  break;
  }
}
