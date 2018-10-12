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

int ags_crop_note_test_init_suite();
int ags_crop_note_test_clean_suite();

void ags_crop_note_test_launch();

#define AGS_CROP_NOTE_TEST_LAUNCH_NOTE_COUNT (16 * 64)
#define AGS_CROP_NOTE_TEST_LAUNCH_MAX_NOTE_WIDTH (4)
#define AGS_CROP_NOTE_TEST_LAUNCH_MAX_NOTE_HEIGHT (88)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_crop_note_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_crop_note_test_clean_suite()
{  
  return(0);
}

void
ags_crop_note_test_launch()
{
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;

  AgsCropNote *crop_note;

  GList *selection;
  GList *list;
  
  guint i;
  gboolean success;

  audio = ags_audio_new(NULL);
  
  notation = ags_notation_new(audio,
			      0);
  ags_audio_add_notation(audio,
			 notation);
    
  for(i = 0; i < AGS_CROP_NOTE_TEST_LAUNCH_NOTE_COUNT; i++){
    note = ags_note_new();

    note->x[0] = rand() % AGS_CROP_NOTE_TEST_LAUNCH_NOTE_COUNT;
    note->x[1] = note->x[0] + 1;
    note->y = rand() % AGS_CROP_NOTE_TEST_LAUNCH_MAX_NOTE_HEIGHT;

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  ags_notation_add_all_to_selection(notation);
  selection = ags_list_util_copy_and_ref(ags_notation_get_selection(notation));
  crop_note = ags_crop_note_new(notation,
				selection,
				0, 4,
				TRUE, TRUE, TRUE);
  
  CU_ASSERT(AGS_IS_CROP_NOTE(crop_note));
  CU_ASSERT(crop_note->notation == notation);
  //  CU_ASSERT(crop_note->selection == selection);
  CU_ASSERT(crop_note->x_padding == 0);
  CU_ASSERT(crop_note->x_crop == 4);
  CU_ASSERT(crop_note->absolute == TRUE);
  CU_ASSERT(crop_note->in_place == TRUE);
  CU_ASSERT(crop_note->do_resize == TRUE);

  /* launch */
  ags_task_launch(crop_note);

  list = notation->note;
  success = TRUE;

  for(i = 0; i < AGS_CROP_NOTE_TEST_LAUNCH_NOTE_COUNT && list != NULL; i++){
    if(AGS_NOTE(list->data)->x[1] - AGS_NOTE(list->data)->x[0] != 4){
      success = FALSE;
      
      break;
    }
    
    list = list->next;
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
  pSuite = CU_add_suite("AgsCropNoteTest", ags_crop_note_test_init_suite, ags_crop_note_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsCropNote launch", ags_crop_note_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
