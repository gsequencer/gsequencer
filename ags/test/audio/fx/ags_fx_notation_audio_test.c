/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

int ags_fx_notation_audio_test_init_suite();
int ags_fx_notation_audio_test_clean_suite();

void ags_fx_notation_audio_stub_finalize();

void ags_fx_notation_audio_test_new();
void ags_fx_notation_audio_test_dispose();
void ags_fx_notation_audio_test_finalize();
void ags_fx_notation_audio_test_test_flags();
void ags_fx_notation_audio_test_set_flags();
void ags_fx_notation_audio_test_unset_flags();
void ags_fx_notation_audio_test_get_feed_note();
void ags_fx_notation_audio_test_add_feed_note();
void ags_fx_notation_audio_test_remove_feed_note();

gboolean finalized;

void
ags_fx_notation_audio_stub_finalize(GObject *gobject)
{
  finalized = TRUE;
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_notation_audio_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_notation_audio_test_clean_suite()
{  
  return(0);
}

void
ags_fx_notation_audio_test_new()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  CU_ASSERT(fx_notation_audio != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO(fx_notation_audio)->audio == audio);

  CU_ASSERT(fx_notation_audio->bpm != NULL);
  CU_ASSERT(fx_notation_audio->tact != NULL);
  CU_ASSERT(fx_notation_audio->delay != NULL);
  CU_ASSERT(fx_notation_audio->duration != NULL);
  CU_ASSERT(fx_notation_audio->loop != NULL);
  CU_ASSERT(fx_notation_audio->loop_start != NULL);
  CU_ASSERT(fx_notation_audio->loop_end != NULL);
}


void
ags_fx_notation_audio_test_dispose()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  g_object_run_dispose(fx_notation_audio);

  CU_ASSERT(fx_notation_audio->bpm == NULL);
  CU_ASSERT(fx_notation_audio->tact == NULL);
  CU_ASSERT(fx_notation_audio->delay == NULL);
  CU_ASSERT(fx_notation_audio->duration == NULL);
  CU_ASSERT(fx_notation_audio->loop == NULL);
  CU_ASSERT(fx_notation_audio->loop_start == NULL);
  CU_ASSERT(fx_notation_audio->loop_end == NULL);

  g_object_unref(fx_notation_audio);
}

void
ags_fx_notation_audio_test_finalize()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  GObjectClass *gobject_class;
  
  gpointer stub;
  
  /* attempt #0 */
  audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  gobject_class = (GObjectClass *) g_type_class_ref(AGS_TYPE_FX_NOTATION_AUDIO);
  
  finalized = FALSE;

  stub = gobject_class->finalize;
  gobject_class->finalize = ags_fx_notation_audio_stub_finalize;

  g_object_unref(fx_notation_audio);

  CU_ASSERT(finalized == TRUE);

  gobject_class->finalize = stub;

  /* attempt #1 */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  g_object_unref(fx_notation_audio);
}

void
ags_fx_notation_audio_test_test_flags()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  /* attempt #0 */
  fx_notation_audio->flags = 0;

  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_PLAY) == FALSE);
  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_RECORD) == FALSE);
  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_FEED) == FALSE);

  /* attempt #1 */
  fx_notation_audio->flags = AGS_FX_NOTATION_AUDIO_PLAY;
  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_PLAY) == TRUE);

  fx_notation_audio->flags = AGS_FX_NOTATION_AUDIO_RECORD;
  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_RECORD) == TRUE);

  fx_notation_audio->flags = AGS_FX_NOTATION_AUDIO_FEED;
  CU_ASSERT(ags_fx_notation_audio_test_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_FEED) == TRUE);
}

void
ags_fx_notation_audio_test_set_flags()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  fx_notation_audio->flags = 0;
  
  ags_fx_notation_audio_set_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_PLAY);
  ags_fx_notation_audio_set_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_RECORD);
  ags_fx_notation_audio_set_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_FEED);

  CU_ASSERT(fx_notation_audio->flags == (AGS_FX_NOTATION_AUDIO_PLAY |
					 AGS_FX_NOTATION_AUDIO_RECORD |
					 AGS_FX_NOTATION_AUDIO_FEED));
}

void
ags_fx_notation_audio_test_unset_flags()
{
  AgsAudio *audio;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  fx_notation_audio->flags = (AGS_FX_NOTATION_AUDIO_PLAY |
			      AGS_FX_NOTATION_AUDIO_RECORD |
			      AGS_FX_NOTATION_AUDIO_FEED);

  ags_fx_notation_audio_unset_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_PLAY);
  ags_fx_notation_audio_unset_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_RECORD);
  ags_fx_notation_audio_unset_flags(fx_notation_audio, AGS_FX_NOTATION_AUDIO_FEED);

  CU_ASSERT(fx_notation_audio->flags == 0);
}

void
ags_fx_notation_audio_test_get_feed_note()
{
  AgsAudio *audio;
  AgsNote *note;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  /* attempt #0 */
  CU_ASSERT(ags_fx_notation_audio_get_feed_note(fx_notation_audio) == NULL);

  /* attempt #1 */
  note = ags_note_new();

  fx_notation_audio->feed_note = g_list_prepend(fx_notation_audio->feed_note,
						note);
  
  CU_ASSERT(g_list_length(ags_fx_notation_audio_get_feed_note(fx_notation_audio)) == 1);
  CU_ASSERT(ags_fx_notation_audio_get_feed_note(fx_notation_audio)->data == note);
}

void
ags_fx_notation_audio_test_add_feed_note()
{
  AgsAudio *audio;
  AgsNote *note;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  /* attempt #0 */
  note = ags_note_new();

  ags_fx_notation_audio_add_feed_note(fx_notation_audio,
				      note);
  
  CU_ASSERT(g_list_length(ags_fx_notation_audio_get_feed_note(fx_notation_audio)) == 1);
  CU_ASSERT(ags_fx_notation_audio_get_feed_note(fx_notation_audio)->data == note);
}

void
ags_fx_notation_audio_test_remove_feed_note()
{
  AgsAudio *audio;
  AgsNote *note;
  AgsFxNotationAudio *fx_notation_audio;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_notation_audio = ags_fx_notation_audio_new(audio);

  /* attempt #1 */
  note = ags_note_new();

  fx_notation_audio->feed_note = g_list_prepend(fx_notation_audio->feed_note,
						note);

  ags_fx_notation_audio_remove_feed_note(fx_notation_audio,
					 note);
  
  CU_ASSERT(ags_fx_notation_audio_get_feed_note(fx_notation_audio) == NULL);
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
  pSuite = CU_add_suite("AgsFxNotationAudioTest", ags_fx_notation_audio_test_init_suite, ags_fx_notation_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxNotationAudio new", ags_fx_notation_audio_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio dispose", ags_fx_notation_audio_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio finalize", ags_fx_notation_audio_test_finalize) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio test flags", ags_fx_notation_audio_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio set flags", ags_fx_notation_audio_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio unset flags", ags_fx_notation_audio_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio get feed note", ags_fx_notation_audio_test_get_feed_note) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio add feed note", ags_fx_notation_audio_test_add_feed_note) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxNotationAudio remove feed note", ags_fx_notation_audio_test_remove_feed_note) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
