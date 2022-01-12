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

#include <ags/audio/ags_linear_interpolate_util.h>

#include <math.h>

/**
 * SECTION:ags_linear_interpolate_util
 * @short_description: util functions to linear interpolate
 * @title: AgsLinearInterpolateUtil
 * @section_id:
 * @include: ags/audio/ags_linear_interpolate_util.h
 *
 * These utility functions allow you to fill linear interpolated
 * data.
 */

GType
ags_linear_interpolate_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_linear_interpolate_util = 0;

    ags_type_linear_interpolate_util =
      g_boxed_type_register_static("AgsLinearInterpolateUtil",
				   (GBoxedCopyFunc) ags_linear_interpolate_util_copy,
				   (GBoxedFreeFunc) ags_linear_interpolate_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_linear_interpolate_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_linear_interpolate_util_alloc:
 * 
 * Allocate #AgsLinearInterpolateUtil-struct.
 * 
 * Returns: the newly allocated #AgsLinearInterpolateUtil-struct
 * 
 * Since: 3.9.7
 */
AgsLinearInterpolateUtil*
ags_linear_interpolate_util_alloc()
{
  AgsLinearInterpolateUtil *ptr;
  
  ptr = (AgsLinearInterpolateUtil *) g_new(AgsLinearInterpolateUtil,
					   1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->factor = 0.0;

  return(ptr);
}

/**
 * ags_linear_interpolate_util_copy:
 * @ptr: the original #AgsLinearInterpolateUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsLinearInterpolateUtil-struct
 *
 * Since: 3.9.7
 */
gpointer
ags_linear_interpolate_util_copy(AgsLinearInterpolateUtil *ptr)
{
  AgsLinearInterpolateUtil *new_ptr;
  
  new_ptr = (AgsLinearInterpolateUtil *) g_new(AgsLinearInterpolateUtil,
					       1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->factor = ptr->factor;

  return(new_ptr);
}

/**
 * ags_linear_interpolate_util_free:
 * @ptr: the #AgsLinearInterpolateUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_free(AgsLinearInterpolateUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_linear_interpolate_util_get_destination:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get destination buffer of @linear_interpolate_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.7
 */
gpointer
ags_linear_interpolate_util_get_destination(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(NULL);
  }

  return(linear_interpolate_util->destination);
}

/**
 * ags_linear_interpolate_util_set_destination:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_destination(AgsLinearInterpolateUtil *linear_interpolate_util,
					    gpointer destination)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->destination = destination;
}

/**
 * ags_linear_interpolate_util_get_destination_stride:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get destination stride of @linear_interpolate_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_destination_stride(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->destination_stride);
}

/**
 * ags_linear_interpolate_util_set_destination_stride:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_destination_stride(AgsLinearInterpolateUtil *linear_interpolate_util,
						   guint destination_stride)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->destination_stride = destination_stride;
}

/**
 * ags_linear_interpolate_util_get_source:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get source buffer of @linear_interpolate_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.7
 */
gpointer
ags_linear_interpolate_util_get_source(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(NULL);
  }

  return(linear_interpolate_util->source);
}

/**
 * ags_linear_interpolate_util_set_source:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_source(AgsLinearInterpolateUtil *linear_interpolate_util,
				       gpointer source)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->source = source;
}

/**
 * ags_linear_interpolate_util_get_source_stride:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get source stride of @linear_interpolate_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_source_stride(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->source_stride);
}

/**
 * ags_linear_interpolate_util_set_source_stride:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_source_stride(AgsLinearInterpolateUtil *linear_interpolate_util,
					      guint source_stride)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->source_stride = source_stride;
}

/**
 * ags_linear_interpolate_util_get_buffer_length:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get buffer length of @linear_interpolate_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_buffer_length(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->buffer_length);
}

/**
 * ags_linear_interpolate_util_set_buffer_length:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_buffer_length(AgsLinearInterpolateUtil *linear_interpolate_util,
					      guint buffer_length)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->buffer_length = buffer_length;
}

/**
 * ags_linear_interpolate_util_get_format:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get format of @linear_interpolate_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_format(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->format);
}

/**
 * ags_linear_interpolate_util_set_format:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @format: the format
 *
 * Set @format of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_format(AgsLinearInterpolateUtil *linear_interpolate_util,
				       guint format)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->format = format;
}

/**
 * ags_linear_interpolate_util_get_samplerate:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get samplerate of @linear_interpolate_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_samplerate(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->samplerate);
}

/**
 * ags_linear_interpolate_util_set_samplerate:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_samplerate(AgsLinearInterpolateUtil *linear_interpolate_util,
					   guint samplerate)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->samplerate = samplerate;
}

/**
 * ags_linear_interpolate_util_get_audio_buffer_util_format:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get audio buffer util format of @linear_interpolate_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.9.7
 */
guint
ags_linear_interpolate_util_get_audio_buffer_util_format(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0);
  }

  return(linear_interpolate_util->audio_buffer_util_format);
}

/**
 * ags_linear_interpolate_util_set_audio_buffer_util_format:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @audio_buffer_util_format: the audio buffer util format
 *
 * Set @audio_buffer_util_format of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_audio_buffer_util_format(AgsLinearInterpolateUtil *linear_interpolate_util,
							 guint audio_buffer_util_format)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->audio_buffer_util_format = audio_buffer_util_format;
}

/**
 * ags_linear_interpolate_util_get_factor:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Get factor of @linear_interpolate_util.
 * 
 * Returns: the factor
 * 
 * Since: 3.9.7
 */
gdouble
ags_linear_interpolate_util_get_factor(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL){
    return(0.0);
  }

  return(linear_interpolate_util->factor);
}

/**
 * ags_linear_interpolate_util_set_factor:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * @factor: the factor
 *
 * Set @factor of @linear_interpolate_util.
 *
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_set_factor(AgsLinearInterpolateUtil *linear_interpolate_util,
				       gdouble factor)
{
  if(linear_interpolate_util == NULL){
    return;
  }

  linear_interpolate_util->factor = factor;
}

/**
 * ags_linear_interpolate_util_pitch_s8:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of signed 8 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_s8(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gint8 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_s16:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of signed 16 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_s16(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gint16 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_s24:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of signed 24 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_s24(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gint32 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_s32:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of signed 32 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_s32(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gint32 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_s64:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of signed 64 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_s64(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gint64 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_float:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of floating point data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_float(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gfloat *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_double:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of double precision floating point data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_double(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  gdouble *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = source[(guint) floor((double) i / factor) * source_stride];
      }else{
	z = source[(buffer_length - 1) * source_stride];
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = source[((guint) floor((double) i / factor) + 1) * source_stride];
      }else{
	mix_z = source[(buffer_length - 1) * source_stride];
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = source[i * source_stride];
    }
    
    destination[i * destination_stride] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_pitch_complex:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util of complex data.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch_complex(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  AgsComplex *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  gdouble factor;
  guint i, j;
  
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  destination = linear_interpolate_util->destination;
  destination_stride = linear_interpolate_util->destination_stride;

  source = linear_interpolate_util->source;
  source_stride = linear_interpolate_util->source_stride;

  buffer_length = linear_interpolate_util->buffer_length;

  factor = linear_interpolate_util->factor;

  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex z, mix_z, im_z;
    gdouble t;

    if(factor != 0.0){
      if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
	j = 0;
      }

      if((guint) floor((double) i / factor) < buffer_length){
	z = ags_complex_get(source + ((guint) floor((double) i / factor) * source_stride));
      }else{
	z = ags_complex_get(source + ((buffer_length - 1) * source_stride));
      }
    
      if((guint) floor((double) i / factor) + 1 < buffer_length){
	mix_z = ags_complex_get(source + (((guint) floor((double) i / factor) + 1) * source_stride));
      }else{
	mix_z = ags_complex_get(source + ((buffer_length - 1) * source_stride));
      }

      if(factor < 1.0){
	t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
      }else{
	t = (gdouble) (j + 1) / factor;
      }
    
      im_z = (1.0 - t) * z + (t * mix_z);
    }else{
      im_z = ags_complex_get(source + (i * source_stride));
    }
    
    ags_complex_set(destination + ( i * destination_stride),
		    im_z);
  }
}

/**
 * ags_linear_interpolate_util_pitch:
 * @linear_interpolate_util: the #AgsLinearInterpolateUtil-struct
 * 
 * Pitch @linear_interpolate_util.
 * 
 * Since: 3.9.7
 */
void
ags_linear_interpolate_util_pitch(AgsLinearInterpolateUtil *linear_interpolate_util)
{
  if(linear_interpolate_util == NULL ||
     linear_interpolate_util->destination == NULL ||
     linear_interpolate_util->source == NULL){
    return;
  }

  switch(linear_interpolate_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_linear_interpolate_util_pitch_s8(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_linear_interpolate_util_pitch_s16(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_linear_interpolate_util_pitch_s24(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_linear_interpolate_util_pitch_s32(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_linear_interpolate_util_pitch_s64(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_linear_interpolate_util_pitch_float(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_linear_interpolate_util_pitch_double(linear_interpolate_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_linear_interpolate_util_pitch_complex(linear_interpolate_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}

/**
 * ags_linear_interpolate_util_fill_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s8(gint8 *destination,
				    gint8 *source,
				    guint buffer_length,
				    gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s16(gint16 *destination,
				     gint16 *source,
				     guint buffer_length,
				     gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
    
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s24(gint32 *destination,
				     gint32 *source,
				     guint buffer_length,
				     gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s32(gint32 *destination,
				     gint32 *source,
				     guint buffer_length,
				     gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s64(gint64 *destination,
				     gint64 *source,
				     guint buffer_length,
				     gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_float(gfloat *destination,
				       gfloat *source,
				       guint buffer_length,
				       gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_double(gdouble *destination,
					gdouble *source,
					guint buffer_length,
					gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = source[(guint) floor((double) i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor((double) i / factor) + 1];
    }else{
      mix_z = source[buffer_length - 1];
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    destination[i] = im_z;
  }
}

/**
 * ags_linear_interpolate_util_fill_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_complex(AgsComplex *destination,
					 AgsComplex *source,
					 guint buffer_length,
					 gdouble factor)
{
  guint i, j;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    return;
  }
  
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex z, mix_z, im_z;
    gdouble t;
    
    if(((gdouble) i * factor) + (gdouble) j > ((gdouble) i + 1.0) * factor){
      j = 0;
    }

    if((guint) floor((double) i / factor) < buffer_length){
      z = ags_complex_get(source + ((guint) floor((double) i / factor)));
    }else{
      z = ags_complex_get(source + (buffer_length - 1));
    }
    
    if((guint) floor((double) i / factor) + 1 < buffer_length){
      mix_z = ags_complex_get(source + ((guint) floor((double) i / factor) + 1));
    }else{
      mix_z = ags_complex_get(source + (buffer_length - 1));
    }

    if(factor < 1.0){
      t = (gdouble) (i * factor) / ((gdouble) (i + 1) * factor);
    }else{
      t = (gdouble) (j + 1) / factor;
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(destination + i,
		    im_z);
  }
}
