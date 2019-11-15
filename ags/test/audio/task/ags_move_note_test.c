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

int ags_move_note_test_init_suite();
int ags_move_note_test_clean_suite();

void ags_move_note_test_launch();

#define AGS_MOVE_NOTE_TEST_LAUNCH_NOTE_COUNT (16 * 64)
#define AGS_MOVE_NOTE_TEST_LAUNCH_MAX_NOTE_WIDTH (4)
#define AGS_MOVE_NOTE_TEST_LAUNCH_MAX_NOTE_HEIGHT (88)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_move_note_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_move_note_test_clean_suite()
{  
  return(0);
}

void
ags_move_note_test_launch()
{
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;

  AgsMoveNote *move_note;

  GList *selection;
  GList *list_notation, *list_note;
  
  guint i;
  gboolean success;

  audio = ags_audio_new(NULL);
  
  notation = ags_notation_new(audio,
			      0);
  ags_audio_add_notation(audio,
			 notation);
    
  for(i = 0; i < AGS_MOVE_NOTE_TEST_LAUNCH_NOTE_COUNT; i++){
    note = ags_note_new();

    note->x[0] = rand() % AGS_MOVE_NOTE_TEST_LAUNCH_NOTE_COUNT;
    note->x[1] = note->x[0] + 1;
    note->y = rand() % AGS_MOVE_NOTE_TEST_LAUNCH_MAX_NOTE_HEIGHT;

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  ags_notation_add_all_to_selection(notation);
  selection = ags_list_util_copy_and_ref(ags_notation_get_selection(notation));
  move_note = ags_move_note_new(audio,
				notation,
				selection,
				0, 0,
				8 * 16, 0,
				FALSE, TRUE);
  
  CU_ASSERT(AGS_IS_MOVE_NOTE(move_note));
  CU_ASSERT(move_note->notation == notation);
  //  CU_ASSERT(move_note->selection == selection);
  CU_ASSERT(move_note->first_x == 0);
  CU_ASSERT(move_note->first_y == 0);
  CU_ASSERT(move_note->move_x == 8 * 16);
  CU_ASSERT(move_note->move_y == 0);
  CU_ASSERT(move_note->relative == FALSE);
  CU_ASSERT(move_note->absolute == TRUE);

  /* launch */
  ags_task_launch(move_note);

  list_notation = audio->notation;
  success = TRUE;

  for(i = 0; i < AGS_MOVE_NOTE_TEST_LAUNCH_NOTE_COUNT && list_notation != NULL;){
    list_note = AGS_NOTATION(list_notation->data)->note;

    while(list_note != NULL){
      if(AGS_NOTE(list_note->data)->x[0] < 8 * 16){
	success = FALSE;
      
	break;
      }

      list_note = list_note->next;
      i++;
    }
    
    list_notation = list_notation->next;
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
  pSuite = CU_add_suite("AgsMoveNoteTest", ags_move_note_test_init_suite, ags_move_note_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMoveNote launch", ags_move_note_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
