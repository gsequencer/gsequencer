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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_track_test_init_suite();
int ags_track_test_clean_suite();

void ags_track_test_duplicate();

#define AGS_TRACK_TEST_DUPLICATE_X_0 (17)
#define AGS_TRACK_TEST_DUPLICATE_X_1 (3 * AGS_MIDI_DEFAULT_OFFSET + 17)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_track_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_track_test_clean_suite()
{
  return(0);
}

void
ags_track_test_duplicate()
{
  AgsTrack *track, *copy_track;

  /* create track - #0 */
  track = g_object_new(AGS_TYPE_TRACK,
			"x", (guint64) AGS_TRACK_TEST_DUPLICATE_X_0,
			NULL);
  
  /* assert duplicate - #0 */
  copy_track = ags_track_duplicate(track);

  CU_ASSERT(AGS_IS_TRACK(copy_track) &&
	    copy_track->x == track->x);

  /* create track - #1 */
  track = g_object_new(AGS_TYPE_TRACK,
			"x", (guint64) AGS_TRACK_TEST_DUPLICATE_X_1,
			NULL);

  /* assert duplicate - #1 */
  copy_track = ags_track_duplicate(track);

  CU_ASSERT(AGS_IS_TRACK(copy_track) &&
	    copy_track->x == track->x);
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
  pSuite = CU_add_suite("AgsTrackTest", ags_track_test_init_suite, ags_track_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTrack duplicate", ags_track_test_duplicate) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
