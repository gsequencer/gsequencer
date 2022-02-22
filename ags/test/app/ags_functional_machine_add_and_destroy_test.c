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

#include "gsequencer_setup_util.h"
#include "ags_functional_test_util.h"

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
  "device=default\n"				       \
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
  
  ags_test_quit();
  
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
ags_functional_machine_add_and_destroy_test_spectrometer()
{
  gboolean success;

  /* add spectrometer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Spectrometer");

  CU_ASSERT(success == TRUE);

  /* destroy spectrometer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_equalizer10()
{
  gboolean success;

  /* add equalizer10 */
  success = ags_functional_test_util_add_machine(NULL,
						 "Equalizer");

  CU_ASSERT(success == TRUE);

  /* destroy equalizer10 */
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
ags_functional_machine_add_and_destroy_test_fm_synth()
{
  gboolean success;

  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "FM Synth");

  CU_ASSERT(success == TRUE);

  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_syncsynth()
{
  gboolean success;

  /* add syncsynth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Syncsynth");

  CU_ASSERT(success == TRUE);

  /* destroy syncsynth */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_fm_syncsynth()
{
  gboolean success;

  /* add syncsynth */
  success = ags_functional_test_util_add_machine(NULL,
						 "FM Syncsynth");

  CU_ASSERT(success == TRUE);

  /* destroy syncsynth */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}


void
ags_functional_machine_add_and_destroy_test_hybrid_synth()
{
  gboolean success;

  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Hybrid Synth");

  CU_ASSERT(success == TRUE);

  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_hybrid_fm_synth()
{
  gboolean success;

  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Hybrid FM Synth");

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

void
ags_functional_machine_add_and_destroy_test_sf2_synth()
{
  gboolean success;

  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "SF2 Synth");

  CU_ASSERT(success == TRUE);

  /* destroy fplayer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_pitch_sampler()
{
  gboolean success;

  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Sampler");

  CU_ASSERT(success == TRUE);

  /* destroy fplayer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_sfz_synth()
{
  gboolean success;

  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "SFZ Synth");

  CU_ASSERT(success == TRUE);

  /* destroy fplayer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_add_and_destroy_test_audiorec()
{
  gboolean success;

  /* add audiorec */
  success = ags_functional_test_util_add_machine(NULL,
						 "Audiorec");

  CU_ASSERT(success == TRUE);

  /* destroy audiorec */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
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
  ags_test_init(&argc, &argv,
		AGS_TEST_CONFIG);
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_test_init(&argc, &argv,
		  str);
  }else{
    ags_test_init(&argc, &argv,
		  AGS_FUNCTIONAL_MACHINE_ADD_AND_DESTROY_TEST_CONFIG);
  }
#endif
    
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_machine_add_and_destroy_test_add_test, &is_available);
  
  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
