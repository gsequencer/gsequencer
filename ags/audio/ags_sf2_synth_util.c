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

#include <ags/audio/ags_sf2_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_pitch_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sf2_synth_util
 * @short_description: SF2 synth util
 * @title: AgsSF2SynthUtil
 * @section_id:
 * @include: ags/audio/ags_sf2_synth_util.h
 *
 * Utility functions to compute SF2 synths.
 */

GType
ags_sf2_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_synth_util = 0;

    ags_type_sf2_synth_util =
      g_boxed_type_register_static("AgsSF2SynthUtil",
				   (GBoxedCopyFunc) ags_sf2_synth_util_boxed_copy,
				   (GBoxedFreeFunc) ags_sf2_synth_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_synth_util);
  }

  return g_define_type_id__volatile;
}

GType
ags_sf2_synth_util_loop_mode_get_type()
{
  static volatile gsize g_enum_type_id__volatile;

  if(g_once_init_enter (&g_enum_type_id__volatile)){
    static const GEnumValue values[] = {
      { AGS_SF2_SYNTH_UTIL_LOOP_NONE, "AGS_SF2_SYNTH_UTIL_LOOP_NONE", "loop-none" },
      { AGS_SF2_SYNTH_UTIL_LOOP_STANDARD, "AGS_SF2_SYNTH_UTIL_LOOP_STANDARD", "loop-standard" },
      { AGS_SF2_SYNTH_UTIL_LOOP_RELEASE, "AGS_SF2_SYNTH_UTIL_LOOP_RELEASE", "loop-release" },
      { AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG, "AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG", "loop-pingpong" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSF2SynthUtilLoopMode"), values);

    g_once_init_leave (&g_enum_type_id__volatile, g_enum_type_id);
  }
  
  return g_enum_type_id__volatile;
}

/**
 * ags_sf2_synth_util_alloc:
 * 
 * Allocate #AgsSF2SynthUtil-struct.
 * 
 * Returns: the newly allocated #AgsSF2SynthUtil-struct
 * 
 * Since: 3.9.6
 */
AgsSF2SynthUtil*
ags_sf2_synth_util_alloc()
{
  AgsSF2SynthUtil *ptr;

  guint i;
  
  ptr = (AgsSF2SynthUtil *) g_new(AgsSF2SynthUtil,
				  1);

  ptr->flags = 0;

  ptr->sf2_file = NULL;

  ptr->sf2_sample_count = 0;
  ptr->sf2_sample = (AgsIpatchSample **) g_malloc(128 * sizeof(AgsIpatchSample*));
  ptr->sf2_note_range = (gint **) g_malloc(128 * sizeof(gint*));
  
  ptr->sf2_orig_buffer_length = (gpointer *) g_malloc(128 * sizeof(guint));
  ptr->sf2_orig_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer));

  ptr->sf2_resampled_buffer_length = (gpointer *) g_malloc(128 * sizeof(guint));
  ptr->sf2_resampled_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer));

  for(i = 0; i < 128; i++){
    ptr->sf2_sample[i] = NULL;

    ptr->sf2_note_range[i] = (gint *) g_malloc(2 * sizeof(gint));
    ptr->sf2_note_range[i][0] = -1;
    ptr->sf2_note_range[i][1] = -1;
    
    ptr->sf2_orig_buffer_length[i] = 0;
    ptr->sf2_orig_buffer[i] = NULL;

    ptr->sf2_resampled_buffer_length[i] = 0;
    ptr->sf2_resampled_buffer[i] = NULL;
  }
  
  ptr->ipatch_sample = NULL;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->sample_buffer = NULL;
  ptr->im_buffer = NULL;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->preset = NULL;
  ptr->instrument = NULL;
  ptr->sample = NULL;

  ptr->bank = -1;
  ptr->program = -1;

  ptr->midi_key = -1;
  
  ptr->note = 0.0;

  ptr->volume = 1.0;

  ptr->frame_count = 0;
  ptr->offset = 0;

  ptr->loop_mode = AGS_SF2_SYNTH_UTIL_LOOP_NONE;

  ptr->loop_start = 0;
  ptr->loop_end = 0;

  ptr->resample_util = ags_resample_util_alloc();

  ags_resample_util_set_format(ptr->resample_util,
			       AGS_SOUNDCARD_DOUBLE);  

  ptr->generic_pitch_util = ags_generic_pitch_util_alloc();

  ptr->hq_pitch_util = ags_hq_pitch_util_alloc();

  ptr->volume_util = ags_volume_util_alloc();

  return(ptr);
}

/**
 * ags_sf2_synth_util_boxed_copy:
 * @ptr: the #AgsSF2SynthUtil-struct
 * 
 * Copy #AgsSF2SynthUtil-struct.
 * 
 * Returns: the newly allocated #AgsSF2SynthUtil-struct
 * 
 * Since: 3.9.6
 */
gpointer
ags_sf2_synth_util_boxed_copy(AgsSF2SynthUtil *ptr)
{
  AgsSF2SynthUtil *new_ptr;
  
  new_ptr = (AgsSF2SynthUtil *) g_new(AgsSF2SynthUtil,
				      1);
  
  new_ptr->flags = ptr->flags;

  new_ptr->ipatch_sample = ptr->ipatch_sample;

  if(new_ptr->ipatch_sample != NULL){
    g_object_ref(new_ptr->ipatch_sample);
  }

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->sample_buffer = ptr->sample_buffer;
  new_ptr->im_buffer = ptr->im_buffer;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->preset = g_strdup(ptr->preset);
  new_ptr->instrument = g_strdup(ptr->instrument);
  new_ptr->sample = g_strdup(ptr->sample);

  new_ptr->bank = ptr->bank;
  new_ptr->program = ptr->program;

  new_ptr->midi_key = ptr->midi_key;

  new_ptr->note = ptr->note;

  new_ptr->volume = ptr->volume;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->loop_mode = ptr->loop_mode;

  new_ptr->loop_start = ptr->loop_start;
  new_ptr->loop_end = ptr->loop_end;

  new_ptr->resample_util = ags_resample_util_copy(ptr->resample_util);
  new_ptr->generic_pitch_util = ags_generic_pitch_util_copy(ptr->generic_pitch_util);
  
  new_ptr->hq_pitch_util = ags_hq_pitch_util_copy(ptr->hq_pitch_util);

  new_ptr->volume_util = ags_hq_pitch_util_copy(ptr->volume_util);

  return(new_ptr);
}

/**
 * ags_sf2_synth_util_free:
 * @ptr: the #AgsSF2SynthUtil-struct
 * 
 * Free #AgsSF2SynthUtil-struct.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_free(AgsSF2SynthUtil *ptr)
{
  if(ptr->ipatch_sample != NULL){
    g_object_unref(ptr->ipatch_sample);
  }

  ags_stream_free(ptr->source);

  ags_stream_free(ptr->sample_buffer);
  ags_stream_free(ptr->im_buffer);
  
  g_free(ptr->preset);
  g_free(ptr->instrument);
  g_free(ptr->sample);

  ags_resample_util_free(ptr->resample_util);
  ags_generic_pitch_util_free(ptr->generic_pitch_util);
  
  g_free(ptr);
}

AgsIpatchSample*
ags_sf2_synth_util_get_ipatch_sample(AgsSF2SynthUtil *sf2_synth_util)
{
  AgsIpatchSample *ipatch_sample;
  
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  ipatch_sample = sf2_synth_util->ipatch_sample;

  if(ipatch_sample != NULL){
    g_object_ref(ipatch_sample);
  }
  
  return(ipatch_sample);
}

void
ags_sf2_synth_util_set_ipatch_sample(AgsSF2SynthUtil *sf2_synth_util,
				     AgsIpatchSample *ipatch_sample)
{
  if(sf2_synth_util == NULL){
    return;
  }
  
  if(ipatch_sample != NULL){
    g_object_ref(ipatch_sample);
  }

  if(sf2_synth_util->ipatch_sample != NULL){
    g_object_unref(sf2_synth_util->ipatch_sample);
  }

  sf2_synth_util->ipatch_sample = ipatch_sample;
  
}

/**
 * ags_sf2_synth_util_get_source:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get source buffer of @sf2_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_sf2_synth_util_get_source(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(sf2_synth_util->source);
}

/**
 * ags_sf2_synth_util_set_source:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_source(AgsSF2SynthUtil *sf2_synth_util,
			      gpointer source)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->source = source;
}

/**
 * ags_sf2_synth_util_get_source_stride:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get source stride of @sf2_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_source_stride(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->source_stride);
}

/**
 * ags_sf2_synth_util_set_source_stride:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_source_stride(AgsSF2SynthUtil *sf2_synth_util,
				     guint source_stride)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->source_stride = source_stride;
}

/**
 * ags_sf2_synth_util_get_buffer_length:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get buffer length of @sf2_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_buffer_length(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->buffer_length);
}

/**
 * ags_sf2_synth_util_set_buffer_length:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_buffer_length(AgsSF2SynthUtil *sf2_synth_util,
				     guint buffer_length)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->buffer_length = buffer_length;
}

/**
 * ags_sf2_synth_util_get_format:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get format of @sf2_synth_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_format(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->format);
}

/**
 * ags_sf2_synth_util_set_format:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @format: the format
 *
 * Set @format of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_format(AgsSF2SynthUtil *sf2_synth_util,
			      guint format)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->format = format;
}

/**
 * ags_sf2_synth_util_get_samplerate:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get samplerate of @sf2_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_samplerate(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->samplerate);
}

/**
 * ags_sf2_synth_util_set_samplerate:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_samplerate(AgsSF2SynthUtil *sf2_synth_util,
				  guint samplerate)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->samplerate = samplerate;
}

/**
 * ags_sf2_synth_util_get_preset:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get preset of @sf2_synth_util.
 * 
 * Returns: the preset
 * 
 * Since: 3.9.6
 */
gchar*
ags_sf2_synth_util_get_preset(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(g_strdup(sf2_synth_util->preset));
}

/**
 * ags_sf2_synth_util_set_preset:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @preset: the preset
 *
 * Set @preset of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_preset(AgsSF2SynthUtil *sf2_synth_util,
			      gchar *preset)
{
  if(sf2_synth_util == NULL){
    return;
  }

  g_free(sf2_synth_util->preset);
  
  sf2_synth_util->preset = g_strdup(preset);
}

/**
 * ags_sf2_synth_util_get_instrument:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get instrument of @sf2_synth_util.
 * 
 * Returns: the instrument
 * 
 * Since: 3.9.6
 */
gchar*
ags_sf2_synth_util_get_instrument(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(g_strdup(sf2_synth_util->instrument));
}

/**
 * ags_sf2_synth_util_set_instrument:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @instrument: the instrument
 *
 * Set @instrument of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_instrument(AgsSF2SynthUtil *sf2_synth_util,
				  gchar *instrument)
{
  if(sf2_synth_util == NULL){
    return;
  }

  g_free(sf2_synth_util->instrument);
  
  sf2_synth_util->instrument = g_strdup(instrument);
}

/**
 * ags_sf2_synth_util_get_sample:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get sample of @sf2_synth_util.
 * 
 * Returns: the sample
 * 
 * Since: 3.9.6
 */
gchar*
ags_sf2_synth_util_get_sample(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(g_strdup(sf2_synth_util->sample));
}

/**
 * ags_sf2_synth_util_set_sample:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @sample: the sample
 *
 * Set @sample of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_sample(AgsSF2SynthUtil *sf2_synth_util,
			      gchar *sample)
{
  if(sf2_synth_util == NULL){
    return;
  }

  g_free(sf2_synth_util->sample);
  
  sf2_synth_util->sample = g_strdup(sample);
}

/**
 * ags_sf2_synth_util_get_bank:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get bank of @sf2_synth_util.
 * 
 * Returns: the bank
 * 
 * Since: 3.9.6
 */
gint
ags_sf2_synth_util_get_bank(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(-1);
  }

  return(sf2_synth_util->bank);
}

/**
 * ags_sf2_synth_util_set_bank:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @bank: the bank
 *
 * Set @bank of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_bank(AgsSF2SynthUtil *sf2_synth_util,
			    gint bank)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->bank = bank;
}

/**
 * ags_sf2_synth_util_get_program:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get program of @sf2_synth_util.
 * 
 * Returns: the program
 * 
 * Since: 3.9.6
 */
gint
ags_sf2_synth_util_get_program(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(-1);
  }

  return(sf2_synth_util->program);
}

/**
 * ags_sf2_synth_util_set_program:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @program: the program
 *
 * Set @program of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_program(AgsSF2SynthUtil *sf2_synth_util,
			       gint program)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->program = program;
}

/**
 * ags_sf2_synth_util_get_midi_key:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get midi_key of @sf2_synth_util.
 * 
 * Returns: the midi key
 * 
 * Since: 3.9.6
 */
gint
ags_sf2_synth_util_get_midi_key(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(-1);
  }

  return(sf2_synth_util->midi_key);
}

/**
 * ags_sf2_synth_util_set_midi_key:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @midi_key: the midi key
 *
 * Set @midi_key of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_midi_key(AgsSF2SynthUtil *sf2_synth_util,
				gint midi_key)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->midi_key = midi_key;
}

/**
 * ags_sf2_synth_util_get_note:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get note of @sf2_synth_util.
 * 
 * Returns: the note
 * 
 * Since: 3.9.6
 */
gdouble
ags_sf2_synth_util_get_note(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0.0);
  }

  return(sf2_synth_util->note);
}

/**
 * ags_sf2_synth_util_set_note:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @note: the note
 *
 * Set @note of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_note(AgsSF2SynthUtil *sf2_synth_util,
			    gdouble note)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->note = note;
}

/**
 * ags_sf2_synth_util_get_volume:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get volume of @sf2_synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.6
 */
gdouble
ags_sf2_synth_util_get_volume(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(1.0);
  }

  return(sf2_synth_util->volume);
}

/**
 * ags_sf2_synth_util_set_volume:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_volume(AgsSF2SynthUtil *sf2_synth_util,
			      gdouble volume)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->volume = volume;
}

/**
 * ags_sf2_synth_util_get_frame_count:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get frame count of @sf2_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_frame_count(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->frame_count);
}

/**
 * ags_sf2_synth_util_set_frame_count:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_frame_count(AgsSF2SynthUtil *sf2_synth_util,
				   guint frame_count)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->frame_count = frame_count;
}

/**
 * ags_sf2_synth_util_get_offset:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get offset of @sf2_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_offset(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->offset);
}

/**
 * ags_sf2_synth_util_set_offset:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_offset(AgsSF2SynthUtil *sf2_synth_util,
			      guint offset)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->offset = offset;
}

/**
 * ags_sf2_synth_util_get_loop_mode:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get loop mode of @sf2_synth_util.
 * 
 * Returns: the loop mode
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_loop_mode(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(AGS_SF2_SYNTH_UTIL_LOOP_NONE);
  }

  return(sf2_synth_util->loop_mode);
}

/**
 * ags_sf2_synth_util_set_loop_mode:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @loop_mode: the loop mode
 *
 * Set @loop_mode of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_loop_mode(AgsSF2SynthUtil *sf2_synth_util,
				 guint loop_mode)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->loop_mode = loop_mode;
}

/**
 * ags_sf2_synth_util_get_loop_start:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get loop start of @sf2_synth_util.
 * 
 * Returns: the loop start
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_loop_start(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->loop_start);
}

/**
 * ags_sf2_synth_util_set_loop_start:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @loop_start: the loop start
 *
 * Set @loop_start of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_loop_start(AgsSF2SynthUtil *sf2_synth_util,
				  guint loop_start)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->loop_start = loop_start;
}

/**
 * ags_sf2_synth_util_get_loop_end:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get loop end of @sf2_synth_util.
 * 
 * Returns: the loop end
 * 
 * Since: 3.9.6
 */
guint
ags_sf2_synth_util_get_loop_end(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(0);
  }

  return(sf2_synth_util->loop_end);
}

/**
 * ags_sf2_synth_util_set_loop_end:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @loop_end: the loop end
 *
 * Set @loop_end of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_loop_end(AgsSF2SynthUtil *sf2_synth_util,
				guint loop_end)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->loop_end = loop_end;
}

/**
 * ags_sf2_synth_util_get_resample_util:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get resample util of @sf2_synth_util.
 * 
 * Returns: (transfer none): the resample util
 * 
 * Since: 3.9.6
 */
AgsResampleUtil*
ags_sf2_synth_util_get_resample_util(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(sf2_synth_util->resample_util);
}

/**
 * ags_sf2_synth_util_set_resample_util:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @resample_util: (transfer full): the resample util
 *
 * Set @resample_util of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_resample_util(AgsSF2SynthUtil *sf2_synth_util,
				     AgsResampleUtil *resample_util)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->resample_util = resample_util;
}

/**
 * ags_sf2_synth_util_get_generic_pitch_util:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Get generic pitch util of @sf2_synth_util.
 * 
 * Returns: (transfer none): the generic pitch util
 * 
 * Since: 3.9.6
 */
AgsGenericPitchUtil*
ags_sf2_synth_util_get_generic_pitch_util(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL){
    return(NULL);
  }

  return(sf2_synth_util->generic_pitch_util);
}

/**
 * ags_sf2_synth_util_set_generic_pitch_util:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @generic_pitch_util: (transfer full): the generic pitch util
 *
 * Set @generic_pitch_util of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_set_generic_pitch_util(AgsSF2SynthUtil *sf2_synth_util,
					  AgsGenericPitchUtil *generic_pitch_util)
{
  if(sf2_synth_util == NULL){
    return;
  }

  sf2_synth_util->generic_pitch_util = generic_pitch_util;
}

/**
 * ags_sf2_synth_util_read_ipatch_sample:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 *
 * Read ipatch sample of @sf2_synth_util.
 *
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_read_ipatch_sample(AgsSF2SynthUtil *sf2_synth_util)
{
  guint ipatch_sample_frame_count;
  guint resampled_ipatch_sample_frame_count;
  guint audio_channels;
  guint ipatch_sample_samplerate;
  
  if(sf2_synth_util == NULL){
    return;
  }

  if(sf2_synth_util->ipatch_sample == NULL){
    ags_stream_free(sf2_synth_util->sample_buffer);
    sf2_synth_util->sample_buffer = NULL;
    
    ags_stream_free(sf2_synth_util->im_buffer);
    sf2_synth_util->im_buffer = NULL;
    
    return;
  }
  
  audio_channels = 1;

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			  &ipatch_sample_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				 &audio_channels,
				 &ipatch_sample_samplerate,
				 NULL,
				 NULL);

  resampled_ipatch_sample_frame_count = ipatch_sample_frame_count;
  
  if(sf2_synth_util->samplerate != ipatch_sample_samplerate){
    resampled_ipatch_sample_frame_count = (guint) ceil(ipatch_sample_frame_count / ipatch_sample_samplerate * sf2_synth_util->samplerate);
  }
  
  ags_stream_free(sf2_synth_util->sample_buffer);

  ags_stream_free(sf2_synth_util->im_buffer);

  if(audio_channels > 0 &&
     resampled_ipatch_sample_frame_count > 0){
    sf2_synth_util->sample_buffer = ags_stream_alloc(audio_channels * resampled_ipatch_sample_frame_count,
						     AGS_SOUNDCARD_DOUBLE);

    sf2_synth_util->im_buffer = ags_stream_alloc(resampled_ipatch_sample_frame_count,
						 sf2_synth_util->format);
  }else{
    sf2_synth_util->sample_buffer = NULL;
    sf2_synth_util->im_buffer = NULL;
  }

  if(sf2_synth_util->samplerate == ipatch_sample_samplerate){
    ags_sound_resource_read(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    sf2_synth_util->sample_buffer, 1,
			    0,
			    ipatch_sample_frame_count, AGS_SOUNDCARD_DOUBLE);
  }else{
    AgsResampleUtil *resample_util;

    gpointer buffer;
    
    resample_util = sf2_synth_util->resample_util;

    ags_resample_util_set_destination(resample_util,
				      sf2_synth_util->sample_buffer);    

    buffer = ags_stream_alloc(ipatch_sample_frame_count,
			      AGS_SOUNDCARD_DOUBLE);
    
    ags_sound_resource_read(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    buffer, 1,
			    0,
			    ipatch_sample_frame_count, AGS_SOUNDCARD_DOUBLE);

    ags_resample_util_set_source(resample_util,
				 buffer);    

    ags_resample_util_set_samplerate(resample_util,
				     ipatch_sample_samplerate);

    ags_resample_util_set_target_samplerate(resample_util,
					    sf2_synth_util->samplerate);

    ags_resample_util_set_buffer_length(resample_util,
					ipatch_sample_frame_count);

    ags_resample_util_compute(resample_util);
  }
}

/**
 * ags_sf2_synth_util_load_midi_locale:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * @bank: the bank
 * @program: the program
 * 
 * Load midi locale of @sf2_synth_util.
 *
 * Since: 3.16.0
 */
void
ags_sf2_synth_util_load_midi_locale(AgsSF2SynthUtil *sf2_synth_util,
				    gint bank,
				    gint program)
{
  IpatchSF2 *sf2;
  IpatchSF2Preset *sf2_preset;
  IpatchSF2Inst *sf2_instrument;
  IpatchItem *pzone;
  IpatchItem *izone;

  IpatchList *pzone_list;
  IpatchList *izone_list;

  IpatchIter pzone_iter;
  IpatchIter izone_iter;

  guint i;
  
  GError *error;

  GRecMutex *audio_container_mutex;
  
  if(sf2_synth_util == NULL ||
     !AGS_IS_AUDIO_CONTAINER(sf2_synth_util->sf2_file) ||
     !AGS_IS_IPATCH(sf2_synth_util->sf2_file->sound_container)){
    return;
  }

  audio_container_mutex = AGS_AUDIO_CONTAINER_GET_OBJ_MUTEX(sf2_synth_util->sf2_file);

  g_rec_mutex_lock(audio_container_mutex);
  
  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) AGS_IPATCH(sf2_synth_util->sf2_file->sound_container)->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }

  for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
    if(sf2_synth_util->sf2_sample[i] != NULL){
      g_object_unref(sf2_synth_util->sf2_sample[i]);

      sf2_synth_util->sf2_sample[i] = NULL;
    }

    sf2_synth_util->sf2_note_range[i][0] = -1;
    sf2_synth_util->sf2_note_range[i][1] = -1;
    
    //TODO:JK: implement me
  }
  
  sf2_preset = ipatch_sf2_find_preset(sf2,
				      NULL,
				      bank,
				      program,
				      NULL);
  
  //  g_message("sf2 preset 0x%x", sf2_preset);

  if(sf2_preset == NULL){
    g_rec_mutex_unlock(audio_container_mutex);
    
    return;
  }
  
  pzone_list = ipatch_sf2_preset_get_zones(sf2_preset);

  i = 0;
  
  if(pzone_list != NULL){
    ipatch_list_init_iter(pzone_list, &pzone_iter);

    if(ipatch_iter_first(&pzone_iter) != NULL){
      do{
	IpatchSF2Sample *sf2_sample;
	IpatchSF2Sample *current;
	IpatchSF2Sample *lower;
	IpatchSF2Sample *higher;
	
	IpatchRange *note_range;

	gint current_root_note;
	gint lower_root_note;
	gint higher_root_note;
	gboolean success;
	
	pzone = ipatch_iter_get(&pzone_iter);

	note_range = NULL;
	g_object_get(pzone,
		     "note-range", &note_range,
		     NULL);

	sf2_instrument = (IpatchItem *) ipatch_sf2_pzone_get_inst(pzone);

	izone_list = ipatch_sf2_inst_get_zones(sf2_instrument);

	sf2_sample = NULL;
	
	current = NULL;
	lower = NULL;
	higher = NULL;

	current_root_note = -1;
	lower_root_note = -1;
	higher_root_note = -1;
		
	success = FALSE;
	
	if(izone_list != NULL){
	  ipatch_list_init_iter(izone_list, &izone_iter);

	  if(ipatch_iter_first(&izone_iter) != NULL){
	    do{
	      gint root_note;
	    
	      izone = ipatch_iter_get(&izone_iter);

	      sf2_sample = ipatch_sf2_izone_get_sample(izone);

	      if(IPATCH_IS_SF2_SAMPLE(sf2_sample)){
		g_object_get(sf2_sample,
			     "root-note", &root_note,
			     NULL);

		if(root_note >= note_range->low &&
		   root_note <= note_range->high){		
		  success = TRUE;

		  current = sf2_sample;
		  //		  g_object_ref(sf2_sample);
		  
		  i++;

		  break;
		}

		/* lower */
		if(root_note < note_range->low){
		  if(lower == NULL ||
		     root_note > lower_root_note){
		    if(lower != NULL){
		      g_object_unref(lower);
		    }
		    
		    lower = sf2_sample;
		    g_object_ref(sf2_sample);

		    lower_root_note = root_note;
		  }
		}

		/* higher */
		if(root_note > note_range->high){
		  if(higher == NULL ||
		     root_note < higher_root_note){
		    if(higher != NULL){
		      g_object_unref(higher);
		    }
		    
		    higher = sf2_sample;
		    g_object_ref(sf2_sample);

		    higher_root_note = root_note;
		  }
		}
	      }	      

	      if(sf2_sample != NULL){
		g_object_unref(sf2_sample);
	      }
	    }while(ipatch_iter_next(&izone_iter) != NULL && !success);

	    if(!success && lower != NULL){
	      success = TRUE;

	      sf2_sample = lower;
//	      g_object_ref(sf2_sample);
	      
	      i++;
	    }

	    if(!success && higher != NULL){
	      success = TRUE;
	      
	      sf2_sample = higher;
//	      g_object_ref(sf2_sample);
	    
	      i++;
	    }
	    
	    if(success && sf2_sample != NULL){
	      IpatchSampleData *sample_data;

	      gpointer buffer;
	      gpointer cache;
	      
	      guint sample_frame_count;
	      guint sample_format;
	      guint format;
	      guint orig_samplerate;
	      guint copy_mode;
	      
	      sf2_synth_util->sf2_sample[i] = sf2_sample;
	      g_object_ref(sf2_sample);

	      sf2_synth_util->sf2_note_range[i][0] = note_range->low;
	      sf2_synth_util->sf2_note_range[i][1] = note_range->high;

	      sample_data = ipatch_sf2_sample_get_data(sf2_sample);

	      sample_frame_count = 0;

	      format = AGS_SOUNDCARD_DOUBLE;
	      sample_format = ipatch_sample_get_format(sf2_sample);

	      g_object_get(sf2_sample,
			   "sample-size", &sample_frame_count,
			   "sample-rate", &orig_samplerate,
			   NULL);

	      cache = NULL;
	      copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							      AGS_AUDIO_BUFFER_UTIL_S16);
	      
	      switch(sample_format){
	      case IPATCH_SAMPLE_8BIT:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_SIGNED_8_BIT);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_S8);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_8BIT | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_16BIT:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_SIGNED_16_BIT);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_S16);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_24BIT:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_SIGNED_24_BIT);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_S24);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_24BIT | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_32BIT:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_SIGNED_32_BIT);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_S32);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_32BIT | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_FLOAT:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_FLOAT);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_FLOAT);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_FLOAT | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_DOUBLE:
	      {
		cache = ags_stream_alloc(sample_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

		copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
								AGS_AUDIO_BUFFER_UTIL_DOUBLE);

		error = NULL;
		ipatch_sample_read_transform(IPATCH_SAMPLE(sample_data),
					     0,
					     sample_frame_count,
					     cache,
					     IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
					     IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					     &error);
	      }
	      break;
	      case IPATCH_SAMPLE_REAL24BIT:
	      default:
		g_warning("unknown format");
	      }
	      
	      ags_stream_free(sf2_synth_util->sf2_orig_buffer[i]);
		
	      ags_stream_free(sf2_synth_util->sf2_resampled_buffer[i]);
		
	      sf2_synth_util->sf2_orig_buffer_length[i] = sample_frame_count;
	      buffer =
		sf2_synth_util->sf2_orig_buffer[i] = ags_stream_alloc(sf2_synth_util->sf2_orig_buffer_length[i],
								      format);
		
	      sf2_synth_util->sf2_resampled_buffer_length[i] = 0;
	      sf2_synth_util->sf2_resampled_buffer[i] = NULL;
		
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer, 1, 0,
							  cache, 1, 0,
							  sample_frame_count, copy_mode);

	      ags_stream_free(cache);
	      
	      if(sf2_synth_util->samplerate != orig_samplerate){
		AgsResampleUtil *resample_util;

		resample_util = sf2_synth_util->resample_util;

		sf2_synth_util->sf2_resampled_buffer_length[i] = floor(sf2_synth_util->samplerate / orig_samplerate) * sample_frame_count;
		sf2_synth_util->sf2_resampled_buffer[i] = ags_stream_alloc(sf2_synth_util->sf2_resampled_buffer_length[i],
									   format);
		  
		resample_util->destination = sf2_synth_util->sf2_resampled_buffer[i];
		resample_util->destination_stride = 1;
		  
		resample_util->source = sf2_synth_util->sf2_orig_buffer[i];
		resample_util->source_stride = 1;

		resample_util->buffer_length = sf2_synth_util->sf2_orig_buffer_length[i];
		resample_util->format = AGS_SOUNDCARD_DOUBLE;
		resample_util->samplerate = orig_samplerate;

		resample_util->audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE;

		resample_util->target_samplerate = sf2_synth_util->samplerate;

		ags_resample_util_compute(resample_util);
	      }
	    }
	  }
	}

	if(!success){
	  g_message("no sample found");
	  
	  if(sf2_synth_util->sf2_sample[i] != NULL){
	    g_object_unref(sf2_synth_util->sf2_sample[i]);

	    sf2_synth_util->sf2_sample[i] = NULL;
	  }
	  
	  sf2_synth_util->sf2_note_range[i][0] = -1;
	  sf2_synth_util->sf2_note_range[i][1] = -1;
	}
	
	if(current != NULL){
	  g_object_unref(current);
	}

	if(lower != NULL){
	  g_object_unref(lower);
	}

	if(higher != NULL){
	  g_object_unref(higher);
	}
      }while(ipatch_iter_next(&pzone_iter) != NULL);
    }
  }

  g_rec_mutex_unlock(audio_container_mutex);
  
  sf2_synth_util->sf2_sample_count = i;
}

/**
 * ags_sf2_synth_util_compute_s8:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_s8(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gint8 *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }
    
    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_SIGNED_8_BIT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_s8_to_s8(((gint8 *) sf2_synth_util->source) + i2, 1,
					      ((gint8 *) sf2_synth_util->im_buffer) + i1, 1,
					      copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_s8(((gint8 *) sf2_synth_util->source) + i2, 1,
					((gint8 *) sf2_synth_util->im_buffer) + i1, 1,
					copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_s16:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_s16(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gint16 *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_SIGNED_16_BIT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_s16_to_s16(((gint16 *) sf2_synth_util->source) + i2, 1,
						((gint16 *) sf2_synth_util->im_buffer) + i1, 1,
						copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_s16(((gint16 *) sf2_synth_util->source) + i2, 1,
					 ((gint16 *) sf2_synth_util->im_buffer) + i1, 1,
					 copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_s24:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_s24(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gint32 *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_SIGNED_24_BIT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_s24_to_s24(((gint32 *) sf2_synth_util->source) + i2, 1,
						((gint32 *) sf2_synth_util->im_buffer) + i1, 1,
						copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_s24(((gint32 *) sf2_synth_util->source) + i2, 1,
					 ((gint32 *) sf2_synth_util->im_buffer) + i1, 1,
					 copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_s32:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_s32(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gint32 *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_SIGNED_32_BIT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_s32_to_s32(((gint32 *) sf2_synth_util->source) + i2, 1,
						((gint32 *) sf2_synth_util->im_buffer) + i1, 1,
						copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_s32(((gint32 *) sf2_synth_util->source) + i2, 1,
					 ((gint32 *) sf2_synth_util->im_buffer) + i1, 1,
					 copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_s64:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_s64(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gint64 *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_SIGNED_64_BIT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_s64_to_s64(((gint64 *) sf2_synth_util->source) + i2, 1,
						((gint64 *) sf2_synth_util->im_buffer) + i1, 1,
						copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_s64(((gint64 *) sf2_synth_util->source) + i2, 1,
					 ((gint64 *) sf2_synth_util->im_buffer) + i1, 1,
					 copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_float:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_float(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gfloat *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_FLOAT);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_FLOAT);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_FLOAT);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_FLOAT);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_FLOAT);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_FLOAT);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_float_to_float(((gfloat *) sf2_synth_util->source) + i2, 1,
						    ((gfloat *) sf2_synth_util->im_buffer) + i1, 1,
						    copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_float(((gfloat *) sf2_synth_util->source) + i2, 1,
					   ((gfloat *) sf2_synth_util->im_buffer) + i1, 1,
					   copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_double:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_double(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    gdouble *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_DOUBLE);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_DOUBLE);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_DOUBLE);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_DOUBLE);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_DOUBLE);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_DOUBLE);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_double_to_double(((gdouble *) sf2_synth_util->source) + i2, 1,
						      ((gdouble *) sf2_synth_util->im_buffer) + i1, 1,
						      copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_double(((gdouble *) sf2_synth_util->source) + i2, 1,
					    ((gdouble *) sf2_synth_util->im_buffer) + i1, 1,
					    copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute_complex:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute_complex(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  if((AGS_SF2_SYNTH_UTIL_FX_ENGINE & (sf2_synth_util->flags)) != 0){
    IpatchSample *current_sf2_sample;
    gint *current_sf2_note_range;

    AgsHQPitchUtil *hq_pitch_util;
    AgsVolumeUtil *volume_util;    
    
    gpointer current_sample_buffer;
    gpointer sample_buffer;
    gpointer im_buffer;
    AgsComplex *source;
    
    guint buffer_length;
    guint samplerate, orig_samplerate;
    guint current_sample_buffer_length;
    
    gint midi_key;
    gdouble note;

    gdouble volume;

    guint frame_count;
    guint offset;

    guint loop_mode;
    gint loop_start;
    gint loop_end;

    guint nth_sample;
    gint position;
    guint copy_mode;
    gboolean pong_copy;
    gint root_note;
    guint i;
    guint j;
    
    if(sf2_synth_util->sf2_file == NULL){
      return;
    }

    hq_pitch_util = sf2_synth_util->hq_pitch_util;

    volume_util = sf2_synth_util->volume_util;

    source = sf2_synth_util->source;
    
    sample_buffer = sf2_synth_util->sample_buffer;
    im_buffer = sf2_synth_util->im_buffer;
    
    buffer_length = sf2_synth_util->buffer_length;
    samplerate = sf2_synth_util->samplerate;

    current_sf2_sample = NULL;
    current_sf2_note_range = NULL;

    midi_key = sf2_synth_util->midi_key;

    nth_sample = 0;
    
    for(i = 0; i < sf2_synth_util->sf2_sample_count && i < 128; i++){
      if(current_sf2_sample == NULL){
	current_sf2_sample = sf2_synth_util->sf2_sample[i];
	current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	nth_sample = i;
      }else{
	if(sf2_synth_util->sf2_note_range[i][0] <= midi_key &&
	   sf2_synth_util->sf2_note_range[i][1] >= midi_key){
	  current_sf2_sample = sf2_synth_util->sf2_sample[i];
	  current_sf2_note_range = sf2_synth_util->sf2_note_range[i];

	  nth_sample = i;
	  
	  break;
	}
      }
    }

    if(current_sf2_sample == NULL){
      return;
    }

    note = sf2_synth_util->note;
    
    volume = sf2_synth_util->volume;
    
    frame_count = sf2_synth_util->frame_count;
    offset = sf2_synth_util->offset;

    loop_mode = sf2_synth_util->loop_mode;
    loop_start = sf2_synth_util->loop_start;
    loop_end = sf2_synth_util->loop_end;

    /* fill buffer */
    orig_samplerate = samplerate;
    
    g_object_get(current_sf2_sample,
		 "sample-rate", &orig_samplerate,
		 NULL);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    pong_copy = FALSE;
	
    if(samplerate == orig_samplerate){
      current_sample_buffer_length = sf2_synth_util->sf2_orig_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_orig_buffer[nth_sample];
    }else{
      current_sample_buffer_length = sf2_synth_util->sf2_resampled_buffer_length[nth_sample];
      current_sample_buffer = sf2_synth_util->sf2_resampled_buffer[nth_sample];
    }
    
    for(i = 0, j = 0, position = 0; i < offset + buffer_length && j < buffer_length && position < current_sample_buffer_length;){
      gboolean incr_j;

      incr_j = FALSE;
      
      if(i > offset){
	incr_j = TRUE;

	((gdouble *) sample_buffer)[j] = ((gdouble *) current_sample_buffer)[position];
      }
      
      i++;

      if(incr_j){
	j++;
      }

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(!pong_copy){
	  if(offset > loop_end &&
	     position + 1 == loop_end){
	    pong_copy = TRUE;
	    
	    position--;
	  }else{
	    position++;
	  }
	}else{
	  if(offset > loop_end &&
	     position == loop_start){
	    pong_copy = FALSE;
	    
	    position++;  
	  }else{
	    position--;
	  }
	}	
      }else{      
	if(offset > loop_end &&
	   loop_end > 0 &&
	   loop_start < loop_end &&
	   position + 1 == loop_end){
	  position = loop_start;
	}else{
	  position++;
	}
      }
    }

    /* pitch */
    ags_hq_pitch_util_set_source(volume_util,
				 sample_buffer);

    ags_hq_pitch_util_set_destination(volume_util,
				      im_buffer);

    root_note = 60;
    g_object_get(current_sf2_sample,
		 "root-note", &root_note,
		 NULL);

    ags_hq_pitch_util_set_base_key(hq_pitch_util,
				   (gdouble) root_note - 21.0);
    
    ags_hq_pitch_util_set_tuning(hq_pitch_util,
				 100.0 * ((midi_key + note) - (root_note - 21.0)));

    ags_audio_buffer_util_clear_buffer(im_buffer, 1,
				       buffer_length, AGS_AUDIO_BUFFER_UTIL_DOUBLE);      
    
    if((double) midi_key - 48.0 + note == 0.0){
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
      ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
						  sample_buffer, 1, 0,
						  buffer_length, copy_mode);
    }else{
      gdouble root_pitch_hz;
      gdouble phase_incr;
      guint fluid_interp_method;

      root_pitch_hz = exp2(((double) root_note - 48.0) / 12.0) * 440.0;
  
      phase_incr = (exp2(((double) midi_key - 48.0 + note) / 12.0) * 440.0) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_fill_double(im_buffer,
						       sample_buffer,
						       buffer_length,
						       phase_incr);
      
//      ags_hq_pitch_util_pitch(hq_pitch_util);
    }
    
    /* volume */
    ags_volume_util_set_audio_buffer_util_format(volume_util,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_volume_util_set_source(volume_util,
			       im_buffer);

    ags_volume_util_set_source_stride(volume_util,
				      1);

    ags_volume_util_set_destination(volume_util,
				    im_buffer);

    ags_volume_util_set_destination_stride(volume_util,
					   1);

    ags_volume_util_set_buffer_length(volume_util,
				      buffer_length);
    
    ags_volume_util_set_volume(volume_util,
			       volume);

    ags_volume_util_compute(volume_util);

    /* to source */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source, 1, 0,
						im_buffer, 1, 0,
						buffer_length, copy_mode);
  }else{
    AgsGenericPitchUtil *generic_pitch_util;
  
    gint midi_key;
    gdouble base_key;
    gdouble tuning;
    guint source_frame_count;
    guint resampled_source_frame_count;
    guint source_samplerate;
    guint source_buffer_size;
    guint source_format;
    guint copy_mode;
    guint i0, i1, i2;
    gboolean success;
    gboolean pong_copy;

    if(!AGS_IS_IPATCH_SAMPLE(sf2_synth_util->ipatch_sample)){
      return;
    }
    
    generic_pitch_util = sf2_synth_util->generic_pitch_util;
  
    source_frame_count = 0;

    source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
    ags_sound_resource_info(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
			    &source_frame_count,
			    NULL, NULL);

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sf2_synth_util->ipatch_sample),
				   NULL,
				   &source_samplerate,
				   &source_buffer_size,
				   &source_format);

    resampled_source_frame_count = source_frame_count;

    if(source_samplerate != sf2_synth_util->samplerate){
      resampled_source_frame_count = (sf2_synth_util->samplerate / source_samplerate) * source_frame_count;
    }
  
    /*  */
    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						    AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    ags_audio_buffer_util_copy_buffer_to_buffer(sf2_synth_util->im_buffer, 1, 0,
						sf2_synth_util->sample_buffer, 1, 0,
						resampled_source_frame_count, copy_mode);

    /* pitch */
    midi_key = 60;

    //FIXME:JK: thread-safety
    if(sf2_synth_util->ipatch_sample->sample != NULL){
      gint tmp_midi_key;
    
      g_object_get(sf2_synth_util->ipatch_sample->sample,
		   "root-note", &tmp_midi_key,
		   NULL);

      if(tmp_midi_key >= 0 &&
	 tmp_midi_key < 128){
	midi_key = tmp_midi_key;
      }
    }
  
    base_key = (gdouble) midi_key - 21.0;

    tuning = 100.0 * ((sf2_synth_util->note + 48.0) - base_key);

    switch(generic_pitch_util->pitch_type){
    case AGS_FAST_PITCH:
    {
      ags_fast_pitch_util_set_format(sf2_synth_util->generic_pitch_util->fast_pitch_util,
				     AGS_SOUNDCARD_COMPLEX);

      ags_fast_pitch_util_set_base_key(generic_pitch_util->fast_pitch_util,
				       base_key);
    
      ags_fast_pitch_util_set_tuning(generic_pitch_util->fast_pitch_util,
				     tuning);
    }
    break;
    case AGS_HQ_PITCH:
    {
      ags_hq_pitch_util_set_format(generic_pitch_util->hq_pitch_util,
				   AGS_SOUNDCARD_COMPLEX);

      ags_hq_pitch_util_set_base_key(generic_pitch_util->hq_pitch_util,
				     base_key);
    
      ags_hq_pitch_util_set_tuning(generic_pitch_util->hq_pitch_util,
				   tuning);
    }
    break;
    case AGS_FLUID_NO_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_none_util_set_format(generic_pitch_util->fluid_interpolate_none_util,
						 AGS_SOUNDCARD_COMPLEX);

      ags_fluid_interpolate_none_util_set_phase_increment(generic_pitch_util->fluid_interpolate_none_util,
							  phase_incr);
    }
    break;
    case AGS_FLUID_LINEAR_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_linear_util_set_format(generic_pitch_util->fluid_interpolate_linear_util,
						   AGS_SOUNDCARD_COMPLEX);

      ags_fluid_interpolate_linear_util_set_phase_increment(generic_pitch_util->fluid_interpolate_linear_util,
							    phase_incr);
    }
    break;
    case AGS_FLUID_4TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_4th_order_util_set_format(generic_pitch_util->fluid_interpolate_4th_order_util,
						      AGS_SOUNDCARD_COMPLEX);

      ags_fluid_interpolate_4th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_4th_order_util,
							       phase_incr);
    }
    break;
    case AGS_FLUID_7TH_ORDER_INTERPOLATE:
    {
      gdouble root_pitch_hz;
      gdouble phase_incr;

      root_pitch_hz = ags_fluid_ct2hz(100.0 * base_key);
  
      phase_incr = ags_fluid_ct2hz(tuning) / root_pitch_hz;

      if(phase_incr == 0.0){
	phase_incr = 1.0;
      }

      ags_fluid_interpolate_7th_order_util_set_format(generic_pitch_util->fluid_interpolate_7th_order_util,
						      AGS_SOUNDCARD_COMPLEX);

      ags_fluid_interpolate_7th_order_util_set_phase_increment(generic_pitch_util->fluid_interpolate_7th_order_util,
							       phase_incr);
    }
    break;
    }
  
    ags_generic_pitch_util_pitch(generic_pitch_util);

    success = FALSE;
    pong_copy = FALSE;
  
    for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2_synth_util->frame_count && !success && i2 < sf2_synth_util->buffer_length; ){
      guint copy_n_frames;
      guint start_frame;

      gboolean set_loop_start;
      gboolean set_loop_end;
      gboolean do_copy;
    
      copy_n_frames = sf2_synth_util->buffer_length;

      set_loop_start = FALSE;
      set_loop_end = FALSE;

      do_copy = FALSE;
    
      if(i0 + copy_n_frames > sf2_synth_util->frame_count){
	copy_n_frames = sf2_synth_util->frame_count - i0;
      }
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	    copy_n_frames = sf2_synth_util->loop_end - i1;
	  
	    set_loop_start = TRUE;
	  }
	}else{
	  if(!pong_copy){
	    if(i1 + copy_n_frames >= sf2_synth_util->loop_end){
	      copy_n_frames = sf2_synth_util->loop_end - i1;

	      set_loop_end = TRUE;
	    }
	  }else{
	    if(i1 - copy_n_frames <= sf2_synth_util->loop_start){
	      copy_n_frames = i1 - sf2_synth_util->loop_start;
	    
	      set_loop_start = TRUE;
	    }
	  }
	}
      }else{
	/* can't loop */
	if(i1 + copy_n_frames > sf2_synth_util->frame_count){
	  copy_n_frames = sf2_synth_util->frame_count - i1;

	  success = TRUE;
	}
      }

      start_frame = 0;
      
      if(i0 + copy_n_frames > sf2_synth_util->offset){
	do_copy = TRUE;
      
	if(i0 < sf2_synth_util->offset){
	  start_frame = (i0 + copy_n_frames) - sf2_synth_util->offset;
	}
      
	if(!pong_copy){	
	  ags_audio_buffer_util_copy_complex_to_complex(((AgsComplex *) sf2_synth_util->source) + i2, 1,
							((AgsComplex *) sf2_synth_util->im_buffer) + i1, 1,
							copy_n_frames - start_frame);
	}else{
	  ags_audio_buffer_util_pong_complex(((AgsComplex *) sf2_synth_util->source) + i2, 1,
					     ((AgsComplex *) sf2_synth_util->im_buffer) + i1, 1,
					     copy_n_frames - start_frame);
	}
      }

      i0 += copy_n_frames;
    
      if((sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	  sf2_synth_util->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
	 sf2_synth_util->loop_start >= 0 &&
	 sf2_synth_util->loop_end >= 0 &&
	 sf2_synth_util->loop_start < sf2_synth_util->loop_end){
	/* can loop */
	if(sf2_synth_util->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	  if(set_loop_start){
	    i1 = sf2_synth_util->loop_start;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(!pong_copy){
	    if(set_loop_end){
	      i1 = sf2_synth_util->loop_end; 
	    
	      pong_copy = TRUE;
	    }else{
	      i1 += copy_n_frames;
	    }
	  }else{
	    if(set_loop_start){
	      i1 = sf2_synth_util->loop_start;
	    
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
  }
}

/**
 * ags_sf2_synth_util_compute:
 * @sf2_synth_util: the #AgsSF2SynthUtil-struct
 * 
 * Compute Soundfont2 synth.
 * 
 * Since: 3.9.6
 */
void
ags_sf2_synth_util_compute(AgsSF2SynthUtil *sf2_synth_util)
{
  if(sf2_synth_util == NULL ||
     sf2_synth_util->source == NULL){
    return;
  }

  switch(sf2_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_sf2_synth_util_compute_s8(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_sf2_synth_util_compute_s16(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_sf2_synth_util_compute_s24(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_sf2_synth_util_compute_s32(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_sf2_synth_util_compute_s64(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_sf2_synth_util_compute_float(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_sf2_synth_util_compute_double(sf2_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_sf2_synth_util_compute_complex(sf2_synth_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}

/**
 * ags_sf2_synth_util_midi_locale_find_sample_near_midi_key:
 * @ipatch: the #AgsIpatch
 * @bank: the bank
 * @program: the program
 * @midi_key: the MIDI key
 * @preset: (out): the preset
 * @instrument: (out): the instrument
 * @sample: (out): the sample
 * 
 * Find sample near MIDI key.
 * 
 * Since: 3.4.0
 */
AgsIpatchSample*
ags_sf2_synth_util_midi_locale_find_sample_near_midi_key(AgsIpatch *ipatch,
							 gint bank,
							 gint program,
							 gint midi_key,
							 gchar **preset,
							 gchar **instrument,
							 gchar **sample)
{
  AgsIpatchSample *ipatch_sample;
  
  IpatchSF2 *sf2;
  IpatchSF2Preset *sf2_preset;
  IpatchSF2Inst *sf2_instrument;
  IpatchSF2Sample *sf2_sample, *first_sf2_sample;
  IpatchSF2Sample *matching_sf2_sample;
  IpatchItem *pzone;
  IpatchItem *izone;

  IpatchList *pzone_list;
  IpatchList *izone_list;

  IpatchIter pzone_iter;
  IpatchIter izone_iter;

  gchar *matching_preset;
  gchar *matching_instrument;
  gchar *matching_sample;
  
  GError *error;

  if(!AGS_IS_IPATCH(ipatch)){
    return(NULL);
  }

  ipatch_sample = NULL;
  matching_sf2_sample = NULL;

  matching_preset = NULL;
  matching_instrument = NULL;
  matching_sample = NULL;
  
  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }
  
  sf2_preset = ipatch_sf2_find_preset(sf2,
				      NULL,
				      bank,
				      program,
				      NULL);

  if(sf2_preset != NULL){
    if(preset != NULL){
      matching_preset = ipatch_sf2_preset_get_name(sf2_preset);
    }
    
    pzone_list = ipatch_sf2_preset_get_zones(sf2_preset);

    ipatch_list_init_iter(pzone_list, &pzone_iter);
    
    if(ipatch_iter_first(&pzone_iter) != NULL){
      do{
	IpatchRange *note_range;
	
	pzone = ipatch_iter_get(&pzone_iter);

	g_object_get(pzone,
		     "note-range", &note_range,
		     NULL);

	first_sf2_sample = NULL;
	
	if(note_range->low <= midi_key &&
	   note_range->high >= midi_key){
	  sf2_instrument = (IpatchItem *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&pzone_iter));

	  if(instrument != NULL){
	    matching_instrument = ipatch_sf2_inst_get_name(sf2_instrument);
	  }
	  
	  izone_list = ipatch_sf2_inst_get_zones(sf2_instrument);

	  if(izone_list != NULL){
	    ipatch_list_init_iter(izone_list, &izone_iter);

	    if(ipatch_iter_first(&izone_iter) != NULL){      
	      izone = ipatch_iter_get(&izone_iter);

	      first_sf2_sample = ipatch_sf2_izone_get_sample(izone);
	      
	      do{
		gint root_note;
		
		izone = ipatch_iter_get(&izone_iter);

		sf2_sample = ipatch_sf2_izone_get_sample(izone);
 
		g_object_get(sf2_sample,
			     "root-note", &root_note,
			     NULL);
		
		if(root_note == 60){
		  matching_sf2_sample = sf2_sample;

		  break;
		}
	      }while(ipatch_iter_next(&izone_iter) != NULL);
	    }
	  }

	  if(matching_sf2_sample == NULL){
	    matching_sf2_sample = first_sf2_sample;
	  }
	  
	  break;
	}
      }while(ipatch_iter_next(&pzone_iter) != NULL);
    }
  }

  if(matching_sf2_sample != NULL){
    GList *start_list, *list;

    if(sample != NULL){
      matching_sample = ipatch_sf2_sample_get_name(matching_sf2_sample);
    }
    
    list =
      start_list = ags_sound_container_get_resource_all(AGS_SOUND_CONTAINER(ipatch));

    while(list != NULL){
      if(AGS_IPATCH_SAMPLE(list->data)->sample == matching_sf2_sample){
	ipatch_sample = list->data;

	break;
      }
      
      list = list->next;
    }
  }

  if(preset != NULL){
    preset[0] = matching_preset;
  }

  if(instrument != NULL){
    instrument[0] = matching_instrument;
  }

  if(sample != NULL){
    sample[0] = matching_sample;
  }
  
  return(ipatch_sample);
}

/**
 * ags_sf2_synth_util_copy_s8:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s8(gint8 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample,
			   gdouble note,
			   gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames,
			   guint loop_mode,
			   gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint8 *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_s16:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s16(gint16 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint16 *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;
  
  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);
    
    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_s24:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s24(gint32 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_s32:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s32(gint32 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_s64:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s64(gint64 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint64 *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_float:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_float(gfloat *buffer,
			      guint buffer_size,
			      AgsIpatchSample *ipatch_sample,
			      gdouble note,
			      gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint loop_mode,
			      gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gfloat *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_double:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_double(gdouble *buffer,
			       guint buffer_size,
			       AgsIpatchSample *ipatch_sample,
			       gdouble note,
			       gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint loop_mode,
			       gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gdouble *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy_complex:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_complex(AgsComplex *buffer,
				guint buffer_size,
				AgsIpatchSample *ipatch_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint loop_mode,
				gint loop_start, gint loop_end)
{
  void *sample_buffer;

  AgsComplex *im_buffer;

  gint midi_key;
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
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
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
  midi_key = 60;

  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
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
 * ags_sf2_synth_util_copy:
 * @buffer: the audio buffer
 * @buffer_size: the buffer size
 * @ipatch_sample: the #AgsIpatchSample
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
ags_sf2_synth_util_copy(void *buffer,
			guint buffer_size,
			AgsIpatchSample *ipatch_sample,
			gdouble note,
			gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames,
			guint loop_mode,
			gint loop_start, gint loop_end)
{
  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_sf2_synth_util_copy_s8((gint8 *) buffer,
			       buffer_size,
			       ipatch_sample,
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
    ags_sf2_synth_util_copy_s16((gint16 *) buffer,
				buffer_size,
				ipatch_sample,
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
    ags_sf2_synth_util_copy_s24((gint32 *) buffer,
				buffer_size,
				ipatch_sample,
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
    ags_sf2_synth_util_copy_s32((gint32 *) buffer,
				buffer_size,
				ipatch_sample,
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
    ags_sf2_synth_util_copy_s64((gint64 *) buffer,
				buffer_size,
				ipatch_sample,
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
    ags_sf2_synth_util_copy_float((float *) buffer,
				  buffer_size,
				  ipatch_sample,
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
    ags_sf2_synth_util_copy_double((double *) buffer,
				   buffer_size,
				   ipatch_sample,
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
    ags_sf2_synth_util_copy_complex((AgsComplex *) buffer,
				    buffer_size,
				    ipatch_sample,
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
    g_warning("ags_sf2_synth_util_copy() - unsupported format");
  }
  }
}
