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

int ags_lv2ui_manager_test_init_suite();
int ags_lv2ui_manager_test_clean_suite();

void ags_lv2ui_manager_test_get_filenames();
void ags_lv2ui_manager_test_find_lv2ui_plugin();
void ags_lv2ui_manager_test_find_lv2ui_plugin_with_index();
void ags_lv2ui_manager_test_load_blacklist();
void ags_lv2ui_manager_test_load_file();
void ags_lv2ui_manager_test_load_default_directory();

#define AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_DELAY_SWH "plugin.so"
#define AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_ZYNADD "zynadd.so"
#define AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER "plugin-linux.so"

#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA "plugin.so"
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA_EFFECT "mono compressor"
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42 "x42.so"
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42_EFFECT "balance"

#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA "plugin.so"
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA_INDEX (1)
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42 "x42.so"
#define AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42_INDEX (3)

gchar **ags_lv2ui_default_path;

/* Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2ui_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2ui_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2ui_manager_test_get_filenames()
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

  gchar **filename;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  /* assert get NULL */
  filename = ags_lv2ui_manager_get_filenames(lv2ui_manager);

  CU_ASSERT(filename == NULL);
  
  /* create some lv2 plugins */
  list = NULL;

  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_DELAY_SWH,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);
  
  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_ZYNADD,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);

  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);

  /* add plugins to manager */
  lv2ui_manager->lv2ui_plugin = list;

  /* assert get filenames */
  filename = ags_lv2ui_manager_get_filenames(lv2ui_manager);

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_DELAY_SWH));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_ZYNADD));

  CU_ASSERT(g_strv_contains(filename,
			    AGS_LV2UI_MANAGER_TEST_GET_FILENAMES_HERMES_FILTER));

  CU_ASSERT(filename[3] == NULL);

  g_object_unref(lv2ui_manager);
}

void
ags_lv2ui_manager_test_find_lv2ui_plugin()
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  CU_ASSERT(ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
						NULL,
						NULL) == NULL);
  /* create some lv2ui plugins */
  list = NULL;

  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA,
			      "effect", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA_EFFECT,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);
  
  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42,
			      "effect", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42_EFFECT,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);

  /* add plugins to manager */
  lv2ui_manager->lv2ui_plugin = list;

  /* assert find lv2ui plugin */
  CU_ASSERT((lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
								AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA,
								AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->effect,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_INVADA_EFFECT));

  CU_ASSERT((lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
								AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42,
								AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42_EFFECT)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->effect,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_X42_EFFECT));

  g_object_unref(lv2ui_manager);
}

void
ags_lv2ui_manager_test_find_lv2ui_plugin_with_index()
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  CU_ASSERT(ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
						NULL,
						NULL) == NULL);
  /* create some lv2ui plugins */
  list = NULL;

  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA,
			      "ui-effect-index", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA_INDEX,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);
  
  lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
			      "ui-filename", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42,
			      "ui-effect-index", AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42_INDEX,
			      NULL);
  list = g_list_prepend(list,
			lv2ui_plugin);

  /* add plugins to manager */
  lv2ui_manager->lv2ui_plugin = list;

  /* assert find lv2ui plugin */
  CU_ASSERT((lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin_with_index(lv2ui_manager,
									   AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA,
									   AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA) &&
	    AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect_index == AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_INVADA_INDEX);

  CU_ASSERT((lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin_with_index(lv2ui_manager,
									   AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42,
									   AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
		       AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42) &&
	    AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect_index == AGS_LV2UI_MANAGER_TEST_FIND_LV2UI_PLUGIN_WITH_INDEX_X42_INDEX);

  g_object_unref(lv2ui_manager);
}

void
ags_lv2ui_manager_test_load_blacklist()
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  /* test NULL */
  ags_lv2ui_manager_load_blacklist(lv2ui_manager,
				   NULL);
  CU_ASSERT(lv2ui_manager->lv2ui_plugin_blacklist == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(lv2ui_manager);
}

void
ags_lv2ui_manager_test_load_file()
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  /* test NULL */
  ags_lv2ui_manager_load_file(lv2ui_manager,
			      NULL,
			      NULL,
			      NULL,
			      NULL);
  CU_ASSERT(lv2ui_manager->lv2ui_plugin == NULL);

  //TODO:JK: implement better test
  
  g_object_unref(lv2ui_manager);
}

void
ags_lv2ui_manager_test_load_default_directory()
{
  AgsLv2uiManager *lv2ui_manager;

  gchar *first_path;
  
  lv2ui_manager = ags_lv2ui_manager_get_instance();

  /* test NULL */
  ags_lv2ui_default_path = ags_lv2ui_manager_get_default_path();

  first_path = ags_lv2ui_default_path[0];
  ags_lv2ui_default_path[0] = NULL;
  
  ags_lv2ui_manager_load_default_directory(lv2ui_manager);
  CU_ASSERT(lv2ui_manager->lv2ui_plugin == NULL);

  //TODO:JK: implement better test

  ags_lv2ui_default_path[0] = first_path;
  
  g_object_unref(lv2ui_manager);
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
  pSuite = CU_add_suite("AgsLv2uiManagerTest", ags_lv2ui_manager_test_init_suite, ags_lv2ui_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2uiManager get filenames", ags_lv2ui_manager_test_get_filenames) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2uiManager find lv2ui plugin", ags_lv2ui_manager_test_find_lv2ui_plugin) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2uiManager find lv2ui plugin with index", ags_lv2ui_manager_test_find_lv2ui_plugin_with_index) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2uiManager load blacklist", ags_lv2ui_manager_test_load_blacklist) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2uiManager load file", ags_lv2ui_manager_test_load_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2uiManager load default directory", ags_lv2ui_manager_test_load_default_directory) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
