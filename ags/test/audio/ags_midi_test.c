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

int ags_midi_test_init_suite();
int ags_midi_test_clean_suite();

void ags_midi_test_find_near_timestamp();
void ags_midi_test_add_track();
void ags_midi_test_remove_track();

#define AGS_MIDI_TEST_FIND_NEAR_TIMESTAMP_N_MIDI (8)

#define AGS_MIDI_TEST_ADD_TRACK_COUNT (1024)

#define AGS_MIDI_TEST_REMOVE_TRACK_TRACK_SIZE (64)
#define AGS_MIDI_TEST_REMOVE_TRACK_COUNT (1024)
#define AGS_MIDI_TEST_REMOVE_TRACK_REMOVE_COUNT (256)

AgsAudio *audio;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_test_init_suite()
{
  audio = ags_audio_new(NULL);
  g_object_ref(audio);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_test_clean_suite()
{
  g_object_unref(audio);
  
  return(0);
}

void
ags_midi_test_find_near_timestamp()
{
  AgsMidi **midi;
  AgsTimestamp *timestamp;

  GList *list, *current;

  guint i;
  
  midi = (AgsMidi **) malloc(AGS_MIDI_TEST_FIND_NEAR_TIMESTAMP_N_MIDI * sizeof(AgsMidi *));
  list = NULL;

  for(i = 0; i < AGS_MIDI_TEST_FIND_NEAR_TIMESTAMP_N_MIDI; i++){
    /* nth midi */
    midi[i] = ags_midi_new(audio,
			   0);
    timestamp = midi[i]->timestamp;

    timestamp->timer.ags_offset.offset = i * AGS_MIDI_DEFAULT_OFFSET;

    list = g_list_prepend(list,
			  midi[i]);
  }

  list = g_list_reverse(list);

  /* instantiate timestamp to check against */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  /* assert find */
  for(i = 0; i < AGS_MIDI_TEST_FIND_NEAR_TIMESTAMP_N_MIDI; i++){
    timestamp->timer.ags_offset.offset = i * AGS_MIDI_DEFAULT_OFFSET;
    current = ags_midi_find_near_timestamp(list, 0,
					   timestamp);

    CU_ASSERT(current != NULL && current->data == midi[i]);
  }  
}

void
ags_midi_test_add_track()
{
  AgsMidi *midi;
  AgsTrack *track;

  GList *list;
  
  guint64 x;
  guint i;
  gboolean success;

  /* create midi */
  midi = ags_midi_new(audio,
			      0);

  for(i = 0; i < AGS_MIDI_TEST_ADD_TRACK_COUNT; i++){
    x = i * AGS_MIDI_DEFAULT_OFFSET;
    
    track = ags_track_new();
    g_object_set(track,
		 "x", x,
		 NULL);
    
    ags_midi_add_track(midi,
		       track,
		       FALSE);
  }

  /* assert position */
  list = midi->track;
  success = TRUE;
  
  for(i = 0; i < AGS_MIDI_TEST_ADD_TRACK_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_TRACK(list->prev->data)->x > AGS_TRACK(list->data)->x){
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
ags_midi_test_remove_track()
{
  AgsMidi *midi;
  AgsTrack *track;

  GList *list;
  GList *current;
  
  guint64 nth;
  guint64 x;
  guint i;
  gboolean success;
  
  /* create midi */
  midi = ags_midi_new(audio,
		      0);
  g_object_set(midi,
	       NULL);
  
  for(i = 0; i < AGS_MIDI_TEST_REMOVE_TRACK_COUNT; i++){
    x = i * AGS_MIDI_TEST_REMOVE_TRACK_TRACK_SIZE;
    
    track = ags_track_new();
    g_object_set(track,
		 "x", x, 
		 NULL);
    
    ags_midi_add_track(midi,
			track,
			FALSE);
  }
  
  for(i = 0; i < AGS_MIDI_TEST_REMOVE_TRACK_REMOVE_COUNT; i++){
    nth = rand() % (AGS_MIDI_TEST_REMOVE_TRACK_COUNT - i);
    current = g_list_nth(midi->track,
			 nth);
    
    ags_midi_remove_track(midi,
			   current->data,
			   FALSE);
  }

  /* assert position */
  list = midi->track;
  success = TRUE;
  
  for(i = 0; i < AGS_MIDI_TEST_REMOVE_TRACK_COUNT - AGS_MIDI_TEST_REMOVE_TRACK_REMOVE_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_TRACK(list->prev->data)->x > AGS_TRACK(list->data)->x){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
  CU_ASSERT(list == NULL);
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
  pSuite = CU_add_suite("AgsMidiTest", ags_midi_test_init_suite, ags_midi_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMidi find near timestamp", ags_midi_test_find_near_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidi add track", ags_midi_test_add_track) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidi remove track", ags_midi_test_remove_track) == NULL)){
    CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

