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

int ags_sfz_synth_util_test_init_suite();
int ags_sfz_synth_util_test_clean_suite();

void ags_sfz_synth_util_test_copy_s8();
void ags_sfz_synth_util_test_copy_s16();
void ags_sfz_synth_util_test_copy_s24();
void ags_sfz_synth_util_test_copy_s32();
void ags_sfz_synth_util_test_copy_s64();
void ags_sfz_synth_util_test_copy_float();
void ags_sfz_synth_util_test_copy_double();
void ags_sfz_synth_util_test_copy_complex();

guint ags_sfz_synth_util_test_stub_read(AgsSoundResource *sound_resource,
					void *dbuffer, guint daudio_channels,
					guint audio_channel,
					guint frame_count, guint format);

#define AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE (1024)
#define AGS_SFZ_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SFZ_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT (1024)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE (AGS_SFZ_SYNTH_UTIL_LOOP_NONE)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START (0)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END (0)

#define AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT (128)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOWER (-70.0)
#define AGS_SFZ_SYNTH_UTIL_TEST_UPPER (57.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sfz_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sfz_synth_util_test_clean_suite()
{
  return(0);
}

guint
ags_sfz_synth_util_test_stub_read(AgsSoundResource *sound_resource,
				  void *dbuffer, guint daudio_channels,
				  guint audio_channel,
				  guint frame_count, guint format)
{
  AgsSFZSample *sfz_sample;

  guint copy_mode;
  guint read_count;
  
  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sfz_sample->format));

  read_count = sfz_sample->buffer_size;

  ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, 0,
					      sfz_sample->full_buffer, sfz_sample->info->channels, audio_channel,
					      read_count, copy_mode);
  
  return(read_count);
}

void
ags_sfz_synth_util_test_copy_s8()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gint8 *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_s8(buffer,
			       AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
			       sfz_sample,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
			       AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
			       AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_s16()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gint16 *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_s16(buffer,
			       AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
			       sfz_sample,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
			       AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
			       AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_s24()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gint32 *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_s24(buffer,
				AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sfz_sample,
				AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
				AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
				AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_s32()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gint32 *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_s32(buffer,
			       AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
			       sfz_sample,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
			       AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
			       AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
			       AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_s64()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gint64 *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_s64(buffer,
				AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
				sfz_sample,
				AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
				AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
				AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
				AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_float()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gfloat *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_float(buffer,
				  AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
				  sfz_sample,
				  AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				  AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
				  AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
				  AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
				  AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_double()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  gdouble *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_double(buffer,
				   AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
				   sfz_sample,
				   AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				   AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
				   AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
				   AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
				   AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_sfz_synth_util_test_copy_complex()
{
  AgsSFZRegion *sfz_region;
  AgsSFZSample *sfz_sample;
  
  AgsComplex *buffer;

  guint i;
  gboolean success;

  sfz_region = ags_sfz_region_new();

  ags_sfz_region_insert_control(sfz_region,
				"key", "60");
  
  sfz_sample = ags_sfz_sample_new();

  sfz_sample->region = sfz_region;
  
  sfz_sample->audio_channels = 1;

  sfz_sample->buffer_size = AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE;
  sfz_sample->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;
  
  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
					     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ags_synth_util_sin(sfz_sample->full_buffer,
		     440.0, 0.0, 1.0,
		     44100, ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DEFAULT_FORMAT),
		     0, AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT);  

  sfz_sample->info = (SF_INFO *) g_malloc(sizeof(SF_INFO)); 

  sfz_sample->info->format = SF_FORMAT_PCM_16;
  sfz_sample->info->channels = 1;
  sfz_sample->info->samplerate = 44100;

  sfz_sample->info->frames = AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT;

  AGS_SOUND_RESOURCE_GET_INTERFACE(sfz_sample)->read = ags_sfz_synth_util_test_stub_read;
  
  buffer = ags_stream_alloc(AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = TRUE;

  for(i = 0; i < AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT; i++){
    ags_sfz_synth_util_copy_complex(buffer,
				    AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE,
				    sfz_sample,
				    AGS_SFZ_SYNTH_UTIL_TEST_LOWER + (gdouble) i,
				    AGS_SFZ_SYNTH_UTIL_TEST_VOLUME,
				    AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_SFZ_SYNTH_UTIL_TEST_OFFSET,
				    AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT,
				    AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE,
				    AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START, AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END);
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
  pSuite = CU_add_suite("AgsSFZSynthUtilTest", ags_sfz_synth_util_test_init_suite, ags_sfz_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy s8", ags_sfz_synth_util_test_copy_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy s16", ags_sfz_synth_util_test_copy_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy s24", ags_sfz_synth_util_test_copy_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy s32", ags_sfz_synth_util_test_copy_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy s64", ags_sfz_synth_util_test_copy_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy float", ags_sfz_synth_util_test_copy_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy double", ags_sfz_synth_util_test_copy_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_sfz_synth_util.c copy complex", ags_sfz_synth_util_test_copy_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
