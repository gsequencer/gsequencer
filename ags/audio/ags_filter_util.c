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
 * ags_filter_util_highpass_s8:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_s8(gint8 *buffer,
			    guint buffer_length,
			    guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_s16:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_s16(gint16 *buffer,
			     guint buffer_length,
			     guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_s24:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_s24(gint32 *buffer,
			     guint buffer_length,
			     guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_s32:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_s32(gint32 *buffer,
			     guint buffer_length,
			     guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_s64:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_s64(gint64 *buffer,
			     guint buffer_length,
			     guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_float:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_float(gfloat *buffer,
			       guint buffer_length,
			       guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_double:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_double(gdouble *buffer,
				guint buffer_length,
				guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_highpass_complex:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply high-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_highpass_complex(AgsComplex *buffer,
				 guint buffer_length,
				 guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_s8:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_s8(gint8 *buffer,
			   guint buffer_length,
			   guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_s16:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_s16(gint16 *buffer,
			    guint buffer_length,
			    guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_s24:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_s24(gint32 *buffer,
			    guint buffer_length,
			    guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_s32:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_s32(gint32 *buffer,
			    guint buffer_length,
			    guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_s64:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_s64(gint64 *buffer,
			    guint buffer_length,
			    guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_float:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_float(gfloat *buffer,
			      guint buffer_length,
			      guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_double:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_double(gdouble *buffer,
			       guint buffer_length,
			       guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_lowpass_complex:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * 
 * Apply low-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_lowpass_complex(AgsComplex *buffer,
				guint buffer_length,
				guint samplerate)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_s8(gint8 *buffer,
			    guint buffer_length,
			    guint samplerate,
			    gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_s16(gint16 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_s24(gint32 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_s32(gint32 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_s64(gint64 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_float(gfloat *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_double(gdouble *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @lower: lower frequency
 * @upper: upper frequency
 * 
 * Apply band-pass filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_bandpass_complex(AgsComplex *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble lower, gdouble upper)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_s8(gint8 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble frequency, gdouble bandwidth,
			     gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_s16(gint16 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble frequency, gdouble bandwidth,
			      gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_s24(gint32 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble frequency, gdouble bandwidth,
			      gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_s32(gint32 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble frequency, gdouble bandwidth,
			      gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_s64(gint64 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble frequency, gdouble bandwidth,
			      gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_float(gfloat *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble frequency, gdouble bandwidth,
				gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_double(gdouble *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency, gdouble bandwidth,
				 gdouble gain)
{
  //TODO:JK: implement me
}

/**
 * ags_filter_util_:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @bandwidth: the bandwidth
 * @gain: the gain
 * 
 * Apply equalizer filter.
 * 
 * Since: 2.3.0
 */
void
ags_filter_util_equalizer_complex(AgsComplex *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble frequency, gdouble bandwidth,
				  gdouble gain)
{
  //TODO:JK: implement me
}

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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(offset, 1,
							 n,
							 1,
							 ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(tmp_offset, 1,
							 n,
							 1,
							 ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(offset, 1,
							 n,
							 1,
							 ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(tmp_offset, 1,
							 n,
							 1,
							 ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(offset, 1,
							  n,
							  1,
							  ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(tmp_offset, 1,
							  n,
							  1,
							  ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(offset, 1,
							    n,
							    1,
							    ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(tmp_offset, 1,
							    n,
							    1,
							    ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(offset, 1,
							    n,
							    1,
							    ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(tmp_offset, 1,
							    n,
							    1,
							    ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);
	
	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(offset, 1,
							     n,
							     1,
							     ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(tmp_offset, 1,
							     n,
							     1,
							     ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(offset, 1,
							     n,
							     1,
							     ptr_ptr_tmp_buffer);
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(tmp_offset, 1,
							     n,
							     1,
							     ptr_ptr_buffer);

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
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
  AgsComplex **ptr_ptr_tmp_buffer;
  AgsComplex *ptr_tmp_buffer;
  AgsComplex *tmp_offset;
  AgsComplex tmp_buffer[AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE];

  gdouble base_freq, new_freq;
  gdouble freq_period;
  gdouble offset_factor;
  gdouble offset_delay, prev_offset_delay;
  gdouble tmp_offset_delay;
  guint n;
  guint i, j, k;

  ptr_ptr_buffer = &ptr_buffer;
  ptr_ptr_tmp_buffer = &ptr_tmp_buffer;

  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);
  
  freq_period = samplerate / new_freq;

  if(freq_period > AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE){
    g_warning("freq bigger than temporary buffer's size");
    
    freq_period = AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE;    
  }
  
  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  if(base_freq < new_freq){
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + (offset_factor * i) < offset_factor * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + (offset_factor * k);

	ags_complex_set(ptr_ptr_tmp_buffer[0][0],
			ags_complex_get(offset[n]));
	
	/* iterate */
	offset_delay += (1.0 / offset_factor);

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;
      
      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;
	
	ags_complex_set(ptr_ptr_buffer[0][0],
			ags_complex_get(tmp_offset[n]));

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }else{
    i = 0;
    offset_delay = 0.0;
    prev_offset_delay = 0.0;
    
    while(i < buffer_length){
      offset_delay = prev_offset_delay;

      for(j = 0, k = i; j < freq_period && j + ((1.0 / offset_factor) * i) < (1.0 / offset_factor) * buffer_length; j++){
	ptr_tmp_buffer = tmp_buffer + j;

	offset = buffer + k;
	n = j + ((1.0 / offset_factor) * k);

	ags_complex_set(ptr_ptr_tmp_buffer[0][0],
			ags_complex_get(offset[n]));
	
	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }

      tmp_offset_delay = prev_offset_delay;

      prev_offset_delay = offset_delay;

      offset_delay = tmp_offset_delay;

      for(k = 0; i < freq_period && i < buffer_length; i++){
	ptr_buffer = buffer + i;

	tmp_offset = tmp_buffer + k;
	n = i;

	ags_complex_set(ptr_ptr_buffer[0][0],
			ags_complex_get(tmp_offset[n]));

	/* iterate */
	offset_delay += offset_factor;

	if(offset_delay >= 1.0){
	  k += floor(offset_delay);
	
	  offset_delay -= floor(offset_delay);
	}
      }
    }
  }
}
