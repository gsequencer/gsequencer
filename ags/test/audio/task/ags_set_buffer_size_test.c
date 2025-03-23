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

int ags_set_buffer_size_test_init_suite();
int ags_set_buffer_size_test_clean_suite();

void ags_set_buffer_size_test_launch();

AgsConfig *config;

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_buffer_size_test_init_suite()
{
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  ags_application_context_prepare(application_context);

  ags_application_context_setup(application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_buffer_size_test_clean_suite()
{  
  return(0);
}

void
ags_set_buffer_size_test_launch()
{
  AgsAlsaDevout *devout;

  AgsSetBufferSize *set_buffer_size;

  guint buffer_size;
  
  devout = ags_alsa_devout_new();
  ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(application_context),
					   devout);
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_prepend(NULL,
						  devout));
  
  set_buffer_size = ags_set_buffer_size_new(devout,
					    256);
  
  CU_ASSERT(AGS_IS_SET_BUFFER_SIZE(set_buffer_size));
  CU_ASSERT(set_buffer_size->scope == devout);
  CU_ASSERT(set_buffer_size->buffer_size == 256);

  /* launch */
  ags_task_launch(set_buffer_size);

  ags_soundcard_get_presets(AGS_SOUNDCARD(devout),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);
  
  CU_ASSERT(buffer_size == 256);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
    
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSetBufferSizeTest", ags_set_buffer_size_test_init_suite, ags_set_buffer_size_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSetBufferSize launch", ags_set_buffer_size_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
