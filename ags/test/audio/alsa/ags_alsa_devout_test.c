/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2023 Joël Krähemann
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

int ags_alsa_devout_test_init_suite();
int ags_alsa_devout_test_clean_suite();

void ags_alsa_devout_test_set_device();
void ags_alsa_devout_test_get_device();
void ags_alsa_devout_test_set_presets();
void ags_alsa_devout_test_get_presets();
void ags_alsa_devout_test_list_cards();
void ags_alsa_devout_test_pcm_info();
void ags_alsa_devout_test_get_capability();
void ags_alsa_devout_test_is_available();
void ags_alsa_devout_test_is_starting();
void ags_alsa_devout_test_is_playing();
void ags_alsa_devout_test_is_recording();
void ags_alsa_devout_test_get_uptime();
void ags_alsa_devout_test_play_init();
void ags_alsa_devout_test_play();
void ags_alsa_devout_test_record_init();
void ags_alsa_devout_test_record();
void ags_alsa_devout_test_stop();
void ags_alsa_devout_test_tic();
void ags_alsa_devout_test_offset_changed();
void ags_alsa_devout_test_get_buffer();
void ags_alsa_devout_test_get_next_buffer();
void ags_alsa_devout_test_get_prev_buffer();
void ags_alsa_devout_test_lock_buffer();
void ags_alsa_devout_test_unlock_buffer();
void ags_alsa_devout_test_set_bpm();
void ags_alsa_devout_test_get_bpm();
void ags_alsa_devout_test_set_delay_factor();
void ags_alsa_devout_test_get_delay_factor();
void ags_alsa_devout_test_get_absolute_delay();
void ags_alsa_devout_test_get_delay();
void ags_alsa_devout_test_get_attack();
void ags_alsa_devout_test_get_delay_counter();
void ags_alsa_devout_test_get_start_note_offset();
void ags_alsa_devout_test_set_start_note_offset();
void ags_alsa_devout_test_get_note_offset();
void ags_alsa_devout_test_set_note_offset();
void ags_alsa_devout_test_get_note_offset_absolute();
void ags_alsa_devout_test_set_note_offset_absolute();
void ags_alsa_devout_test_set_loop();
void ags_alsa_devout_test_get_loop();
void ags_alsa_devout_test_get_loop_offset();
void ags_alsa_devout_test_get_sub_block_count();
void ags_alsa_devout_test_trylock_sub_block();
void ags_alsa_devout_test_unlock_sub_block();
void ags_alsa_devout_test_get_note_256th_offset();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devout_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devout_test_clean_suite()
{
  return(0);
}

void
ags_alsa_devout_test_set_device()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_device()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_presets()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_presets()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_list_cards()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_pcm_info()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_capability()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_is_available()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_is_starting()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_is_playing()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_is_recording()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_uptime()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_play_init()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_play()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_record_init()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_record()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_stop()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_tic()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_offset_changed()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_buffer()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_next_buffer()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_prev_buffer()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_lock_buffer()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_unlock_buffer()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_bpm()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_bpm()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_absolute_delay()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_attack()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay_counter()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_start_note_offset()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_start_note_offset()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_note_offset()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_note_offset()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_note_offset_absolute()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_note_offset_absolute()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_set_loop()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_loop()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_loop_offset()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_sub_block_count()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_trylock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_unlock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_note_256th_offset()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsAlsaDevoutTest", ags_alsa_devout_test_init_suite, ags_alsa_devout_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAlsaDevout set device", ags_alsa_devout_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get device", ags_alsa_devout_test_get_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set presets", ags_alsa_devout_test_set_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get presets", ags_alsa_devout_test_get_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout list cards", ags_alsa_devout_test_list_cards) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout PCM info", ags_alsa_devout_test_pcm_info) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get capability", ags_alsa_devout_test_get_capability) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is available", ags_alsa_devout_test_is_available) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is starting", ags_alsa_devout_test_is_starting) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is playing", ags_alsa_devout_test_is_playing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is recording", ags_alsa_devout_test_is_recording) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get uptime", ags_alsa_devout_test_get_uptime) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout play init", ags_alsa_devout_test_play_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout play", ags_alsa_devout_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout record init", ags_alsa_devout_test_record_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout record", ags_alsa_devout_test_record) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout stop", ags_alsa_devout_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout tic", ags_alsa_devout_test_tic) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout offset changed", ags_alsa_devout_test_offset_changed) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get buffer", ags_alsa_devout_test_get_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get next buffer", ags_alsa_devout_test_get_next_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get prev buffer", ags_alsa_devout_test_get_prev_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout lock buffer", ags_alsa_devout_test_lock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout unlock buffer", ags_alsa_devout_test_unlock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set bpm", ags_alsa_devout_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get bpm", ags_alsa_devout_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set delay factor", ags_alsa_devout_test_set_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay factor", ags_alsa_devout_test_get_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get absolute delay", ags_alsa_devout_test_get_absolute_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay", ags_alsa_devout_test_get_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get attack", ags_alsa_devout_test_get_attack) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay counter", ags_alsa_devout_test_get_delay_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set start note offset", ags_alsa_devout_test_set_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get start note offset", ags_alsa_devout_test_get_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set note offset", ags_alsa_devout_test_set_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get note offset", ags_alsa_devout_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set loop", ags_alsa_devout_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get loop", ags_alsa_devout_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get loop offset", ags_alsa_devout_test_get_loop_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get sub block count", ags_alsa_devout_test_get_sub_block_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout trylock sub block", ags_alsa_devout_test_trylock_sub_block) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout unock sub block", ags_alsa_devout_test_unlock_sub_block) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

