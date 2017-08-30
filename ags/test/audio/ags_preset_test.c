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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_preset_test_init_suite();
int ags_preset_test_clean_suite();

void ags_preset_test_find_scope();
void ags_preset_test_find_name();
void ags_preset_test_add_parameter();
void ags_preset_test_remove_parameter();
void ags_preset_test_get_parameter();

#define AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X "attack-x"
#define AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X_VALUE (128)

#define AGS_PRESET_TEST_REMOVE_PARAMETER_ATTACK_X "attack-x"
#define AGS_PRESET_TEST_REMOVE_PARAMETER_ATTACK_X_VALUE (128)
#define AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X "decay-x"
#define AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X_VALUE (256)

#define AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X "attack-x"
#define AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X_VALUE (128)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_preset_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_preset_test_clean_suite()
{  
  return(0);
}

void
ags_preset_test_find_scope()
{
  AgsPreset *preset[3];

  GList *list, *current;

  list = NULL;

  /* preset #0 */
  preset[0] = g_object_new(AGS_TYPE_PRESET,
			   "scope", "ags-preset-test-scope-0",
			   NULL);
  list = g_list_prepend(list,
			preset[0]);
  
  /* preset #1 */
  preset[1] = g_object_new(AGS_TYPE_PRESET,
			   "scope", "ags-preset-test-scope-1",
			   NULL);
  list = g_list_prepend(list,
			preset[1]);

  /* preset #2 */
  preset[2] = g_object_new(AGS_TYPE_PRESET,
			   "scope", "ags-preset-test-scope-2",
			   NULL);
  list = g_list_prepend(list,
			preset[2]);

  /* assert */
  CU_ASSERT((current = ags_preset_find_scope(list,
					     "ags-preset-test-scope-0")) != NULL &&
	    current->data == preset[0]);

  CU_ASSERT((current = ags_preset_find_scope(list,
					     "ags-preset-test-scope-1")) != NULL &&
	    current->data == preset[1]);

  CU_ASSERT((current = ags_preset_find_scope(list,
					     "ags-preset-test-scope-2")) != NULL &&
	    current->data == preset[2]);
}

void
ags_preset_test_find_name()
{
  AgsPreset *preset[3];

  GList *list, *current;

  list = NULL;

  /* preset #0 */
  preset[0] = g_object_new(AGS_TYPE_PRESET,
			   "preset-name", "ags-preset-test-name-0",
			   NULL);
  list = g_list_prepend(list,
			preset[0]);
  
  /* preset #1 */
  preset[1] = g_object_new(AGS_TYPE_PRESET,
			   "preset-name", "ags-preset-test-name-1",
			   NULL);
  list = g_list_prepend(list,
			preset[1]);

  /* preset #2 */
  preset[2] = g_object_new(AGS_TYPE_PRESET,
			   "preset-name", "ags-preset-test-name-2",
			   NULL);
  list = g_list_prepend(list,
			preset[2]);

  /* assert */
  CU_ASSERT((current = ags_preset_find_name(list,
					     "ags-preset-test-name-0")) != NULL &&
	    current->data == preset[0]);

  CU_ASSERT((current = ags_preset_find_name(list,
					     "ags-preset-test-name-1")) != NULL &&
	    current->data == preset[1]);

  CU_ASSERT((current = ags_preset_find_name(list,
					     "ags-preset-test-name-2")) != NULL &&
	    current->data == preset[2]);
}

void
ags_preset_test_add_parameter()
{
  AgsPreset *preset;

  GValue value = {0,};
  
  preset = g_object_new(AGS_TYPE_PRESET,
			NULL);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X_VALUE);

  ags_preset_add_parameter(preset,
			   AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X, &value);

  CU_ASSERT(preset->n_params == 1 &&
	    !g_strcmp0(preset->parameter[0].name,
		       AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X) &&
	    g_value_get_uint(&preset->parameter[0].value) == AGS_PRESET_TEST_ADD_PARAMETER_ATTACK_X_VALUE);
}

void
ags_preset_test_remove_parameter()
{
  AgsPreset *preset;

  GValue value = {0,};
  
  preset = g_object_new(AGS_TYPE_PRESET,
			NULL);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   AGS_PRESET_TEST_REMOVE_PARAMETER_ATTACK_X_VALUE);
  ags_preset_add_parameter(preset,
			   AGS_PRESET_TEST_REMOVE_PARAMETER_ATTACK_X, &value);

  g_value_reset(&value);
  g_value_set_uint(&value,
		   AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X_VALUE);
  ags_preset_add_parameter(preset,
			   AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X, &value);

  /* remove and assert */
  ags_preset_remove_parameter(preset,
			      0);
  
  CU_ASSERT(preset->n_params == 1 &&
	    !g_strcmp0(preset->parameter[0].name,
		       AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X) &&
	    g_value_get_uint(&preset->parameter[0].value) == AGS_PRESET_TEST_REMOVE_PARAMETER_DECAY_X_VALUE);

  ags_preset_remove_parameter(preset,
			      0);

  CU_ASSERT(preset->n_params == 0);
}

void
ags_preset_test_get_parameter()
{
  AgsPreset *preset;

  GValue value = {0,};
  GValue ret_value = {0,};

  GError *error;
  
  preset = g_object_new(AGS_TYPE_PRESET,
			NULL);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X_VALUE);

  ags_preset_add_parameter(preset,
			   AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X, &value);

  /* get and assert */
  g_value_init(&ret_value,
	       G_TYPE_UINT);

  error = NULL;
  ags_preset_get_parameter(preset,
			   AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X, &ret_value,
			   &error);

  CU_ASSERT(error == NULL &&
	    g_value_get_uint(&ret_value) == AGS_PRESET_TEST_GET_PARAMETER_ATTACK_X_VALUE);
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
  pSuite = CU_add_suite("AgsPresetTest", ags_preset_test_init_suite, ags_preset_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPreset find scope", ags_preset_test_find_scope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPreset find name", ags_preset_test_find_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPreset add parameter", ags_preset_test_add_parameter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPreset remove parameter", ags_preset_test_remove_parameter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPreset get parameter", ags_preset_test_get_parameter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

