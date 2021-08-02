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

#include <ags/audio/ags_sfz_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_diatonic_scale.h>
#include <ags/audio/ags_fluid_pitch_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>

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
  
  ptr = (AgsSFZSynthUtil *) g_new(AgsSFZSynthUtil,
				  1);

  ptr->sfz_sample = NULL;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  ptr->note = 0.0;

  ptr->volume = 1.0;

  ptr->frame_count = 0;
  ptr->offset = 0;

  ptr->loop_mode = AGS_SFZ_SYNTH_UTIL_LOOP_NONE;

  ptr->loop_start = 0;
  ptr->loop_end = 0;

  ptr->generic_pitch_util = ags_generic_pitch_util_alloc();

  return(ptr);
}

/**
 * ags_sfz_synth_util_copy:
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

  new_ptr->note = ptr->note;

  new_ptr->volume = ptr->volume;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->loop_mode = ptr->loop_mode;

  new_ptr->loop_start = ptr->loop_start;
  new_ptr->loop_end = ptr->loop_end;

  new_ptr->generic_pitch_util = ags_generic_pitch_util_copy(ptr->generic_pitch_util);
  
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

  ags_generic_pitch_util_free(ptr->generic_pitch_util);
  
  g_free(ptr);
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
 * ags_sfz_synth_util_get_generic_pitch_util:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Get generic pitch util of @sfz_synth_util.
 * 
 * Returns: (transfer none): the generic pitch util
 * 
 * Since: 3.9.6
 */
AgsGenericPitchUtil*
ags_sfz_synth_util_get_generic_pitch_util(AgsSFZSynthUtil *sfz_synth_util)
{
  if(sfz_synth_util == NULL){
    return(NULL);
  }

  return(sfz_synth_util->generic_pitch_util);
}

/**
 * ags_sfz_synth_util_set_generic_pitch_util:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * @generic_pitch_util: (transfer full): the generic pitch util
 *
 * Set @generic_pitch_util of @sfz_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_set_generic_pitch_util(AgsSFZSynthUtil *sfz_synth_util,
					  AgsGenericPitchUtil *generic_pitch_util)
{
  if(sfz_synth_util == NULL){
    return;
  }

  sfz_synth_util->generic_pitch_util = generic_pitch_util;
}

/**
 * ags_sfz_synth_util_compute_s8:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s8(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s16:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s16(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s24:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s24(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s32:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s32(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_s64:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_s64(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_float:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_float(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_double:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_double(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute_complex:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute_complex(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_compute:
 * @sfz_synth_util: the #AgsSFZSynthUtil-struct
 * 
 * Compute Soundfont2 synth.
 * 
 * Since: 3.9.6
 */
void
ags_sfz_synth_util_compute(AgsSFZSynthUtil *sfz_synth_util)
{
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_util_copy_s8:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_s8(gint8 *buffer,
			   guint buffer_size,
			   AgsSFZSample *sfz_sample,
			   gdouble note,
			   gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames,
			   guint loop_mode,
			   gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint8 *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s8(im_buffer,
			       frame_count,
			       samplerate,
			       base_key,
			       tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s8_to_s8(buffer + i2, 1,
					    im_buffer + i1, 1,
					    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s8(buffer + i2, 1,
				      im_buffer + i1, 1,
				      copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_s16:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_s16(gint16 *buffer,
			    guint buffer_size,
			    AgsSFZSample *sfz_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint16 *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_16_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);

//  g_message("tuning %f", tuning);
  
  ags_fluid_pitch_util_compute_s16(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s16_to_s16(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s16(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_s24:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_s24(gint32 *buffer,
			    guint buffer_size,
			    AgsSFZSample *sfz_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_24_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s24(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s24_to_s24(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s24(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_s32:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_s32(gint32 *buffer,
			    guint buffer_size,
			    AgsSFZSample *sfz_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_32_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s32(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s32_to_s32(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s32(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_s64:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_s64(gint64 *buffer,
			    guint buffer_size,
			    AgsSFZSample *sfz_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint64 *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_64_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s64(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s64_to_s64(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s64(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_float:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_float(gfloat *buffer,
			      guint buffer_size,
			      AgsSFZSample *sfz_sample,
			      gdouble note,
			      gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint loop_mode,
			      gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gfloat *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_FLOAT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_float(im_buffer,
				  frame_count,
				  samplerate,
				  base_key,
				  tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_float_to_float(buffer + i2, 1,
						  im_buffer + i1, 1,
						  copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_float(buffer + i2, 1,
					 im_buffer + i1, 1,
					 copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_double:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_double(gdouble *buffer,
			       guint buffer_size,
			       AgsSFZSample *sfz_sample,
			       gdouble note,
			       gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint loop_mode,
			       gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gdouble *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
    
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_DOUBLE);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_double(im_buffer,
				   frame_count,
				   samplerate,
				   base_key,
				   tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_double_to_double(buffer + i2, 1,
						    im_buffer + i1, 1,
						    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_double(buffer + i2, 1,
					  im_buffer + i1, 1,
					  copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy_complex:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate SFZ wave.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy_complex(AgsComplex *buffer,
				guint buffer_size,
				AgsSFZSample *sfz_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint loop_mode,
				gint loop_start, gint loop_end)
{
  void *sample_buffer;

  AgsComplex *im_buffer;

  gint key;
  gint pitch_keycenter;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_COMPLEX);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_complex(im_buffer,
				    frame_count,
				    samplerate,
				    base_key,
				    tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_complex_to_complex(buffer + i2, 1,
						      im_buffer + i1, 1,
						      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_complex(buffer + i2, 1,
					   im_buffer + i1, 1,
					   copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sfz_synth_util_copy:
 * @buffer: the audio buffer
 * @buffer_size: the buffer size
 * @sfz_sample: the #AgsSFZSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: the loop start
 * @loop_end: the loop end
 *
 * Generate Soundfont2 wave.
 *
 * Since: 3.4.0
 */
void
ags_sfz_synth_util_copy(void *buffer,
			guint buffer_size,
			AgsSFZSample *sfz_sample,
			gdouble note,
			gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames,
			guint loop_mode,
			gint loop_start, gint loop_end)
{
  if(buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_sfz_synth_util_copy_s8((gint8 *) buffer,
			       buffer_size,
			       sfz_sample,
			       note,
			       volume,
			       samplerate,
			       offset, n_frames,
			       loop_mode,
			       loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_sfz_synth_util_copy_s16((gint16 *) buffer,
				buffer_size,
				sfz_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_sfz_synth_util_copy_s24((gint32 *) buffer,
				buffer_size,
				sfz_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_sfz_synth_util_copy_s32((gint32 *) buffer,
				buffer_size,
				sfz_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_sfz_synth_util_copy_s64((gint64 *) buffer,
				buffer_size,
				sfz_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_sfz_synth_util_copy_float((float *) buffer,
				  buffer_size,
				  sfz_sample,
				  note,
				  volume,
				  samplerate,
				  offset, n_frames,
				  loop_mode,
				  loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_sfz_synth_util_copy_double((double *) buffer,
				   buffer_size,
				   sfz_sample,
				   note,
				   volume,
				   samplerate,
				   offset, n_frames,
				   loop_mode,
				   loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_sfz_synth_util_copy_complex((AgsComplex *) buffer,
				    buffer_size,
				    sfz_sample,
				    note,
				    volume,
				    samplerate,
				    offset, n_frames,
				    loop_mode,
				    loop_start, loop_end);
  }
  break;
  default:
  {
    g_warning("ags_sfz_synth_util_copy() - unsupported format");
  }
  }
}
