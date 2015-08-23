/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_application_context.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/task/ags_open_file.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_functional_drum_test_init_suite();
int ags_functional_drum_test_clean_suite();

void ags_functional_drum_test_open_drumkit();
void ags_functional_drum_test_edit_pattern();

#define AGS_FUNCTIONAL_DRUM_N_PATTERNS (3)

AgsApplicationContext *application_context;

static const gchar *drumkit_directory = "/usr/share/hydrogen/data/drumkits/Synthie-1\0";
static const guint64 drumkit_pattern[AGS_FUNCTIONAL_DRUM_N_PATTERNS] = {
  0x8888888888888888,
  0x2222222222222222,
  0xcccccccccccccccc,
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_drum_test_init_suite()
{ 
  //TODO:JK: implement me
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_drum_test_clean_suite()
{
  //TODO:JK: implement me
}

/**
 * ags_functional_drum_open_drumkit_test:
 * 
 * Instantiates an AgsDrum open a drum kit for it.
 */
void
ags_functional_drum_test_open_drumkit()
{
  AgsDrum *drum;

  AgsOpenFile *open_file;

  //TODO:JK: implement me  
}

/**
 * ags_functional_drum_edit_pattern_test:
 *
 * Instantiates an AgsDrum and edit its pattern.
 */
void
ags_functional_drum_test_edit_pattern()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsDrumTest\0", ags_functional_drum_test_init_suite, ags_functional_drum_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsDrum opening drum kit\0", ags_functional_drum_test_open_drumkit) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDrum editing pattern\0", ags_functional_drum_test_edit_pattern) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

