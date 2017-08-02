/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
#include <ags/libags-audio.h>

int ags_note_test_init_suite();
int ags_note_test_clean_suite();

void ags_note_test_find_prev();
void ags_note_test_find_next();
void ags_note_test_to_raw_midi();
void ags_note_test_to_seq_event();
void ags_note_test_from_raw_midi();
void ags_note_test_from_seq_event();
void ags_note_test_duplicate();

#define AGS_NOTE_TEST_FIND_PREV_COUNT (128)
#define AGS_NOTE_TEST_FIND_NEXT_COUNT (128)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_note_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_note_test_clean_suite()
{
  return(0);
}

void
ags_note_test_find_prev()
{
  AgsNote *note[AGS_NOTE_TEST_FIND_PREV_COUNT];
  
  GList *list, *current;
  
  guint x0, y;
  guint i;
  gboolean success;
  
  /* create some notes */
  list = NULL;

  for(i = 0; i < AGS_NOTE_TEST_FIND_PREV_COUNT; i++){
    x0 = rand() % 256;
    y = rand() % 8;
    
    note[i] = g_object_new(AGS_TYPE_NOTE,
			   "x0", x0,
			   "x1", x0 + 1,
			   "y", y,
			   NULL);
    list = g_list_prepend(list,
			  note[i]);
  }

  list = g_list_sort(list,
		     ags_note_sort_func);

  /* assert find prev */
  success = TRUE;
  
  for(i = 0; list != NULL; i += 16){
    x0 = i;
    y = rand() % 8;
    
    current = ags_note_find_prev(list,
				 x0, y);

    if(current != NULL &&
       (AGS_NOTE(current->data)->x[0] > x0 ||
	AGS_NOTE(current->data)->y != y)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_note_test_find_next()
{
  AgsNote *note[AGS_NOTE_TEST_FIND_NEXT_COUNT];
  
  GList *list, *current;
  
  guint x0, y;
  guint i;
  gboolean success;
  
  /* create some notes */
  list = NULL;

  for(i = 0; i < AGS_NOTE_TEST_FIND_NEXT_COUNT; i++){
    x0 = rand() % 256;
    y = rand() % 8;
    
    note[i] = g_object_new(AGS_TYPE_NOTE,
			   "x0", x0,
			   "x1", x0 + 1,
			   "y", y,
			   NULL);
    list = g_list_prepend(list,
			  note[i]);
  }

  list = g_list_sort(list,
		     ags_note_sort_func);

  /* assert find next */
  success = TRUE;
  
  for(i = 0; list != NULL; i += 16){
    x0 = i;
    y = rand() % 8;
    
    current = ags_note_find_next(list,
				 x0, y);

    if(current != NULL &&
       (AGS_NOTE(current->data)->x[0] < x0 ||
	AGS_NOTE(current->data)->y != y)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_note_test_to_raw_midi()
{
  //TODO:JK: implement me
}

void
ags_note_test_to_seq_event()
{
  //TODO:JK: implement me
}

void
ags_note_test_from_raw_midi()
{
  //TODO:JK: implement me
}

void
ags_note_test_from_seq_event()
{
  //TODO:JK: implement me
}

void
ags_note_test_duplicate()
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
  pSuite = CU_add_suite("AgsNoteTest", ags_note_test_init_suite, ags_note_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsNote find prev", ags_note_test_find_prev) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote find next", ags_note_test_find_next) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote to raw midi", ags_note_test_to_raw_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote to seq event", ags_note_test_to_seq_event) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote from raw midi", ags_note_test_from_raw_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote from seq event", ags_note_test_from_seq_event) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNote duplicate", ags_note_test_duplicate) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
