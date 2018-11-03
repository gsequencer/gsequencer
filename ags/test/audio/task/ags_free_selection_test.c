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

int ags_free_selection_test_init_suite();
int ags_free_selection_test_clean_suite();

void ags_free_selection_test_launch();

#define AGS_FREE_SELECTION_TEST_LAUNCH_NOTE_COUNT (16 * 64)
#define AGS_FREE_SELECTION_TEST_LAUNCH_MAX_NOTE_WIDTH (4)
#define AGS_FREE_SELECTION_TEST_LAUNCH_MAX_NOTE_HEIGHT (88)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_free_selection_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_free_selection_test_clean_suite()
{  
  return(0);
}

void
ags_free_selection_test_launch()
{
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;
  
  AgsFreeSelection *free_selection;

  guint i;

  audio = ags_audio_new(NULL);
  g_object_ref(audio);

  notation = ags_notation_new(audio,
			      0);
  ags_audio_add_notation(audio,
			 notation);

  for(i = 0; i < AGS_FREE_SELECTION_TEST_LAUNCH_NOTE_COUNT; i++){
    note = ags_note_new();

    note->x[0] = rand() % AGS_FREE_SELECTION_TEST_LAUNCH_NOTE_COUNT;
    note->x[1] = note->x[0] + 1;
    note->y = rand() % AGS_FREE_SELECTION_TEST_LAUNCH_MAX_NOTE_HEIGHT;

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }
  
  ags_notation_add_all_to_selection(notation);

  free_selection = ags_free_selection_new(notation);

  CU_ASSERT(AGS_IS_FREE_SELECTION(free_selection));
  CU_ASSERT(free_selection->notation == notation);

  /* launch */
  ags_task_launch(free_selection);

  CU_ASSERT(notation->selection == NULL);
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
  pSuite = CU_add_suite("AgsFreeSelectionTest", ags_free_selection_test_init_suite, ags_free_selection_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFreeSelection launch", ags_free_selection_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
