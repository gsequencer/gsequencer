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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_synth_util_test_init_suite();
int ags_synth_util_test_clean_suite();

void ags_synth_util_test_sin_s8();
void ags_synth_util_test_sin_s16();
void ags_synth_util_test_sin_s24();
void ags_synth_util_test_sin_s32();
void ags_synth_util_test_sin_s64();
void ags_synth_util_test_sin_float();
void ags_synth_util_test_sin_double();
void ags_synth_util_test_sin_complex();

void ags_synth_util_test_sawtooth_s8();
void ags_synth_util_test_sawtooth_s16();
void ags_synth_util_test_sawtooth_s24();
void ags_synth_util_test_sawtooth_s32();
void ags_synth_util_test_sawtooth_s64();
void ags_synth_util_test_sawtooth_float();
void ags_synth_util_test_sawtooth_double();
void ags_synth_util_test_sawtooth_complex();

void ags_synth_util_test_triangle_s8();
void ags_synth_util_test_triangle_s16();
void ags_synth_util_test_triangle_s24();
void ags_synth_util_test_triangle_s32();
void ags_synth_util_test_triangle_s64();
void ags_synth_util_test_triangle_float();
void ags_synth_util_test_triangle_double();
void ags_synth_util_test_triangle_complex();

void ags_synth_util_test_square_s8();
void ags_synth_util_test_square_s16();
void ags_synth_util_test_square_s24();
void ags_synth_util_test_square_s32();
void ags_synth_util_test_square_s64();
void ags_synth_util_test_square_float();
void ags_synth_util_test_square_double();
void ags_synth_util_test_square_complex();

void ags_synth_util_test_impulse_s8();
void ags_synth_util_test_impulse_s16();
void ags_synth_util_test_impulse_s24();
void ags_synth_util_test_impulse_s32();
void ags_synth_util_test_impulse_s64();
void ags_synth_util_test_impulse_float();
void ags_synth_util_test_impulse_double();
void ags_synth_util_test_impulse_complex();

#define AGS_SYNTH_UTIL_TEST_PHASE (0.0)
#define AGS_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_SYNTH_UTIL_TEST_FRAME_COUNT (1024)

gdouble test_freq[] = {
  13289.75,
  7040.00,
  880.0,
  440.0,
  220.0,
  27.5,
  8.18,
  -1.0,
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_util_test_clean_suite()
{
  return(0);
}

void
ags_synth_util_test_sin_s8()
{
  gint8 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S8);
    
    ags_synth_util_sin_s8(buffer,
			  test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			  AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			  AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S8,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_s16()
{
  gint16 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S16);
    
    ags_synth_util_sin_s16(buffer,
			   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S16,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_s24()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S24);
    
    ags_synth_util_sin_s24(buffer,
			   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S24,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_s32()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S32);
    
    ags_synth_util_sin_s32(buffer,
			   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S32,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_s64()
{
  gint64 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S64);
    
    ags_synth_util_sin_s64(buffer,
			   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S64,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_float()
{
  gfloat *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    
    ags_synth_util_sin_float(buffer,
			     test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			     AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			     AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_FLOAT,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_double()
{
  gdouble *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_synth_util_sin_double(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sin_complex()
{
  AgsComplex *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = FALSE;
  
  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_COMPLEX);
    
    ags_synth_util_sin_complex(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_s8()
{
  gint8 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S8);
    
    ags_synth_util_sawtooth_s8(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S8,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_s16()
{
  gint16 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S16);
    
    ags_synth_util_sawtooth_s16(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S16,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
  //TODO:JK: implement me  
}

void
ags_synth_util_test_sawtooth_s24()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S24);
    
    ags_synth_util_sawtooth_s24(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S24,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_s32()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S32);
    
    ags_synth_util_sawtooth_s32(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S32,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_s64()
{
  gint64 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S64);
    
    ags_synth_util_sawtooth_s64(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S64,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_float()
{
  gfloat *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    
    ags_synth_util_sawtooth_float(buffer,
				  test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_FLOAT,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_double()
{
  gdouble *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_synth_util_sawtooth_double(buffer,
				   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_sawtooth_complex()
{
  AgsComplex *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_COMPLEX);
    
    ags_synth_util_sawtooth_complex(buffer,
				    test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				    AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_s8()
{
  gint8 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S8);
    
    ags_synth_util_triangle_s8(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S8,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_s16()
{
  gint16 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S16);
    
    ags_synth_util_triangle_s16(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S16,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_s24()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S24);
    
    ags_synth_util_triangle_s24(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S24,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_s32()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S32);
    
    ags_synth_util_triangle_s32(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S32,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_s64()
{
  gint64 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S64);
    
    ags_synth_util_triangle_s64(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S64,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_float()
{
  gfloat *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    
    ags_synth_util_triangle_float(buffer,
				  test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_FLOAT,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_double()
{
  gdouble *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_synth_util_triangle_double(buffer,
				   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_triangle_complex()
{
  AgsComplex *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_COMPLEX);
    
    ags_synth_util_triangle_complex(buffer,
				    test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				    AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_s8()
{
  gint8 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S8);
    
    ags_synth_util_square_s8(buffer,
			     test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			     AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			     AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S8,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_s16()
{
  gint16 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S16);
    
    ags_synth_util_square_s16(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S16,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_s24()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S24);
    
    ags_synth_util_square_s24(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S24,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_s32()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S32);
    
    ags_synth_util_square_s32(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S32,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_s64()
{
  gint64 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S64);
    
    ags_synth_util_square_s64(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S64,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_float()
{
  gfloat *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    
    ags_synth_util_square_float(buffer,
				test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_FLOAT,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_double()
{
  gdouble *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_synth_util_square_double(buffer,
				 test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				 AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_square_complex()
{
  AgsComplex *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_COMPLEX);
    
    ags_synth_util_square_complex(buffer,
				  test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_s8()
{
  gint8 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S8);
    
    ags_synth_util_impulse_s8(buffer,
			      test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			      AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S8,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_s16()
{
  gint16 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S16);
    
    ags_synth_util_impulse_s16(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S16,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_s24()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S24);
    
    ags_synth_util_impulse_s24(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S24,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_s32()
{
  gint32 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S32);
    
    ags_synth_util_impulse_s32(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S32,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_s64()
{
  gint64 *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_S64);
    
    ags_synth_util_impulse_s64(buffer,
			       test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
			       AGS_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_S64,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_float()
{
  gfloat *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    
    ags_synth_util_impulse_float(buffer,
				 test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				 AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_FLOAT,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_double()
{
  gdouble *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    ags_synth_util_impulse_double(buffer,
				  test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				  AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_synth_util_test_impulse_complex()
{
  AgsComplex *buffer;

  guint xcross_count;
  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  success = FALSE;

  for(i = 0; test_freq[i] != -1.0; i++){
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       AGS_SYNTH_UTIL_TEST_FRAME_COUNT, AGS_AUDIO_BUFFER_UTIL_COMPLEX);
    
    ags_synth_util_impulse_complex(buffer,
				   test_freq[i], AGS_SYNTH_UTIL_TEST_PHASE, AGS_SYNTH_UTIL_TEST_VOLUME,
				   AGS_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_SYNTH_UTIL_TEST_OFFSET, AGS_SYNTH_UTIL_TEST_FRAME_COUNT);
    
    xcross_count = ags_synth_util_get_xcross_count(buffer,
						   AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						   AGS_SYNTH_UTIL_TEST_FRAME_COUNT);

    success = (xcross_count + 2 > ((gdouble) AGS_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_SYNTH_UTIL_TEST_SAMPLERATE) * test_freq[i]) ? TRUE: FALSE;
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
  pSuite = CU_add_suite("AgsSynthUtilTest", ags_synth_util_test_init_suite, ags_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_synth_util.c sin s8", ags_synth_util_test_sin_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin s16", ags_synth_util_test_sin_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin s24", ags_synth_util_test_sin_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin s32", ags_synth_util_test_sin_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin s64", ags_synth_util_test_sin_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin float", ags_synth_util_test_sin_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin double", ags_synth_util_test_sin_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sin complex", ags_synth_util_test_sin_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth s8", ags_synth_util_test_sawtooth_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth s16", ags_synth_util_test_sawtooth_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth s24", ags_synth_util_test_sawtooth_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth s32", ags_synth_util_test_sawtooth_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth s64", ags_synth_util_test_sawtooth_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth float", ags_synth_util_test_sawtooth_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth double", ags_synth_util_test_sawtooth_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c sawtooth complex", ags_synth_util_test_sawtooth_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle s8", ags_synth_util_test_triangle_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle s16", ags_synth_util_test_triangle_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle s24", ags_synth_util_test_triangle_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle s32", ags_synth_util_test_triangle_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle s64", ags_synth_util_test_triangle_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle float", ags_synth_util_test_triangle_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle double", ags_synth_util_test_triangle_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c triangle complex", ags_synth_util_test_triangle_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square s8", ags_synth_util_test_square_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square s16", ags_synth_util_test_square_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square s24", ags_synth_util_test_square_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square s32", ags_synth_util_test_square_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square s64", ags_synth_util_test_square_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square float", ags_synth_util_test_square_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square double", ags_synth_util_test_square_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c square complex", ags_synth_util_test_square_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse s16", ags_synth_util_test_impulse_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse s24", ags_synth_util_test_impulse_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse s32", ags_synth_util_test_impulse_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse s64", ags_synth_util_test_impulse_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse float", ags_synth_util_test_impulse_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse double", ags_synth_util_test_impulse_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c impulse complex", ags_synth_util_test_impulse_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
