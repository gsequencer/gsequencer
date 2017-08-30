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

int ags_lv2ui_plugin_test_init_suite();
int ags_lv2ui_plugin_test_clean_suite();

void ags_lv2ui_plugin_test_find_gui_uri();

#define AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_SWH "http://plugin.org.uk/ladspa-swh"
#define AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_INVADA "http://invadarecords.com/plugins/lv2"
#define AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_ZYN "http://home.gna.org/zyn/zynadd"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2ui_plugin_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2ui_plugin_test_clean_suite()
{
  return(0);
}

void
ags_lv2ui_plugin_test_find_gui_uri()
{
  AgsLv2uiPlugin *lv2ui_plugin[3];
  
  GList *list, *current;

  list = NULL;
  
  lv2ui_plugin[0] = (AgsLv2uiPlugin *) g_object_new(AGS_TYPE_LV2UI_PLUGIN,
						    "gui-uri", AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_SWH,
						    NULL);
  list = g_list_prepend(list,
			lv2ui_plugin[0]);
  
  lv2ui_plugin[1] = (AgsLv2uiPlugin *) g_object_new(AGS_TYPE_LV2UI_PLUGIN,
						    "gui-uri", AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_INVADA,
						    NULL);
  list = g_list_prepend(list,
			lv2ui_plugin[1]);
  
  lv2ui_plugin[2] = (AgsLv2uiPlugin *) g_object_new(AGS_TYPE_LV2UI_PLUGIN,
						    "gui-uri", AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_ZYN,
						    NULL);
  list = g_list_prepend(list,
			lv2ui_plugin[2]);

  /* assert */
  CU_ASSERT((current = ags_lv2ui_plugin_find_gui_uri(list, AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_SWH)) != NULL &&
	    current->data == lv2ui_plugin[0]);

  CU_ASSERT((current = ags_lv2ui_plugin_find_gui_uri(list, AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_INVADA)) != NULL &&
	    current->data == lv2ui_plugin[1]);

  CU_ASSERT((current = ags_lv2ui_plugin_find_gui_uri(list, AGS_LV2UI_PLUGIN_TEST_FIND_GUI_URI_ZYN)) != NULL &&
	    current->data == lv2ui_plugin[2]);
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
  pSuite = CU_add_suite("AgsLv2uiPluginTest", ags_lv2ui_plugin_test_init_suite, ags_lv2ui_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2uiPlugin find GUI URI", ags_lv2ui_plugin_test_find_gui_uri) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
