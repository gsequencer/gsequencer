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

int ags_lv2_option_manager_test_init_suite();
int ags_lv2_option_manager_test_clean_suite();

void ags_lv2_option_manager_test_ressource_insert();
void ags_lv2_option_manager_test_ressource_remove();
void ags_lv2_option_manager_test_ressource_lookup();
void ags_lv2_option_manager_test_get_option();
void ags_lv2_option_manager_test_set_option();
void ags_lv2_option_manager_test_lv2_options_get();
void ags_lv2_option_manager_test_lv2_options_set();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_option_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_option_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2_option_manager_test_ressource_insert()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
  
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  CU_ASSERT(g_hash_table_lookup(lv2_option_manager->ressource,
				lv2_option_ressource) == &value);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_ressource_remove()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
    
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  ags_lv2_option_manager_ressource_remove(lv2_option_manager,
					  lv2_option_ressource);
  
  CU_ASSERT(g_hash_table_lookup(lv2_option_manager->ressource,
				lv2_option_ressource) == NULL);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_ressource_lookup()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
    
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  CU_ASSERT(ags_lv2_option_manager_ressource_lookup(lv2_option_manager,
						    lv2_option_ressource) == &value);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_get_option()
{
  //TODO:JK: implement me
}

void
ags_lv2_option_manager_test_set_option()
{
  //TODO:JK: implement me
}

void
ags_lv2_option_manager_test_lv2_options_get()
{
  //TODO:JK: implement me
}

void
ags_lv2_option_manager_test_lv2_options_set()
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
  pSuite = CU_add_suite("AgsLv2OptionManagerTest\0", ags_lv2_option_manager_test_init_suite, ags_lv2_option_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2OptionManager ressource insert\0", ags_lv2_option_manager_test_ressource_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager ressource remove\0", ags_lv2_option_manager_test_ressource_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager ressource lookup\0", ags_lv2_option_manager_test_ressource_lookup) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager get option\0", ags_lv2_option_manager_test_get_option) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager set option\0", ags_lv2_option_manager_test_set_option) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager lv2 options get\0", ags_lv2_option_manager_test_lv2_options_get) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager lv2 options set\0", ags_lv2_option_manager_test_lv2_options_set) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
