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
void ags_frame_clock_test_get_absolute_frame_offset();
void ags_frame_clock_test_get_frame_offset();
void ags_frame_clock_test_get_has_16th_pulse();
void ags_frame_clock_test_get_loop();
void ags_frame_clock_test_set_loop();
void ags_frame_clock_test_get_absolute_note_offset();
void ags_frame_clock_test_get_note_offset();
void ags_frame_clock_test_get_note_frame_offset();
void ags_frame_clock_test_get_absolute_note_256th_offset();
void ags_frame_clock_test_get_note_256th_offset();
void ags_frame_clock_test_get_note_256th_frame_offset();
void ags_frame_clock_test_start();
void ags_frame_clock_test_stop();
void ags_frame_clock_test_increment_counter();
void ags_frame_clock_test_from_string();
void ags_frame_clock_test_to_string();
void ags_frame_clock_test_to_time_string();

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

void
ags_frame_clock_test_get_absolute_frame_offset()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->absolute_frame_offset = 0;

  CU_ASSERT(ags_frame_clock_get_absolute_frame_offset(frame_clock) == 0);
  
  frame_clock->absolute_frame_offset = 2048;

  CU_ASSERT(ags_frame_clock_get_absolute_frame_offset(frame_clock) == 2048);
}

void
ags_frame_clock_test_get_frame_offset()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->frame_offset = 0;

  CU_ASSERT(ags_frame_clock_get_frame_offset(frame_clock) == 0);
  
  frame_clock->frame_offset = 2048;

  CU_ASSERT(ags_frame_clock_get_frame_offset(frame_clock) == 2048);
}

void
ags_frame_clock_test_get_has_16th_pulse()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->has_16th_pulse = FALSE;

  CU_ASSERT(ags_frame_clock_get_has_16th_pulse(frame_clock) == FALSE);
  
  frame_clock->has_16th_pulse = TRUE;

  CU_ASSERT(ags_frame_clock_get_has_16th_pulse(frame_clock) == TRUE);
}

void
ags_frame_clock_test_get_loop()
{
  AgsFrameClock *frame_clock;

  guint64 loop_left;
  guint64 loop_right;
  
  gboolean do_loop;
  
  frame_clock = ags_frame_clock_new();

  frame_clock->do_loop = TRUE;

  frame_clock->loop_left = 0;
  frame_clock->loop_right = 64;
  
  loop_left = 0;
  loop_right = 0;
  
  do_loop = ags_frame_clock_get_loop(frame_clock,
				     &loop_left,
				     &loop_right);

  CU_ASSERT(do_loop == TRUE && loop_left == 0 && loop_right == 64);
}

void
ags_frame_clock_test_set_loop()
{
  AgsFrameClock *frame_clock;

  guint64 loop_left;
  guint64 loop_right;
  
  gboolean do_loop;
  
  frame_clock = ags_frame_clock_new();

  do_loop = TRUE;
  
  loop_left = 0;
  loop_right = 256;
  
  ags_frame_clock_set_loop(frame_clock,
			   do_loop,
			   loop_left,
			   loop_right);

  CU_ASSERT(frame_clock->do_loop == TRUE && frame_clock->loop_left == 0 && frame_clock->loop_right == 256);
}

void
ags_frame_clock_test_get_absolute_note_offset()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->absolute_note_offset = 0;

  CU_ASSERT(ags_frame_clock_get_absolute_note_offset(frame_clock) == 0);
  
  frame_clock->absolute_note_offset = 1;

  CU_ASSERT(ags_frame_clock_get_absolute_note_offset(frame_clock) == 1);
}

void
ags_frame_clock_test_get_note_offset()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->note_offset = 0;

  CU_ASSERT(ags_frame_clock_get_note_offset(frame_clock) == 0);
  
  frame_clock->note_offset = 1;

  CU_ASSERT(ags_frame_clock_get_note_offset(frame_clock) == 1);
}

void
ags_frame_clock_test_get_note_frame_offset()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  frame_clock->note_frame_offset = 0;

  CU_ASSERT(ags_frame_clock_get_note_frame_offset(frame_clock) == 0);
  
  frame_clock->note_frame_offset = 3046;

  CU_ASSERT(ags_frame_clock_get_note_frame_offset(frame_clock) == 3046);
}

void
ags_frame_clock_test_get_absolute_note_256th_offset()
{
  AgsFrameClock *frame_clock;

  guint64 absolute_note_256th_offset[16];

  guint length;
  guint i;
  gboolean success;
  
  frame_clock = ags_frame_clock_new();
  
  frame_clock->absolute_note_256th_offset[0] = 0;
  
  frame_clock->absolute_note_256th_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_absolute_note_256th_offset(frame_clock,
						 &(absolute_note_256th_offset[0]),
						 &length);
  
  CU_ASSERT(length == 1 && absolute_note_256th_offset[0] == 0);
  
  frame_clock->absolute_note_256th_offset[0] = 16;
  
  frame_clock->absolute_note_256th_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_absolute_note_256th_offset(frame_clock,
						 &(absolute_note_256th_offset[0]),
						 &length);
  
  CU_ASSERT(length == 1 && absolute_note_256th_offset[0] == 16);
}

void
ags_frame_clock_test_get_note_256th_offset()
{
  AgsFrameClock *frame_clock;

  guint64 note_256th_offset[16];

  guint length;
  guint i;
  gboolean success;
  
  frame_clock = ags_frame_clock_new();
  
  frame_clock->note_256th_offset[0] = 0;
  
  frame_clock->note_256th_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_note_256th_offset(frame_clock,
					&(note_256th_offset[0]),
					&length);
  
  CU_ASSERT(length == 1 && note_256th_offset[0] == 0);
  
  frame_clock->note_256th_offset[0] = 16;
  
  frame_clock->note_256th_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_note_256th_offset(frame_clock,
					&(note_256th_offset[0]),
					&length);
  
  CU_ASSERT(length == 1 && note_256th_offset[0] == 16);
}

void
ags_frame_clock_test_get_note_256th_frame_offset()
{
  AgsFrameClock *frame_clock;

  guint64 note_256th_frame_offset[16];

  guint length;
  guint i;
  gboolean success;
  
  frame_clock = ags_frame_clock_new();
  
  frame_clock->note_256th_frame_offset[0] = 0;
  
  frame_clock->note_256th_frame_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_note_256th_frame_offset(frame_clock,
					      &(note_256th_frame_offset[0]),
					      &length);
  
  CU_ASSERT(length == 1 && note_256th_frame_offset[0] == 0);
  
  frame_clock->note_256th_frame_offset[0] = 16;
  
  frame_clock->note_256th_frame_offset_length = 1;

  length = 0;
  
  ags_frame_clock_get_note_256th_frame_offset(frame_clock,
					      &(note_256th_frame_offset[0]),
					      &length);
  
  CU_ASSERT(length == 1 && note_256th_frame_offset[0] == 16);
}

void
ags_frame_clock_test_start()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED) == FALSE);
  
  ags_frame_clock_start(frame_clock);
  
  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED) == TRUE);
}

void
ags_frame_clock_test_stop()
{
  AgsFrameClock *frame_clock;

  frame_clock = ags_frame_clock_new();

  ags_frame_clock_start(frame_clock);
  
  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED) == TRUE);

  ags_frame_clock_stop(frame_clock);  
  
  CU_ASSERT(ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED) == FALSE);  
}

void
ags_frame_clock_test_increment_counter()
{
  AgsFrameClock *frame_clock;

  guint64 frame_offset;
  guint note_16th_pulse_count;
  guint i;
  gboolean success;
  
  frame_clock = ags_frame_clock_new();

  /* #1 attempt */
  ags_frame_clock_start(frame_clock);
  
  frame_offset = 0;

  note_16th_pulse_count = 0;
  
  success = TRUE;
  
  for(i = 0; frame_offset < 64 * frame_clock->samplerate; i++){
    if(frame_clock->has_16th_pulse){
      note_16th_pulse_count++;
    }
    
    if(frame_offset != frame_clock->frame_offset){
      success = FALSE;

      break;
    }
    
    ags_frame_clock_increment_counter(frame_clock);
    
    frame_offset += frame_clock->buffer_size;
  }

  g_message("16th pulse count - %d", note_16th_pulse_count);
  
  CU_ASSERT(frame_offset >= 64 * frame_clock->samplerate);
  
  CU_ASSERT(success == TRUE);

  ags_frame_clock_stop(frame_clock);
  
  /* #2 attempt */
  ags_frame_clock_start(frame_clock);

  frame_offset = 0;

  note_16th_pulse_count = 0;
  
  success = TRUE;
  
  for(i = 0; frame_offset < 64 * frame_clock->samplerate; i++){
    if(frame_clock->has_16th_pulse){
      note_16th_pulse_count++;
    }
    
    if(frame_offset != frame_clock->frame_offset){
      success = FALSE;

      break;
    }
    
    ags_frame_clock_increment_counter(frame_clock);
    
    frame_offset += frame_clock->buffer_size;
  }

  g_message("16th pulse count - %d", note_16th_pulse_count);
  
  CU_ASSERT(frame_offset >= 64 * frame_clock->samplerate);
  
  CU_ASSERT(success == TRUE);

  ags_frame_clock_stop(frame_clock);
  
  /* #3 attempt */
  ags_frame_clock_set_bpm(frame_clock,
			  138.0);
  
  ags_frame_clock_start(frame_clock);

  frame_offset = 0;

  note_16th_pulse_count = 0;
  
  success = TRUE;
  
  for(i = 0; frame_offset < 64 * frame_clock->samplerate; i++){
    if(frame_clock->has_16th_pulse){
      note_16th_pulse_count++;
    }
    
    if(frame_offset != frame_clock->frame_offset){
      success = FALSE;

      break;
    }
    
    ags_frame_clock_increment_counter(frame_clock);
    
    frame_offset += frame_clock->buffer_size;
  }

  g_message("16th pulse count - %d", note_16th_pulse_count);
  
  CU_ASSERT(frame_offset >= 64 * frame_clock->samplerate);
  
  CU_ASSERT(success == TRUE);

  ags_frame_clock_stop(frame_clock);
}

void
ags_frame_clock_test_from_string()
{
  //TODO:JK: implement me
}

void
ags_frame_clock_test_to_string()
{
  //TODO:JK: implement me
}

void
ags_frame_clock_test_to_time_string()
{
  //TODO:JK: implement me
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
     (CU_add_test(pSuite, "test of AgsFrameClock get bpm", ags_frame_clock_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get absolute frame offset", ags_frame_clock_test_get_absolute_frame_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get frame offset", ags_frame_clock_test_get_frame_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get has 16th pulse", ags_frame_clock_test_get_has_16th_pulse) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get loop", ags_frame_clock_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock set loop", ags_frame_clock_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get absolute note offset", ags_frame_clock_test_get_absolute_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get note offset", ags_frame_clock_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get note frame offset", ags_frame_clock_test_get_note_frame_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get absolute note 256th offset", ags_frame_clock_test_get_absolute_note_256th_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get note 256th offset", ags_frame_clock_test_get_note_256th_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock get note 256th frame offset", ags_frame_clock_test_get_note_256th_frame_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock start", ags_frame_clock_test_start) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock stop", ags_frame_clock_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock increment counter", ags_frame_clock_test_increment_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock from string", ags_frame_clock_test_from_string) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock to string", ags_frame_clock_test_to_string) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFrameClock to time string", ags_frame_clock_test_to_time_string) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
