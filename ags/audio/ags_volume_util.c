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

#include <ags/audio/ags_volume_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

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

  ptr->destination = NULL;
  ptr->source = NULL;

  ptr->audio_channels = 0;

  ptr->buffer_length = 0;
  ptr->audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;
  
  ptr->volume = 1.0;

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
  
  new_ptr = (AgsVolumeUtil *) g_new(AgsVolumeUtil,
				      1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->source = ptr->source;

  new_ptr->audio_channels = ptr->audio_channels;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->audio_buffer_util_format = ptr->audio_buffer_util_format;

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
  g_free(ptr);
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint8 *) volume_util->destination;
  source = (gint8 *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint8) v_buffer[0];
    *(destination++) = (gint8) v_buffer[1];
    *(destination++) = (gint8) v_buffer[2];
    *(destination++) = (gint8) v_buffer[3];
    *(destination++) = (gint8) v_buffer[4];
    *(destination++) = (gint8) v_buffer[5];
    *(destination++) = (gint8) v_buffer[6];
    *(destination++) = (gint8) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint8) ret_v_buffer[0];
    *(destination++) = (gint8) ret_v_buffer[1];
    *(destination++) = (gint8) ret_v_buffer[2];
    *(destination++) = (gint8) ret_v_buffer[3];
    *(destination++) = (gint8) ret_v_buffer[4];
    *(destination++) = (gint8) ret_v_buffer[5];
    *(destination++) = (gint8) ret_v_buffer[6];
    *(destination++) = (gint8) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (volume_util->volume)));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint8) ((gint16) ((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint8) ((gint16) (source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint16 *) volume_util->destination;
  source = (gint16 *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint16) v_buffer[0];
    *(destination++) = (gint16) v_buffer[1];
    *(destination++) = (gint16) v_buffer[2];
    *(destination++) = (gint16) v_buffer[3];
    *(destination++) = (gint16) v_buffer[4];
    *(destination++) = (gint16) v_buffer[5];
    *(destination++) = (gint16) v_buffer[6];
    *(destination++) = (gint16) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint16) ret_v_buffer[0];
    *(destination++) = (gint16) ret_v_buffer[1];
    *(destination++) = (gint16) ret_v_buffer[2];
    *(destination++) = (gint16) ret_v_buffer[3];
    *(destination++) = (gint16) ret_v_buffer[4];
    *(destination++) = (gint16) ret_v_buffer[5];
    *(destination++) = (gint16) ret_v_buffer[6];
    *(destination++) = (gint16) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (volume_util->volume)));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint16) ((gint32) ((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint16) ((gint32) (source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint32 *) volume_util->destination;
  source = (gint32 *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination++) = (gint32) v_buffer[1];
    *(destination++) = (gint32) v_buffer[2];
    *(destination++) = (gint32) v_buffer[3];
    *(destination++) = (gint32) v_buffer[4];
    *(destination++) = (gint32) v_buffer[5];
    *(destination++) = (gint32) v_buffer[6];
    *(destination++) = (gint32) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination++) = (gint32) ret_v_buffer[1];
    *(destination++) = (gint32) ret_v_buffer[2];
    *(destination++) = (gint32) ret_v_buffer[3];
    *(destination++) = (gint32) ret_v_buffer[4];
    *(destination++) = (gint32) ret_v_buffer[5];
    *(destination++) = (gint32) ret_v_buffer[6];
    *(destination++) = (gint32) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint32) ((source)[0] * (volume_util->volume)));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint32) ((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint32) ((gint32) (source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint32 *) volume_util->destination;
  source = (gint32 *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination++) = (gint32) v_buffer[1];
    *(destination++) = (gint32) v_buffer[2];
    *(destination++) = (gint32) v_buffer[3];
    *(destination++) = (gint32) v_buffer[4];
    *(destination++) = (gint32) v_buffer[5];
    *(destination++) = (gint32) v_buffer[6];
    *(destination++) = (gint32) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination++) = (gint32) ret_v_buffer[1];
    *(destination++) = (gint32) ret_v_buffer[2];
    *(destination++) = (gint32) ret_v_buffer[3];
    *(destination++) = (gint32) ret_v_buffer[4];
    *(destination++) = (gint32) ret_v_buffer[5];
    *(destination++) = (gint32) ret_v_buffer[6];
    *(destination++) = (gint32) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (volume_util->volume)));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint32) ((gint64) ((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint32) ((gint64) (source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gint64 *) volume_util->destination;
  source = (gint64 *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gint64) v_buffer[0];
    *(destination++) = (gint64) v_buffer[1];
    *(destination++) = (gint64) v_buffer[2];
    *(destination++) = (gint64) v_buffer[3];
    *(destination++) = (gint64) v_buffer[4];
    *(destination++) = (gint64) v_buffer[5];
    *(destination++) = (gint64) v_buffer[6];
    *(destination++) = (gint64) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gint64) ret_v_buffer[0];
    *(destination++) = (gint64) ret_v_buffer[1];
    *(destination++) = (gint64) ret_v_buffer[2];
    *(destination++) = (gint64) ret_v_buffer[3];
    *(destination++) = (gint64) ret_v_buffer[4];
    *(destination++) = (gint64) ret_v_buffer[5];
    *(destination++) = (gint64) ret_v_buffer[6];
    *(destination++) = (gint64) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (volume_util->volume)));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));
    *(destination++) = (gint64) ((gint64) ((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gint64) ((gint64) (source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gfloat *) volume_util->destination;
  source = (gfloat *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gfloat) v_buffer[0];
    *(destination++) = (gfloat) v_buffer[1];
    *(destination++) = (gfloat) v_buffer[2];
    *(destination++) = (gfloat) v_buffer[3];
    *(destination++) = (gfloat) v_buffer[4];
    *(destination++) = (gfloat) v_buffer[5];
    *(destination++) = (gfloat) v_buffer[6];
    *(destination++) = (gfloat) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gfloat) ret_v_buffer[0];
    *(destination++) = (gfloat) ret_v_buffer[1];
    *(destination++) = (gfloat) ret_v_buffer[2];
    *(destination++) = (gfloat) ret_v_buffer[3];
    *(destination++) = (gfloat) ret_v_buffer[4];
    *(destination++) = (gfloat) ret_v_buffer[5];
    *(destination++) = (gfloat) ret_v_buffer[6];
    *(destination++) = (gfloat) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gfloat) (((source)[0] * (volume_util->volume)));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));
    *(destination++) = (gfloat) (((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gfloat) ((source[0] * volume_util->volume));

    destination++;
    source++;
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
  guint i, i_stop;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }

  destination = (gdouble *) volume_util->destination;
  source = (gdouble *) volume_util->source;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++),
      (gdouble) *(source++)
    };

    source++;
    
    v_buffer *= volume_util->volume;

    *(destination) = (gdouble) v_buffer[0];
    *(destination++) = (gdouble) v_buffer[1];
    *(destination++) = (gdouble) v_buffer[2];
    *(destination++) = (gdouble) v_buffer[3];
    *(destination++) = (gdouble) v_buffer[4];
    *(destination++) = (gdouble) v_buffer[5];
    *(destination++) = (gdouble) v_buffer[6];
    *(destination++) = (gdouble) v_buffer[7];

    destination++;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++),
      (double) *(source++)};

    double v_volume[] = {(double) volume_util->volume};

    source++;

    vDSP_vmulD(v_buffer, 1, v_volume, 0, ret_v_buffer, 1, 8);

    *(destination) = (gdouble) ret_v_buffer[0];
    *(destination++) = (gdouble) ret_v_buffer[1];
    *(destination++) = (gdouble) ret_v_buffer[2];
    *(destination++) = (gdouble) ret_v_buffer[3];
    *(destination++) = (gdouble) ret_v_buffer[4];
    *(destination++) = (gdouble) ret_v_buffer[5];
    *(destination++) = (gdouble) ret_v_buffer[6];
    *(destination++) = (gdouble) ret_v_buffer[7];

    destination++;
    i += 8;
  }
#else
  i_stop = volume_util->buffer_length - (volume_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gdouble) (((source)[0] * (volume_util->volume)));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));
    *(destination++) = (gdouble) (((source++)[0] * volume_util->volume));

    destination++;
    source++;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = (gdouble) ((source[0] * volume_util->volume));
    
    destination++;
    source++;
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
  guint i;

  if(volume_util == NULL ||
     volume_util->destination == NULL ||
     volume_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) volume_util->destination;
  source = (AgsComplex *) volume_util->source;
  
  i = 0;
  
  for(; i < volume_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    ags_complex_set(destination,
		    z * (volume_util->volume));

    destination++;
    source++;
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

  switch(volume_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_volume_util_compute_s8(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_volume_util_compute_s16(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_volume_util_compute_s24(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_volume_util_compute_s32(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_volume_util_compute_s64(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_volume_util_compute_float(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_volume_util_compute_double(volume_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_volume_util_compute_complex(volume_util);
  }
  break;
  }
}
