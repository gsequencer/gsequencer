/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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
#include <CUnit/Basic.h>

#include <ags/libags.h>

int ags_frame_clock_test_init_suite();
int ags_frame_clock_test_clean_suite();

void ags_frame_clock_test_test_flags();
void ags_frame_clock_test_set_flags();
void ags_frame_clock_test_unset_flags();
void ags_frame_clock_test_set_buffer_size();
void ags_frame_clock_test_get_buffer_size();
void ags_frame_clock_test_set_samplerate();
void ags_frame_clock_test_get_samplerate();
void ags_frame_clock_test_set_bpm();
void ags_frame_clock_test_get_bpm();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_frame_clock_test_init_suite()
{    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_frame_clock_test_clean_suite()
{
  
  return(0);
}

void
ags_frame_clock_test_test_flags()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  /* test started */
  frame_clock->flags = AGS_FRAME_CLOCK_STARTED;

  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED) == TRUE);

  /* test running */
  frame_clock->flags = AGS_FRAME_CLOCK_RUNNING;

  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_RUNNING) == TRUE);
}

void
ags_frame_clock_test_set_flags()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->flags = 0;

  /* test started */
  ags_frame_clock_set_flags(frame_clock, AGS_FRAME_CLOCK_STARTED);
  
  CU_ASSERT((AGS_FRAME_CLOCK_STARTED & (frame_clock->flags)) != 0);

  /* test running*/
  ags_frame_clock_set_flags(frame_clock, AGS_FRAME_CLOCK_RUNNING);
  
  CU_ASSERT((AGS_FRAME_CLOCK_RUNNING & (frame_clock->flags)) != 0);
}

void
ags_frame_clock_test_unset_flags()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  /* test started */
  frame_clock->flags = AGS_FRAME_CLOCK_STARTED;

  ags_frame_clock_unset_flags(frame_clock, AGS_FRAME_CLOCK_STARTED);
  
  CU_ASSERT((AGS_FRAME_CLOCK_STARTED & (frame_clock->flags)) == 0);

  /* test running */
  frame_clock->flags = AGS_FRAME_CLOCK_RUNNING;

  ags_frame_clock_unset_flags(frame_clock, AGS_FRAME_CLOCK_RUNNING);
  
  CU_ASSERT((AGS_FRAME_CLOCK_RUNNING & (frame_clock->flags)) == 0);
}

void
ags_frame_clock_test_set_buffer_size()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  ags_frame_clock_set_buffer_size(frame_clock,
				  512);

  CU_ASSERT(frame_clock->buffer_size == 512);
}

void
ags_frame_clock_test_get_buffer_size()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->buffer_size = 2048;
  
  CU_ASSERT(ags_frame_clock_get_buffer_size(frame_clock) == 2048);

  frame_clock->buffer_size = 512;
  
  CU_ASSERT(ags_frame_clock_get_buffer_size(frame_clock) == 512);
}

void
ags_frame_clock_test_set_samplerate()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  ags_frame_clock_set_samplerate(frame_clock,
				 48000);

  CU_ASSERT(frame_clock->samplerate == 48000);

  ags_frame_clock_set_samplerate(frame_clock,
				 44100);

  CU_ASSERT(frame_clock->samplerate == 44100);
}

void
ags_frame_clock_test_get_samplerate()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->samplerate = 48000;
  
  CU_ASSERT(ags_frame_clock_get_samplerate(frame_clock) == 48000);

  frame_clock->samplerate = 44100;
  
  CU_ASSERT(ags_frame_clock_get_samplerate(frame_clock) == 44100);
}

void
ags_frame_clock_test_set_bpm()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  ags_frame_clock_set_bpm(frame_clock,
			  138.0);

  CU_ASSERT(frame_clock->bpm == 138.0);

  ags_frame_clock_set_bpm(frame_clock,
			  152.0);

  CU_ASSERT(frame_clock->bpm == 152.0);
}

void
ags_frame_clock_test_get_bpm()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->bpm = 138.0;
  
  CU_ASSERT(ags_frame_clock_get_bpm(frame_clock) == 138.0);

  frame_clock->bpm = 152.0;
  
  CU_ASSERT(ags_frame_clock_get_bpm(frame_clock) == 152.0);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFrameClockTest", ags_frame_clock_test_init_suite, ags_frame_clock_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFrameClock test flags", ags_frame_clock_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock set flags", ags_frame_clock_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock unset flags", ags_frame_clock_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock set buffer size", ags_frame_clock_test_set_buffer_size) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get buffer size", ags_frame_clock_test_get_buffer_size) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock set samplerate", ags_frame_clock_test_set_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get samplerate", ags_frame_clock_test_get_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock set bpm", ags_frame_clock_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get bpm", ags_frame_clock_test_get_bpm) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
