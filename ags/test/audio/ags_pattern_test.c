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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_timestamp.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_pattern.h>

int ags_pattern_test_init_suite();
int ags_pattern_test_clean_suite();

void ags_pattern_test_find_near_timestamp();
void ags_pattern_test_set_dim();
void ags_pattern_test_get_bit();
void ags_pattern_test_toggle_bit();

#define AGS_PATTERN_TEST_FIND_NEAR_TIMESTAMP_N_PATTERN (8)

#define AGS_PATTERN_TEST_SET_DIM_BANK_0 (16)
#define AGS_PATTERN_TEST_SET_DIM_BANK_1 (4)
#define AGS_PATTERN_TEST_SET_DIM_BIT (64)

#define AGS_PATTERN_TEST_GET_BIT_BANK_0 (16)
#define AGS_PATTERN_TEST_GET_BIT_BANK_1 (4)
#define AGS_PATTERN_TEST_GET_BIT_BIT (256)

#define AGS_PATTERN_TEST_TOGGLE_BIT_BANK_0 (16)
#define AGS_PATTERN_TEST_TOGGLE_BIT_BANK_1 (4)
#define AGS_PATTERN_TEST_TOGGLE_BIT_BIT (256)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_pattern_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_pattern_test_clean_suite()
{
  return(0);
}

void
ags_pattern_test_find_near_timestamp()
{
  AgsPattern **pattern;
  AgsTimestamp *timestamp;

  GList *list, *current;

  guint i;
  
  pattern = (AgsPattern **) malloc(AGS_PATTERN_TEST_FIND_NEAR_TIMESTAMP_N_PATTERN * sizeof(AgsPattern *));
  list = NULL;

  for(i = 0; i < AGS_PATTERN_TEST_FIND_NEAR_TIMESTAMP_N_PATTERN; i++){
    /* nth pattern */
    pattern[i] = ags_pattern_new();
    timestamp = ags_timestamp_new();
    g_object_set(pattern[i],
		 "timestamp\0", timestamp,
		 NULL);

    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(pattern[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_PATTERN_DEFAULT_DURATION);

    list = g_list_prepend(list,
			  pattern[i]);
  }

  /* instantiate timestamp to check against */
  timestamp = ags_timestamp_new();
  timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(pattern[0]->timestamp)->timer.unix_time.time_val;
  
  /* assert find */
  for(i = 0; i + 1 < AGS_PATTERN_TEST_FIND_NEAR_TIMESTAMP_N_PATTERN; i++){
    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(pattern[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_PATTERN_DEFAULT_DURATION + 1);
    current = ags_pattern_find_near_timestamp(list,
					      timestamp);

    CU_ASSERT(current != NULL && current->data == pattern[i + 1]);
  }  
}

void
ags_pattern_test_set_dim()
{
  AgsPattern *pattern;
  
  guint i, j;
  gboolean success;
  
  pattern = ags_pattern_new();
  ags_pattern_set_dim(pattern,
		      AGS_PATTERN_TEST_SET_DIM_BANK_0,
		      AGS_PATTERN_TEST_SET_DIM_BANK_1,
		      AGS_PATTERN_TEST_SET_DIM_BIT);
  
  /* assert dim */
  CU_ASSERT(pattern->dim[0] == AGS_PATTERN_TEST_SET_DIM_BANK_0);
  CU_ASSERT(pattern->dim[1] == AGS_PATTERN_TEST_SET_DIM_BANK_1);
  CU_ASSERT(pattern->dim[2] == AGS_PATTERN_TEST_SET_DIM_BIT);
  
  /* assert pattern */
  if(pattern->pattern != NULL){
    success = TRUE;
  }else{
    success = FALSE;
  }
  
  for(i = 0; i < AGS_PATTERN_TEST_SET_DIM_BANK_0 &&
	success;
      i++){
    if(pattern->pattern[i] == NULL){
      success = FALSE;

      break;
    }
    
    for(j = 0; j < AGS_PATTERN_TEST_SET_DIM_BANK_1; j++){
      if(pattern->pattern[i][j] == NULL){
	success = FALSE;
	
	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_pattern_test_get_bit()
{
  AgsPattern *pattern;

  guint ***bitmap;
  guint i, j, k;
  
  gboolean success;

  /* instantiate pattern */
  pattern = ags_pattern_new();
  ags_pattern_set_dim(pattern,
		      AGS_PATTERN_TEST_GET_BIT_BANK_0,
		      AGS_PATTERN_TEST_GET_BIT_BANK_1,
		      AGS_PATTERN_TEST_GET_BIT_BIT);

  /* create and fill bitmap to check against */
  bitmap = (guint ***) malloc(AGS_PATTERN_TEST_GET_BIT_BANK_0 * sizeof(guint **));
  
  for(i = 0; i < AGS_PATTERN_TEST_GET_BIT_BANK_0; i++){
    bitmap[i] = (guint **) malloc(AGS_PATTERN_TEST_GET_BIT_BANK_1 * sizeof(guint *));
    
    for(j = 0; j < AGS_PATTERN_TEST_GET_BIT_BANK_1; j++){
      bitmap[i][j] = (guint *) malloc((int) ceil((double) AGS_PATTERN_TEST_GET_BIT_BIT / (double) (sizeof(guint) * 8)) * sizeof(guint));
      memset(bitmap[i][j], 0, (int) ceil((double) AGS_PATTERN_TEST_GET_BIT_BIT / (double) (sizeof(guint) * 8)) * sizeof(guint));
  
      for(k = 0; k < AGS_PATTERN_TEST_GET_BIT_BIT; k++){
	if(rand() % 2 == 1){
	  bitmap[i][j][(guint) floor((double) k / (double) (sizeof(guint) * 8))] |= (1 << (k % (sizeof(guint) * 8)));
	}
      }

      for(k = 0; k < (int) ceil((double) AGS_PATTERN_TEST_GET_BIT_BIT / (double) (sizeof(guint) * 8)); k++){
	pattern->pattern[i][j][k] = bitmap[i][j][k];
      }
    }
  }
  
  /* assert values to be present */
  for(i = 0; i < AGS_PATTERN_TEST_GET_BIT_BANK_0 &&
	success;
      i++){    
    for(j = 0; j < AGS_PATTERN_TEST_GET_BIT_BANK_1 &&
	  success;
	j++){
      for(k = 0; k < AGS_PATTERN_TEST_GET_BIT_BIT; k++){
	if(((1 << (k % (sizeof(guint) * 8))) & bitmap[i][j][(guint) floor((double) k / (double) (sizeof(guint) * 8))]) != 0){
	  success = (ags_pattern_get_bit(pattern, i, j, k) == TRUE);
	}else{
	  success = (ags_pattern_get_bit(pattern, i, j, k) == FALSE);
	}
	
	if(!success){
	  break;
	}
      }
    }
  }

  CU_ASSERT(success = TRUE);
}

void
ags_pattern_test_toggle_bit()
{
  AgsPattern *pattern;

  guint i, j, k;
  gboolean success;
  
  /* instantiate pattern */
  pattern = ags_pattern_new();
  ags_pattern_set_dim(pattern,
		      AGS_PATTERN_TEST_GET_BIT_BANK_0,
		      AGS_PATTERN_TEST_GET_BIT_BANK_1,
		      AGS_PATTERN_TEST_TOGGLE_BIT_BIT);

  /* assert values to be present */
  for(i = 0; i < AGS_PATTERN_TEST_TOGGLE_BIT_BANK_0; i++){
    for(j = 0; j < AGS_PATTERN_TEST_TOGGLE_BIT_BANK_1; j++){
      for(k = 0; k < AGS_PATTERN_TEST_GET_BIT_BIT; k++){
	if(rand() % 2 == 1){
	  ags_pattern_toggle_bit(pattern,
				 i,
				 j,
				 k);
      
	  success = (ags_pattern_get_bit(pattern, i, j, k) == TRUE);
	}else{
	  ags_pattern_toggle_bit(pattern,
				 i,
				 j,
				 k);
	  ags_pattern_toggle_bit(pattern,
				 i,
				 j,
				 k);
      
	  success = (ags_pattern_get_bit(pattern, i, j, k) == FALSE);
	}
      }  
    }
  }
  
  CU_ASSERT(success = TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsPatternTest\0", ags_pattern_test_init_suite, ags_pattern_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPattern find near timestamp\0", ags_pattern_test_find_near_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPattern set dim\0", ags_pattern_test_set_dim) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPattern get bit\0", ags_pattern_test_get_bit) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPattern toggle bit\0", ags_pattern_test_toggle_bit) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

