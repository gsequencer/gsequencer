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
void ags_notation_test_get_selection();
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
void ags_notation_test_get_current();

AgsApplicationContext *application_context;
AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_notation_test_init_suite()
{
  application_context = ags_application_context_new(NULL,
						    NULL);
  
  devout = ags_devout_new(NULL);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_notation_test_clean_suite()
{
  g_object_unref(devout);
  g_object_unref(application_context);
  
  return(0);
}

void
ags_notation_test_find_near_timestamp()
{
}

void
ags_notation_test_add_note()
{
}

void
ags_notation_test_remove_note_at_position()
{
}

void
ags_notation_test_get_selection()
{
}

void
ags_notation_test_is_note_selected()
{
}

void
ags_notation_test_find_point()
{
}

void
ags_notation_test_find_region()
{
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

void
ags_notation_test_get_current()
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
     (CU_add_test(pSuite, "test of AgsNotation get selection\0", ags_notation_test_get_selection) == NULL) ||
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
     (CU_add_test(pSuite, "test of AgsNotation insert from clipboard\0", ags_notation_test_insert_from_clipboard) == NULL) ||
     (CU_add_test(pSuite, "test of AgsNotation get current\0", ags_notation_test_get_current) == NULL)){
    CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

