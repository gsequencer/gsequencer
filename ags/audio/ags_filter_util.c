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

#include <ags/audio/ags_filter_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>

/**
 * ags_filter_util_pitch_s8:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_s8(gint8 *buffer,
			 guint buffer_length,
			 guint samplerate,
			 gdouble base_key,
			 gdouble tuning)
{
  gint8 **ptr_ptr_buffer;
  gint8 *ptr_buffer;
  gint8 *offset;
  gint8 mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(offset, 1,
						       n,
						       freq_period,
						       ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      if(new_freq > base_freq){
	tmp_offset = tmp_buffer + (guint) (k);

	n = ((gdouble) k / freq_period * new_freq_period);
      }else{
	tmp_offset = tmp_buffer + (guint) (k / freq_period * new_freq_period);

	n = ((gdouble) k);
      }
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(tmp_offset, 1,
						       n,
						       new_freq_period,
						       ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_s16:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_s16(gint16 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  gint16 **ptr_ptr_buffer;
  gint16 *ptr_buffer;
  gint16 *offset;
  gint16 mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
							n,
							freq_period,
							ptr_ptr_tmp_buffer);
    }
    
    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);
      
      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(tmp_offset, 1,
							n,
							new_freq_period,
							ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
      
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
      
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_s24:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_s24(gint32 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  gint32 **ptr_ptr_buffer;
  gint32 *ptr_buffer;
  gint32 *offset;
  gint32 mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(offset, 1,
							n,
							freq_period,
							ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(tmp_offset, 1,
							n,
							new_freq_period,
							ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_s32:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_s32(gint32 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  gint32 **ptr_ptr_buffer;
  gint32 *ptr_buffer;
  gint32 *offset;
  gint32 mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(offset, 1,
							n,
							freq_period,
							ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(tmp_offset, 1,
							n,
							new_freq_period,
							ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_s64:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_s64(gint64 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  gint64 **ptr_ptr_buffer;
  gint64 *ptr_buffer;
  gint64 *offset;
  gint64 mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(offset, 1,
							n,
							freq_period,
							ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(tmp_offset, 1,
							n,
							new_freq_period,
							ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_float:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_float(gfloat *buffer,
			    guint buffer_length,
			    guint samplerate,
			    gdouble base_key,
			    gdouble tuning)
{
  gfloat **ptr_ptr_buffer;
  gfloat *ptr_buffer;
  gfloat *offset;
  gfloat mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(offset, 1,
							  n,
							  freq_period,
							  ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(tmp_offset, 1,
							  n,
							  new_freq_period,
							  ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_double:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_double(gdouble *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble base_key,
			     gdouble tuning)
{
  gdouble **ptr_ptr_buffer;
  gdouble *ptr_buffer;
  gdouble *offset;
  gdouble mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  gdouble n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }
  
  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(offset, 1,
							   n,
							   freq_period,
							   ptr_ptr_tmp_buffer);
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
      
      AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(tmp_offset, 1,
							   n,
							   new_freq_period,
							   ptr_ptr_buffer);
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      mix_buffer[i] = sum;
    }    

    for(i = 1; i < buffer_length; i++){
      buffer[i] = mix_buffer[i];
    }
  }
}

/**
 * ags_filter_util_pitch_complex:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_pitch_complex(AgsComplex *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  AgsComplex **ptr_ptr_buffer;
  AgsComplex *ptr_buffer;
  AgsComplex *offset;
  AgsComplex mix_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period, new_freq_period;
  gdouble offset_factor;
  guint n;
  guint i, j, k;

  if(tuning == 0.0){
    return;
  }  

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / base_freq;
  new_freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  i = 0;
    
  while(i < buffer_length){
    for(j = 0, k = i; j * offset_factor < freq_period &&
	  k / offset_factor < buffer_length &&
	  j < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; j++, k++){
      ptr_tmp_buffer = tmp_buffer + j;

      offset = buffer + (guint) (k / offset_factor);
      n = (gdouble) (j * offset_factor);

      ags_complex_set(ptr_ptr_tmp_buffer[0][0],
		      ags_complex_get(offset[n]));
    }

    for(k = 0; k < new_freq_period &&
	  i < buffer_length &&
	  k * (new_freq_period / freq_period) < AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE; i++, k++){
      if(new_freq < base_freq &&
	 k > freq_period / offset_factor){
	break;
      }

      ptr_buffer = buffer + i;

      tmp_offset = tmp_buffer + (guint) (k);

      n = ((gdouble) k / freq_period * new_freq_period);
	
      ags_complex_set(ptr_ptr_buffer[0][0],
		      ags_complex_get(tmp_offset[n]));
    }
  }

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);
    
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(buffer + i,
		      sum);
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(mix_buffer + i,
		      sum);
    }    

    for(i = 1; i < buffer_length; i++){
      ags_complex_set(buffer + i,
		      ags_complex_get(mix_buffer + i));
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(buffer + i,
		      sum);
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(mix_buffer + i,
		      sum);
    }    

    for(i = 1; i < buffer_length; i++){
      ags_complex_set(buffer + i,
		      ags_complex_get(mix_buffer + i));
    }
  }
}
