/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

int ags_fx_volume_audio_test_init_suite();
int ags_fx_volume_audio_test_clean_suite();

void ags_fx_volume_audio_stub_finalize(GObject *gobject);

void ags_fx_volume_audio_test_new();
void ags_fx_volume_audio_test_dispose();
void ags_fx_volume_audio_test_finalize();

gboolean finalized;

void
ags_fx_volume_audio_stub_finalize(GObject *gobject)
{
  finalized = TRUE;
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_volume_audio_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_volume_audio_test_clean_suite()
{  
  return(0);
}

void
ags_fx_volume_audio_test_new()
{
  AgsAudio *audio;
  AgsFxVolumeAudio *fx_volume_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_volume_audio = ags_fx_volume_audio_new(audio);

  CU_ASSERT(fx_volume_audio != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO(fx_volume_audio)->audio == audio);

  CU_ASSERT(fx_volume_audio->muted != NULL);
}

void
ags_fx_volume_audio_test_dispose()
{
  AgsAudio *audio;
  AgsFxVolumeAudio *fx_volume_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
  
  fx_volume_audio = ags_fx_volume_audio_new(audio);

  g_object_run_dispose(fx_volume_audio);

  CU_ASSERT(fx_volume_audio->muted == NULL);

  g_object_unref(fx_volume_audio);
}

void
ags_fx_volume_audio_test_finalize()
{
  AgsAudio *audio;
  AgsFxVolumeAudio *fx_volume_audio;

  GObjectClass *gobject_class;
  
  gpointer stub;
  
  /* attempt #0 */
  audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
  
  fx_volume_audio = ags_fx_volume_audio_new(audio);

  gobject_class = (GObjectClass *) g_type_class_ref(AGS_TYPE_FX_VOLUME_AUDIO);
  
  finalized = FALSE;

  stub = gobject_class->finalize;
  gobject_class->finalize = ags_fx_volume_audio_stub_finalize;

  g_object_unref(fx_volume_audio);

  CU_ASSERT(finalized == TRUE);

  gobject_class->finalize = stub;

  /* attempt #1 */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_volume_audio = ags_fx_volume_audio_new(audio);

  g_object_unref(fx_volume_audio);
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
  pSuite = CU_add_suite("AgsFxVolumeAudioTest", ags_fx_volume_audio_test_init_suite, ags_fx_volume_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxVolumeAudio new", ags_fx_volume_audio_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxVolumeAudio dispose", ags_fx_volume_audio_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxVolumeAudio finalize", ags_fx_volume_audio_test_finalize) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
