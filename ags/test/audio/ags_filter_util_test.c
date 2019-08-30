/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2019 Joël Krähemann
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

int ags_filter_util_test_init_suite();
int ags_filter_util_test_clean_suite();

void ags_filter_util_test_pitch_s8();
void ags_filter_util_test_pitch_s16();
void ags_filter_util_test_pitch_s24();
void ags_filter_util_test_pitch_s32();
void ags_filter_util_test_pitch_s64();
void ags_filter_util_test_pitch_float();
void ags_filter_util_test_pitch_double();
void ags_filter_util_test_pitch_complex();

#define AGS_FILTER_UTIL_TEST_FREQ (440.0)
#define AGS_FILTER_UTIL_TEST_PHASE (0.0)
#define AGS_FILTER_UTIL_TEST_VOLUME (1.0)
#define AGS_FILTER_UTIL_TEST_SAMPLERATE (44100)
#define AGS_FILTER_UTIL_TEST_OFFSET (0)
#define AGS_FILTER_UTIL_TEST_FRAME_COUNT (1024)
#define AGS_FILTER_UTIL_TEST_BASE_KEY (0.0)
#define AGS_FILTER_UTIL_TEST_TUNING (200.0)

void
ags_filter_util_test_pitch_s8()
{
  gint8 *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_synth_util_sin_s8(buffer,
			AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			AGS_FILTER_UTIL_TEST_SAMPLERATE,
			AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_s8(buffer,
			   AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			   AGS_FILTER_UTIL_TEST_SAMPLERATE,
			   AGS_FILTER_UTIL_TEST_BASE_KEY,
			   AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_s16()
{
  gint16 *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_synth_util_sin_s16(buffer,
			 AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			 AGS_FILTER_UTIL_TEST_SAMPLERATE,
			 AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_s16(buffer,
			    AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			    AGS_FILTER_UTIL_TEST_SAMPLERATE,
			    AGS_FILTER_UTIL_TEST_BASE_KEY,
			    AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_s24()
{
  gint32 *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_synth_util_sin_s24(buffer,
			 AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			 AGS_FILTER_UTIL_TEST_SAMPLERATE,
			 AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_s24(buffer,
			    AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			    AGS_FILTER_UTIL_TEST_SAMPLERATE,
			    AGS_FILTER_UTIL_TEST_BASE_KEY,
			    AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_s32()
{
  gint32 *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_synth_util_sin_s32(buffer,
			 AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			 AGS_FILTER_UTIL_TEST_SAMPLERATE,
			 AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_s32(buffer,
			    AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			    AGS_FILTER_UTIL_TEST_SAMPLERATE,
			    AGS_FILTER_UTIL_TEST_BASE_KEY,
			    AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_s64()
{
  gint64 *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_synth_util_sin_s64(buffer,
			 AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			 AGS_FILTER_UTIL_TEST_SAMPLERATE,
			 AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_s64(buffer,
			    AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			    AGS_FILTER_UTIL_TEST_SAMPLERATE,
			    AGS_FILTER_UTIL_TEST_BASE_KEY,
			    AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_float()
{
  gfloat *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_synth_util_sin_float(buffer,
			   AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			   AGS_FILTER_UTIL_TEST_SAMPLERATE,
			   AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_float(buffer,
			      AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			      AGS_FILTER_UTIL_TEST_SAMPLERATE,
			      AGS_FILTER_UTIL_TEST_BASE_KEY,
			      AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_double()
{
  gdouble *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_synth_util_sin_double(buffer,
			    AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			    AGS_FILTER_UTIL_TEST_SAMPLERATE,
			    AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_double(buffer,
			       AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			       AGS_FILTER_UTIL_TEST_SAMPLERATE,
			       AGS_FILTER_UTIL_TEST_BASE_KEY,
			       AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

  CU_ASSERT(success == TRUE);
}

void
ags_filter_util_test_pitch_complex()
{
  AgsComplex *buffer;

  gboolean success;
  
  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_synth_util_sin_complex(buffer,
			AGS_FILTER_UTIL_TEST_FREQ, AGS_FILTER_UTIL_TEST_PHASE, AGS_FILTER_UTIL_TEST_VOLUME,
			AGS_FILTER_UTIL_TEST_SAMPLERATE,
			AGS_FILTER_UTIL_TEST_OFFSET, AGS_FILTER_UTIL_TEST_FRAME_COUNT);

  ags_filter_util_pitch_complex(buffer,
			   AGS_FILTER_UTIL_TEST_FRAME_COUNT,
			   AGS_FILTER_UTIL_TEST_SAMPLERATE,
			   AGS_FILTER_UTIL_TEST_BASE_KEY,
			   AGS_FILTER_UTIL_TEST_TUNING);

  success = TRUE;
  
  //TODO:JK: implement me

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
  pSuite = CU_add_suite("AgsFMSynthUtilTest", ags_filter_util_test_init_suite, ags_filter_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_filter_util.c pitch signed 8 bit", ags_filter_util_test_pitch_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch signed 16 bit", ags_filter_util_test_pitch_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch signed 24 bit", ags_filter_util_test_pitch_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch signed 32 bit", ags_filter_util_test_pitch_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch signed 64 bit", ags_filter_util_test_pitch_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch float", ags_filter_util_test_pitch_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch double", ags_filter_util_test_pitch_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.c pitch complex", ags_filter_util_test_pitch_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
