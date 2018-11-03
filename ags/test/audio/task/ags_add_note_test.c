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

int ags_add_note_test_init_suite();
int ags_add_note_test_clean_suite();

void ags_add_note_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_note_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_note_test_clean_suite()
{  
  return(0);
}

void
ags_add_note_test_launch()
{
  AgsAudio *audio;
  AgsNote *note;
  
  AgsAddNote *add_note;

  audio = ags_audio_new(NULL);
  g_object_ref(audio);

  note = ags_note_new();
  g_object_ref(note);
  
  add_note = ags_add_note_new(audio,
			      note,
			      0,
			      FALSE);

  CU_ASSERT(AGS_IS_ADD_NOTE(add_note));
  CU_ASSERT(add_note->audio == audio);
  CU_ASSERT(add_note->note == note);
  CU_ASSERT(add_note->audio_channel == 0);
  CU_ASSERT(add_note->use_selection_list == FALSE);

  /* test */
  ags_task_launch(add_note);

  CU_ASSERT(audio->notation != NULL);
  CU_ASSERT(AGS_NOTATION(audio->notation->data)->note != NULL);
  CU_ASSERT(AGS_NOTATION(audio->notation->data)->note->data == note);
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
  pSuite = CU_add_suite("AgsAddNoteTest", ags_add_note_test_init_suite, ags_add_note_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAddNote launch", ags_add_note_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
