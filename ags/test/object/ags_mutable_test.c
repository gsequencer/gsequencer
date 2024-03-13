/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

int ags_mutable_test_init_suite();
int ags_mutable_test_clean_suite();

void ags_mutable_test_set_muted();

GType mutable_test_types[3];

extern AgsApplicationContext *ags_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_mutable_test_init_suite()
{
  guint i;

  ags_audio_application_context_new();
  
  i = 0;
  
  mutable_test_types[(i++)] = AGS_TYPE_FX_VOLUME_AUDIO;
  mutable_test_types[(i++)] = AGS_TYPE_FX_VOLUME_CHANNEL;
  mutable_test_types[(i++)] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_mutable_test_clean_suite()
{  
  return(0);
}

void
ags_mutable_test_set_muted()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; mutable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(mutable_test_types[i],
			   NULL);
    
    g_message("instantiated %s", G_OBJECT_TYPE_NAME(current));    

    if(AGS_MUTABLE_GET_INTERFACE(AGS_MUTABLE(current))->set_muted == NULL){
      g_message("AgsMutable::set-muted missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_mutable_set_muted(AGS_MUTABLE(current),
			  FALSE);
  }

  CU_ASSERT(success);
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

  /* remove a suite to the registry */
  pSuite = CU_add_suite("AgsMutableTest", ags_mutable_test_init_suite, ags_mutable_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMutable set muted", ags_mutable_test_set_muted) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
