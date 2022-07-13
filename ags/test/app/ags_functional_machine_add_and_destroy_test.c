/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/test/app/libgsequencer.h>

#include "ags_functional_test_util.h"

#include "config.h"

void ags_functional_machine_add_and_destroy_test_add_test();

int ags_functional_machine_add_and_destroy_test_init_suite();
int ags_functional_machine_add_and_destroy_test_clean_suite();

void ags_functional_machine_add_and_destroy_test_panel();
void ags_functional_machine_add_and_destroy_test_mixer();
void ags_functional_machine_add_and_destroy_test_spectrometer();
void ags_functional_machine_add_and_destroy_test_equalizer10();
void ags_functional_machine_add_and_destroy_test_drum();
void ags_functional_machine_add_and_destroy_test_matrix();
void ags_functional_machine_add_and_destroy_test_synth();
void ags_functional_machine_add_and_destroy_test_fm_synth();
void ags_functional_machine_add_and_destroy_test_syncsynth();
void ags_functional_machine_add_and_destroy_test_fm_syncsynth();
void ags_functional_machine_add_and_destroy_test_hybrid_synth();
void ags_functional_machine_add_and_destroy_test_hybrid_fm_synth();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_machine_add_and_destroy_test_ffplayer();
void ags_functional_machine_add_and_destroy_test_sf2_synth();
#endif
void ags_functional_machine_add_and_destroy_test_pitch_sampler();
void ags_functional_machine_add_and_destroy_test_sfz_synth();
void ags_functional_machine_add_and_destroy_test_audiorec();

#define AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"						\
  "simple-file=true\n"							\
  "disable-feature=experimental\n"					\
  "segmentation=4/4\n"							\
  "\n"									\
  "[thread]\n"								\
  "model=super-threaded\n"						\
  "super-threaded-scope=channel\n"					\
  "lock-global=ags-thread\n"						\
  "lock-parent=ags-recycling-thread\n"					\
  "\n"									\
  "[soundcard-0]\n"							\
  "backend=alsa\n"							\
  "device=default\n"							\
  "samplerate=44100\n"							\
  "buffer-size=1024\n"							\
  "pcm-channels=2\n"							\
  "dsp-channels=2\n"							\
  "format=16\n"								\
  "\n"									\
  "[recall]\n"								\
  "auto-sense=true\n"							\
  "\n"

CU_pSuite pSuite = NULL;
volatile gboolean is_available;

extern AgsApplicationContext *ags_application_context;

struct timespec ags_functional_machine_add_and_destroy_test_default_timeout = {
  300,
  0,
};

void
ags_functional_machine_add_and_destroy_test_add_test()
{  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsPanel", ags_functional_machine_add_and_destroy_test_panel) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsMixer", ags_functional_machine_add_and_destroy_test_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSpectrometer", ags_functional_machine_add_and_destroy_test_spectrometer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsEqualizer10", ags_functional_machine_add_and_destroy_test_equalizer10) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsDrum", ags_functional_machine_add_and_destroy_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsMatrix", ags_functional_machine_add_and_destroy_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSynth", ags_functional_machine_add_and_destroy_test_synth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsFMSynth", ags_functional_machine_add_and_destroy_test_fm_synth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSyncsynth", ags_functional_machine_add_and_destroy_test_syncsynth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsFMSyncsynth", ags_functional_machine_add_and_destroy_test_fm_syncsynth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsHybridSynth", ags_functional_machine_add_and_destroy_test_hybrid_synth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsHybridFMSynth", ags_functional_machine_add_and_destroy_test_hybrid_fm_synth) == NULL)
#ifdef AGS_WITH_LIBINSTPATCH
     ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsFFPlayer", ags_functional_machine_add_and_destroy_test_ffplayer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSF2Synth", ags_functional_machine_add_and_destroy_test_sf2_synth) == NULL)
#endif
     || (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsPitchSampler", ags_functional_machine_add_and_destroy_test_pitch_sampler) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsSFZSynth", ags_functional_machine_add_and_destroy_test_sfz_synth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine add and destroy AgsAudiorec", ags_functional_machine_add_and_destroy_test_audiorec) == NULL)
     ){
    
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  ags_functional_test_util_quit();
  
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
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_machine_add_and_destroy_test_default_timeout,
						      &(gsequencer_application_context->window));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_machine_add_and_destroy_test_clean_suite()
{  
  return(0);
}

void
ags_functional_machine_add_and_destroy_test_panel()
{  
  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_PANEL);
  
  /* destroy panel */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_mixer()
{
  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Mixer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MIXER);

  /* destroy mixer */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_spectrometer()
{
  /* add spectrometer */
  ags_functional_test_util_add_machine(NULL,
				       "Spectrometer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SPECTROMETER);

  /* destroy spectrometer */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_equalizer10()
{
  /* add equalizer10 */
  ags_functional_test_util_add_machine(NULL,
				       "Equalizer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_EQUALIZER10);

  /* destroy equalizer10 */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_drum()
{
  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  /* destroy drum */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_matrix()
{
  /* add matrix */
  ags_functional_test_util_add_machine(NULL,
				       "Matrix");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MATRIX);

  /* destroy matrix */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_synth()
{
  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SYNTH);

  /* destroy synth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_fm_synth()
{
  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "FM Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FM_SYNTH);

  /* destroy synth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_syncsynth()
{
  /* add syncsynth */
  ags_functional_test_util_add_machine(NULL,
				       "Syncsynth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SYNCSYNTH);

  /* destroy syncsynth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_fm_syncsynth()
{
  /* add syncsynth */
  ags_functional_test_util_add_machine(NULL,
				       "FM Syncsynth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FM_SYNCSYNTH);

  /* destroy syncsynth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_hybrid_synth()
{
  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "Hybrid Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_HYBRID_SYNTH);

  /* destroy synth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_hybrid_fm_synth()
{
  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "Hybrid FM Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_HYBRID_FM_SYNTH);

  /* destroy synth */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_ffplayer()
{
  /* add fplayer */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  /* destroy fplayer */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_sf2_synth()
{
  /* add fplayer */
  ags_functional_test_util_add_machine(NULL,
				       "SF2 Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SF2_SYNTH);

  /* destroy fplayer */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_pitch_sampler()
{
  /* add fplayer */
  ags_functional_test_util_add_machine(NULL,
				       "Pitch Sampler");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_PITCH_SAMPLER);

  /* destroy fplayer */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_sfz_synth()
{
  /* add fplayer */
  ags_functional_test_util_add_machine(NULL,
				       "SFZ Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SFZ_SYNTH);

  /* destroy fplayer */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

void
ags_functional_machine_add_and_destroy_test_audiorec()
{
  /* add audiorec */
  ags_functional_test_util_add_machine(NULL,
				       "Audiorec");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_AUDIOREC);

  /* destroy audiorec */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_sync();
  ags_functional_test_util_stack_clear();
}

int
main(int argc, char **argv)
{
  gchar *str;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalMachineAddAndDestroyTest", ags_functional_machine_add_and_destroy_test_init_suite, ags_functional_machine_add_and_destroy_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
#if defined(AGS_TEST_CONFIG)
  ags_functional_test_util_init(&argc, &argv,
				AGS_TEST_CONFIG);
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_functional_test_util_init(&argc, &argv,
				  str);
  }else{
    ags_functional_test_util_init(&argc, &argv,
				  AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_CONFIG);
  }
#endif
    
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_machine_add_and_destroy_test_add_test, &is_available);
  
  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
