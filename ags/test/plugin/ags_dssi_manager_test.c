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

int ags_dssi_manager_test_init_suite();
int ags_dssi_manager_test_clean_suite();

void ags_dssi_manager_test_get_filenames();
void ags_dssi_manager_test_find_dssi_plugin();
void ags_dssi_manager_test_load_blacklist();
void ags_dssi_manager_test_load_file();
void ags_dssi_manager_test_load_default_directory();

#define AGS_DSSI_MANAGER_TEST_GET_FILENAMES_XSYNTH "xsynth.so"
#define AGS_DSSI_MANAGER_TEST_GET_FILENAMES_HEXTER "hexter.so"
#define AGS_DSSI_MANAGER_TEST_GET_FILENAMES_YOSHIMI "yoshimi.so"

#define AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH "wsynth.so"
#define AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH_EFFECT "wsynth"
#define AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI "yoshimi.so"
#define AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI_EFFECT "yoshimi"

gchar **ags_dssi_default_path;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_dssi_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_dssi_manager_test_clean_suite()
{
  return(0);
}

void
ags_dssi_manager_test_get_filenames()
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  GList *list;

  gchar **filename;

  dssi_manager = ags_dssi_manager_get_instance();

  /* assert get NULL */
  filename = ags_dssi_manager_get_filenames(dssi_manager);

  CU_ASSERT(filename == NULL);
  
  /* create some dssi plugins */
  list = NULL;

  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     "filename", AGS_DSSI_MANAGER_TEST_GET_FILENAMES_XSYNTH,
			     NULL);
  list = g_list_prepend(list,
			dssi_plugin);
  
  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     "filename", AGS_DSSI_MANAGER_TEST_GET_FILENAMES_HEXTER,
			     NULL);
  list = g_list_prepend(list,
			dssi_plugin);

  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     "filename", AGS_DSSI_MANAGER_TEST_GET_FILENAMES_YOSHIMI,
			     NULL);
  list = g_list_prepend(list,
			dssi_plugin);

  /* add plugins to manager */
  dssi_manager->dssi_plugin = list;

  /* assert get filenames */
  filename = ags_dssi_manager_get_filenames(dssi_manager);

  CU_ASSERT(g_strv_contains(filename,
			    AGS_DSSI_MANAGER_TEST_GET_FILENAMES_XSYNTH));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_DSSI_MANAGER_TEST_GET_FILENAMES_HEXTER));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_DSSI_MANAGER_TEST_GET_FILENAMES_YOSHIMI));

  CU_ASSERT(filename[3] == NULL);

  g_object_unref(dssi_manager);
}

void
ags_dssi_manager_test_find_dssi_plugin()
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  GList *list;

  dssi_manager = ags_dssi_manager_get_instance();

  CU_ASSERT(ags_dssi_manager_find_dssi_plugin(dssi_manager,
					      NULL,
					      NULL) == NULL);
  /* create some dssi plugins */
  list = NULL;

  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     "filename", AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH,
			     "effect", AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH_EFFECT,
			     NULL);
  list = g_list_prepend(list,
			dssi_plugin);
  
  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     "filename", AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI,
			     "effect", AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI_EFFECT,
			     NULL);
  list = g_list_prepend(list,
			dssi_plugin);

  /* add plugins to manager */
  dssi_manager->dssi_plugin = list;

  /* assert find dssi plugin */
  CU_ASSERT((dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
							     AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH,
							     AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->filename,
		       AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->effect,
		       AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_WSYNTH_EFFECT));

  CU_ASSERT((dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
							     AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI,
							     AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->filename,
		       AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->effect,
		       AGS_DSSI_MANAGER_TEST_FIND_DSSI_PLUGIN_YOSHIMI_EFFECT));

  g_object_unref(dssi_manager);
}

void
ags_dssi_manager_test_load_blacklist()
{
  AgsDssiManager *dssi_manager;

  dssi_manager = ags_dssi_manager_get_instance();

  /* test NULL */
  ags_dssi_manager_load_blacklist(dssi_manager,
				  NULL);
  CU_ASSERT(dssi_manager->dssi_plugin_blacklist == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(dssi_manager);
}

void
ags_dssi_manager_test_load_file()
{
  AgsDssiManager *dssi_manager;

  dssi_manager = ags_dssi_manager_get_instance();

  /* test NULL */
  ags_dssi_manager_load_file(dssi_manager,
			     NULL,
			     NULL);
  CU_ASSERT(dssi_manager->dssi_plugin == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(dssi_manager);
}

void
ags_dssi_manager_test_load_default_directory()
{
  AgsDssiManager *dssi_manager;

  gchar *first_path;
  
  dssi_manager = ags_dssi_manager_get_instance();

  /* test NULL */
  ags_dssi_default_path = ags_dssi_manager_get_default_path();

  first_path = ags_dssi_default_path[0];
  ags_dssi_default_path[0] = NULL;
  
  ags_dssi_manager_load_default_directory(dssi_manager);
  CU_ASSERT(dssi_manager->dssi_plugin == NULL);

  //TODO:JK: implement better test

  ags_dssi_default_path[0] = first_path;
  
  g_object_unref(dssi_manager);
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
  pSuite = CU_add_suite("AgsDssiManagerTest\0", ags_dssi_manager_test_init_suite, ags_dssi_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsDssiManager get filenames\0", ags_dssi_manager_test_get_filenames) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDssiManager find dssi plugin\0", ags_dssi_manager_test_find_dssi_plugin) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDssiManager find dssi plugin\0", ags_dssi_manager_test_load_blacklist) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDssiManager find dssi plugin\0", ags_dssi_manager_test_load_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDssiManager find dssi plugin\0", ags_dssi_manager_test_load_default_directory) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
