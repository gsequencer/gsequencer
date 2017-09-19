/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/config.h>

#include <ags/gsequencer_main.h>

#include <ags/test/X/libgsequencer.h>

#include "gsequencer_setup_util.h"
#include "ags_functional_test_util.h"

void ags_functional_machine_add_and_destroy_test_add_test();

int ags_functional_machine_add_and_destroy_test_init_suite();
int ags_functional_machine_add_and_destroy_test_clean_suite();

void ags_functional_machine_add_and_destroy_test_panel();
void ags_functional_machine_add_and_destroy_test_mixer();
void ags_functional_machine_add_and_destroy_test_drum();
void ags_functional_machine_add_and_destroy_test_matrix();
void ags_functional_machine_add_and_destroy_test_synth();

#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_machine_add_and_destroy_test_ffplayer();
#endif

#define AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=true\n"				       \
  "disable-feature=experimental\n"		       \
  "segmentation=4/4\n"				       \
  "\n"						       \
  "[thread]\n"					       \
  "model=super-threaded\n"			       \
  "super-threaded-scope=channel\n"		       \
  "lock-global=ags-thread\n"			       \
  "lock-parent=ags-recycling-thread\n"		       \
  "\n"						       \
  "[soundcard-0]\n"				       \
  "backend=alsa\n"                                     \
  "device=hw:0,0\n"                                    \
  "samplerate=44100\n"				       \
  "buffer-size=1024\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

CU_pSuite pSuite = NULL;
volatile gboolean is_available;

extern AgsApplicationContext *ags_application_context;
AgsGuiThread *gui_thread;

void
ags_functional_machine_add_and_destroy_test_add_test()
{  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsPanel\0", ags_functional_machine_add_and_destroy_test_panel) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsMixer\0", ags_functional_machine_add_and_destroy_test_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsDrum\0", ags_functional_machine_add_and_destroy_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsMatrix\0", ags_functional_machine_add_and_destroy_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSynth\0", ags_functional_machine_add_and_destroy_test_synth) == NULL)
#ifdef AGS_WITH_LIBINSTPATCH
     ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsFFPlayer\0", ags_functional_machine_add_and_destroy_test_ffplayer) == NULL)
#endif
     ){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_machine_add_and_destroy_test_init_suite()
{
  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);
    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_machine_add_and_destroy_test_clean_suite()
{  
  ags_thread_stop(gui_thread);  

  return(0);
}

void
ags_functional_machine_add_and_destroy_test_panel()
{  
  gboolean success;
  
  /* add panel */
  success = ags_functional_test_util_add_machine(NULL,
						 "Panel");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* destroy panel */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_mixer()
{
  gboolean success;

  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  /* destroy mixer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_drum()
{
  gboolean success;

  /* add drum */
  success = ags_functional_test_util_add_machine(NULL,
						 "Drum");

  CU_ASSERT(success == TRUE);

  /* destroy drum */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_matrix()
{
  gboolean success;

  /* add matrix */
  success = ags_functional_test_util_add_machine(NULL,
						 "Matrix");

  CU_ASSERT(success == TRUE);

  /* destroy matrix */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_synth()
{
  gboolean success;

  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Synth");

  CU_ASSERT(success == TRUE);

  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_ffplayer()
{
  gboolean success;

  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /* destroy fplayer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalMachineAddAndDestroyTest\0", ags_functional_machine_add_and_destroy_test_init_suite, ags_functional_machine_add_and_destroy_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_machine_add_and_destroy_test_add_test, &is_available);

  
  return(-1);
}

