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

int ags_add_effect_test_init_suite();
int ags_add_effect_test_clean_suite();

void ags_add_effect_test_launch();

void ags_add_effect_test_launch_add_effect_callback();

#define AGS_ADD_EFFECT_TEST_LAUNCH_FILENAME "/usr/lib/ladspa/cmt.so"
#define AGS_ADD_EFFECT_TEST_LAUNCH_EFFECT "Freeverb (Version 3)"

gboolean ags_add_effect_test_launch_add_effect_invoked = FALSE;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_effect_test_init_suite()
{ 
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = ags_ladspa_manager_get_instance();
  ags_ladspa_manager_load_default_directory(ladspa_manager);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_effect_test_clean_suite()
{  
  return(0);
}

void
ags_add_effect_test_launch_add_effect_callback()
{
  ags_add_effect_test_launch_add_effect_invoked = TRUE;
}

void
ags_add_effect_test_launch()
{
  AgsChannel *channel;
  AgsAddEffect *add_effect;

  channel = ags_channel_new(NULL);
  g_object_ref(channel);

  add_effect = ags_add_effect_new(channel,
				  AGS_ADD_EFFECT_TEST_LAUNCH_FILENAME,
				  AGS_ADD_EFFECT_TEST_LAUNCH_EFFECT);

  CU_ASSERT(AGS_IS_ADD_EFFECT(add_effect));
  CU_ASSERT(!g_strcmp0(add_effect->filename,
		       AGS_ADD_EFFECT_TEST_LAUNCH_FILENAME));
  CU_ASSERT(!g_strcmp0(add_effect->effect,
		       AGS_ADD_EFFECT_TEST_LAUNCH_EFFECT));

  /* test */
  g_signal_connect(channel, "add-effect",
		   G_CALLBACK(ags_add_effect_test_launch_add_effect_callback), NULL);

  ags_task_launch(add_effect);

  CU_ASSERT(ags_add_effect_test_launch_add_effect_invoked == TRUE);
  CU_ASSERT(ags_recall_find_type(channel->play, AGS_TYPE_RECALL_LADSPA) != NULL);
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
  pSuite = CU_add_suite("AgsAddEffectTest", ags_add_effect_test_init_suite, ags_add_effect_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAddEffect launch", ags_add_effect_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
