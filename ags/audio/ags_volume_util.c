/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_volume_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>

#define LARGE_VECTOR 1
#endif

/**
 * SECTION:ags_volume_util
 * @short_description: Boxed type of volume util
 * @title: AgsVolumeUtil
 * @section_id:
 * @include: ags/audio/ags_volume_util.h
 *
 * Boxed type of volume util data type.
 */

GType
ags_volume_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_volume_util = 0;

    ags_type_volume_util =
      g_boxed_type_register_static("AgsVolumeUtil",
				   (GBoxedCopyFunc) ags_volume_util_copy,
				   (GBoxedFreeFunc) ags_volume_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_volume_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_volume_util_alloc:
 *
 * Allocate #AgsVolumeUtil-struct
 *
 * Returns: a new #AgsVolumeUtil-struct
 *
 * Since: 3.9.2
 */
AgsVolumeUtil*
ags_volume_util_alloc()
{
  AgsVolumeUtil *ptr;

  ptr = (AgsVolumeUtil *) g_new(AgsVolumeUtil,
				1);

  ptr[0] = AGS_VOLUME_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_volume_util_copy:
 * @ptr: the original #AgsVolumeUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsVolumeUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_volume_util_copy(AgsVolumeUtil *ptr)
{
  AgsVolumeUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsVolumeUtil *) g_new(AgsVolumeUtil,
				    1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  new_ptr->volume = ptr->volume;

  return(new_ptr);
}

/**
 * ags_volume_util_free:
 * @ptr: the #AgsVolumeUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_free(AgsVolumeUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_volume_util_get_destination:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get destination buffer of @volume_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_volume_util_get_destination(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(NULL);
  }

  return(volume_util->destination);
}

/**
 * ags_volume_util_set_destination:
 * @volume_util: the #AgsVolumeUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_destination(AgsVolumeUtil *volume_util,
				gpointer destination)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->destination = destination;
}

/**
 * ags_volume_util_get_destination_stride:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get destination stride of @volume_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_volume_util_get_destination_stride(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(0);
  }

  return(volume_util->destination_stride);
}

/**
 * ags_volume_util_set_destination_stride:
 * @volume_util: the #AgsVolumeUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_destination_stride(AgsVolumeUtil *volume_util,
				       guint destination_stride)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->destination_stride = destination_stride;
}

/**
 * ags_volume_util_get_source:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get source buffer of @volume_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_volume_util_get_source(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(NULL);
  }

  return(volume_util->source);
}

/**
 * ags_volume_util_set_source:
 * @volume_util: the #AgsVolumeUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_source(AgsVolumeUtil *volume_util,
			   gpointer source)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->source = source;
}

/**
 * ags_volume_util_get_source_stride:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get source stride of @volume_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_volume_util_get_source_stride(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(0);
  }

  return(volume_util->source_stride);
}

/**
 * ags_volume_util_set_source_stride:
 * @volume_util: the #AgsVolumeUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_source_stride(AgsVolumeUtil *volume_util,
				  guint source_stride)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->source_stride = source_stride;
}

/**
 * ags_volume_util_get_buffer_length:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get buffer length of @volume_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_volume_util_get_buffer_length(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(0);
  }

  return(volume_util->buffer_length);
}

/**
 * ags_volume_util_set_buffer_length:
 * @volume_util: the #AgsVolumeUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_buffer_length(AgsVolumeUtil *volume_util,
				  guint buffer_length)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->buffer_length = buffer_length;
}

/**
 * ags_volume_util_get_format:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get format of @volume_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
AgsSoundcardFormat
ags_volume_util_get_format(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(0);
  }

  return(volume_util->format);
}

/**
 * ags_volume_util_set_format:
 * @volume_util: the #AgsVolumeUtil-struct
 * @format: the format
 *
 * Set @format of @volume_util.
 *
 * Since: 3.9.6
 */
void
ags_volume_util_set_format(AgsVolumeUtil *volume_util,
			   AgsSoundcardFormat format)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->format = format;
}

/**
 * ags_volume_util_get_volume:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Get volume of @volume_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.2
 */
gdouble
ags_volume_util_get_volume(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL){
    return(1.0);
  }

  return(volume_util->volume);
}

/**
 * ags_volume_util_set_volume:
 * @volume_util: the #AgsVolumeUtil-struct
 * @volume: the volume
 *
 * Set @volume of @volume_util.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_set_volume(AgsVolumeUtil *volume_util,
			   gdouble volume)
{
  if(volume_util == NULL){
    return;
  }

  volume_util->volume = volume;
}

/**
 * ags_volume_util_compute_s8:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_s8(AgsVolumeUtil *volume_util)
{
  gint8 *destination;
  gint8 *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint8 *) volume_util->destination;
  source = (gint8 *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint8) v_buffer[0];
    *(destination += destination_stride) = (gint8) v_buffer[1];
    *(destination += destination_stride) = (gint8) v_buffer[2];
    *(destination += destination_stride) = (gint8) v_buffer[3];
    *(destination += destination_stride) = (gint8) v_buffer[4];
    *(destination += destination_stride) = (gint8) v_buffer[5];
    *(destination += destination_stride) = (gint8) v_buffer[6];
    *(destination += destination_stride) = (gint8) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gint8) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint8) ret_v_buffer[0];
    *(destination += destination_stride) = (gint8) ret_v_buffer[1];
    *(destination += destination_stride) = (gint8) ret_v_buffer[2];
    *(destination += destination_stride) = (gint8) ret_v_buffer[3];
    *(destination += destination_stride) = (gint8) ret_v_buffer[4];
    *(destination += destination_stride) = (gint8) ret_v_buffer[5];
    *(destination += destination_stride) = (gint8) ret_v_buffer[6];
    *(destination += destination_stride) = (gint8) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint8) ((gint16) (source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_s16:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_s16(AgsVolumeUtil *volume_util)
{
  gint16 *destination;
  gint16 *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint16 *) volume_util->destination;
  source = (gint16 *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint16) v_buffer[0];
    *(destination += destination_stride) = (gint16) v_buffer[1];
    *(destination += destination_stride) = (gint16) v_buffer[2];
    *(destination += destination_stride) = (gint16) v_buffer[3];
    *(destination += destination_stride) = (gint16) v_buffer[4];
    *(destination += destination_stride) = (gint16) v_buffer[5];
    *(destination += destination_stride) = (gint16) v_buffer[6];
    *(destination += destination_stride) = (gint16) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gint16) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint16) ret_v_buffer[0];
    *(destination += destination_stride) = (gint16) ret_v_buffer[1];
    *(destination += destination_stride) = (gint16) ret_v_buffer[2];
    *(destination += destination_stride) = (gint16) ret_v_buffer[3];
    *(destination += destination_stride) = (gint16) ret_v_buffer[4];
    *(destination += destination_stride) = (gint16) ret_v_buffer[5];
    *(destination += destination_stride) = (gint16) ret_v_buffer[6];
    *(destination += destination_stride) = (gint16) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint16) ((gint32) (source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_s24:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_s24(AgsVolumeUtil *volume_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint32 *) volume_util->destination;
  source = (gint32 *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gint32) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += destination_stride) = (gint32) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint32) ((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint32) ((gint32) (source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_s32:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_s32(AgsVolumeUtil *volume_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint32 *) volume_util->destination;
  source = (gint32 *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gint32) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += destination_stride) = (gint32) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint32) ((gint64) (source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_s64:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_s64(AgsVolumeUtil *volume_util)
{
  gint64 *destination;
  gint64 *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint64 *) volume_util->destination;
  source = (gint64 *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint64) v_buffer[0];
    *(destination += destination_stride) = (gint64) v_buffer[1];
    *(destination += destination_stride) = (gint64) v_buffer[2];
    *(destination += destination_stride) = (gint64) v_buffer[3];
    *(destination += destination_stride) = (gint64) v_buffer[4];
    *(destination += destination_stride) = (gint64) v_buffer[5];
    *(destination += destination_stride) = (gint64) v_buffer[6];
    *(destination += destination_stride) = (gint64) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gint64) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint64) ret_v_buffer[0];
    *(destination += destination_stride) = (gint64) ret_v_buffer[1];
    *(destination += destination_stride) = (gint64) ret_v_buffer[2];
    *(destination += destination_stride) = (gint64) ret_v_buffer[3];
    *(destination += destination_stride) = (gint64) ret_v_buffer[4];
    *(destination += destination_stride) = (gint64) ret_v_buffer[5];
    *(destination += destination_stride) = (gint64) ret_v_buffer[6];
    *(destination += destination_stride) = (gint64) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint64) ((gint64) (source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_float:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_float(AgsVolumeUtil *volume_util)
{
  gfloat *destination;
  gfloat *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gfloat *) volume_util->destination;
  source = (gfloat *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gfloat) v_buffer[0];
    *(destination += destination_stride) = (gfloat) v_buffer[1];
    *(destination += destination_stride) = (gfloat) v_buffer[2];
    *(destination += destination_stride) = (gfloat) v_buffer[3];
    *(destination += destination_stride) = (gfloat) v_buffer[4];
    *(destination += destination_stride) = (gfloat) v_buffer[5];
    *(destination += destination_stride) = (gfloat) v_buffer[6];
    *(destination += destination_stride) = (gfloat) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gfloat) ret_v_buffer[0];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[1];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[2];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[3];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[4];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[5];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[6];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gfloat) (((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gfloat) (((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gfloat) ((source[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_double:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_double(AgsVolumeUtil *volume_util)
{
  gdouble *destination;
  gdouble *source;

  guint source_stride, destination_stride;
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gdouble *) volume_util->destination;
  source = (gdouble *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gdouble) v_buffer[0];
    *(destination += destination_stride) = (gdouble) v_buffer[1];
    *(destination += destination_stride) = (gdouble) v_buffer[2];
    *(destination += destination_stride) = (gdouble) v_buffer[3];
    *(destination += destination_stride) = (gdouble) v_buffer[4];
    *(destination += destination_stride) = (gdouble) v_buffer[5];
    *(destination += destination_stride) = (gdouble) v_buffer[6];
    *(destination += destination_stride) = (gdouble) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#if defined(LARGE_VECTOR)
  double ret_v_buffer[256];
  double v_buffer[256];

  gint j;

  double v_volume[1] = { (double) volume_util->volume };

  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 256);
  
  while(i < i_stop){
    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      v_buffer[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_buffer, 1, v_volume, ret_v_buffer, 1, ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256));

    j = 0;

    while(i + j < volume_util->buffer_length && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) ret_v_buffer[j];

      j++;
    }
    
    i += ((volume_util->buffer_length < 256) ? volume_util->buffer_length: 256);
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) volume_util->volume};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gdouble) ret_v_buffer[0];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[1];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[2];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[3];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[4];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[5];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[6];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gdouble) (((source)[0] * (volume_util->volume)));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));
    *(destination += destination_stride) = (gdouble) (((source += source_stride)[0] * volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gdouble) ((source[0] * volume_util->volume));
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute_complex:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute_complex(AgsVolumeUtil *volume_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  guint source_stride, destination_stride;
  guint i;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) volume_util->destination;
  source = (AgsComplex *) volume_util->source;
  
  source_stride = volume_util->source_stride;
  destination_stride = volume_util->destination_stride;

  i = 0;
  
  for(; i < volume_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    ags_complex_set(destination,
		    z * (volume_util->volume));

    destination += destination_stride;
    source += source_stride;
    i++;
  }
}

/**
 * ags_volume_util_compute:
 * @volume_util: the #AgsVolumeUtil-struct
 * 
 * Compute volume.
 *
 * Since: 3.9.2
 */
void
ags_volume_util_compute(AgsVolumeUtil *volume_util)
{
  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  switch(volume_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_volume_util_compute_s8(volume_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_volume_util_compute_s16(volume_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_volume_util_compute_s24(volume_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_volume_util_compute_s32(volume_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_volume_util_compute_s64(volume_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_volume_util_compute_float(volume_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_volume_util_compute_double(volume_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_volume_util_compute_complex(volume_util);
  }
  break;
  }
}
