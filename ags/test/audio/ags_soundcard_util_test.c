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

#include <string.h>

int ags_soundcard_util_test_init_suite();
int ags_soundcard_util_test_clean_suite();

void ags_soundcard_util_test_compute_delay_and_attack();

#define AGS_SOUNDCARD_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE (1024)
#define AGS_SOUNDCARD_UTIL_TEST_BPM (138.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_util_test_clean_suite()
{
  return(0);
}

void
ags_soundcard_util_test_compute_delay_and_attack()
{
  gdouble *delay;
  guint *attack;
  guint *note_256th_attack;

  gdouble absolute_delay;
  gdouble corrected_delay;
  guint default_tact_frames;
  guint total_correct_frame_count;
  gdouble correct_frame_count;
  gdouble correct_note_256th_frame_count;
  gdouble note_256th_tic_size;
  gint64 frame_counter;
  gint64 frame_counter_delay_remainder;
  guint nth_list;
  gint64 current_attack;
  gint64 current_note_256th_attack;
  gboolean increment_delay;
  guint i, j;
  
  GList *start_note_256th_attack;

  absolute_delay = (60.0 * (((gdouble) AGS_SOUNDCARD_UTIL_TEST_SAMPLERATE / (gdouble) AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE) / (gdouble) AGS_SOUNDCARD_UTIL_TEST_BPM) * ((1.0 / 16.0) * (1.0 / (gdouble) AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR)));
  
  delay = (gdouble *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
			       sizeof(gdouble));
  
  attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
			      sizeof(guint));
    
  start_note_256th_attack = NULL;

  for(i = 0; i < 16; i++){
    guint *note_256th_attack;
    
    note_256th_attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(guint));
    
    start_note_256th_attack = g_list_prepend(start_note_256th_attack,
					     note_256th_attack);
  }

  default_tact_frames = (guint) floor(absolute_delay * (gdouble) AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE);

  total_correct_frame_count = AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE * ((guint) floor((gdouble) ((guint) AGS_SOUNDCARD_DEFAULT_PERIOD * default_tact_frames) / (gdouble) AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE));

  correct_frame_count = (gdouble) total_correct_frame_count / AGS_SOUNDCARD_DEFAULT_PERIOD;

  correct_note_256th_frame_count = correct_frame_count / 16.0;
  
  corrected_delay = correct_frame_count / (gdouble) AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;
  
  attack[0] = 0;
  delay[0] = floor(corrected_delay);

  fprintf(stdout, "-> delay{#0} = %f, attack{#0} = %d\n", delay[0], attack[0]);
  
  frame_counter = (guint) 0;
  frame_counter_delay_remainder = 0;

  nth_list = 0;
  note_256th_attack = g_list_nth_data(start_note_256th_attack,
				      nth_list);

  for(j = 0; j < 16; j++){
    gint64 current_attack;
    gint64 current_note_256th_attack;

    current_attack = attack[0];
    current_note_256th_attack = (guint) floor(current_attack + (j * correct_note_256th_frame_count)) % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;

    note_256th_attack[j] = current_note_256th_attack;

    fprintf(stdout, " `-> note_256th_attack{#%d} = %d\n", j, current_note_256th_attack);
  }

  current_attack = (guint) (correct_frame_count) % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;
    
  attack[1] = current_attack;

  frame_counter_delay_remainder += ((guint) correct_frame_count % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE);
    
  increment_delay = FALSE;

  if(frame_counter_delay_remainder >= AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE){
    frame_counter_delay_remainder -= AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;
      
    increment_delay = TRUE;
  }

  if(!increment_delay){
    delay[1] = floor(corrected_delay);
  }else{
    delay[1] = floor(corrected_delay) + 1.0;
  }

  fprintf(stdout, "-> delay{#%d} = %f, attack{#%d} = %d\n", 1, delay[1], 1, attack[1]);

  for(i = 1; i < AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    nth_list = floor(16 * i / AGS_SOUNDCARD_DEFAULT_PERIOD);
    note_256th_attack = g_list_nth_data(start_note_256th_attack,
					nth_list);

    for(j = 0; j < 16; j++){
      current_attack = attack[i];
      current_note_256th_attack = (guint) (current_attack + (j * correct_note_256th_frame_count)) % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;

      note_256th_attack[(16 * i) + j] = current_note_256th_attack;

      fprintf(stdout, " `-> note_256th_attack{#%d} = %d\n", (16 * i) + j, current_note_256th_attack);
    }

    if(i + 1 < AGS_SOUNDCARD_DEFAULT_PERIOD){
      current_attack = (guint) ((i + 1) * correct_frame_count) % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;
    
      attack[i + 1] = current_attack;

      frame_counter_delay_remainder += ((guint) correct_frame_count % AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE);
    
      increment_delay = FALSE;

      if(frame_counter_delay_remainder >= AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE){
	frame_counter_delay_remainder -= AGS_SOUNDCARD_UTIL_TEST_BUFFER_SIZE;
      
	increment_delay = TRUE;
      }

      if(!increment_delay){
	delay[i + 1] = floor(corrected_delay);
      }else{
	delay[i + 1] = floor(corrected_delay) + 1.0;
      }

      fprintf(stdout, "-> delay{#%d} = %f, attack{#%d} = %d\n", i, delay[i], i, attack[i]);
    }
  }
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
  pSuite = CU_add_suite("AgsSoundcardUtilTest", ags_soundcard_util_test_init_suite, ags_soundcard_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSoundcardUtil compute delay and attack", ags_soundcard_util_test_compute_delay_and_attack) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

