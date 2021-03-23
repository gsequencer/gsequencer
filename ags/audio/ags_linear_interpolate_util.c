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

/**
 * SECTION:ags_linear_interpolate_util
 * @short_description: util functions to linear interpolat
 * @title: AgsLinearInterpolateUtil
 * @section_id:
 * @include: ags/audio/ags_linear_interpolate_util.h
 *
 * These utility functions allow you to fill linear interpolated
 * data.
 */

/**
 * ags_linear_interpolate_util_fill_s8:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s8(gint8 *buffer, guint channels,
				    guint buffer_length,
				    gdouble factor,
				    gint8 **output_buffer,
				    guint *output_buffer_length)
{
  gint8 *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint8 *) g_malloc(out_length * sizeof(gint8));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gint8 z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_s16:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s16(gint16 *buffer, guint channels,
				     guint buffer_length,
				     gdouble factor,
				     gint16 **output_buffer,
				     guint *output_buffer_length)
{
  gint16 *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint16 *) g_malloc(out_length * sizeof(gint16));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gint16 z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_s24:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s24(gint32 *buffer, guint channels,
				     guint buffer_length,
				     gdouble factor,
				     gint32 **output_buffer,
				     guint *output_buffer_length)
{
  gint32 *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint32 *) g_malloc(out_length * sizeof(gint32));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_s32:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s32(gint32 *buffer, guint channels,
				     guint buffer_length,
				     gdouble factor,
				     gint32 **output_buffer,
				     guint *output_buffer_length)
{
  gint32 *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint32 *) g_malloc(out_length * sizeof(gint32));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_s64:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_s64(gint64 *buffer, guint channels,
				     guint buffer_length,
				     gdouble factor,
				     gint64 **output_buffer,
				     guint *output_buffer_length)
{
  gint64 *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint64 *) g_malloc(out_length * sizeof(gint64));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gint64 z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_float:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_float(gfloat *buffer, guint channels,
				       guint buffer_length,
				       gdouble factor,
				       gfloat **output_buffer,
				       guint *output_buffer_length)
{
  gfloat *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gfloat *) g_malloc(out_length * sizeof(gfloat));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_double:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_double(gdouble *buffer, guint channels,
					guint buffer_length,
					gdouble factor,
					gdouble **output_buffer,
					guint *output_buffer_length)
{
  gdouble *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gdouble *) g_malloc(out_length * sizeof(gdouble));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    gdouble z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = buffer[(guint) floor(i / factor)];
    }else{
      z = buffer[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = buffer[(guint) floor(i / factor) + 1];
    }else{
      mix_z = buffer[buffer_length - 1];
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    out[i] = im_z;
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}

/**
 * ags_linear_interpolate_util_fill_complex:
 * @buffer: the source buffer
 * @channels: the channel count of source buffer
 * @buffer_length: the buffer length
 * @factor: the factor to interpolate
 * @output_buffer: (out): the return location of output buffer
 * @output_buffer_length: (out): the return location of output buffer length
 * 
 * Perform linear interpolate on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.0
 */
void
ags_linear_interpolate_util_fill_complex(AgsComplex *buffer, guint channels,
					 guint buffer_length,
					 gdouble factor,
					 AgsComplex **output_buffer,
					 guint *output_buffer_length)
{
  AgsComplex *out;
  guint out_length;
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0 ||
     factor == 0.0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }

    if(output_buffer_length != NULL){
      output_buffer_length[0] = 0;
    }
    
    return;
  } 
  
  out_length = (guint) floor(buffer_length * factor);
  
  out = (gint8 *) g_malloc(out_length * sizeof(AgsComplex));
  
  for(i = 0, j = 0; i < out_length; i++, j++){
    double _Complex z, mix_z, im_z;
    gdouble t;
    
    if((i * factor) + j > (i + 1) * factor){
      j = 0;
    }

    if((guint) floor(i / factor) < buffer_length){
      z = ags_complex_get(buffer + ((guint) floor(i / factor)));
    }else{
      z = ags_complex_get(buffer + (buffer_length - 1));
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = ags_complex_get(buffer + ((guint) floor(i / factor) + 1));
    }else{
      mix_z = ags_complex_get(buffer + (buffer_length - 1));
    }

    if(factor < 1.0){
      t = i / (i / factor);
    }else{
      t = j * (1.0 / (i / factor));
    }
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(out + i,
		    im_z);
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }

  if(output_buffer_length != NULL){
    output_buffer_length[0] = out_length;
  }
}
