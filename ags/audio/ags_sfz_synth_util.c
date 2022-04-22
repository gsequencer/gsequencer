/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_sfz_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_diatonic_scale.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_iir_filter_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sfz_synth_util
 * @short_description: SFZ synth util
 * @title: AgsSFZSynthUtil
 * @section_id:
 * @include: ags/audio/ags_sfz_synth_util.h
 *
 * Utility functions to compute SFZ synths.
 */

GType
ags_sfz_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_synth_util = 0;

    ags_type_sfz_synth_util =
      g_boxed_type_register_static("AgsSFZSynthUtil",
				   (GBoxedCopyFunc) ags_sfz_synth_util_boxed_copy,
				   (GBoxedFreeFunc) ags_sfz_synth_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_synth_util);
  }

  return g_define_type_id__volatile;
}

GType
ags_sfz_synth_util_loop_mode_get_type()
{
  static volatile gsize g_enum_type_id__volatile;

  if(g_once_init_enter (&g_enum_type_id__volatile)){
    static const GEnumValue values[] = {
      { AGS_SFZ_SYNTH_UTIL_LOOP_NONE, "AGS_SFZ_SYNTH_UTIL_LOOP_NONE", "loop-none" },
      { AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD, "AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD", "loop-standard" },
      { AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE, "AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE", "loop-release" },
      { AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG, "AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG", "loop-pingpong" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSFZSynthUtilLoopMode"), values);

    g_once_init_leave (&g_enum_type_id__volatile, g_enum_type_id);
  }
  
  return g_enum_type_id__volatile;
}

/**
 * ags_sfz_synth_util_alloc:
 * 
 * Allocate #AgsSFZSynthUtil-struct.
 * 
 * Returns: the newly allocated #AgsSFZSynthUtil-struct
 * 
 * Since: 3.9.6
 */
AgsSFZSynthUtil*
ags_sfz_synth_util_alloc()
{
  AgsSFZSynthUtil *ptr;

  guint i;
  
  ptr = (AgsSFZSynthUtil *) g_new(AgsSFZSynthUtil,
				  1);

  ptr->flags = 0;

  ptr->sfz_file = NULL;

  ptr->sfz_sample_count = 0;
  ptr->sfz_sample_arr = (AgsSFZSample **) g_malloc(128 * sizeof(AgsSFZSample*));
  ptr->sfz_note_range = (gint **) g_malloc(128 * sizeof(gint*));
  
  ptr->sfz_orig_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer));

  ptr->sfz_resampled_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer));

  for(i = 0; i < 128; i++){
    ptr->sfz_sample_arr[i] = NULL;

    ptr->sfz_note_range[i] = (gint *) g_malloc(2 * sizeof(gint));
    ptr->sfz_note_range[i][0] = -1;
    ptr->sfz_note_range[i][1] = -1;
    
    ptr->sfz_orig_buffer_length[i] = 0;
    ptr->sfz_orig_buffer[i] = NULL;

    ptr->sfz_resampled_buffer_length[i] = 0;
    ptr->sfz_resampled_buffer[i] = NULL;

    ptr->sfz_loop_mode[i] = AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD;

    ptr->sfz_loop_start[i] = 0;
    ptr->sfz_loop_end[i] = 0;
  }

  ptr->sfz_sample = NULL;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->sample_buffer = NULL;
  ptr->im_buffer = NULL;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->midi_key = -1;
  
  ptr->note = 0.0;
  ptr->volume = 1.0;

  ptr->frame_count = 0;
  ptr->offset = 0;

  ptr->loop_mode = AGS_SFZ_SYNTH_UTIL_LOOP_NONE;

  ptr->loop_start = 0;
  ptr->loop_end = 0;

  ptr->resample_util = ags_resample_util_alloc();

  ags_resample_util_set_format(ptr->resample_util,
			       AGS_SOUNDCARD_DOUBLE);  

  ptr->pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL;
  ptr->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();

  ptr->volume_util = ags_volume_util_alloc();

  return(ptr);
}

/**
 * ags_sfz_synth_util_boxed_copy:
 * @ptr: the #AgsSFZSynthUtil-struct
 * 
 * Copy #AgsSFZSynthUtil-struct.
 * 
 * Returns: the newly allocated #AgsSFZSynthUtil-struct
 * 
 * Since: 3.9.6
 */
gpointer
ags_sfz_synth_util_boxed_copy(AgsSFZSynthUtil *ptr)
{
  AgsSFZSynthUtil *new_ptr;

  guint i;
  
  new_ptr = (AgsSFZSynthUtil *) g_new(AgsSFZSynthUtil,
				      1);
  
  new_ptr->sfz_sample = ptr->sfz_sample;

  if(new_ptr->sfz_sample != NULL){
    g_object_ref(new_ptr->sfz_sample);
  }
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->midi_key = ptr->midi_key;

  new_ptr->note = ptr->note;
  new_ptr->volume = ptr->volume;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->loop_mode = ptr->loop_mode;

  new_ptr->loop_start = ptr->loop_start;
  new_ptr->loop_end = ptr->loop_end;

  new_ptr->resample_util = ags_resample_util_copy(ptr->resample_util);

  new_ptr->pitch_type = ptr->pitch_type;

  if(new_ptr->pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    new_ptr->pitch_util = ags_fast_pitch_util_copy(ptr->pitch_util);
  }else if(new_ptr->pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    new_ptr->pitch_util = ags_hq_pitch_util_copy(ptr->pitch_util);
  }else if(new_ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    new_ptr->pitch_util = ags_fluid_interpolate_none_util_copy(ptr->pitch_util);
  }else if(new_ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    new_ptr->pitch_util = ags_fluid_interpolate_linear_util_copy(ptr->pitch_util);
  }else if(new_ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    new_ptr->pitch_util = ags_fluid_interpolate_4th_order_util_copy(ptr->pitch_util);
  }else if(new_ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    new_ptr->pitch_util = ags_fluid_interpolate_7th_order_util_copy(ptr->pitch_util);
  }

  new_ptr->volume_util = ags_hq_pitch_util_copy(ptr->volume_util);
  
  return(new_ptr);
}

/**
 * ags_sfz_synth_util_free:
 * @ptr: the #AgsSFZSynthUtil-struct
 * 
 * Free #AgsSFZSynthUtil-struct.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_free(AgsSFZSynthUtil *ptr)
{
  if(ptr->sfz_sample != NULL){
    g_object_unref(ptr->sfz_sample);
  }

  g_free(ptr->source);

  if(ptr->pitch_type == AGS_TYPE_FAST_PITCH_UTIL){
    ags_fast_pitch_util_free(ptr->pitch_util);
  }else if(ptr->pitch_type == AGS_TYPE_HQ_PITCH_UTIL){
    ags_hq_pitch_util_free(ptr->pitch_util);
  }else if(ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
    ags_fluid_interpolate_none_util_free(ptr->pitch_util);
  }else if(ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
    ags_fluid_interpolate_linear_util_free(ptr->pitch_util);
  }else if(ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
    ags_fluid_interpolate_4th_order_util_free(ptr->pitch_util);
  }else if(ptr->pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
    ags_fluid_interpolate_7th_order_util_free(ptr->pitch_util);
  }
  
  g_free(ptr);
}

/**
 * ags_sfz_synth_util_get_sfz_file:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get SFZ file of @sfz_synth_util.
 * 
 * Returns: the SFZ file
 * 
 * Since: 4.0.0
 */
AgsAudioContainer*
ags_sfz_synth_util_get_sfz_file(AgsSFZSynthUtil *sfz_synth_util)
{
  AgsAudioContainer *sfz_file;
  
  if(sfz_synth_util == NULL){
    return(NULL);
  }

  sfz_file = sfz_synth_util->sfz_file;

  if(sfz_file != NULL){
    g_object_ref(sfz_file);
  }
  
  return(sfz_file);
}

/**
 * ags_sfz_synth_util_set_sfz_file:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @sfz_file: the SFZ file
 *
 * Set SFZ file of @sfz_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_sfz_synth_util_set_sfz_file(AgsSFZSynthUtil *sfz_synth_util,
				AgsAudioContainer *sfz_file)
{
  if(sfz_synth_util == NULL){
    return;
  }
  
  if(sfz_file != NULL){
    g_object_ref(sfz_file);
  }

  if(sfz_synth_util->sfz_file != NULL){
    g_object_unref(sfz_synth_util->sfz_file);
  }

  sfz_synth_util->sfz_file = sfz_file;
}

/**
 * ags_sfz_synth_util_get_sfz_sample:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get SFZ sample of @sfz_synth_util.
 * 
 * Returns: the SFZ sample
 * 
 * Since: 4.0.0
 */
AgsSFZSample*
ags_sfz_synth_util_get_sfz_sample(AgsSFZSynthUtil *sfz_synth_util)
{
  AgsSFZSample *sfz_sample;
  
  if(sfz_synth_util == NULL){
    return(NULL);
  }

  sfz_sample = sfz_synth_util->sfz_sample;

  if(sfz_sample != NULL){
    g_object_ref(sfz_sample);
  }
  
  return(sfz_sample);
}

/**
 * ags_sfz_synth_util_set_sfz_sample:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @sfz_sample: the SFZ sample
 *
 * Set SFZ sample of @sfz_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_sfz_synth_util_set_sfz_sample(AgsSFZSynthUtil *sfz_synth_util,
				  AgsSFZSample *sfz_sample)
{
  if(sfz_synth_util == NULL){
    return;
  }
  
  if(sfz_sample != NULL){
    g_object_ref(sfz_sample);
  }

  if(sfz_synth_util->sfz_sample != NULL){
    g_object_unref(sfz_synth_util->sfz_sample);
  }

  sfz_synth_util->sfz_sample = sfz_sample;  
}

/**
 * ags_sfz_synth_util_get_source:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get source buffer of @sfz_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_sfz_synth_util_get_source(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(NULL);
  }

  return(sfz_synth_util->source);
}

/**
 * ags_sfz_synth_util_set_source:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_source(AgsSFZSynthUtil *sfz_synth_util,
			      gpointer source)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->source = source;
}

/**
 * ags_sfz_synth_util_get_source_stride:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get source stride of @sfz_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_source_stride(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->source_stride);
}

/**
 * ags_sfz_synth_util_set_source_stride:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_source_stride(AgsSFZSynthUtil *sfz_synth_util,
				     guint source_stride)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->source_stride = source_stride;
}

/**
 * ags_sfz_synth_util_get_buffer_length:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get buffer length of @sfz_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_buffer_length(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->buffer_length);
}

/**
 * ags_sfz_synth_util_set_buffer_length:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_buffer_length(AgsSFZSynthUtil *sfz_synth_util,
					  guint buffer_length)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->buffer_length = buffer_length;
}

/**
 * ags_sfz_synth_util_get_format:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get format of @sfz_synth_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_format(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->format);
}

/**
 * ags_sfz_synth_util_set_format:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @format: the format
 *
 * Set @format of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_format(AgsSFZSynthUtil *sfz_synth_util,
			      guint format)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->format = format;
}

/**
 * ags_sfz_synth_util_get_samplerate:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get samplerate of @sfz_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_samplerate(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->samplerate);
}

/**
 * ags_sfz_synth_util_set_samplerate:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_samplerate(AgsSFZSynthUtil *sfz_synth_util,
				  guint samplerate)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->samplerate = samplerate;
}

/**
 * ags_sfz_synth_util_get_midi_key:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get midi_key of @sfz_synth_util.
 * 
 * Returns: the midi key
 * 
 * Since: 3.17.0
 */
gint
ags_sfz_synth_util_get_midi_key(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(-1);
  }

  return(sfz_synth_util->midi_key);
}

/**
 * ags_sfz_synth_util_set_midi_key:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @midi_key: the midi key
 *
 * Set @midi_key of @sfz_synth_util.
 *
 * Since: 3.17.0
 */
void
ags_sfz_synth_util_set_midi_key(AgsSFZSynthUtil *sfz_synth_util,
				gint midi_key)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->midi_key = midi_key;
}

/**
 * ags_sfz_synth_util_get_note:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get note of @sfz_synth_util.
 * 
 * Returns: the note
 * 
 * Since: 3.9.6
 */
gdouble
ags_sfz_synth_util_get_note(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0.0);
  }

  return(sfz_synth_util->note);
}

/**
 * ags_sfz_synth_util_set_note:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @note: the note
 *
 * Set @note of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_note(AgsSFZSynthUtil *sfz_synth_util,
			    gdouble note)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->note = note;
}

/**
 * ags_sfz_synth_util_get_volume:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get volume of @sfz_synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.6
 */
gdouble
ags_sfz_synth_util_get_volume(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(1.0);
  }

  return(sfz_synth_util->volume);
}

/**
 * ags_sfz_synth_util_set_volume:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_volume(AgsSFZSynthUtil *sfz_synth_util,
			      gdouble volume)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->volume = volume;
}

/**
 * ags_sfz_synth_util_get_frame_count:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get frame count of @sfz_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_frame_count(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->frame_count);
}

/**
 * ags_sfz_synth_util_set_frame_count:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_frame_count(AgsSFZSynthUtil *sfz_synth_util,
				   guint frame_count)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->frame_count = frame_count;
}

/**
 * ags_sfz_synth_util_get_offset:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get offset of @sfz_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_offset(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->offset);
}

/**
 * ags_sfz_synth_util_set_offset:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_offset(AgsSFZSynthUtil *sfz_synth_util,
			      guint offset)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->offset = offset;
}

/**
 * ags_sfz_synth_util_get_loop_mode:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get loop mode of @sfz_synth_util.
 * 
 * Returns: the loop mode
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_loop_mode(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(AGS_SFZ_SYNTH_UTIL_LOOP_NONE);
  }

  return(sfz_synth_util->loop_mode);
}

/**
 * ags_sfz_synth_util_set_loop_mode:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @loop_mode: the loop mode
 *
 * Set @loop_mode of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_loop_mode(AgsSFZSynthUtil *sfz_synth_util,
				 guint loop_mode)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->loop_mode = loop_mode;
}

/**
 * ags_sfz_synth_util_get_loop_start:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get loop start of @sfz_synth_util.
 * 
 * Returns: the loop start
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_loop_start(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->loop_start);
}

/**
 * ags_sfz_synth_util_set_loop_start:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @loop_start: the loop start
 *
 * Set @loop_start of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_loop_start(AgsSFZSynthUtil *sfz_synth_util,
				  guint loop_start)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->loop_start = loop_start;
}

/**
 * ags_sfz_synth_util_get_loop_end:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get loop end of @sfz_synth_util.
 * 
 * Returns: the loop end
 * 
 * Since: 3.9.6
 */
guint
ags_sfz_synth_util_get_loop_end(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(0);
  }

  return(sfz_synth_util->loop_end);
}

/**
 * ags_sfz_synth_util_set_loop_end:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @loop_end: the loop end
 *
 * Set @loop_end of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_loop_end(AgsSFZSynthUtil *sfz_synth_util,
				guint loop_end)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->loop_end = loop_end;
}

/**
 * ags_sfz_synth_util_get_pitch_type:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get pitch type of @sfz_synth_util.
 * 
 * Returns: the pitch type
 * 
 * Since: 4.0.0
 */
GType
ags_sfz_synth_util_get_pitch_type(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(G_TYPE_NONE);
  }

  return(sfz_synth_util->pitch_type);
}

/**
 * ags_sfz_synth_util_set_pitch_type:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @pitch_type: the pitch type
 *
 * Set @pitch_type of @sfz_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_sfz_synth_util_set_pitch_type(AgsSFZSynthUtil *sfz_synth_util,
				  GType pitch_type)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->pitch_type = pitch_type;
}

/**
 * ags_sfz_synth_util_get_pitch_util:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get pitch util of @sfz_synth_util.
 * 
 * Returns: (transfer none): the pitch util
 * 
 * Since: 4.0.0
 */
gpointer
ags_sfz_synth_util_get_pitch_util(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(NULL);
  }

  return(sfz_synth_util->pitch_util);
}

/**
 * ags_sfz_synth_util_set_pitch_util:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @pitch_util: (transfer full): the pitch util
 *
 * Set @pitch_util of @sfz_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_sfz_synth_util_set_pitch_util(AgsSFZSynthUtil *sfz_synth_util,
				  gpointer pitch_util)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->pitch_util = pitch_util;
}

/**
 * ags_sfz_synth_util_load_instrument:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Load instrument of @sfz_synth_util.
 *
 * Since: 3.17.0
 */
void
ags_sfz_synth_util_load_instrument(AgsSFZSynthUtil *sfz_synth_util)
{
  GObject *sound_container;
  
  GList *start_sound_resource, *sound_resource;
  
  guint i;
  guint j;
  guint k;
  
  GRecMutex *audio_container_mutex;
  
  if(sfz_synth_util == NULL ||
     sfz_synth_util->sfz_file == NULL ||
     sfz_synth_util->sfz_file->sound_container == NULL){
    return;
  }

  audio_container_mutex = AGS_AUDIO_CONTAINER_GET_OBJ_MUTEX(sfz_synth_util->sfz_file);

  g_rec_mutex_lock(audio_container_mutex);

  sound_container = sfz_synth_util->sfz_file->sound_container;
  
  g_rec_mutex_unlock(audio_container_mutex);

  for(i = 0; i < sfz_synth_util->sfz_sample_count && i < 128; i++){
    if(sfz_synth_util->sfz_sample_arr[i] != NULL){
      g_object_unref(sfz_synth_util->sfz_sample_arr[i]);

      sfz_synth_util->sfz_sample_arr[i] = NULL;
    }

    sfz_synth_util->sfz_note_range[i][0] = -1;
    sfz_synth_util->sfz_note_range[i][1] = -1;
    
    sfz_synth_util->sfz_loop_start[i] = 0;
    sfz_synth_util->sfz_loop_end[i] = 0;

    ags_stream_free(sfz_synth_util->sfz_orig_buffer[i]);
		
    ags_stream_free(sfz_synth_util->sfz_resampled_buffer[i]);

    sfz_synth_util->sfz_orig_buffer_length[i] = 0;
    sfz_synth_util->sfz_orig_buffer[i] = NULL;

    sfz_synth_util->sfz_resampled_buffer_length[i] = 0;
    sfz_synth_util->sfz_resampled_buffer[i] = NULL;
  }

  sfz_synth_util->sfz_sample_count = 0;

  sound_resource =
    start_sound_resource = ags_sfz_file_get_sample(AGS_SFZ_FILE(sound_container));
  
  i = 0;
  
  while(sound_resource != NULL){
    AgsSFZSample *sample;
    
    gpointer buffer;
    gpointer cache;
	      
    guint sample_frame_count;
    guint sample_format;
    guint format;
    guint orig_samplerate;
    gint loop_start, loop_end;
    guint audio_channels;
    guint channel;
    int sfz_sample_format;
    guint copy_mode;

    GRecMutex *sfz_sample_mutex;
    
    sample = AGS_SFZ_SAMPLE(sound_resource->data);

    sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sample);
    
    sfz_synth_util->sfz_sample_arr[i] = sample;
    g_object_ref(sample);

    sample_frame_count = 0;

    format = AGS_SOUNDCARD_DOUBLE;
    sample_format = AGS_SOUNDCARD_DOUBLE;

    sample_frame_count = 0;
    orig_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
	      
    loop_start = 0;
    loop_end = 0;

    audio_channels = 0;
    channel = 0;

    ags_sound_resource_info(AGS_SOUND_RESOURCE(sample),
			    &sample_frame_count,
			    &loop_start, &loop_end);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sample),
				   &audio_channels,
				   &orig_samplerate,
				   NULL,
				   &sample_format);
    
    sfz_synth_util->sfz_note_range[i][0] = ags_sfz_sample_get_lokey(sample);
    sfz_synth_util->sfz_note_range[i][1] = ags_sfz_sample_get_hikey(sample);

    sfz_synth_util->sfz_loop_start[i] = loop_start;
    sfz_synth_util->sfz_loop_end[i] = loop_end;

    cache = NULL;
    copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						    ags_audio_buffer_util_format_from_soundcard(sample_format));

    cache = ags_stream_alloc(audio_channels * sample_frame_count,
			     sample_format);

    for(j = 0; j < audio_channels; j++){
      gpointer current_cache;

      g_rec_mutex_lock(sfz_sample_mutex);
      
      ags_sound_resource_seek(AGS_SOUND_RESOURCE(sample),
			      0, G_SEEK_SET);

      for(k = 0; k < sample_frame_count; ){
	current_cache = cache;
      
	switch(sample_format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  current_cache = ((gint8 *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  current_cache = ((gint16 *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  current_cache = ((gint32 *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  current_cache = ((gint32 *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  current_cache = ((gint64 *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_FLOAT:
	{
	  current_cache = ((gfloat *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_DOUBLE:
	{
	  current_cache = ((gdouble *) cache) + j + (audio_channels * k);
	}
	break;
	case AGS_SOUNDCARD_COMPLEX:
	{
	  current_cache = ((AgsComplex *) cache) + j + (audio_channels * k);
	}
	break;
	default:
	  g_warning("unknown format");
	}
	
	ags_sound_resource_read(AGS_SOUND_RESOURCE(sample),
				current_cache, audio_channels,
				j,
				256, sample_format);

	k += 256;
      }
      
      g_rec_mutex_unlock(sfz_sample_mutex);
    }
    
    sfz_synth_util->sfz_orig_buffer_length[i] = sample_frame_count;
    buffer =
      sfz_synth_util->sfz_orig_buffer[i] = ags_stream_alloc(sfz_synth_util->sfz_orig_buffer_length[i],
							    format);
		
    sfz_synth_util->sfz_resampled_buffer_length[i] = 0;
    sfz_synth_util->sfz_resampled_buffer[i] = NULL;
		
    ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
						cache, audio_channels, 0,
						sample_frame_count, copy_mode);

    ags_stream_free(cache);
	      
    if(sfz_synth_util->samplerate != orig_samplerate){
      AgsResampleUtil *resample_util;

      resample_util = sfz_synth_util->resample_util;

      sfz_synth_util->sfz_resampled_buffer_length[i] = floor(sfz_synth_util->samplerate / orig_samplerate) * sample_frame_count;
      sfz_synth_util->sfz_resampled_buffer[i] = ags_stream_alloc(sfz_synth_util->sfz_resampled_buffer_length[i],
								 format);
		  
      sfz_synth_util->sfz_loop_start[i] = floor(sfz_synth_util->samplerate / orig_samplerate) * loop_start;
      sfz_synth_util->sfz_loop_end[i] = floor(sfz_synth_util->samplerate / orig_samplerate) * loop_end;
		
      resample_util->destination = sfz_synth_util->sfz_resampled_buffer[i];
      resample_util->destination_stride = 1;
		  
      resample_util->source = sfz_synth_util->sfz_orig_buffer[i];
      resample_util->source_stride = 1;

      if(resample_util->secret_rabbit.data_in != NULL){
	g_free(resample_util->secret_rabbit.data_in);
      }

      if(resample_util->secret_rabbit.data_out != NULL){
	g_free(resample_util->secret_rabbit.data_out);
      }
		
      resample_util->secret_rabbit.src_ratio = sfz_synth_util->samplerate / orig_samplerate;
		
      resample_util->secret_rabbit.input_frames = sample_frame_count;

      resample_util->secret_rabbit.data_in = g_malloc(sample_frame_count * sizeof(gfloat));

      resample_util->secret_rabbit.output_frames = sfz_synth_util->sfz_resampled_buffer_length[i];

      resample_util->secret_rabbit.data_out = g_malloc(sfz_synth_util->sfz_resampled_buffer_length[i] * sizeof(gfloat));
		
      resample_util->buffer_length = sfz_synth_util->sfz_orig_buffer_length[i];
      resample_util->format = AGS_SOUNDCARD_DOUBLE;
      resample_util->samplerate = orig_samplerate;

      resample_util->audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE;

      resample_util->target_samplerate = sfz_synth_util->samplerate;

      ags_resample_util_compute(resample_util);
    }

    /* iterate */
    sound_resource = sound_resource->next;
    
    i++;    
  }

  sfz_synth_util->sfz_sample_count = i;
  
  g_list_free_full(start_sound_resource,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_sfz_synth_util_compute_s8:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s8(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s16:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s16(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s24:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s24(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s32:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s32(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s64:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s64(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_float:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_float(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_double:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_double(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_complex:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_complex(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute SFZ synth.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL ||
     sfz_synth_util->source == NULL){
    return;
  }

  switch(sfz_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_sfz_synth_util_compute_s8(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_sfz_synth_util_compute_s16(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_sfz_synth_util_compute_s24(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_sfz_synth_util_compute_s32(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_sfz_synth_util_compute_s64(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_sfz_synth_util_compute_float(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_sfz_synth_util_compute_double(sfz_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_sfz_synth_util_compute_complex(sfz_synth_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
