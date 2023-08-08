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
/*
** Copyright (c) 2002-2021, Erik de Castro Lopo <erikd@mega-nerd.com>
** All rights reserved.
**
** This code is released under 2-clause BSD license. Please see the
** file at : https://github.com/libsndfile/libsamplerate/blob/master/COPYING
*/

#include <ags/audio/ags_resample_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_samplerate_coeffs.h>

#include <complex.h>

#define MAKE_INCREMENT_T(x) 	((increment_t) (x))

#define	SHIFT_BITS				12
#define	FP_ONE					((gdouble) (((increment_t) 1) << SHIFT_BITS))
#define	INV_FP_ONE				(1.0 / FP_ONE)

#define	SRC_MAX_RATIO			256
#define	SRC_MAX_RATIO_STR		"256"

#define	SRC_MIN_RATIO_DIFF		(1e-20)

#ifndef MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#define	ARRAY_LEN(x)		((int) (sizeof (x) / sizeof ((x) [0])))
#define OFFSETOF(type,member)	((int) (&((type*) 0)->member))

typedef int32_t increment_t;

static inline int psf_lrint(double x)
{
  return lrint(x);
} /* psf_lrint */

/*----------------------------------------------------------
**	Common static inline functions.
*/

static inline double
fmod_one(double x)
{
  double res ;

  res = x - psf_lrint (x) ;

  if(res < 0.0){
    return res + 1.0 ;
  }
  
  return res ;
} /* fmod_one */

static inline int
is_bad_src_ratio (double ratio)
{	return (ratio < (1.0 / SRC_MAX_RATIO) || ratio > (1.0 * SRC_MAX_RATIO)) ;
} /* is_bad_src_ratio */

/* double_to_fp */
static inline increment_t
double_to_fp (double x)
{
  return (increment_t) (psf_lrint ((x) * FP_ONE)) ;
}

static inline increment_t
int_to_fp (int x)
{	return (((increment_t) (x)) << SHIFT_BITS) ;
} /* int_to_fp */

static inline int
fp_to_int (increment_t x)
{	return (((x) >> SHIFT_BITS)) ;
} /* fp_to_int */

static inline increment_t
fp_fraction_part (increment_t x)
{	return ((x) & ((((increment_t) 1) << SHIFT_BITS) - 1)) ;
} /* fp_fraction_part */

static inline double
fp_to_double (increment_t x)
{	return fp_fraction_part (x) * INV_FP_ONE ;
} /* fp_to_double */

static inline int
int_div_ceil (int divident, int divisor) /* == (int) ceil ((float) divident / divisor) */
{	assert (divident >= 0 && divisor > 0) ; /* For positive numbers only */
  return (divident + (divisor - 1)) / divisor ;
}

/**
 * SECTION:ags_resample_util
 * @short_description: Boxed type of resample util
 * @title: AgsResampleUtil
 * @section_id:
 * @include: ags/audio/ags_resample_util.h
 *
 * Boxed type of resample util data type.
 */

GType
ags_resample_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_resample_util = 0;

    ags_type_resample_util =
      g_boxed_type_register_static("AgsResampleUtil",
				   (GBoxedCopyFunc) ags_resample_util_copy,
				   (GBoxedFreeFunc) ags_resample_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_resample_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_resample_util_alloc:
 *
 * Allocate #AgsResampleUtil-struct
 *
 * Returns: a new #AgsResampleUtil-struct
 *
 * Since: 3.9.2
 */
AgsResampleUtil*
ags_resample_util_alloc()
{
  AgsResampleUtil *ptr;

  ptr = (AgsResampleUtil *) g_new(AgsResampleUtil,
				  1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_RESAMPLE_UTIL_DEFAULT_FORMAT;
  ptr->samplerate = AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE;

  ptr->target_samplerate = AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE;

  ptr->src_ratio = 1.0;

  ptr->input_frames = 0;
  ptr->data_in = NULL;

  ptr->output_frames = 0;
  ptr->data_out = NULL;
  
  ptr->increment = 2381;

  ptr->bypass_cache = FALSE;
  
  ptr->in_count = 0;
  ptr->in_used = 0;
  ptr->out_count = 0;
  ptr->out_gen = 0;
  
  ptr->coeff_half_len = ARRAY_LEN (ags_samplerate_coeffs) - 2;
  ptr->index_inc = 2381;
  
  ptr->input_index = 0;
  
  ptr->b_current = 0;
  ptr->b_end = 0;
  ptr->b_real_end = 0;
  ptr->b_len = 3 * (int) psf_lrint ((ptr->coeff_half_len + 2.0) / ptr->index_inc * SRC_MAX_RATIO + 1);
  ptr->b_len = MAX (ptr->b_len, 4096);
  ptr->b_len += 1;
  
  ptr->coeffs = ags_samplerate_coeffs;
  
  ptr->left_calc = 0.0;
  ptr->right_calc = 0.0;

  ptr->last_ratio = -1.0;
  ptr->last_position = 0.0;
  
  ptr->buffer = NULL;

  return(ptr);
}

/**
 * ags_resample_util_init:
 * @resample_util: the original #AgsResampleUtil-struct
 *
 * Init #AgsResampleUtil-struct
 *
 * Since: 5.5.1
 */
void
ags_resample_util_init(AgsResampleUtil *resample_util)
{
  resample_util->destination = NULL;
  resample_util->destination_stride = 1;

  resample_util->source = NULL;
  resample_util->source_stride = 1;

  resample_util->buffer_length = 0;
  resample_util->format = AGS_RESAMPLE_UTIL_DEFAULT_FORMAT;
  resample_util->samplerate = AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE;

  resample_util->target_samplerate = AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE;

  resample_util->src_ratio = 1.0;

  resample_util->input_frames = 0;
  resample_util->data_in = NULL;

  resample_util->output_frames = 0;
  resample_util->data_out = NULL;
  
  resample_util->increment = 2381;

  resample_util->bypass_cache = FALSE;
  
  resample_util->in_count = 0;
  resample_util->in_used = 0;
  resample_util->out_count = 0;
  resample_util->out_gen = 0;
  
  resample_util->coeff_half_len = ARRAY_LEN (ags_samplerate_coeffs) - 2;
  resample_util->index_inc = 2381;
  
  resample_util->input_index = 0;
  
  resample_util->b_current = 0;
  resample_util->b_end = 0;
  resample_util->b_real_end = 0;
  resample_util->b_len = 3 * (int) psf_lrint ((resample_util->coeff_half_len + 2.0) / resample_util->index_inc * SRC_MAX_RATIO + 1);
  resample_util->b_len = MAX (resample_util->b_len, 4096);
  resample_util->b_len += 1;
  
  resample_util->coeffs = ags_samplerate_coeffs;
  
  resample_util->left_calc = 0.0;
  resample_util->right_calc = 0.0;

  resample_util->last_ratio = -1.0;
  resample_util->last_position = 0.0;
  
  resample_util->buffer = NULL;
}

/**
 * ags_resample_util_copy:
 * @ptr: the original #AgsResampleUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsResampleUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_resample_util_copy(AgsResampleUtil *ptr)
{
  AgsResampleUtil *new_ptr;
  
  new_ptr = (AgsResampleUtil *) g_new(AgsResampleUtil,
				      1);

  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->target_samplerate = ptr->target_samplerate;

  new_ptr->src_ratio = ptr->src_ratio;

  new_ptr->input_frames = ptr->input_frames;
  new_ptr->data_in = NULL;

  if(new_ptr->input_frames > 0){
    new_ptr->data_in = ags_stream_alloc(ptr->input_frames,
					ptr->format);
  }

  new_ptr->output_frames = ptr->output_frames;
  new_ptr->data_out = NULL;

  if(new_ptr->output_frames > 0){
    new_ptr->data_out = ags_stream_alloc(ptr->output_frames,
					 ptr->format);
  }

  new_ptr->increment = ptr->increment;

  new_ptr->bypass_cache = ptr->bypass_cache;
  
  new_ptr->in_count = ptr->in_count;
  new_ptr->in_used = ptr->in_used;
  new_ptr->out_count = ptr->out_count;
  new_ptr->out_gen = ptr->out_gen;
  
  new_ptr->coeff_half_len = ptr->coeff_half_len;
  new_ptr->index_inc = ptr->index_inc;
  
  new_ptr->input_index = ptr->input_index;
  
  new_ptr->b_current = ptr->b_current;
  new_ptr->b_end = ptr->b_end;
  new_ptr->b_real_end = ptr->b_real_end;
  new_ptr->b_len = ptr->b_len;
  
  new_ptr->coeffs = ptr->coeffs;
  
  new_ptr->left_calc = ptr->left_calc;
  new_ptr->right_calc = ptr->right_calc;

  new_ptr->coeffs = ptr->coeffs;

  new_ptr->last_ratio = ptr->last_ratio;
  new_ptr->last_position = ptr->last_position;

  if(ptr->input_frames < ptr->output_frames){
    new_ptr->buffer = ags_stream_alloc(ptr->output_frames,
				       ptr->format);
  }else{
    new_ptr->buffer = ags_stream_alloc(ptr->input_frames,
				       ptr->format);
  }
  
  return(new_ptr);
}

/**
 * ags_resample_util_free:
 * @ptr: the #AgsResampleUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_free(AgsResampleUtil *ptr)
{
  ags_stream_free(ptr->data_in);
  ags_stream_free(ptr->data_out);
  
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  ags_stream_free(ptr->buffer);
  
  g_free(ptr);
}

/**
 * ags_resample_util_get_destination:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get destination buffer of @resample_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_resample_util_get_destination(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(NULL);
  }

  return(resample_util->destination);
}

/**
 * ags_resample_util_set_destination:
 * @resample_util: the #AgsResampleUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_destination(AgsResampleUtil *resample_util,
				  gpointer destination)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->destination = destination;
}

/**
 * ags_resample_util_get_destination_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get destination stride of @resample_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_destination_stride(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->destination_stride);
}

/**
 * ags_resample_util_set_destination_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_destination_stride(AgsResampleUtil *resample_util,
					 guint destination_stride)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->destination_stride = destination_stride;
}

/**
 * ags_resample_util_get_source:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get source buffer of @resample_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_resample_util_get_source(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(NULL);
  }

  return(resample_util->source);
}

/**
 * ags_resample_util_set_source:
 * @resample_util: the #AgsResampleUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_source(AgsResampleUtil *resample_util,
			     gpointer source)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->source = source;
}

/**
 * ags_resample_util_get_source_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get source stride of @resample_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_source_stride(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->source_stride);
}

/**
 * ags_resample_util_set_source_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_source_stride(AgsResampleUtil *resample_util,
				    guint source_stride)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->source_stride = source_stride;
}

/**
 * ags_resample_util_get_buffer_length:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get buffer length of @resample_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_buffer_length(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->buffer_length);
}

/**
 * ags_resample_util_set_buffer_length:
 * @resample_util: the #AgsResampleUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_buffer_length(AgsResampleUtil *resample_util,
				    guint buffer_length)
{
  if(resample_util == NULL ||
     resample_util->buffer_length == buffer_length){
    return;
  }

  resample_util->buffer_length = buffer_length;

  if(buffer_length > 0){
    resample_util->input_frames = buffer_length;
    resample_util->output_frames = ceil(resample_util->src_ratio * buffer_length);

    ags_stream_free(resample_util->data_in);
    ags_stream_free(resample_util->data_out);
    
    ags_stream_free(resample_util->buffer);

    resample_util->data_in = ags_stream_alloc(resample_util->input_frames,
					      resample_util->format);
    resample_util->data_out = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(resample_util->input_frames,
					       resample_util->format);
    }
  }else{
    resample_util->input_frames = 0;
    resample_util->output_frames = 0;
    
    ags_stream_free(resample_util->data_out);
    ags_stream_free(resample_util->data_in);

    ags_stream_free(resample_util->buffer);
    
    resample_util->data_out = NULL;
    resample_util->data_in = NULL;

    resample_util->buffer = NULL;
  }
}

/**
 * ags_resample_util_get_format:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get format of @resample_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
AgsSoundcardFormat
ags_resample_util_get_format(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->format);
}

/**
 * ags_resample_util_set_format:
 * @resample_util: the #AgsResampleUtil-struct
 * @format: the format
 *
 * Set @format of @resample_util.
 *
 * Since: 3.9.6
 */
void
ags_resample_util_set_format(AgsResampleUtil *resample_util,
			     AgsSoundcardFormat format)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->format = format;
}

/**
 * ags_resample_util_get_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get samplerate of @resample_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_samplerate(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE);
  }
  
  return(resample_util->samplerate);
}

/**
 * ags_resample_util_set_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_samplerate(AgsResampleUtil *resample_util,
				 guint samplerate)
{
  if(resample_util == NULL ||
     resample_util->samplerate == samplerate){
    return;
  }

  resample_util->samplerate = samplerate;

  resample_util->src_ratio = resample_util->target_samplerate / samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->output_frames = ceil(resample_util->src_ratio * resample_util->buffer_length);

    ags_stream_free(resample_util->data_out);
    
    ags_stream_free(resample_util->buffer);

    resample_util->data_out = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(resample_util->input_frames,
					       resample_util->format);
    }
  }
}

/**
 * ags_resample_util_get_target_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get target samplerate of @resample_util.
 * 
 * Returns: the  samplerate
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_target_samplerate(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE);
  }

  return(resample_util->target_samplerate);
}

/**
 * ags_resample_util_set_target_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * @target_samplerate: the target samplerate
 *
 * Set @target_samplerate of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_target_samplerate(AgsResampleUtil *resample_util,
					guint target_samplerate)
{
  if(resample_util == NULL ||
     resample_util->target_samplerate == target_samplerate){
    return;
  }

  resample_util->target_samplerate = target_samplerate;
  
  resample_util->src_ratio = target_samplerate / resample_util->samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->output_frames = ceil(resample_util->src_ratio * resample_util->buffer_length);

    ags_stream_free(resample_util->data_out);

    ags_stream_free(resample_util->buffer);
    
    resample_util->data_out = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(resample_util->output_frames,
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(resample_util->input_frames,
					       resample_util->format);
    }
  }
}

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
  guint copy_mode;
  
  if (resample_util->b_real_end >= 0)
    return 0 ;	/* Should be terminating. Just return. */

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

    ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, 0,
						resample_util->buffer, 1, (resample_util->b_current - half_filter_chan_len),
						half_filter_chan_len + len, copy_mode);

    resample_util->b_current = half_filter_chan_len ;
    resample_util->b_end = resample_util->b_current + len ;

    /* Now load data at current end of buffer. */
    len = MAX (resample_util->b_len - resample_util->b_current - half_filter_chan_len, 0) ;
  } ;

  len = MIN ((int) (resample_util->in_count - resample_util->in_used), len) ;

  if (len < 0 || resample_util->b_end + len > resample_util->b_len)
    return -1;

  ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, resample_util->b_end,
					      resample_util->data_in, 1, resample_util->in_used,
					      len, copy_mode);

  resample_util->b_end += len ;
  resample_util->in_used += len ;

  if (resample_util->in_used == resample_util->in_count &&
      resample_util->b_end - resample_util->b_current < 2 * half_filter_chan_len && resample_util->end_of_input){
    /* Handle the case where all data in the current buffer has been
     * consumed and this is the last buffer.
     */
    
    if (resample_util->b_len - resample_util->b_end < half_filter_chan_len + 5){
      /* If necessary, move data down to the start of the buffer. */
      len = resample_util->b_end - resample_util->b_current ;
      ags_audio_buffer_util_copy_buffer_to_buffer(resample_util->buffer, 1, 0,
						  resample_util->buffer, 1, resample_util->b_current - half_filter_chan_len,
						  half_filter_chan_len + len, copy_mode);  
      
      resample_util->b_current = half_filter_chan_len ;
      resample_util->b_end = resample_util->b_current + len ;
    }
    
    resample_util->b_real_end = resample_util->b_end ;
    len = half_filter_chan_len + 5 ;
    
    if (len < 0 || resample_util->b_end + len > resample_util->b_len)
      len = resample_util->b_len - resample_util->b_end ;
    
    switch(resample_util->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint8 *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint16 *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint32 *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint32 *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	ags_audio_buffer_util_clear_buffer(((gint64 *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	ags_audio_buffer_util_clear_buffer(((gfloat *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	ags_audio_buffer_util_clear_buffer(((gdouble *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    case AGS_SOUNDCARD_COMPLEX:
      {
	ags_audio_buffer_util_clear_buffer(((AgsComplex *) resample_util->buffer) + resample_util->b_end, 1,
					   len, ags_audio_buffer_util_format_from_soundcard(resample_util->format));
      }
      break;
    }

    resample_util->b_end += len ;
  }

  return 0;
} /* prepare_data */

static inline double _Complex
calc_output_single (AgsResampleUtil *resample_util, increment_t increment, increment_t start_filter_index)
{
  double _Complex fraction, left, right, icoeff ;
  increment_t filter_index, max_filter_index ;
  gint data_index, coeff_count, indx ;
  
  /* Convert input parameters into fixed point. */
  max_filter_index = int_to_fp (resample_util->coeff_half_len) ;

  /* First apply the left half of the filter. */
  filter_index = start_filter_index ;
  coeff_count = (max_filter_index - filter_index) / increment ;
  filter_index = filter_index + coeff_count * increment ;
  data_index = resample_util->b_current - coeff_count ;

  if (data_index < 0) /* Avoid underflow access to resample_util->buffer. */
    {	int steps = -data_index ;
      /* If the assert triggers we would have to take care not to underflow/overflow */
      assert (steps <= int_div_ceil (filter_index, increment)) ;
      filter_index -= increment * steps ;
      data_index += steps ;
    }
  left = 0.0 + I * 0.0 ;
  while (filter_index >= MAKE_INCREMENT_T (0))
    {
      fraction = fp_to_double (filter_index) + I * 0.0;
      indx = fp_to_int (filter_index) ;
      assert (indx >= 0 && indx + 1 < resample_util->coeff_half_len + 2) ;
      icoeff = resample_util->coeffs [indx] + fraction * (resample_util->coeffs [indx + 1] - resample_util->coeffs [indx]);
      assert (data_index >= 0 && data_index < resample_util->b_len) ;
      assert (data_index < resample_util->b_end) ;
      
      switch(resample_util->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  left += icoeff * (((double) ((gint8 *) resample_util->buffer)[data_index] / (double) G_MAXINT8) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  left += icoeff * (((double) ((gint16 *) resample_util->buffer)[data_index] / (double) G_MAXINT16) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  left += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) (0xffffff - 1)) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  left += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) G_MAXINT32) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  left += icoeff * (((double) ((gint64 *) resample_util->buffer)[data_index] / (double) G_MAXINT64) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  left += icoeff * ((double) ((gfloat *) resample_util->buffer)[data_index] + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  left += icoeff * ((double) ((gdouble *) resample_util->buffer)[data_index] + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_COMPLEX:
	{
	  left += icoeff * ags_complex_get(((AgsComplex *) resample_util->buffer) + data_index);
	}
	break;
      }

      filter_index -= increment ;
      data_index = data_index + 1 ;
    } ;

  /* Now apply the right half of the filter. */
  filter_index = increment - start_filter_index ;
  coeff_count = (max_filter_index - filter_index) / increment ;
  filter_index = filter_index + coeff_count * increment ;
  data_index = resample_util->b_current + 1 + coeff_count ;

  right = 0.0 + I * 0.0 ;
  do
    {
      fraction = fp_to_double (filter_index) + I * 0.0;
      indx = fp_to_int (filter_index) ;
      assert (indx < resample_util->coeff_half_len + 2) ;
      icoeff = resample_util->coeffs [indx] + fraction * (resample_util->coeffs [indx + 1] - resample_util->coeffs [indx]) ;
      assert (data_index >= 0 && data_index < resample_util->b_len) ;
      assert (data_index < resample_util->b_end) ;

      switch(resample_util->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  right += icoeff * (((double) ((gint8 *) resample_util->buffer)[data_index] / (double) G_MAXINT8) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  right += icoeff * (((double) ((gint16 *) resample_util->buffer)[data_index] / (double) G_MAXINT16) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  right += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) (0xffffff - 1)) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  right += icoeff * (((double) ((gint32 *) resample_util->buffer)[data_index] / (double) G_MAXINT32) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  right += icoeff * (((double) ((gint64 *) resample_util->buffer)[data_index] / (double) G_MAXINT64) + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  right += icoeff * ((double) ((gfloat *) resample_util->buffer)[data_index] + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  right += icoeff * ((double) ((gdouble *) resample_util->buffer)[data_index] + I * 0.0);
	}
	break;
      case AGS_SOUNDCARD_COMPLEX:
	{
	  right += icoeff * ags_complex_get(((AgsComplex *) resample_util->buffer) + data_index);
	}
	break;
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
  double _Complex out_val;
  gdouble input_index, src_ratio, count, float_increment, terminate, rem ;
  increment_t increment, start_filter_index ;
  gint half_filter_chan_len, samples_in_hand ;
  
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
  while (resample_util->out_gen < resample_util->out_count)
    {
      gint error;
      
      /* Need to reload buffer? */
      samples_in_hand = (resample_util->b_end - resample_util->b_current + resample_util->b_len) % resample_util->b_len ;

      if (samples_in_hand <= half_filter_chan_len)
	{	if ((error = prepare_data (resample_util, half_filter_chan_len)) != 0)
	    return error ;

	  samples_in_hand = (resample_util->b_end - resample_util->b_current + resample_util->b_len) % resample_util->b_len ;
	  if (samples_in_hand <= half_filter_chan_len)
	    break ;
	} ;

      /* This is the termination condition. */
      if (resample_util->b_real_end >= 0)
	{	if (resample_util->b_current + input_index + terminate > resample_util->b_real_end)
	    break ;
	} ;

      if (resample_util->out_count > 0 && fabs (resample_util->last_ratio - resample_util->src_ratio) > 1e-10)
	src_ratio = resample_util->last_ratio + resample_util->out_gen * (resample_util->src_ratio - resample_util->last_ratio) / resample_util->out_count ;

      float_increment = resample_util->index_inc * (src_ratio < 1.0 ? src_ratio : 1.0) ;
      increment = double_to_fp (float_increment) ;

      start_filter_index = double_to_fp (input_index * float_increment) ;

      out_val = ((float_increment / resample_util->index_inc) * calc_output_single(resample_util, increment, start_filter_index));

      switch(resample_util->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  ((gint8 *) resample_util->data_out)[resample_util->out_gen] = (gint8) ((double) G_MAXINT8 * creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  ((gint16 *) resample_util->data_out)[resample_util->out_gen] = (gint16) ((double) G_MAXINT16 * creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  ((gint32 *) resample_util->data_out)[resample_util->out_gen] = (gint32) ((double) (0xffffff - 1) * creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  ((gint32 *) resample_util->data_out)[resample_util->out_gen] = (gint32) ((double) G_MAXINT32 * creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  ((gint64 *) resample_util->data_out)[resample_util->out_gen] = (gint64) ((double) G_MAXINT64 * creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  ((gfloat *) resample_util->data_out)[resample_util->out_gen] = (gfloat) (creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  ((gdouble *) resample_util->data_out)[resample_util->out_gen] = (gdouble) (creal(out_val));
	}
	break;
      case AGS_SOUNDCARD_COMPLEX:
	{
	  ags_complex_set(((AgsComplex *) resample_util->data_out) + resample_util->out_gen,
			  out_val);
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
ags_src_process (AgsResampleUtil *resample_util)
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

  if (resample_util->data_in < resample_util->data_out)
    {	if (resample_util->data_in + resample_util->input_frames > resample_util->data_out)
	{	/*-printf ("\n\ndata_in: %p    data_out: %p\n",
		  (void*) (resample_util->data_in + resample_util->input_frames * psrc->channels), (void*) resample_util->data_out) ;-*/
	  return -1;
	} ;
    }
  else if (resample_util->data_out + resample_util->output_frames > resample_util->data_in)
    {	/*-printf ("\n\ndata_in : %p   ouput frames: %ld    data_out: %p\n", (void*) resample_util->data_in, resample_util->output_frames, (void*) resample_util->data_out) ;

printf ("data_out: %p (%p)    data_in: %p\n", (void*) resample_util->data_out,
(void*) (resample_util->data_out + resample_util->input_frames * psrc->channels), (void*) resample_util->data_in) ;-*/
      return -1;
    } ;

  /* Set the input and output counts to zero. */
  resample_util->input_frames_used = 0 ;
  resample_util->output_frames_gen = 0 ;

  /* Special case for when last_ratio has not been set. */
  if (resample_util->last_ratio < (1.0 / SRC_MAX_RATIO))
    resample_util->last_ratio = resample_util->src_ratio ;

  if(resample_util->bypass_cache){
    gint length;
    
    resample_util->last_ratio = resample_util->src_ratio ;

    if(resample_util->input_frames < resample_util->output_frames){
      length = resample_util->output_frames;
    }else{
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


/**
 * ags_resample_util_compute_s8:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s8(AgsResampleUtil *resample_util)
{
  gint8 *destination;
  gint8 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint8 *) resample_util->destination;
  source = (gint8 *) resample_util->source;
    
  ags_audio_buffer_util_clear_buffer(resample_util->data_in, 1,
				     resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S8);

  ags_audio_buffer_util_copy_s8_to_s8(resample_util->data_in, 1,
				      source, resample_util->source_stride,
				      resample_util->input_frames);
  
  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gint8));
  
  ags_audio_buffer_util_copy_s8_to_s8(destination, resample_util->destination_stride,
				      resample_util->data_out, 1,
				      resample_util->output_frames);
}

/**
 * ags_resample_util_compute_s16:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s16(AgsResampleUtil *resample_util)
{
  gint16 *destination;
  gint16 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint16 *) resample_util->destination;
  source = (gint16 *) resample_util->source;
    
  ags_audio_buffer_util_clear_buffer(resample_util->data_in, 1,
				     resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S16);

  ags_audio_buffer_util_copy_s16_to_s16(resample_util->data_in, 1,
					source, resample_util->source_stride,
					resample_util->input_frames);

  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gint16));
  
  ags_audio_buffer_util_copy_s16_to_s16(destination, resample_util->destination_stride,
					resample_util->data_out, 1,
					resample_util->output_frames);
}

/**
 * ags_resample_util_compute_s24:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s24(AgsResampleUtil *resample_util)
{
  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  ags_audio_buffer_util_clear_buffer(resample_util->data_in, 1,
				     resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S24);

  ags_audio_buffer_util_copy_s24_to_s24(resample_util->data_in, 1,
					source, resample_util->source_stride,
					resample_util->input_frames);
  
  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gint32));
  
  ags_audio_buffer_util_copy_s24_to_s24(destination, resample_util->destination_stride,
					resample_util->data_out, 1,
					resample_util->output_frames);
}

/**
 * ags_resample_util_compute_s32:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s32(AgsResampleUtil *resample_util)
{
  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  ags_audio_buffer_util_clear_buffer(resample_util->data_in, 1,
				     resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S32);

  ags_audio_buffer_util_copy_s32_to_s32(resample_util->data_in, 1,
					source, resample_util->source_stride,
					resample_util->input_frames);

  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gint32));
  
  ags_audio_buffer_util_copy_s32_to_s32(destination, resample_util->destination_stride,
					resample_util->data_out, 1,
					resample_util->output_frames);
}

/**
 * ags_resample_util_compute_s64:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s64(AgsResampleUtil *resample_util)
{
  gint64 *destination;
  gint64 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint64 *) resample_util->destination;
  source = (gint64 *) resample_util->source;
  
  ags_audio_buffer_util_clear_buffer(resample_util->data_in, 1,
				     resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S64);

  ags_audio_buffer_util_copy_s64_to_s64(resample_util->data_in, 1,
					source, resample_util->source_stride,
					resample_util->input_frames);

  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gint64));
  
  ags_audio_buffer_util_copy_s64_to_s64(destination, resample_util->destination_stride,
					resample_util->data_out, 1,
					resample_util->output_frames);
}

/**
 * ags_resample_util_compute_float:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_float(AgsResampleUtil *resample_util)
{
  gfloat *destination;
  gfloat *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gfloat *) resample_util->destination;
  source = (gfloat *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(resample_util->data_in, 1,
				    resample_util->input_frames);

  ags_audio_buffer_util_copy_float_to_float(resample_util->data_in, 1,
					    source, resample_util->source_stride,
					    resample_util->input_frames);

  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(gfloat));
  
  ags_audio_buffer_util_copy_float_to_float(destination, resample_util->destination_stride,
					    resample_util->data_out, 1,
					    resample_util->output_frames);
}

/**
 * ags_resample_util_compute_double:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_double(AgsResampleUtil *resample_util)
{
  gdouble *destination;
  gdouble *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }
  
  destination = (gdouble *) resample_util->destination;
  source = (gdouble *) resample_util->source;
  
  ags_audio_buffer_util_clear_double(resample_util->data_in, 1,
				     resample_util->input_frames);

  ags_audio_buffer_util_copy_double_to_double(resample_util->data_in, 1,
					      source, resample_util->source_stride,
					      resample_util->input_frames);

  //  g_message("in %d", ags_synth_util_get_xcross_count_float(resample_util->data_in,
  //							   resample_util->input_frames));

  ags_src_process(resample_util);

  //  g_message("out %d", ags_synth_util_get_xcross_count_float(resample_util->data_out,
  //							    resample_util->output_frames));
  
  memset(destination, 0, resample_util->output_frames * sizeof(gdouble));
  
  ags_audio_buffer_util_copy_double_to_double(destination, resample_util->destination_stride,
					      resample_util->data_out, 1,
					      resample_util->output_frames);
}

/**
 * ags_resample_util_compute_complex:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_complex(AgsResampleUtil *resample_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) resample_util->destination;
  source = (AgsComplex *) resample_util->source;
  
  ags_audio_buffer_util_clear_complex(resample_util->data_in, 1,
				      resample_util->input_frames);

  ags_audio_buffer_util_copy_complex_to_complex(resample_util->data_in, 1,
						source, resample_util->source_stride,
						resample_util->input_frames);

  ags_src_process(resample_util);

  memset(destination, 0, resample_util->output_frames * sizeof(AgsComplex));
  
  ags_audio_buffer_util_copy_complex_to_complex(destination, resample_util->destination_stride,
						resample_util->data_out, 1,
						resample_util->output_frames);
}

/**
 * ags_resample_util_compute:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  switch(resample_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_resample_util_compute_s8(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_resample_util_compute_s16(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_resample_util_compute_s24(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_resample_util_compute_s32(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_resample_util_compute_s64(resample_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_resample_util_compute_float(resample_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_resample_util_compute_double(resample_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_resample_util_compute_complex(resample_util);
  }
  break;
  }
}
