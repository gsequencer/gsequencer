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

int ags_playback_domain_test_init_suite();
int ags_playback_domain_test_clean_suite();

void ags_playback_domain_test_set_audio_thread();
void ags_playback_domain_test_get_audio_thread();
void ags_playback_domain_test_add_playback();
void ags_playback_domain_test_remove_playback();

#define AGS_PLAYBACK_DOMAIN_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=true\n"				       \
  "disable-feature=experimental\n"		       \
  "segmentation=4/4\n"				       \
  "\n"						       \
  "[thread]\n"					       \
  "model=super-threaded\n"			       \
  "super-threaded-scope=recycling\n"		       \
  "lock-global=ags-thread\n"			       \
  "lock-parent=ags-recycling-thread\n"		       \
  "\n"						       \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_playback_domain_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_PLAYBACK_DOMAIN_TEST_CONFIG,
			    strlen(AGS_PLAYBACK_DOMAIN_TEST_CONFIG));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_playback_domain_test_clean_suite()
{
  return(0);
}

void
ags_playback_domain_test_set_audio_thread()
{
  AgsPlaybackDomain *playback_domain;
  
  playback_domain = g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
				 NULL);

  /* set playback to NULL and assert */
  ags_playback_domain_set_audio_thread(playback_domain,
				       NULL,
				       AGS_PLAYBACK_DOMAIN_SCOPE_PLAYBACK);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_PLAYBACK] == NULL);
    
  /* set sequencer to NULL and assert */
  ags_playback_domain_set_audio_thread(playback_domain,
				       NULL,
				       AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER] == NULL);

  /* set notation to NULL and assert */
  ags_playback_domain_set_audio_thread(playback_domain,
				       NULL,
				       AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION] == NULL);
}

void
ags_playback_domain_test_get_audio_thread()
{
  AgsPlaybackDomain *playback_domain;

  AgsThread *thread;
  
  playback_domain = g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
				 NULL);

  /* get playback to NULL and assert */
  thread = ags_playback_domain_get_audio_thread(playback_domain,
						AGS_PLAYBACK_DOMAIN_SCOPE_PLAYBACK);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_PLAYBACK] == thread);
  
  /* get sequencer to NULL and assert */
  thread = ags_playback_domain_get_audio_thread(playback_domain,
						AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER] == thread);

  /* get notation to NULL and assert */
  thread = ags_playback_domain_get_audio_thread(playback_domain,
						AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION);
  CU_ASSERT(playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION] == thread);
}

void
ags_playback_domain_test_add_playback()
{
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback[3];
  
  playback_domain = g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
				 NULL);

  playback[0] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[0]);

  playback[1] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[1]);

  playback[2] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[2]);

  /* assert */
  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[0]) != NULL);

  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[1]) != NULL);

  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[2]) != NULL);
}

void
ags_playback_domain_test_remove_playback()
{
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback[3];
  
  playback_domain = g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
				 NULL);

  playback[0] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[0]);

  playback[1] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[1]);

  playback[2] = g_object_new(AGS_TYPE_PLAYBACK,
			     NULL);
  ags_playback_domain_add_playback(playback_domain,
				   playback[2]);

  /* assert */
  ags_playback_domain_remove_playback(playback_domain,
				      playback[1]);
  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[1]) == NULL);

  ags_playback_domain_remove_playback(playback_domain,
				      playback[2]);
  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[2]) == NULL);

  ags_playback_domain_remove_playback(playback_domain,
				      playback[0]);
  CU_ASSERT(g_list_find(playback_domain->playback,
			playback[0]) == NULL);
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
  pSuite = CU_add_suite("AgsPlaybackDomainTest", ags_playback_domain_test_init_suite, ags_playback_domain_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPlaybackDomain set audio thread", ags_playback_domain_test_set_audio_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlaybackDomain get audio thread", ags_playback_domain_test_get_audio_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlaybackDomain add playback", ags_playback_domain_test_add_playback) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlaybackDomain remove playback", ags_playback_domain_test_remove_playback) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

