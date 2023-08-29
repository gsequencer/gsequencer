/*
** Copyright (c) 2002-2021, Erik de Castro Lopo <erikd@mega-nerd.com>
** All rights reserved.
**
** This code is released under 2-clause BSD license. Please see the
** file at : https://github.com/libsndfile/libsamplerate/blob/master/COPYING
*/

#include <ags/audio/ags_samplerate_process.h>

#include <complex.h>

void
sinc_reset(AgsResampleUtil *resample_util)
{
  resample_util->b_current =
    resample_util->b_end = 0;
  resample_util->b_real_end = -1;

  resample_util->src_ratio =
    resample_util->input_index = 0.0;

} /* sinc_reset */

gint
prepare_data(AgsResampleUtil *resample_util, int half_filter_chan_len)
{
  gint len = 0 ;
  gint offset;
  guint copy_mode;
  
  if (resample_util->b_real_end >= 0)
    return 0;	/* Should be terminating. Just return. */

  if (resample_util->data_in == NULL)
    return 0;
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(resample_util->format),
						  ags_audio_buffer_util_format_from_soundcard(resample_util->format));

  if (resample_util->b_current == 0){
    /* Initial state. Set up zeros at the start of the buffer and
     * then load new data after that.
     */
    len = resample_util->b_len - 2 * half_filter_chan_len ;
    
    resample_util->b_current = resample_util->b_end = half_filter_chan_len ;
  }else if (resample_util->b_end + half_filter_chan_len < resample_util->b_len){
    /*  Load data at current end position. */
    len = MAX (resample_util->b_len - resample_util->b_current - half_filter_chan_len, 0) ;
  }else{
    /* Move data at end of buffer back to the start of the buffer. */
    len = resample_util->b_end - resample_util->b_current ;

    offset = resample_util->b_current - half_filter_chan_len;

    if(half_filter_chan_len + len < resample_util->buffer_length &&
       offset + half_filter_chan_len + len < resample_util->input_frames){
      ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, 0,
						  resample_util->data_in, 1, (resample_util->b_current - half_filter_chan_len),
						  half_filter_chan_len + len, copy_mode);
    }
    
    resample_util->b_current = half_filter_chan_len ;
    resample_util->b_end = resample_util->b_current + len ;

    /* Now load data at current end of buffer. */
    len = MAX (resample_util->b_len - resample_util->b_current - half_filter_chan_len, 0) ;
  } ;

  len = MIN ((int) (resample_util->in_count - resample_util->in_used), len) ;

  if (len < 0 || resample_util->b_end + len > resample_util->b_len)
    return -1;

  len = MIN (resample_util->buffer_length, len) ;

  offset = resample_util->b_end;

  if(resample_util->in_used > offset){
    offset = resample_util->in_used;
  }

  if(len > offset &&
     resample_util->b_end + len - offset < resample_util->buffer_length &&
     resample_util->in_used + len - offset < resample_util->input_frames){
    ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, resample_util->b_end,
						resample_util->data_in, 1, resample_util->in_used,
						len - offset, copy_mode);
  }
  
  resample_util->b_end += len ;
  resample_util->in_used += len ;
  resample_util->end_of_input = -1;

  if (resample_util->in_used == resample_util->in_count &&
      resample_util->b_end - resample_util->b_current < 2 * half_filter_chan_len && resample_util->end_of_input){
    /* Handle the case where all data in the current buffer has been
     * consumed and this is the last buffer.
     */
    
    if (resample_util->b_len - resample_util->b_end < half_filter_chan_len + 5){
      /* If necessary, move data down to the start of the buffer. */
      len = resample_util->b_end - resample_util->b_current ;
      offset = resample_util->b_current - half_filter_chan_len;

      if(half_filter_chan_len + len < resample_util->buffer_length &&
	 offset + half_filter_chan_len + len < resample_util->input_frames){
	ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, 0,
						    resample_util->data_in, 1, resample_util->b_current - half_filter_chan_len,
						    half_filter_chan_len + len, copy_mode);
      }
      
      resample_util->b_current = half_filter_chan_len ;
      resample_util->b_end = resample_util->b_current + len ;
    }
    
    resample_util->b_real_end = resample_util->b_end ;
    len = half_filter_chan_len + 5 ;
    
    if (len < 0 || resample_util->b_end + len > resample_util->b_len)
      len = resample_util->b_len - resample_util->b_end ;
        
    resample_util->b_end += len ;
  }

  return 0;
} /* prepare_data */

static inline double
calc_output_single (AgsResampleUtil *resample_util, increment_t increment, increment_t start_filter_index)
{
  double fraction, left, right, icoeff ;
  increment_t filter_index, max_filter_index ;
  gint data_index, coeff_count, indx ;
  
  /* Convert input parameters into fixed point. */
  max_filter_index = int_to_fp (resample_util->coeff_half_len) ;

  /* First apply the left half of the filter. */
  filter_index = start_filter_index ;
  coeff_count = (max_filter_index - filter_index) / increment ;
  filter_index = filter_index + coeff_count * increment ;

  data_index = resample_util->b_current - coeff_count;
  
  if (data_index < 0) /* Avoid underflow access to resample_util->buffer. */
    {
      gint steps = -1 * data_index;
      
      /* If the assert triggers we would have to take care not to underflow/overflow */
      assert (steps <= int_div_ceil (filter_index, increment)) ;
      filter_index -= increment * steps ;
      data_index += steps ;
    }
  left = 0.0;
  while (filter_index >= MAKE_INCREMENT_T (0))
    {
      fraction = fp_to_double (filter_index);
      indx = fp_to_int (filter_index) ;
      assert (indx >= 0 && indx + 1 < resample_util->coeff_half_len + 2) ;
      icoeff = (resample_util->coeffs [indx]) + fraction * ((resample_util->coeffs [indx + 1]) - (resample_util->coeffs [indx]));
      assert (data_index >= 0 && data_index < resample_util->b_len) ;
      assert (data_index < resample_util->b_end) ;
      
      if(data_index < resample_util->buffer_length){
	switch(resample_util->format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  {
	    left += icoeff * (((double) ((gint8 *) resample_util->buffer)[data_index] / (double) G_MAXINT8));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  {
	    left += icoeff * (((double) ((gint16 *) resample_util->buffer)[data_index] / (double) G_MAXINT16));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  {
	    left += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) (0x7fffff)));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  {
	    left += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) G_MAXINT32));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	  {
	    left += icoeff * (((double) ((gint64 *) resample_util->buffer)[data_index] / (double) G_MAXINT64));
	  }
	  break;
	case AGS_SOUNDCARD_FLOAT:
	  {
	    left += icoeff * ((double) ((gfloat *) resample_util->buffer)[data_index]);
	  }
	  break;
	case AGS_SOUNDCARD_DOUBLE:
	  {
	    left += icoeff * ((double) ((gdouble *) resample_util->buffer)[data_index]);
	  }
	  break;
	case AGS_SOUNDCARD_COMPLEX:
	  {
	    left += icoeff * ags_complex_get(((AgsComplex *) resample_util->buffer) + data_index);
	  }
	  break;
	}
      }
      
      filter_index -= increment ;
      data_index = data_index + 1 ;
    } ;

  /* Now apply the right half of the filter. */
  filter_index = increment - start_filter_index ;
  coeff_count = (max_filter_index - filter_index) / increment ;
  filter_index = filter_index + coeff_count * increment ;
  data_index = resample_util->b_current + 1 + coeff_count ;

  right = 0.0;
  do
    {
      fraction = fp_to_double (filter_index);
      indx = fp_to_int (filter_index) ;
      assert (indx < resample_util->coeff_half_len + 2) ;
      icoeff = resample_util->coeffs [indx] + fraction * (resample_util->coeffs [indx + 1] - resample_util->coeffs [indx]) ;
      assert (data_index >= 0 && data_index < resample_util->b_len) ;
      assert (data_index < resample_util->b_end) ;

      if(data_index < resample_util->buffer_length){
	switch(resample_util->format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  {
	    right += icoeff * (((double) ((gint8 *) resample_util->buffer)[data_index] / (double) G_MAXINT8));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  {
	    right += icoeff * (((double) ((gint16 *) resample_util->buffer)[data_index] / (double) G_MAXINT16));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  {
	    right += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) (0x7fffff)));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  {
	    right += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) G_MAXINT32));
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	  {
	    right += icoeff * (((double) ((gint64 *) resample_util->buffer)[data_index] / (double) G_MAXINT64));
	  }
	  break;
	case AGS_SOUNDCARD_FLOAT:
	  {
	    right += icoeff * ((double) ((gfloat *) resample_util->buffer)[data_index]);
	  }
	  break;
	case AGS_SOUNDCARD_DOUBLE:
	  {
	    right += icoeff * ((double) ((gdouble *) resample_util->buffer)[data_index]);
	  }
	  break;
	case AGS_SOUNDCARD_COMPLEX:
	  {
	    right += icoeff * ags_complex_get(((AgsComplex *) resample_util->buffer) + data_index);
	  }
	  break;
	}
      }
      
      filter_index -= increment ;
      data_index = data_index - 1 ;
    }
  while (filter_index > MAKE_INCREMENT_T (0)) ;

  return (left + right) ;
} /* calc_output_single */

gint
sinc_mono_vari_process (AgsResampleUtil *resample_util)
{
  double out_val;
  gdouble input_index, src_ratio, count, float_increment, terminate, rem ;
  increment_t increment, start_filter_index ;
  gint half_filter_chan_len, samples_in_hand;
  
  resample_util->in_count = resample_util->input_frames;
  resample_util->out_count = resample_util->output_frames;
  resample_util->in_used =
    resample_util->out_gen = 0 ;

  src_ratio = resample_util->last_ratio ;

  if (is_bad_src_ratio (src_ratio))
    return -1;

  /* Check the sample rate ratio wrt the buffer len. */
  count = (resample_util->coeff_half_len + 2.0) / resample_util->index_inc ;
  if (MIN (resample_util->last_ratio, resample_util->src_ratio) < 1.0)
    count /= MIN (resample_util->last_ratio, resample_util->src_ratio) ;

  /* Maximum coefficientson either side of center point. */
  half_filter_chan_len = (int) (psf_lrint (count) + 1) ;

  input_index = resample_util->last_position ;

  rem = fmod_one (input_index) ;
  resample_util->b_current = (resample_util->b_current + psf_lrint (input_index - rem)) % resample_util->b_len ;
  input_index = rem ;

  terminate = 1.0 / src_ratio + 1e-20 ;

  /* Main processing loop. */
  while (resample_util->out_gen < resample_util->out_count &&
	 resample_util->out_gen < resample_util->buffer_length)
    {
      gint error;

      samples_in_hand = (resample_util->b_end - resample_util->b_current + resample_util->b_len) % resample_util->b_len ;

      if(samples_in_hand <= half_filter_chan_len){
	if ((error = prepare_data (resample_util, half_filter_chan_len)) != 0){
	  return error ;
	}

	samples_in_hand = (resample_util->b_end - resample_util->b_current + resample_util->b_len) % resample_util->b_len ;

	if (samples_in_hand <= half_filter_chan_len){
	  break ;
	}
      }

      /* This is the termination condition. */
      if (resample_util->b_real_end >= 0){
	if (resample_util->b_current + input_index + terminate > resample_util->b_real_end){
	  break ;
	}
      }
      
      if (resample_util->out_count > 0 && fabs (resample_util->last_ratio - resample_util->src_ratio) > 1e-10){
	src_ratio = resample_util->last_ratio + resample_util->out_gen * (resample_util->src_ratio - resample_util->last_ratio) / resample_util->out_count ;
      }

      float_increment = resample_util->index_inc * (src_ratio < 1.0 ? src_ratio : 1.0) ;
      increment = double_to_fp (float_increment) ;

      start_filter_index = double_to_fp (input_index * float_increment) ;

      out_val = ((float_increment / resample_util->index_inc) * calc_output_single(resample_util, increment, start_filter_index));

      switch(resample_util->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  ((gint8 *) resample_util->data_out)[resample_util->out_gen] = (gint8) ((double) G_MAXINT8 * out_val);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  ((gint16 *) resample_util->data_out)[resample_util->out_gen] = (gint16) ((double) G_MAXINT16 * out_val);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  ((gint32 *) resample_util->data_out)[resample_util->out_gen] = (gint32) ((double) (0x7fffff) * out_val);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  ((gint32 *) resample_util->data_out)[resample_util->out_gen] = (gint32) ((double) G_MAXINT32 * out_val);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  ((gint64 *) resample_util->data_out)[resample_util->out_gen] = (gint64) ((double) G_MAXINT64 * out_val);
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  ((gfloat *) resample_util->data_out)[resample_util->out_gen] = (gfloat) (out_val);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  ((gdouble *) resample_util->data_out)[resample_util->out_gen] = (gdouble) (out_val);
	}
	break;
      case AGS_SOUNDCARD_COMPLEX:
	{
	  ags_complex_set(((AgsComplex *) resample_util->data_out) + resample_util->out_gen,
			  out_val + I * 0.0);
	}
	break;
      }
      
      resample_util->out_gen ++ ;

      /* Figure out the next index. */
      input_index += 1.0 / src_ratio ;
      rem = fmod_one (input_index) ;

      resample_util->b_current = (resample_util->b_current + psf_lrint (input_index - rem)) % resample_util->b_len ;
      input_index = rem ;
    } ;

  resample_util->last_position = input_index ;

  /* Save current ratio rather then target ratio. */
  resample_util->last_ratio = src_ratio ;

  resample_util->input_frames_used = resample_util->in_used;
  resample_util->output_frames_gen = resample_util->out_gen;

  return 0 ;
} /* sinc_mono_vari_process */

int
ags_samplerate_process_resample_util(AgsResampleUtil *resample_util)
{
  int error ;

  /* And that data_in and data_out are valid. */
  if ((resample_util->data_in == NULL && resample_util->input_frames > 0)
      || (resample_util->data_out == NULL && resample_util->output_frames > 0))
    return -1;

  /* Check src_ratio is in range. */
  if (is_bad_src_ratio (resample_util->src_ratio))
    return -1;

  if (resample_util->input_frames < 0)
    resample_util->input_frames = 0 ;
  if (resample_util->output_frames < 0)
    resample_util->output_frames = 0 ;
  
  /* Set the input and output counts to zero. */
  resample_util->input_frames_used = 0 ;
  resample_util->output_frames_gen = 0 ;

  /* Special case for when last_ratio has not been set. */
  if (resample_util->last_ratio < (1.0 / SRC_MAX_RATIO))
    resample_util->last_ratio = resample_util->src_ratio ;

  if(resample_util->bypass_cache){
    gint length;
    guint copy_mode;
    
    resample_util->last_ratio = resample_util->src_ratio ;

    copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(resample_util->format),
						    ags_audio_buffer_util_format_from_soundcard(resample_util->format));

    length = resample_util->output_frames;
    
    if(resample_util->input_frames > resample_util->output_frames){
      length = resample_util->input_frames;
    }

    switch(resample_util->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint8 *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint16 *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint32 *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint32 *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint64 *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	ags_audio_buffer_util_clear_buffer(((gfloat *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	ags_audio_buffer_util_clear_buffer(((gdouble *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_COMPLEX:
      {
	ags_audio_buffer_util_clear_buffer(((AgsComplex *) resample_util->buffer), 1,
					   length, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    }    
  }
  
  /* Now process. */
  sinc_mono_vari_process(resample_util);

  return 0;
} /* src_process */
