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

int ags_ladspa_manager_test_init_suite();
int ags_ladspa_manager_test_clean_suite();

void ags_ladspa_manager_test_get_filenames();
void ags_ladspa_manager_test_find_ladspa_plugin();
void ags_ladspa_manager_test_load_blacklist();
void ags_ladspa_manager_test_load_file();
void ags_ladspa_manager_test_load_default_directory();

#define AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_CMT "cmt.so"
#define AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_SWH "swh.so"
#define AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_VOCODER "vocoder.so"

#define AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT "cmt.so"
#define AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT_EFFECT "freeverb"
#define AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH "swh.so"
#define AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH_EFFECT "flanger"

gchar **ags_ladspa_default_path;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_ladspa_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_ladspa_manager_test_clean_suite()
{
  return(0);
}

void
ags_ladspa_manager_test_get_filenames()
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;

  GList *list;

  gchar **filename;

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* assert get NULL */
  filename = ags_ladspa_manager_get_filenames(ladspa_manager);

  CU_ASSERT(filename == NULL);
  
  /* create some ladspa plugins */
  list = NULL;

  ladspa_plugin = g_object_new(AGS_TYPE_LADSPA_PLUGIN,
			       "filename", AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_CMT,
			       NULL);
  list = g_list_prepend(list,
			ladspa_plugin);
  
  ladspa_plugin = g_object_new(AGS_TYPE_LADSPA_PLUGIN,
			       "filename", AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_SWH,
			       NULL);
  list = g_list_prepend(list,
			ladspa_plugin);

  ladspa_plugin = g_object_new(AGS_TYPE_LADSPA_PLUGIN,
			       "filename", AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_VOCODER,
			       NULL);
  list = g_list_prepend(list,
			ladspa_plugin);

  /* add plugins to manager */
  ladspa_manager->ladspa_plugin = list;

  /* assert get filenames */
  filename = ags_ladspa_manager_get_filenames(ladspa_manager);

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_CMT));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_SWH));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LADSPA_MANAGER_TEST_GET_FILENAMES_VOCODER));

  CU_ASSERT(filename[3] == NULL);

  g_object_unref(ladspa_manager);
}

void
ags_ladspa_manager_test_find_ladspa_plugin()
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;

  GList *list;

  ladspa_manager = ags_ladspa_manager_get_instance();

  CU_ASSERT(ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
						  NULL,
						  NULL) == NULL);
  /* create some ladspa plugins */
  list = NULL;

  ladspa_plugin = g_object_new(AGS_TYPE_LADSPA_PLUGIN,
			       "filename", AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT,
			       "effect", AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT_EFFECT,
			       NULL);
  list = g_list_prepend(list,
			ladspa_plugin);
  
  ladspa_plugin = g_object_new(AGS_TYPE_LADSPA_PLUGIN,
			       "filename", AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH,
			       "effect", AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH_EFFECT,
			       NULL);
  list = g_list_prepend(list,
			ladspa_plugin);

  /* add plugins to manager */
  ladspa_manager->ladspa_plugin = list;

  /* assert find ladspa plugin */
  CU_ASSERT((ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
								   AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT,
								   AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->filename,
		       AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->effect,
		       AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_CMT_EFFECT));

  CU_ASSERT((ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
								   AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH,
								   AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->filename,
		       AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->effect,
		       AGS_LADSPA_MANAGER_TEST_FIND_LADSPA_PLUGIN_SWH_EFFECT));

  g_object_unref(ladspa_manager);
}

void
ags_ladspa_manager_test_load_blacklist()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* test NULL */
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    NULL);
  CU_ASSERT(ladspa_manager->ladspa_plugin_blacklist == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(ladspa_manager);
}

void
ags_ladspa_manager_test_load_file()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* test NULL */
  ags_ladspa_manager_load_file(ladspa_manager,
			       NULL,
			       NULL);
  CU_ASSERT(ladspa_manager->ladspa_plugin == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(ladspa_manager);
}

void
ags_ladspa_manager_test_load_default_directory()
{
  AgsLadspaManager *ladspa_manager;

  gchar *first_path;
  
  ladspa_manager = ags_ladspa_manager_get_instance();

  /* test NULL */
  ags_ladspa_default_path = ags_ladspa_manager_get_default_path();

  first_path = ags_ladspa_default_path[0];
  ags_ladspa_default_path[0] = NULL;
  
  ags_ladspa_manager_load_default_directory(ladspa_manager);
  CU_ASSERT(ladspa_manager->ladspa_plugin == NULL);

  //TODO:JK: implement better test

  ags_ladspa_default_path[0] = first_path;
  
  g_object_unref(ladspa_manager);
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
  pSuite = CU_add_suite("AgsLadspaManagerTest\0", ags_ladspa_manager_test_init_suite, ags_ladspa_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLadspaManager get filenames\0", ags_ladspa_manager_test_get_filenames) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaManager find ladspa plugin\0", ags_ladspa_manager_test_find_ladspa_plugin) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaManager load blacklist\0", ags_ladspa_manager_test_load_blacklist) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaManager load file\0", ags_ladspa_manager_test_load_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaManager load default directory\0", ags_ladspa_manager_test_load_default_directory) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
