/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <math.h>

int ags_soundcard_test_init_suite();
int ags_soundcard_test_clean_suite();

void ags_soundcard_test_set_application_context();
void ags_soundcard_test_get_application_context();
void ags_soundcard_test_set_device();
void ags_soundcard_test_get_device();
void ags_soundcard_test_pcm_info();
void ags_soundcard_test_get_capability();
void ags_soundcard_test_set_presets();
void ags_soundcard_test_get_presets();
void ags_soundcard_test_list_cards();
void ags_soundcard_test_get_poll_fd();
void ags_soundcard_test_is_available();
void ags_soundcard_test_is_starting();
void ags_soundcard_test_is_playing();
void ags_soundcard_test_is_recording();
void ags_soundcard_test_get_uptime();
void ags_soundcard_test_play_init();
void ags_soundcard_test_play();
void ags_soundcard_test_record_init();
void ags_soundcard_test_record();
void ags_soundcard_test_stop();
void ags_soundcard_test_tic();
void ags_soundcard_test_offset_changed();
void ags_soundcard_test_get_buffer();
void ags_soundcard_test_get_next_buffer();
void ags_soundcard_test_get_prev_buffer();
void ags_soundcard_test_lock_buffer();
void ags_soundcard_test_unlock_buffer();
void ags_soundcard_test_set_bpm();
void ags_soundcard_test_get_bpm();
void ags_soundcard_test_set_delay_factor();
void ags_soundcard_test_get_delay_factor();
void ags_soundcard_test_get_absolute_delay();
void ags_soundcard_test_get_delay();
void ags_soundcard_test_get_attack();
void ags_soundcard_test_get_delay_counter();
void ags_soundcard_test_set_note_offset();
void ags_soundcard_test_get_note_offset();
void ags_soundcard_test_set_note_offset_absolute();
void ags_soundcard_test_get_note_offset_absolute();
void ags_soundcard_test_set_loop();
void ags_soundcard_test_get_loop();
void ags_soundcard_test_get_loop_offset();

GType soundcard_test_types[10];
 
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_test_init_suite()
{
  guint i;

  i = 0;
  
  soundcard_test_types[i++] = AGS_TYPE_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_FIFOOUT;
  soundcard_test_types[i++] = AGS_TYPE_CORE_AUDIO_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_CORE_AUDIO_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_JACK_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_JACK_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_PULSE_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_PULSE_DEVIN;
  soundcard_test_types[i++] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_test_clean_suite()
{  
  return(0);
}

void
ags_soundcard_test_set_application_context()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  application_context = ags_thread_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_application_context == NULL){
      g_message("AgsSoundcard::set-application-context missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_soundcard_set_application_context(current,
					  application_context);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_application_context()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_device()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_device()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_pcm_info()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_capability()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_presets()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_presets()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_list_cards()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_poll_fd()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_is_available()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_is_starting()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_is_playing()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_is_recording()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_uptime()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_play_init()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_play()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_record_init()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_record()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_stop()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_tic()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_offset_changed()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_next_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_prev_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_lock_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_unlock_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_bpm()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_bpm()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_absolute_delay()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_delay()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_attack()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_delay_counter()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_note_offset()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_note_offset()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_note_offset_absolute()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_note_offset_absolute()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_loop()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_loop()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_get_loop_offset()
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

  /* remove a suite to the registry */
  pSuite = CU_add_suite("AgsSoundcardTest", ags_soundcard_test_init_suite, ags_soundcard_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSoundcard set application context", ags_soundcard_test_set_application_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get application context", ags_soundcard_test_get_application_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set device", ags_soundcard_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get device", ags_soundcard_test_get_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard pcm info", ags_soundcard_test_pcm_info) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get capability", ags_soundcard_test_get_capability) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set presets", ags_soundcard_test_set_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get presets", ags_soundcard_test_get_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard list cards", ags_soundcard_test_list_cards) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get poll fd", ags_soundcard_test_get_poll_fd) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is available", ags_soundcard_test_is_available) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is starting", ags_soundcard_test_is_starting) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is playing", ags_soundcard_test_is_playing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is recording", ags_soundcard_test_is_recording) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get uptime", ags_soundcard_test_get_uptime) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard play init", ags_soundcard_test_play_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard play", ags_soundcard_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard record init", ags_soundcard_test_record_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard record", ags_soundcard_test_record) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard stop", ags_soundcard_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard tic", ags_soundcard_test_tic) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard offset changed", ags_soundcard_test_offset_changed) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get buffer", ags_soundcard_test_get_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get next buffer", ags_soundcard_test_get_next_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get prev buffer", ags_soundcard_test_get_prev_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard lock buffer", ags_soundcard_test_lock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard unlock buffer", ags_soundcard_test_unlock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set bpm", ags_soundcard_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get bpm", ags_soundcard_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set delay factor", ags_soundcard_test_set_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay factor", ags_soundcard_test_get_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get absolute delay", ags_soundcard_test_get_absolute_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay", ags_soundcard_test_get_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get attack", ags_soundcard_test_get_attack) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay counter", ags_soundcard_test_get_delay_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set note offset", ags_soundcard_test_set_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get note offset", ags_soundcard_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set note offset absolute", ags_soundcard_test_set_note_offset_absolute) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get note offset absolute", ags_soundcard_test_get_note_offset_absolute) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set loop", ags_soundcard_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get loop", ags_soundcard_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get loop offset", ags_soundcard_test_get_loop_offset) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
