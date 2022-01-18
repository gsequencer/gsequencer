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
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <math.h>

int ags_fx_synth_audio_test_init_suite();
int ags_fx_synth_audio_test_clean_suite();

void ags_fx_synth_audio_stub_finalize();

void ags_fx_synth_audio_test_new();
void ags_fx_synth_audio_test_dispose();
void ags_fx_synth_audio_test_finalize();
void ags_fx_synth_audio_test_scope_data_alloc();
void ags_fx_synth_audio_test_scope_data_free();
void ags_fx_synth_audio_test_channel_data_alloc();
void ags_fx_synth_audio_test_channel_data_free();
void ags_fx_synth_audio_test_input_data_alloc();
void ags_fx_synth_audio_test_input_data_free();

gboolean finalized;

void
ags_fx_synth_audio_stub_finalize(GObject *gobject)
{
  finalized = TRUE;
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_synth_audio_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_synth_audio_test_clean_suite()
{  
  return(0);
}

void
ags_fx_synth_audio_test_new()
{
  AgsAudio *audio;
  AgsFxSynthAudio *fx_synth_audio;

  guint i;
  gboolean success;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_synth_audio = ags_fx_synth_audio_new(audio);

  CU_ASSERT(fx_synth_audio != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO(fx_synth_audio)->audio == audio);
  
  success = TRUE;
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(fx_synth_audio->scope_data[i]->parent == NULL){
	success = FALSE;
      
	break;
      }
    }else{
      if(fx_synth_audio->scope_data[i] != NULL){
	success = FALSE;
	
	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fx_synth_audio_test_dispose()
{
  AgsAudio *audio;
  AgsFxSynthAudio *fx_synth_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_synth_audio = ags_fx_synth_audio_new(audio);

  g_object_run_dispose(fx_synth_audio);

  g_object_unref(fx_synth_audio);
}

void
ags_fx_synth_audio_test_finalize()
{
  AgsAudio *audio;
  AgsFxSynthAudio *fx_synth_audio;

  GObjectClass *gobject_class;
  
  gpointer stub;
  
  /* attempt #0 */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_synth_audio = ags_fx_synth_audio_new(audio);

  gobject_class = (GObjectClass *) g_type_class_ref(AGS_TYPE_FX_SYNTH_AUDIO);
  
  finalized = FALSE;

  stub = gobject_class->finalize;
  gobject_class->finalize = ags_fx_synth_audio_stub_finalize;
  
  g_object_unref(fx_synth_audio);

  CU_ASSERT(finalized == TRUE);

  gobject_class->finalize = stub;

  /* attempt #1 */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_synth_audio = ags_fx_synth_audio_new(audio);

  g_object_unref(fx_synth_audio);
}

void
ags_fx_synth_audio_test_scope_data_alloc()
{
  AgsFxSynthAudioScopeData *scope_data;

  scope_data = ags_fx_synth_audio_scope_data_alloc();

  CU_ASSERT(scope_data != NULL);
}

void
ags_fx_synth_audio_test_scope_data_free()
{
  AgsFxSynthAudioScopeData *scope_data;

  scope_data = ags_fx_synth_audio_scope_data_alloc();

  CU_ASSERT(scope_data != NULL);

  ags_fx_synth_audio_scope_data_free(scope_data);
}

void
ags_fx_synth_audio_test_channel_data_alloc()
{
  AgsFxSynthAudioChannelData *channel_data;

  channel_data = ags_fx_synth_audio_channel_data_alloc();

  CU_ASSERT(channel_data != NULL);
}

void
ags_fx_synth_audio_test_channel_data_free()
{
  AgsFxSynthAudioChannelData *channel_data;

  channel_data = ags_fx_synth_audio_channel_data_alloc();

  CU_ASSERT(channel_data != NULL);

  ags_fx_synth_audio_channel_data_free(channel_data);
}

void
ags_fx_synth_audio_test_input_data_alloc()
{
  AgsFxSynthAudioInputData *input_data;

  input_data = ags_fx_synth_audio_input_data_alloc();

  CU_ASSERT(input_data != NULL);
}

void
ags_fx_synth_audio_test_input_data_free()
{
  AgsFxSynthAudioInputData *input_data;

  input_data = ags_fx_synth_audio_input_data_alloc();

  CU_ASSERT(input_data != NULL);

  ags_fx_synth_audio_input_data_free(input_data);
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
  putenv("VST3_PATH=\"\"");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFxSynthAudioTest", ags_fx_synth_audio_test_init_suite, ags_fx_synth_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxSynthAudio new", ags_fx_synth_audio_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio dispose", ags_fx_synth_audio_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio finalize", ags_fx_synth_audio_test_finalize) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio scope data alloc", ags_fx_synth_audio_test_scope_data_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio scope data free", ags_fx_synth_audio_test_scope_data_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio channel data alloc", ags_fx_synth_audio_test_channel_data_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio channel data free", ags_fx_synth_audio_test_channel_data_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio input data alloc", ags_fx_synth_audio_test_input_data_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxSynthAudio input data free", ags_fx_synth_audio_test_input_data_free) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
