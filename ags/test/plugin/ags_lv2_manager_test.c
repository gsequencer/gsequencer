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

int ags_lv2_manager_test_init_suite();
int ags_lv2_manager_test_clean_suite();

void ags_lv2_manager_test_get_filenames();
void ags_lv2_manager_test_find_lv2_plugin();
void ags_lv2_manager_test_load_blacklist();
void ags_lv2_manager_test_load_preset();
void ags_lv2_manager_test_load_file();
void ags_lv2_manager_test_load_default_directory();

#define AGS_LV2_MANAGER_TEST_GET_FILENAMES_DELAY_SWH "plugin.so"
#define AGS_LV2_MANAGER_TEST_GET_FILENAMES_ZYNADD "zynadd.so"
#define AGS_LV2_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER "plugin-linux.so"

#define AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA "plugin.so"
#define AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA_EFFECT "mono compressor"
#define AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42 "x42.so"
#define AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42_EFFECT "balance"

gchar **ags_lv2_default_path;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2_manager_test_get_filenames()
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *list;

  gchar **filename;

  lv2_manager = ags_lv2_manager_get_instance();

  /* assert get NULL */
  filename = ags_lv2_manager_get_filenames(lv2_manager);

  CU_ASSERT(filename == NULL);
  
  /* create some lv2 plugins */
  list = NULL;

  lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
			    "filename", AGS_LV2_MANAGER_TEST_GET_FILENAMES_DELAY_SWH,
			    NULL);
  list = g_list_prepend(list,
			lv2_plugin);
  
  lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
			    "filename", AGS_LV2_MANAGER_TEST_GET_FILENAMES_ZYNADD,
			    NULL);
  list = g_list_prepend(list,
			lv2_plugin);

  lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
			    "filename", AGS_LV2_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER,
			    NULL);
  list = g_list_prepend(list,
			lv2_plugin);

  /* add plugins to manager */
  lv2_manager->lv2_plugin = list;

  /* assert get filenames */
  filename = ags_lv2_manager_get_filenames(lv2_manager);

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2_MANAGER_TEST_GET_FILENAMES_DELAY_SWH));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2_MANAGER_TEST_GET_FILENAMES_ZYNADD));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER));

  CU_ASSERT(filename[3] == NULL);

  g_object_unref(lv2_manager);
}

void
ags_lv2_manager_test_find_lv2_plugin()
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *list;

  lv2_manager = ags_lv2_manager_get_instance();

  CU_ASSERT(ags_lv2_manager_find_lv2_plugin(lv2_manager,
					    NULL,
					    NULL) == NULL);
  /* create some lv2 plugins */
  list = NULL;

  lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
			    "filename", AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA,
			    "effect", AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA_EFFECT,
			    NULL);
  list = g_list_prepend(list,
			lv2_plugin);
  
  lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
			    "filename", AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42,
			    "effect", AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42_EFFECT,
			    NULL);
  list = g_list_prepend(list,
			lv2_plugin);

  /* add plugins to manager */
  lv2_manager->lv2_plugin = list;

  /* assert find lv2 plugin */
  CU_ASSERT((lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
							  AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA,
							  AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->filename,
		       AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->effect,
		       AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_INVADA_EFFECT));

  CU_ASSERT((lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
							  AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42,
							  AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->filename,
		       AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->effect,
		       AGS_LV2_MANAGER_TEST_FIND_LV2_PLUGIN_X42_EFFECT));

  g_object_unref(lv2_manager);
}

void
ags_lv2_manager_test_load_blacklist()
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = ags_lv2_manager_get_instance();

  /* test NULL */
  ags_lv2_manager_load_blacklist(lv2_manager,
				 NULL);
  CU_ASSERT(lv2_manager->lv2_plugin_blacklist == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(lv2_manager);
}

void
ags_lv2_manager_test_load_preset()
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = ags_lv2_manager_get_instance();

  //TODO:JK: implement better test
  
  g_object_unref(lv2_manager);
}

void
ags_lv2_manager_test_load_file()
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = ags_lv2_manager_get_instance();

  /* test NULL */
  ags_lv2_manager_load_file(lv2_manager,
			    NULL,
			    NULL,
			    NULL,
			    NULL);
  CU_ASSERT(lv2_manager->lv2_plugin == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(lv2_manager);
}

void
ags_lv2_manager_test_load_default_directory()
{
  AgsLv2Manager *lv2_manager;

  gchar *first_path;
  
  lv2_manager = ags_lv2_manager_get_instance();

  /* test NULL */
  ags_lv2_default_path = ags_lv2_manager_get_default_path();

  first_path = ags_lv2_default_path[0];
  ags_lv2_default_path[0] = NULL;
  
  ags_lv2_manager_load_default_directory(lv2_manager);
  CU_ASSERT(lv2_manager->lv2_plugin == NULL);

  //TODO:JK: implement better test

  ags_lv2_default_path[0] = first_path;
  
  g_object_unref(lv2_manager);
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
  pSuite = CU_add_suite("AgsLv2ManagerTest\0", ags_lv2_manager_test_init_suite, ags_lv2_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2Manager get filenames\0", ags_lv2_manager_test_get_filenames) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Manager find lv2 plugin\0", ags_lv2_manager_test_find_lv2_plugin) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Manager load blacklist\0", ags_lv2_manager_test_load_blacklist) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Manager load file\0", ags_lv2_manager_test_load_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Manager load preset\0", ags_lv2_manager_test_load_preset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Manager load default directory\0", ags_lv2_manager_test_load_default_directory) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
