/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2020 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_sf2_synth_util_test_init_suite();
int ags_sf2_synth_util_test_clean_suite();

void ags_sf2_synth_util_test_copy_s8();
void ags_sf2_synth_util_test_copy_s16();
void ags_sf2_synth_util_test_copy_s24();
void ags_sf2_synth_util_test_copy_s32();
void ags_sf2_synth_util_test_copy_s64();
void ags_sf2_synth_util_test_copy_float();
void ags_sf2_synth_util_test_copy_double();
void ags_sf2_synth_util_test_copy_complex();

gboolean ags_sf2_synth_util_test_stub_info(AgsSoundResource *sound_resource,
					   guint *frame_count,
					   guint *loop_start, guint *loop_end);
void ags_sf2_synth_util_test_stub_get_presets(AgsSoundResource *sound_resource,
					      guint *channels,
					      guint *samplerate,
					      guint *buffer_size,
					      guint *format);

guint ags_sf2_synth_util_test_stub_read(AgsSoundResource *sound_resource,
					void *dbuffer, guint daudio_channels,
					guint audio_channel,
					guint frame_count, guint format);

#define AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE (1024)
#define AGS_SF2_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SF2_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT (1024)
#define AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE (AGS_SF2_SYNTH_UTIL_LOOP_NONE)
#define AGS_SF2_SYNTH_UTIL_TEST_LOOP_START (0)
#define AGS_SF2_SYNTH_UTIL_TEST_LOOP_END (0)

#define AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT (128)
#define AGS_SF2_SYNTH_UTIL_TEST_LOWER (-70.0)
#define AGS_SF2_SYNTH_UTIL_TEST_UPPER (57.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sf2_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sf2_synth_util_test_clean_suite()
{
  return(0);
}

gboolean
ags_sf2_synth_util_test_stub_info(AgsSoundResource *sound_resource,
				  guint *frame_count,
				  guint *loop_start, guint *loop_end)
{
  AgsIpatchSample *sf2_sample;

  sf2_sample = AGS_IPATCH_SAMPLE(sound_resource);

  if(frame_count != NULL){
    *frame_count = AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT;
  }
  
  if(loop_start != NULL){
    *loop_start = 0;
  }

  if(loop_end != NULL){
    *loop_end = 0;
  }

  return(TRUE);
}

void
ags_sf2_synth_util_test_stub_get_presets(AgsSoundResource *sound_resource,
					 guint *channels,
					 guint *samplerate,
					 guint *buffer_size,
					 guint *format)
{
  AgsIpatchSample *sf2_sample;

  sf2_sample = AGS_IPATCH_SAMPLE(sound_resource);

  if(channels != NULL){
    *channels = sf2_sample->audio_channels;
  }
  
  if(samplerate != NULL){
    *samplerate = AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE;
  }

  if(buffer_size != NULL){
    *buffer_size = sf2_sample->buffer_size;
  }

  if(format != NULL){
    *format = sf2_sample->format;
  }
}

guint
ags_sf2_synth_util_test_stub_read(AgsSoundResource *sound_resource,
				  void *dbuffer, guint daudio_channels,
				  guint audio_channel,
				  guint frame_count, guint format)
{
  AgsIpatchSample *sf2_sample;

  guint copy_mode;
  guint read_count;
  
  sf2_sample = AGS_IPATCH_SAMPLE(sound_resource);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sf2_sample->format));

  read_count = sf2_sample->buffer_size;

  ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, 0,
					      sf2_sample->full_buffer, sf2_sample->audio_channels, audio_channel,
					      read_count, copy_mode);
  
  return(read_count);
}

void
ags_sf2_synth_util_test_copy_s8()
{
  AgsIpatchSample *sf2_sample;
  
  gint8 *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_s8(buffer,
			       AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
			       sf2_sample,
			       AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
			       AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
			       AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
			       AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_s16()
{
  AgsIpatchSample *sf2_sample;
  
  gint16 *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_s16(buffer,
				AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sf2_sample,
				AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_s24()
{
  AgsIpatchSample *sf2_sample;
  
  gint32 *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_s24(buffer,
				AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sf2_sample,
				AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_s32()
{
  AgsIpatchSample *sf2_sample;
  
  gint32 *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_s32(buffer,
				AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sf2_sample,
				AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_s64()
{
  AgsIpatchSample *sf2_sample;
  
  gint64 *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_s64(buffer,
				AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sf2_sample,
				AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_float()
{
  AgsIpatchSample *sf2_sample;
  
  gfloat *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_float(buffer,
				  AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				  sf2_sample,
				  AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				  AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				  AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				  AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				  AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_double()
{
  AgsIpatchSample *sf2_sample;
  
  gdouble *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_double(buffer,
				   AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				   sf2_sample,
				   AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				   AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				   AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				   AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				   AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sf2_synth_util_test_copy_complex()
{
  AgsIpatchSample *sf2_sample;
  
  AgsComplex *buffer;

  guint i;
  gboolean success;
  
  sf2_sample = ags_ipatch_sample_new();
  
  sf2_sample->audio_channels = 1;

  sf2_sample->buffer_size = AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sf2_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sf2_sample->offset = 0;
  sf2_sample->buffer_offset = 0;

  sf2_sample->full_buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sf2_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sf2_sample->sample = ipatch_sf2_sample_new();

  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->info = ags_sf2_synth_util_test_stub_info;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->get_presets = ags_sf2_synth_util_test_stub_get_presets;
  AGS_SOUND_RESOURCE_GET_INTERFACE(sf2_sample)->read = ags_sf2_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = TRUE;

  for(i = 0; i < AGS_SF2_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sf2_synth_util_copy_complex(buffer,
				    AGS_SF2_SYNTH_UTIL_TEST_BUFFER_SIZE,
				    sf2_sample,
				    AGS_SF2_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				    AGS_SF2_SYNTH_UTIL_TEST_VOLUME,
				    AGS_SF2_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_SF2_SYNTH_UTIL_TEST_OFFSET,
				    AGS_SF2_SYNTH_UTIL_TEST_FRAME_COUNT,
				    AGS_SF2_SYNTH_UTIL_TEST_LOOP_MODE,
				    AGS_SF2_SYNTH_UTIL_TEST_LOOP_START, AGS_SF2_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSF2SynthUtilTest", ags_sf2_synth_util_test_init_suite, ags_sf2_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy s8", ags_sf2_synth_util_test_copy_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy s16", ags_sf2_synth_util_test_copy_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy s24", ags_sf2_synth_util_test_copy_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy s32", ags_sf2_synth_util_test_copy_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy s64", ags_sf2_synth_util_test_copy_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy float", ags_sf2_synth_util_test_copy_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy double", ags_sf2_synth_util_test_copy_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sf2_synth_util.c copy complex", ags_sf2_synth_util_test_copy_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

