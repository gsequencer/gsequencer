/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

int ags_timestamp_test_init_suite();
int ags_timestamp_test_clean_suite();

void ags_timestamp_test_test_flags();
void ags_timestamp_test_set_flags();
void ags_timestamp_test_unset_flags();
void ags_timestamp_test_set_ags_offset();
void ags_timestamp_test_get_ags_offset();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_timestamp_test_init_suite()
{    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_timestamp_test_clean_suite()
{
  
  return(0);
}

void
ags_timestamp_test_test_flags()
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  timestamp->flags = AGS_TIMESTAMP_OFFSET;

  CU_ASSERT(ags_timestamp_test_flags(timestamp, AGS_TIMESTAMP_OFFSET) == TRUE);
}

void
ags_timestamp_test_set_flags()
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  timestamp->flags = 0;

  ags_timestamp_set_flags(timestamp, AGS_TIMESTAMP_OFFSET);
  
  CU_ASSERT((AGS_TIMESTAMP_OFFSET & (timestamp->flags)) != 0);
}

void
ags_timestamp_test_unset_flags()
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  timestamp->flags = AGS_TIMESTAMP_OFFSET;

  ags_timestamp_unset_flags(timestamp, AGS_TIMESTAMP_OFFSET);
  
  CU_ASSERT((AGS_TIMESTAMP_OFFSET & (timestamp->flags)) == 0);
}

void
ags_timestamp_test_set_ags_offset()
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  timestamp->flags = AGS_TIMESTAMP_OFFSET;

  ags_timestamp_set_ags_offset(timestamp,
			       8192);

  CU_ASSERT(timestamp->timer.ags_offset.offset == 8192);
}

void
ags_timestamp_test_get_ags_offset()
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  timestamp->flags = AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = 8192;
  
  CU_ASSERT(ags_timestamp_get_ags_offset(timestamp) == 8192);
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
  pSuite = CU_add_suite("AgsTimestampTest", ags_timestamp_test_init_suite, ags_timestamp_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTimestamp test flags", ags_timestamp_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTimestamp set flags", ags_timestamp_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTimestamp unset flags", ags_timestamp_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTimestamp set ags offset", ags_timestamp_test_set_ags_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTimestamp get ags offset", ags_timestamp_test_get_ags_offset) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
