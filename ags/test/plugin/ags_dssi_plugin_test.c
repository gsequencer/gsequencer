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

int ags_dssi_plugin_test_init_suite();
int ags_dssi_plugin_test_clean_suite();

void ags_dssi_plugin_test_change_program();

void ags_dssi_plugin_test_stub_change_program(AgsDssiPlugin *dssi_plugin,
					      gpointer ladspa_handle,
					      guint bank_index,
					      guint program_index);

gboolean stub_change_program = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_dssi_plugin_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_dssi_plugin_test_clean_suite()
{
  return(0);
}

void
ags_dssi_plugin_test_change_program()
{
  AgsDssiPlugin *dssi_plugin;

  gpointer ptr;
  
  dssi_plugin = g_object_new(AGS_TYPE_DSSI_PLUGIN,
			     NULL);

  ptr = AGS_DSSI_PLUGIN_GET_CLASS(dssi_plugin)->change_program;
  AGS_DSSI_PLUGIN_GET_CLASS(dssi_plugin)->change_program = ags_dssi_plugin_test_stub_change_program;

  /* assert change program */
  ags_dssi_plugin_change_program(dssi_plugin,
				 NULL,
				 0,
				 0);

  CU_ASSERT(stub_change_program == TRUE);
  AGS_DSSI_PLUGIN_GET_CLASS(dssi_plugin)->change_program = ptr;
}

void
ags_dssi_plugin_test_stub_change_program(AgsDssiPlugin *dssi_plugin,
					 gpointer ladspa_handle,
					 guint bank_index,
					 guint program_index)
{
  stub_change_program = TRUE;
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
  pSuite = CU_add_suite("AgsDssiPluginTest", ags_dssi_plugin_test_init_suite, ags_dssi_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsDssiPlugin change program", ags_dssi_plugin_test_change_program) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
