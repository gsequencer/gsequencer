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

int ags_start_audio_test_init_suite();
int ags_start_audio_test_clean_suite();

void ags_start_audio_test_launch();

#define AGS_START_AUDIO_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=default\n"					\
  "samplerate=48000\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

GThread *add_thread = NULL;

AgsConfig *config;

AgsApplicationContext *application_context;

gpointer
ags_start_audio_test_add_thread(gpointer data)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");    
  putenv("VST3_PATH=\"\"");
    
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    exit(CU_get_error());
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsStartAudioTest", ags_start_audio_test_init_suite, ags_start_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    exit(CU_get_error());
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsStartAudio launch", ags_start_audio_test_launch) == NULL)){
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
ags_start_audio_test_init_suite()
{
  AgsThread *main_loop;

  gint64 start_thread_timeout;

  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_START_AUDIO_TEST_CONFIG,
			    strlen(AGS_START_AUDIO_TEST_CONFIG));
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  ags_application_context_prepare(application_context);

  ags_application_context_setup(application_context);

  main_loop = application_context->main_loop;
  ags_thread_set_flags(main_loop,
		       AGS_THREAD_TIME_ACCOUNTING);

  start_thread_timeout = g_get_monotonic_time() + 20 * G_USEC_PER_SEC;
  
  while(!ags_thread_test_status_flags(main_loop, AGS_THREAD_STATUS_RUNNING)){
    g_usleep(4);
    
    if(g_get_monotonic_time() > start_thread_timeout){
      g_critical("start timeout");

      break;
    }
  }
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_start_audio_test_clean_suite()
{  
  return(0);
}

void
ags_start_audio_test_launch()
{
  AgsAudio *audio;
  AgsStartAudio *start_audio;

  AgsTaskLauncher *task_launcher;
  
  audio = ags_audio_new(NULL);
  ags_audio_set_flags(audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio,
			      (AGS_SOUND_ABILITY_SEQUENCER |
			       AGS_SOUND_ABILITY_NOTATION |
			       AGS_SOUND_ABILITY_WAVE));
  
  ags_audio_set_audio_channels(audio,
			       2, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  start_audio = ags_start_audio_new(audio,
				    -1);
  
  CU_ASSERT(AGS_IS_START_AUDIO(start_audio));
  CU_ASSERT(start_audio->audio == audio);
  CU_ASSERT(start_audio->sound_scope == -1);

  /* launch */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  ags_task_launcher_add_task(task_launcher,
			     start_audio);

  g_usleep(5 * G_USEC_PER_SEC);
  
  CU_ASSERT(audio->recall_id != NULL);
}

int
main(int argc, char **argv)
{
  add_thread = g_thread_new("libags_audio.so - functional pitch test",
			    ags_start_audio_test_add_thread,
			    NULL);

  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  FALSE));
  
  g_thread_join(add_thread);

  return(-1);
}
