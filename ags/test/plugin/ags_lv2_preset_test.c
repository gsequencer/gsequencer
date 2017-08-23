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

int ags_lv2_preset_test_init_suite();
int ags_lv2_preset_test_clean_suite();

void ags_lv2_preset_test_parse_turtle();
void ags_lv2_preset_test_find_preset_label();

#define AGS_LV2_PRESET_TEST_PARSE_TURTLE_URI "<http://drobilla.net/plugins/mda/presets#EPiano-default>"
#define AGS_LV2_PRESET_TEST_PARSE_TURTLE_TTL SRCDIR "/lv2.lib/mda/EPiano-presets.ttl"

#define AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_DEFAULT "Default"
#define AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_BRIGHT "Bright"
#define AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_TREMOLO "Tremolo"

/* Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_preset_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_preset_test_clean_suite()
{
  return(0);
}

void
ags_lv2_preset_test_parse_turtle()
{
  AgsLv2Preset *lv2_preset;

  AgsTurtle *turtle;

  GError *error;

  g_message("%s", AGS_LV2_PRESET_TEST_PARSE_TURTLE_TTL);
  turtle = ags_turtle_new(AGS_LV2_PRESET_TEST_PARSE_TURTLE_TTL);

  error = NULL;
  ags_turtle_load(turtle,
		  &error);

  //  xmlSaveFormatFileEnc("-", turtle->doc, "UTF-8", 1);
    
  lv2_preset = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
					     "turtle", turtle,
					     "uri", AGS_LV2_PRESET_TEST_PARSE_TURTLE_URI,
					     NULL);

  /* parse and assert */
  ags_lv2_preset_parse_turtle(lv2_preset);

  CU_ASSERT(lv2_preset->port_preset != NULL);
}

void
ags_lv2_preset_test_find_preset_label()
{
  AgsLv2Preset *lv2_preset[3];

  GList *list, *current;

  list = NULL;
  
  lv2_preset[0] = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
						"preset-label", AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_DEFAULT,
						NULL);
  list = g_list_prepend(list,
			lv2_preset[0]);
  
  lv2_preset[1] = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
						"preset-label", AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_BRIGHT,
						NULL);
  list = g_list_prepend(list,
			lv2_preset[1]);

  lv2_preset[2] = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
						"preset-label", AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_TREMOLO,
						NULL);
  list = g_list_prepend(list,
			lv2_preset[2]);

  /* assert */
  CU_ASSERT((current = ags_lv2_preset_find_preset_label(list,
							AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_DEFAULT)) != NULL &&
	    current->data == lv2_preset[0]);

  CU_ASSERT((current = ags_lv2_preset_find_preset_label(list,
							AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_BRIGHT)) != NULL &&
	    current->data == lv2_preset[1]);

  CU_ASSERT((current = ags_lv2_preset_find_preset_label(list,
							AGS_LV2_PRESET_TEST_FIND_PRESET_LABEL_TREMOLO)) != NULL &&
	    current->data == lv2_preset[2]);
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
  pSuite = CU_add_suite("AgsLv2PresetTest", ags_lv2_preset_test_init_suite, ags_lv2_preset_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2Preset parse turtle", ags_lv2_preset_test_parse_turtle) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Preset find preset label", ags_lv2_preset_test_find_preset_label) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
