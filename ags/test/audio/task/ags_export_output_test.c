/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <math.h>

int ags_export_output_test_init_suite();
int ags_export_output_test_clean_suite();

void ags_export_output_test_launch();

#define AGS_EXPORT_OUTPUT_TEST_LAUNCH_FILENAME "out.wav"
#define AGS_EXPORT_OUTPUT_TEST_LAUNCH_TIC (256)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_export_output_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_export_output_test_clean_suite()
{  
  return(0);
}

void
ags_export_output_test_launch()
{
  AgsAlsaDevout *devout;

  AgsExportThread *export_thread;
  
  AgsExportOutput *export_output;
  
  devout = ags_alsa_devout_new(NULL);

  export_thread = ags_export_thread_new(devout,
					NULL);
  
  export_output = ags_export_output_new(export_thread,
					devout,
					AGS_EXPORT_OUTPUT_TEST_LAUNCH_FILENAME,
					AGS_EXPORT_OUTPUT_TEST_LAUNCH_TIC,
					TRUE);

  CU_ASSERT(AGS_IS_EXPORT_OUTPUT(export_output));
  CU_ASSERT(export_output->soundcard == devout);
  CU_ASSERT(!g_strcmp0(export_output->filename,
		       AGS_EXPORT_OUTPUT_TEST_LAUNCH_FILENAME));
  CU_ASSERT(export_output->tic == AGS_EXPORT_OUTPUT_TEST_LAUNCH_TIC);
  CU_ASSERT(export_output->live_performance == TRUE);

  //TODO:JK: improve me
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsExportOutputTest", ags_export_output_test_init_suite, ags_export_output_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsExportOutput launch", ags_export_output_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
