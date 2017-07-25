/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_file_id_ref_test_init_suite();
int ags_file_id_ref_test_clean_suite();

void ags_file_id_ref_test_resolved();

void ags_file_id_ref_test_stub_resolved(AgsFileIdRef *file_id_ref);

gboolean stub_resolved;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_id_ref_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_id_ref_test_clean_suite()
{
  return(0);
}

void
ags_file_id_ref_test_resolved()
{
  AgsFileIdRef *file_id_ref;

  gpointer ptr;
  
  file_id_ref = g_object_new(AGS_TYPE_FILE_ID_REF,
			     NULL);

  ptr = AGS_FILE_ID_REF_GET_CLASS(file_id_ref)->resolved;
  AGS_FILE_ID_REF_GET_CLASS(file_id_ref)->resolved = ags_file_id_ref_test_stub_resolved;

  /* assert resolved */
  ags_file_id_ref_resolved(file_id_ref);

  CU_ASSERT(stub_resolved == TRUE);
  AGS_FILE_ID_REF_GET_CLASS(file_id_ref)->resolved = ptr;
}

void
ags_file_id_ref_test_stub_resolved(AgsFileIdRef *file_id_ref)
{
  stub_resolved = TRUE;
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
  pSuite = CU_add_suite("AgsFileIdRefTest\0", ags_file_id_ref_test_init_suite, ags_file_id_ref_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFileIdRef resolved\0", ags_file_id_ref_test_resolved) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
