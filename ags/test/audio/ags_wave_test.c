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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

int ags_wave_test_init_suite();
int ags_wave_test_clean_suite();

void ags_wave_test_find_near_timestamp();
void ags_wave_test_add_buffer();
void ags_wave_test_remove_buffer();
void ags_wave_test_get_selection();
void ags_wave_test_is_buffer_selected();
void ags_wave_test_find_point();
void ags_wave_test_find_region();
void ags_wave_test_free_selection();
void ags_wave_test_add_region_to_selection();
void ags_wave_test_remove_region_from_selection();
void ags_wave_test_add_all_to_selection();
void ags_wave_test_copy_selection();
void ags_wave_test_cut_selection();
void ags_wave_test_insert_from_clipboard();
void ags_wave_test_insert_from_clipboard_extended();

#define AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_N_WAVE (8)
#define AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_SAMPLERATE (44100)

#define AGS_WAVE_TEST_ADD_BUFFER_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_ADD_BUFFER_COUNT (1024)

#define AGS_WAVE_TEST_REMOVE_BUFFER_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_REMOVE_BUFFER_COUNT (1024)
#define AGS_WAVE_TEST_REMOVE_BUFFER_REMOVE_COUNT (256)

#define AGS_WAVE_TEST_IS_BUFFER_SELECTED_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_IS_BUFFER_SELECTED_COUNT (1024)
#define AGS_WAVE_TEST_IS_BUFFER_SELECTED_SELECTION_COUNT (128)

#define AGS_WAVE_TEST_FIND_POINT_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_FIND_POINT_COUNT (1024)
#define AGS_WAVE_TEST_FIND_POINT_N_ATTEMPTS (128)

#define AGS_WAVE_TEST_FIND_REGION_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_FIND_REGION_COUNT (1024)
#define AGS_WAVE_TEST_FIND_REGION_N_ATTEMPTS (128)
#define AGS_WAVE_TEST_FIND_REGION_SELECTION_WIDTH (128)

#define AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_FREE_SELECTION_COUNT (1024)
#define AGS_WAVE_TEST_FREE_SELECTION_SELECTION_COUNT (64)

#define AGS_WAVE_TEST_ADD_ALL_TO_SELECTION_BUFFER_SIZE (1024)
#define AGS_WAVE_TEST_ADD_ALL_TO_SELECTION_COUNT (1024)

AgsAudio *audio;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_wave_test_init_suite()
{
  audio = ags_audio_new(NULL);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_wave_test_clean_suite()
{
  g_object_unref(audio);
  
  return(0);
}

void
ags_wave_test_find_near_timestamp()
{
  AgsWave **wave;
  AgsTimestamp *timestamp;

  GList *list, *current;

  guint64 relative_offset;
  guint i;
  
  wave = (AgsWave **) malloc(AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_N_WAVE * sizeof(AgsWave *));
  list = NULL;

  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_SAMPLERATE;

  for(i = 0; i < AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_N_WAVE; i++){
    /* nth wave */
    wave[i] = ags_wave_new(audio,
			   0);
    g_object_set(wave[i],
		 "samplerate", AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_SAMPLERATE,
		 NULL);
    timestamp = wave[i]->timestamp;

    timestamp->timer.ags_offset.offset = i * relative_offset;

    list = g_list_prepend(list,
			  wave[i]);
  }

  list = g_list_reverse(list);

  /* instantiate timestamp to check against */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  /* assert find */
  for(i = 0; i < AGS_WAVE_TEST_FIND_NEAR_TIMESTAMP_N_WAVE; i++){
    timestamp->timer.ags_offset.offset = i * relative_offset;
    current = ags_wave_find_near_timestamp(list, 0,
					   timestamp);

    CU_ASSERT(current != NULL && current->data == wave[i]);
  }  
}

void
ags_wave_test_add_buffer()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *current;

  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_ADD_BUFFER_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_ADD_BUFFER_COUNT; i++){
    x = i * AGS_WAVE_TEST_ADD_BUFFER_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_ADD_BUFFER_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }

  /* assert position */
  list = wave->buffer;
  success = TRUE;
  
  for(i = 0; i < AGS_WAVE_TEST_ADD_BUFFER_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_BUFFER(list->prev->data)->x < AGS_BUFFER(list->data)->x){
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
ags_wave_test_remove_buffer()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *current;
  
  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_REMOVE_BUFFER_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_REMOVE_BUFFER_COUNT; i++){
    x = i * AGS_WAVE_TEST_REMOVE_BUFFER_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_REMOVE_BUFFER_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }
  
  for(i = 0; i < AGS_WAVE_TEST_REMOVE_BUFFER_REMOVE_COUNT; i++){
    nth = rand() % (AGS_WAVE_TEST_REMOVE_BUFFER_COUNT - i);
    current = g_list_nth(wave->buffer,
			 nth);
    
    ags_wave_remove_buffer(wave,
			   current->data,
			   FALSE);
  }

  /* assert position */
  list = wave->buffer;
  success = TRUE;
  
  for(i = 0; i < AGS_WAVE_TEST_REMOVE_BUFFER_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_BUFFER(list->prev->data)->x < AGS_BUFFER(list->data)->x){
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
ags_wave_test_get_selection()
{
  //TODO:JK: implement me
}

void
ags_wave_test_is_buffer_selected()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *current;
  
  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_IS_BUFFER_SELECTED_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_IS_BUFFER_SELECTED_COUNT; i++){
    x = i * AGS_WAVE_TEST_IS_BUFFER_SELECTED_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_IS_BUFFER_SELECTED_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }

  /* select buffers */
  for(i = 0; i < AGS_WAVE_TEST_IS_BUFFER_SELECTED_COUNT; i++){
    nth = rand() % (AGS_WAVE_TEST_IS_BUFFER_SELECTED_COUNT - i);
    current = g_list_nth(wave->buffer,
			 nth);
    
    ags_wave_add_buffer(wave,
			current->data,
			TRUE);
  }

  /* assert position */
  list = wave->selection;
  success = TRUE;
  
  for(i = 0; i < AGS_WAVE_TEST_IS_BUFFER_SELECTED_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_BUFFER(list->prev->data)->x < AGS_BUFFER(list->data)->x){
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
ags_wave_test_find_point()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *current;

  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_FIND_POINT_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_FIND_POINT_COUNT; i++){
    x = i * AGS_WAVE_TEST_FIND_POINT_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_FIND_POINT_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_find_point(wave,
			buffer,
			FALSE);
  }

  /* assert find point */
  success = TRUE;

  for(i = 0; i < AGS_WAVE_TEST_FIND_POINT_N_ATTEMPTS; i++){
    nth = rand() % AGS_WAVE_TEST_FIND_POINT_COUNT;
    current = g_list_nth(wave->buffer,
			 nth);
    
    buffer = ags_wave_find_point(wave,
				 AGS_BUFFER(current->data)->x,
				 FALSE);
    
    if(buffer->x != AGS_BUFFER(current->data)->x){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_wave_test_find_region()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *region;
  GList *current;

  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_FIND_REGION_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_FIND_REGION_COUNT; i++){
    x = i * AGS_WAVE_TEST_FIND_REGION_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_FIND_REGION_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }

  /* assert find region */
  success = TRUE;

  for(i = 0;
      i < AGS_WAVE_TEST_FIND_REGION_N_ATTEMPTS &&
	success;
      i++){
    nth = rand() % AGS_WAVE_TEST_FIND_REGION_COUNT;
    current = g_list_nth(wave->buffer,
			 nth);
    
    region = ags_wave_find_region(wave,
				  AGS_BUFFER(current->data)->x,
				  AGS_BUFFER(current->data)->x + AGS_WAVE_TEST_FIND_REGION_SELECTION_WIDTH,
				  FALSE);
    while(region != NULL){
      if(!(AGS_BUFFER(region->data)->x >= AGS_BUFFER(current->data)->x &&
	   AGS_BUFFER(current->data)->x < AGS_BUFFER(current->data)->x + AGS_WAVE_TEST_FIND_REGION_SELECTION_WIDTH)){
	success = FALSE;
	
	break;
      }
      
      region = region->next;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_wave_test_free_selection()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list;
  GList *current;

  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_FREE_SELECTION_COUNT; i++){
    x = i * AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }

  /* select buffers */
  for(i = 0; i < AGS_WAVE_TEST_FREE_SELECTION_COUNT; i++){
    nth = rand() % (AGS_WAVE_TEST_FREE_SELECTION_COUNT - i);
    current = g_list_nth(wave->buffer,
			 nth);
    
    ags_wave_add_buffer(wave,
			current->data,
			TRUE);
  }

  /* assert free slection */
  CU_ASSERT(wave->selection != NULL);

  ags_wave_free_selection(wave);
  
  CU_ASSERT(wave->selection == NULL);
}

void
ags_wave_test_add_region_to_selection()
{
  //TODO:JK: implement me
}

void
ags_wave_test_remove_region_from_selection()
{
  //TODO:JK: implement me
}

void
ags_wave_test_add_all_to_selection()
{
  AgsWave *wave;
  AgsBuffer *buffer;

  GList *list, *current, *current_selection;
  
  guint64 x;
  guint nth;
  guint i;
  gboolean success;

  /* create wave */
  wave = ags_wave_new(audio,
		      0);
  g_object_set(wave,
	       "buffer-size", AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE,
	       NULL);
  
  for(i = 0; i < AGS_WAVE_TEST_FREE_SELECTION_COUNT; i++){
    x = i * AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE;
    
    buffer = ags_buffer_new();
    g_object_set(buffer,
		 "buffer-size", AGS_WAVE_TEST_FREE_SELECTION_BUFFER_SIZE,
		 "x", x, 
		 NULL);
    
    ags_wave_add_buffer(wave,
			buffer,
			FALSE);
  }

  /* select all */
  ags_wave_add_all_to_selection(wave);

  /* assert all present */
  current = wave->buffer;
  current_selection = wave->selection;

  success = TRUE;
  
  while(current != NULL &&
	current_selection != NULL){
    if(current->data != current_selection->data){
      success = FALSE;
      
      break;
    }
    
    current = current->next;
    current_selection = current_selection->next;
  }

  CU_ASSERT(current == NULL);
  CU_ASSERT(current_selection == NULL);
  CU_ASSERT(success == TRUE);
}

void
ags_wave_test_copy_selection()
{
  //TODO:JK: implement me
}

void
ags_wave_test_cut_selection()
{
  //TODO:JK: implement me
}

void
ags_wave_test_insert_from_clipboard()
{
  //TODO:JK: implement me
}

void
ags_wave_test_insert_from_clipboard_extended()
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
  pSuite = CU_add_suite("AgsWaveTest", ags_wave_test_init_suite, ags_wave_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsWave find near timestamp", ags_wave_test_find_near_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave add buffer", ags_wave_test_add_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave remove buffer", ags_wave_test_remove_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave get selection", ags_wave_test_get_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave is buffer selected", ags_wave_test_is_buffer_selected) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave find point", ags_wave_test_find_point) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave find region", ags_wave_test_find_region) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave free selection", ags_wave_test_free_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave add region to selection", ags_wave_test_add_region_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave remove region from selection", ags_wave_test_remove_region_from_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave add all to selection", ags_wave_test_add_all_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave copy selection", ags_wave_test_copy_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave cut selection", ags_wave_test_cut_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave insert from clipboard", ags_wave_test_insert_from_clipboard) == NULL) ||
     (CU_add_test(pSuite, "test of AgsWave insert from clipboard extended", ags_wave_test_insert_from_clipboard_extended) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
