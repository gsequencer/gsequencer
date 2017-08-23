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

int ags_lv2_urid_manager_test_init_suite();
int ags_lv2_urid_manager_test_clean_suite();

void ags_lv2_urid_manager_test_insert();
void ags_lv2_urid_manager_test_remove();
void ags_lv2_urid_manager_test_lookup();
void ags_lv2_urid_manager_test_map();
void ags_lv2_urid_manager_test_unmap();

gboolean ags_lv2_urid_manager_test_map_ghrfunc(gpointer key,
					       gpointer value,
					       gpointer user_data);

#define AGS_LV2_URID_MANAGER_TEST_INSERT_URI "http://drobilla.net/plugins/mda/EPiano"

#define AGS_LV2_URID_MANAGER_TEST_REMOVE_URI "http://drobilla.net/plugins/mda/EPiano"

#define AGS_LV2_URID_MANAGER_TEST_LOOKUP_URI "http://drobilla.net/plugins/mda/EPiano"

#define AGS_LV2_URID_MANAGER_TEST_MAP_URI "http://drobilla.net/plugins/mda/EPiano"

#define AGS_LV2_URID_MANAGER_TEST_UNMAP_URI "http://drobilla.net/plugins/mda/EPiano"

/* Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_urid_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_urid_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2_urid_manager_test_insert()
{
  AgsLv2UridManager *urid_manager;
  
  GValue value;

  urid_manager = ags_lv2_urid_manager_get_instance();

  /* insert and assert */
  ags_lv2_urid_manager_insert(urid_manager,
			      AGS_LV2_URID_MANAGER_TEST_INSERT_URI, &value);

  CU_ASSERT(g_hash_table_lookup(urid_manager->urid,
				AGS_LV2_URID_MANAGER_TEST_INSERT_URI) == &value);
  
  g_object_unref(urid_manager);
}

void
ags_lv2_urid_manager_test_remove()
{
  AgsLv2UridManager *urid_manager;
  
  GValue value;

  urid_manager = ags_lv2_urid_manager_get_instance();

  ags_lv2_urid_manager_insert(urid_manager,
			      AGS_LV2_URID_MANAGER_TEST_REMOVE_URI, &value);

  /* remove and assert */
  ags_lv2_urid_manager_remove(urid_manager,
			      AGS_LV2_URID_MANAGER_TEST_REMOVE_URI);

  CU_ASSERT(g_hash_table_lookup(urid_manager->urid,
				AGS_LV2_URID_MANAGER_TEST_REMOVE_URI) == NULL);
  
  g_object_unref(urid_manager);
}

void
ags_lv2_urid_manager_test_lookup()
{
  AgsLv2UridManager *urid_manager;
  
  GValue value;

  urid_manager = ags_lv2_urid_manager_get_instance();

  /* lookup and assert */
  ags_lv2_urid_manager_lookup(urid_manager,
			      AGS_LV2_URID_MANAGER_TEST_LOOKUP_URI);

  CU_ASSERT(g_hash_table_lookup(urid_manager->urid,
				AGS_LV2_URID_MANAGER_TEST_LOOKUP_URI) != NULL);
  
  g_object_unref(urid_manager);
}

void
ags_lv2_urid_manager_test_map()
{
  AgsLv2UridManager *urid_manager;

  uint32_t id;
    
  GValue value = {0,};

  urid_manager = ags_lv2_urid_manager_get_instance();

  id = ags_lv2_urid_manager_map(NULL,
				AGS_LV2_URID_MANAGER_TEST_MAP_URI);

  g_value_init(&value,
	       G_TYPE_ULONG);
  g_value_set_ulong(&value,
		    id);

  CU_ASSERT(g_hash_table_find(urid_manager->urid,
			      ags_lv2_urid_manager_test_map_ghrfunc,
			      &value) != NULL);

  g_object_unref(urid_manager);
}

void
ags_lv2_urid_manager_test_unmap()
{
  AgsLv2UridManager *urid_manager;

  uint32_t id;
    
  GValue value = {0,};

  urid_manager = ags_lv2_urid_manager_get_instance();

  id = ags_lv2_urid_manager_map(NULL,
				AGS_LV2_URID_MANAGER_TEST_UNMAP_URI);

  g_value_init(&value,
	       G_TYPE_ULONG);
  g_value_set_ulong(&value,
		    id);

  ags_lv2_urid_manager_unmap(NULL,
			     id);

  CU_ASSERT(g_hash_table_lookup(urid_manager->urid,
				AGS_LV2_URID_MANAGER_TEST_UNMAP_URI) == NULL);

  g_object_unref(urid_manager);
}

gboolean
ags_lv2_urid_manager_test_map_ghrfunc(gpointer key,
				      gpointer value,
				      gpointer user_data)
{
  if(value != NULL &&
     user_data != NULL &&
     g_value_get_ulong((GValue *) value) == g_value_get_ulong((GValue *) user_data)){
    return(TRUE);
  }
  
  return(FALSE);
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
  pSuite = CU_add_suite("AgsLv2UridManagerTest", ags_lv2_urid_manager_test_init_suite, ags_lv2_urid_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2UridManager insert", ags_lv2_urid_manager_test_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2UridManager remove", ags_lv2_urid_manager_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2UridManager lookup", ags_lv2_urid_manager_test_lookup) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2UridManager map", ags_lv2_urid_manager_test_map) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2UridManager unmap", ags_lv2_urid_manager_test_unmap) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
