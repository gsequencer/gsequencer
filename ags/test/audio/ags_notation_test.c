/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

int ags_notation_test_init_suite();
int ags_notation_test_clean_suite();

void ags_notation_test_find_near_timestamp();
void ags_notation_test_add_note();
void ags_notation_test_remove_note_at_position();
void ags_notation_test_is_note_selected();
void ags_notation_test_find_point();
void ags_notation_test_find_region();
void ags_notation_test_free_selection();
void ags_notation_test_add_all_to_selection();
void ags_notation_test_add_point_to_selection();
void ags_notation_test_remove_point_from_selection();
void ags_notation_test_add_region_to_selection();
void ags_notation_test_remove_region_from_selection();
void ags_notation_test_copy_selection();
void ags_notation_test_cut_selection();
void ags_notation_test_insert_from_clipboard();

#define AGS_NOTATION_TEST_FIND_NEAR_TIMESTAMP_N_NOTATION (8)

#define AGS_NOTATION_TEST_ADD_NOTE_WIDTH (4096)
#define AGS_NOTATION_TEST_ADD_NOTE_HEIGHT (88)
#define AGS_NOTATION_TEST_ADD_NOTE_COUNT (1024)

#define AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_WIDTH (4096)
#define AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_HEIGHT (88)
#define AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_COUNT (1024)
#define AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_REMOVE_COUNT (256)

#define AGS_NOTATION_TEST_IS_NOTE_SELECTED_WIDTH (4096)
#define AGS_NOTATION_TEST_IS_NOTE_SELECTED_HEIGHT (88)
#define AGS_NOTATION_TEST_IS_NOTE_SELECTED_COUNT (1024)
#define AGS_NOTATION_TEST_IS_NOTE_SELECTED_SELECTION_COUNT (128)

#define AGS_NOTATION_TEST_FIND_POINT_WIDTH (4096)
#define AGS_NOTATION_TEST_FIND_POINT_HEIGHT (88)
#define AGS_NOTATION_TEST_FIND_POINT_COUNT (1024)
#define AGS_NOTATION_TEST_FIND_POINT_N_ATTEMPTS (1024)

#define AGS_NOTATION_TEST_FIND_REGION_WIDTH (4096)
#define AGS_NOTATION_TEST_FIND_REGION_HEIGHT (88)
#define AGS_NOTATION_TEST_FIND_REGION_COUNT (1024)
#define AGS_NOTATION_TEST_FIND_REGION_N_ATTEMPTS (1024)
#define AGS_NOTATION_TEST_FIND_REGION_SELECTION_WIDTH (128)
#define AGS_NOTATION_TEST_FIND_REGION_SELECTION_HEIGHT (24)

AgsAudio *audio;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_notation_test_init_suite()
{
  audio = ags_audio_new(NULL);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_notation_test_clean_suite()
{
  g_object_unref(audio);
  
  return(0);
}

void
ags_notation_test_find_near_timestamp()
{
  AgsNotation **notation;
  AgsTimestamp *timestamp;

  GList *list, *current;

  guint i;
  
  notation = (AgsNotation **) malloc(AGS_NOTATION_TEST_FIND_NEAR_TIMESTAMP_N_NOTATION * sizeof(AgsNotation *));
  list = NULL;

  for(i = 0; i < AGS_NOTATION_TEST_FIND_NEAR_TIMESTAMP_N_NOTATION; i++){
    /* nth notation */
    notation[i] = ags_notation_new(audio,
				   0);
    timestamp = ags_timestamp_new();
    g_object_set(notation[i],
		 "timestamp\0", timestamp,
		 NULL);

    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(notation[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_NOTATION_DEFAULT_DURATION);

    list = g_list_prepend(list,
			  notation[i]);
  }

  /* instantiate timestamp to check against */
  timestamp = ags_timestamp_new();
  timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(notation[0]->timestamp)->timer.unix_time.time_val;
  
  /* assert find */
  for(i = 0; i < AGS_NOTATION_TEST_FIND_NEAR_TIMESTAMP_N_NOTATION; i++){
    current = ags_notation_find_near_timestamp(list, 0,
					       timestamp);

    CU_ASSERT(current != NULL && current->data == notation[i]);
    
    timestamp->timer.unix_time.time_val += ((i + 1) * AGS_NOTATION_DEFAULT_DURATION);    
  }  
}

void
ags_notation_test_add_note()
{
  AgsNotation *notation;
  AgsNote *note;

  GList *list;
  
  guint x0, y;
  guint i;
  gboolean success;

  /* create notation */
  notation = ags_notation_new(audio,
			      0);

  for(i = 0; i < AGS_NOTATION_TEST_ADD_NOTE_COUNT; i++){
    x0 = rand() % AGS_NOTATION_TEST_ADD_NOTE_WIDTH;
    y = rand() % AGS_NOTATION_TEST_ADD_NOTE_HEIGHT;
    
    note = ags_note_new_with_offset(x0, x0 + 1,
				    y,
				    0.0, 0.0);

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  /* assert position */
  list = notation->notes;
  success = TRUE;
  
  for(i = 0; i < AGS_NOTATION_TEST_ADD_NOTE_COUNT; i++){
    if(list->prev != NULL){
      if(!(AGS_NOTE(list->prev->data)->x[0] < AGS_NOTE(list->data)->x[0] ||
	   (AGS_NOTE(list->prev->data)->x[0] == AGS_NOTE(list->data)->x[0] &&
	    (AGS_NOTE(list->prev->data)->y <= AGS_NOTE(list->data)->y)))){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
  CU_ASSERT(list == NULL);
}

void
ags_notation_test_remove_note_at_position()
{
  AgsNotation *notation;
  AgsNote *note;

  GList *list, *current;
  
  guint x0, y;
  guint nth;
  guint i;
  gboolean success;

  /* create notation */
  notation = ags_notation_new(audio,
			      0);

  for(i = 0; i < AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_COUNT; i++){
    x0 = rand() % AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_WIDTH;
    y = rand() % AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_HEIGHT;
    
    note = ags_note_new_with_offset(x0, x0 + 1,
				    y,
				    0.0, 0.0);

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  for(i = 0; i < AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_REMOVE_COUNT; i++){
    nth = rand() % (AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_COUNT - i);
    current = g_list_nth(notation->notes,
			 nth);
    
    ags_notation_remove_note_at_position(notation,
					 AGS_NOTE(current)->x[0],
					 AGS_NOTE(current)->y);
  }
  
  /* assert position */
  list = notation->notes;
  success = TRUE;
  
  for(i = 0; i < AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_COUNT - AGS_NOTATION_TEST_REMOVE_NOTE_AT_POSITION_REMOVE_COUNT; i++){
    if(list->prev != NULL){
      if(!(AGS_NOTE(list->prev->data)->x[0] < AGS_NOTE(list->data)->x[0] ||
	   (AGS_NOTE(list->prev->data)->x[0] == AGS_NOTE(list->data)->x[0] &&
	    (AGS_NOTE(list->prev->data)->y <= AGS_NOTE(list->data)->y)))){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
  CU_ASSERT(list == NULL);
}

void
ags_notation_test_is_note_selected()
{
  AgsNotation *notation;
  AgsNote *note;

  GList *list, *current;
  
  guint x0, y;
  guint nth;
  guint i;
  gboolean success;

  /* create notation */
  notation = ags_notation_new(audio,
			      0);

  for(i = 0; i < AGS_NOTATION_TEST_IS_NOTE_SELECTED_COUNT; i++){
    x0 = rand() % AGS_NOTATION_TEST_IS_NOTE_SELECTED_WIDTH;
    y = rand() % AGS_NOTATION_TEST_IS_NOTE_SELECTED_HEIGHT;
    
    note = ags_note_new_with_offset(x0, x0 + 1,
				    y,
				    0.0, 0.0);

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  /* select notes */
  for(i = 0; i < AGS_NOTATION_TEST_IS_NOTE_SELECTED_SELECTION_COUNT; i++){
    nth = rand() % (AGS_NOTATION_TEST_IS_NOTE_SELECTED_COUNT - i);
    current = g_list_nth(notation->notes,
			 nth);

    ags_notation_add_note(notation,
			  current->data,
			  TRUE);
  }

  /* assert position */
  list = notation->selection;
  success = TRUE;
  
  for(i = 0; i < AGS_NOTATION_TEST_IS_NOTE_SELECTED_SELECTION_COUNT; i++){
    if(list->prev != NULL){
      if(!(AGS_NOTE(list->prev->data)->x[0] < AGS_NOTE(list->data)->x[0] ||
	   (AGS_NOTE(list->prev->data)->x[0] == AGS_NOTE(list->data)->x[0] &&
	    (AGS_NOTE(list->prev->data)->y <= AGS_NOTE(list->data)->y)))){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
  CU_ASSERT(list == NULL);
}

void
ags_notation_test_find_point()
{
  AgsNotation *notation;
  AgsNote *note;

  GList *list, *current;
  
  guint x0, y;
  guint nth;
  guint i;
  gboolean success;

  /* create notation */
  notation = ags_notation_new(audio,
			      0);

  for(i = 0; i < AGS_NOTATION_TEST_FIND_POINT_COUNT; i++){
    x0 = rand() % AGS_NOTATION_TEST_FIND_POINT_WIDTH;
    y = rand() % AGS_NOTATION_TEST_FIND_POINT_HEIGHT;
    
    note = ags_note_new_with_offset(x0, x0 + 1,
				    y,
				    0.0, 0.0);

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  /* assert find point */
  success = TRUE;

  for(i = 0; i < AGS_NOTATION_TEST_FIND_POINT_N_ATTEMPTS; i++){
    nth = rand() % AGS_NOTATION_TEST_FIND_POINT_COUNT;
    current = g_list_nth(notation->notes,
			 nth);
    
    note = ags_notation_find_point(notation,
				   AGS_NOTE(current->data)->x[0],
				   AGS_NOTE(current->data)->y,
				   FALSE);

    if(note->x[0] != AGS_NOTE(current->data)->x[0] ||
       note->y != AGS_NOTE(current->data)->y){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_notation_test_find_region()
{
  AgsNotation *notation;
  AgsNote *note;

  GList *list, *current, *region;
  
  guint x0, y;
  guint nth;
  guint i;
  gboolean success;

  /* create notation */
  notation = ags_notation_new(audio,
			      0);

  for(i = 0; i < AGS_NOTATION_TEST_FIND_REGION_COUNT; i++){
    x0 = rand() % AGS_NOTATION_TEST_FIND_REGION_WIDTH;
    y = rand() % AGS_NOTATION_TEST_FIND_REGION_HEIGHT;
    
    note = ags_note_new_with_offset(x0, x0 + 1,
				    y,
				    0.0, 0.0);

    ags_notation_add_note(notation,
			  note,
			  FALSE);
  }

  /* assert find region */
  success = TRUE;

  for(i = 0;
      i < AGS_NOTATION_TEST_FIND_REGION_N_ATTEMPTS &&
	success;
      i++){
    nth = rand() % AGS_NOTATION_TEST_FIND_REGION_COUNT;
    current = g_list_nth(notation->notes,
			 nth);
    
    region = ags_notation_find_region(notation,
				      AGS_NOTE(current->data)->x[0],
				      AGS_NOTE(current->data)->y,
				      AGS_NOTE(current->data)->x[0] + AGS_NOTATION_TEST_FIND_REGION_SELECTION_WIDTH,
				      AGS_NOTE(current->data)->y + AGS_NOTATION_TEST_FIND_REGION_SELECTION_HEIGHT,
				      FALSE);
    while(region != NULL){
      if(!(AGS_NOTE(region->data)->x[0] >= AGS_NOTE(current->data)->x[0] &&
	   AGS_NOTE(current->data)->x[0] < AGS_NOTE(current->data)->x[0] + AGS_NOTATION_TEST_FIND_REGION_SELECTION_WIDTH &&
	   AGS_NOTE(region->data)->y >= AGS_NOTE(current->data)->y &&
	   AGS_NOTE(current->data)->y < AGS_NOTE(current->data)->y + AGS_NOTATION_TEST_FIND_REGION_SELECTION_HEIGHT)){
	success = FALSE;
	
	break;
      }
      
      region = region->next;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_notation_test_free_selection()
{
}

void
ags_notation_test_add_all_to_selection()
{
}

void
ags_notation_test_add_point_to_selection()
{
}

void
ags_notation_test_remove_point_from_selection()
{
}

void
ags_notation_test_add_region_to_selection()
{
}

void
ags_notation_test_remove_region_from_selection()
{
}

void
ags_notation_test_copy_selection()
{
}

void
ags_notation_test_cut_selection()
{
}

void
ags_notation_test_insert_from_clipboard()
{
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
  pSuite = CU_add_suite("AgsNotationTest\0", ags_notation_test_init_suite, ags_notation_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsNotation find near timestamp\0", ags_notation_test_find_near_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation add note\0", ags_notation_test_add_note) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation remove note at position\0", ags_notation_test_remove_note_at_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation is note selected\0", ags_notation_test_is_note_selected) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation find point\0", ags_notation_test_find_point) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation free selection\0", ags_notation_test_free_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation add all to selection\0", ags_notation_test_add_all_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation add point to selection\0", ags_notation_test_add_point_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation remove point from selection\0", ags_notation_test_remove_point_from_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation add region to selection\0", ags_notation_test_add_region_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation remove region from selection\0", ags_notation_test_remove_region_from_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation copy selection\0", ags_notation_test_copy_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation cut selection\0", ags_notation_test_cut_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation insert from clipboard\0", ags_notation_test_insert_from_clipboard) == NULL)){
    CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

