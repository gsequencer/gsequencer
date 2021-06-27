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

gpointer ags_linear_interpolate_util_strct_copy(gpointer ptr);
void ags_linear_interpolate_util_strct_free(gpointer ptr);

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
				   (GBoxedCopyFunc) ags_linear_interpolate_util_strct_copy,
				   (GBoxedFreeFunc) ags_linear_interpolate_util_strct_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_linear_interpolate_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_linear_interpolate_util_strct_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsLinearInterpolateUtil));
 
  return(retval);
}

void
ags_linear_interpolate_util_strct_free(gpointer ptr)
{
  g_free(ptr);
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = source[(guint) floor(i / factor)];
    }else{
      z = source[buffer_length - 1];
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = source[(guint) floor(i / factor) + 1];
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

    if((guint) floor(i / factor) < buffer_length){
      z = ags_complex_get(source + ((guint) floor(i / factor)));
    }else{
      z = ags_complex_get(source + (buffer_length - 1));
    }
    
    if((guint) floor(i / factor) + 1 < buffer_length){
      mix_z = ags_complex_get(source + ((guint) floor(i / factor) + 1));
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
