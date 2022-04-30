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

int ags_fx_pattern_audio_processor_test_init_suite();
int ags_fx_pattern_audio_processor_test_clean_suite();

void ags_fx_pattern_audio_processor_test_new();
void ags_fx_pattern_audio_processor_test_run_inter();
void ags_fx_pattern_audio_processor_test_key_on();
void ags_fx_pattern_audio_processor_test_counter_change();
void ags_fx_pattern_audio_processor_test_play();

#define AGS_FX_PATTERN_AUDIO_PROCESSOR_TEST_CONFIG "[generic]\n"	\
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

AgsAudioApplicationContext *audio_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_pattern_audio_processor_test_init_suite()
{ 
  AgsConfig *config;

  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FX_PATTERN_AUDIO_PROCESSOR_TEST_CONFIG,
			    strlen(AGS_FX_PATTERN_AUDIO_PROCESSOR_TEST_CONFIG));

  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_pattern_audio_processor_test_clean_suite()
{  
  g_object_unref(audio_application_context);

  return(0);
}

void
ags_fx_pattern_audio_processor_test_new()
{
  AgsAudio *audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  fx_pattern_audio_processor = ags_fx_pattern_audio_processor_new(audio);

  CU_ASSERT(fx_pattern_audio_processor != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO_RUN(fx_pattern_audio_processor)->audio == audio);
}

void
ags_fx_pattern_audio_processor_test_run_inter()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;
  AgsChannel *input;
  AgsNote *note;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  AgsRecallContainer *recall_container;
  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  guint i;
  
  devout = audio_application_context->default_soundcard;

  parent_recall_id = ags_recall_id_new();
  recall_id = ags_recall_id_new();

  recycling_context = ags_recycling_context_new(8);
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  parent_recycling_context = ags_recycling_context_new(1);
  g_object_set(parent_recall_id,
	       "recycling-context", parent_recycling_context,
	       NULL);
  g_object_set(recycling_context,
	       "parent", parent_recycling_context,
	       NULL);
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       "output-soundcard", devout,
		       NULL);
  ags_audio_set_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING);
  ags_audio_set_flags(audio, AGS_AUDIO_INPUT_HAS_RECYCLING);

  ags_audio_set_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER);

  audio->bank_dim[0] = 1;
  audio->bank_dim[1] = 1;

  audio->bank_dim[2] = 64;
  
  ags_audio_add_recycling_context(audio,
				  parent_recycling_context);
  
  ags_audio_set_audio_channels(audio,
			       1, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  ags_recycling_context_replace(parent_recycling_context,
				audio->output->first_recycling,
				0);

  input = audio->input;
  
  for(i = 0; i < 8 && input != NULL; i++){
    g_object_set(input,
		 "recycling-context", recycling_context,
		 NULL);
    
    ags_channel_add_recall_id(input,
			      recall_id);
    ags_recycling_context_replace(recycling_context,
				  input->first_recycling,
				  i);

    input = input->next;
  }
  
  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);

  fx_pattern_audio = ags_fx_pattern_audio_new(audio);
  
  ags_recall_set_sound_scope(fx_pattern_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_pattern_audio);

  g_object_set(fx_pattern_audio,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio != NULL);

  /* audio processor */  
  fx_pattern_audio_processor = ags_fx_pattern_audio_processor_new(audio);
  ags_recall_set_sound_scope(fx_pattern_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_pattern_audio_processor,
	       "recall-audio", fx_pattern_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_pattern_audio_processor);

  g_object_set(fx_pattern_audio_processor,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio_processor != NULL);

  /* attempt #0 */
  input = audio->input;

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
  
  ags_recall_run_inter(fx_pattern_audio_processor);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);

  /* attempt #1 */
  input = ags_channel_nth(audio->input, 7);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
  
  ags_recall_run_inter(fx_pattern_audio_processor);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
}

void
ags_fx_pattern_audio_processor_test_key_on()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;
  AgsChannel *input;
  AgsNote *note;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  AgsRecallContainer *recall_container;
  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  guint i;
  
  devout = audio_application_context->default_soundcard;

  parent_recall_id = ags_recall_id_new();
  recall_id = ags_recall_id_new();

  recycling_context = ags_recycling_context_new(8);
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  parent_recycling_context = ags_recycling_context_new(1);
  g_object_set(parent_recall_id,
	       "recycling-context", parent_recycling_context,
	       NULL);
  g_object_set(recycling_context,
	       "parent", parent_recycling_context,
	       NULL);
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       "output-soundcard", devout,
		       NULL);
  ags_audio_set_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING);
  ags_audio_set_flags(audio, AGS_AUDIO_INPUT_HAS_RECYCLING);

  ags_audio_add_recycling_context(audio,
				  parent_recycling_context);
  
  ags_audio_set_audio_channels(audio,
			       1, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  ags_recycling_context_replace(parent_recycling_context,
				audio->output->first_recycling,
				0);

  input = audio->input;
  
  for(i = 0; i < 8 && input != NULL; i++){
    g_object_set(input,
		 "recycling-context", recycling_context,
		 NULL);
    
    ags_channel_add_recall_id(input,
			      recall_id);
    ags_recycling_context_replace(recycling_context,
				  input->first_recycling,
				  i);

    input = input->next;
  }
  
  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);

  fx_pattern_audio = ags_fx_pattern_audio_new(audio);
  
  ags_recall_set_sound_scope(fx_pattern_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_pattern_audio);

  g_object_set(fx_pattern_audio,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio != NULL);

  /* audio processor */  
  fx_pattern_audio_processor = ags_fx_pattern_audio_processor_new(audio);
  ags_recall_set_sound_scope(fx_pattern_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_pattern_audio_processor,
	       "recall-audio", fx_pattern_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_pattern_audio_processor);

  g_object_set(fx_pattern_audio_processor,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio_processor != NULL);

  /* attempt #0 */
  note = ags_note_new();
  g_object_set(note,
	       "x0", 0,
	       "x1", 1,
	       "y", 0,
	       NULL);

  ags_fx_pattern_audio_processor_key_on(fx_pattern_audio_processor,
					note,
					AGS_FX_PATTERN_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					0);

  /* attempt #1 */
  g_object_set(note,
	       "y", 7,
	       NULL);

  ags_fx_pattern_audio_processor_key_on(fx_pattern_audio_processor,
					note,
					AGS_FX_PATTERN_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					0);

  /* attempt #2 */
  g_object_set(note,
	       "y", 48,
	       NULL);

  ags_fx_pattern_audio_processor_key_on(fx_pattern_audio_processor,
					note,
					AGS_FX_PATTERN_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					0);
}

void
ags_fx_pattern_audio_processor_test_counter_change()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;
  AgsChannel *input;
  AgsNote *note;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  AgsRecallContainer *recall_container;
  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  guint i;
  
  devout = audio_application_context->default_soundcard;

  parent_recall_id = ags_recall_id_new();
  recall_id = ags_recall_id_new();

  recycling_context = ags_recycling_context_new(8);
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  parent_recycling_context = ags_recycling_context_new(1);
  g_object_set(parent_recall_id,
	       "recycling-context", parent_recycling_context,
	       NULL);
  g_object_set(recycling_context,
	       "parent", parent_recycling_context,
	       NULL);
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       "output-soundcard", devout,
		       NULL);
  ags_audio_set_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING);
  ags_audio_set_flags(audio, AGS_AUDIO_INPUT_HAS_RECYCLING);

  ags_audio_add_recycling_context(audio,
				  parent_recycling_context);
  
  ags_audio_set_audio_channels(audio,
			       1, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  ags_recycling_context_replace(parent_recycling_context,
				audio->output->first_recycling,
				0);

  input = audio->input;
  
  for(i = 0; i < 8 && input != NULL; i++){
    g_object_set(input,
		 "recycling-context", recycling_context,
		 NULL);
    
    ags_channel_add_recall_id(input,
			      recall_id);
    ags_recycling_context_replace(recycling_context,
				  input->first_recycling,
				  i);

    input = input->next;
  }
  
  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);

  fx_pattern_audio = ags_fx_pattern_audio_new(audio);
  
  ags_recall_set_sound_scope(fx_pattern_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_pattern_audio);

  g_object_set(fx_pattern_audio,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio != NULL);

  /* audio processor */  
  fx_pattern_audio_processor = ags_fx_pattern_audio_processor_new(audio);
  ags_recall_set_sound_scope(fx_pattern_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_pattern_audio_processor,
	       "recall-audio", fx_pattern_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_pattern_audio_processor);

  g_object_set(fx_pattern_audio_processor,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio_processor != NULL);

  /* attempt #0 */
  ags_fx_pattern_audio_processor_counter_change(fx_pattern_audio_processor);

  /* attempt #1 */
  ags_fx_pattern_audio_processor_counter_change(fx_pattern_audio_processor);
}

void
ags_fx_pattern_audio_processor_test_play()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;
  AgsChannel *input;
  AgsNote *note;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  AgsRecallContainer *recall_container;
  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  guint i;
  
  devout = audio_application_context->default_soundcard;

  parent_recall_id = ags_recall_id_new();
  recall_id = ags_recall_id_new();

  recycling_context = ags_recycling_context_new(8);
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  parent_recycling_context = ags_recycling_context_new(1);
  g_object_set(parent_recall_id,
	       "recycling-context", parent_recycling_context,
	       NULL);
  g_object_set(recycling_context,
	       "parent", parent_recycling_context,
	       NULL);
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       "output-soundcard", devout,
		       NULL);
  ags_audio_set_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING);
  ags_audio_set_flags(audio, AGS_AUDIO_INPUT_HAS_RECYCLING);

  ags_audio_set_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER);

  audio->bank_dim[0] = 1;
  audio->bank_dim[1] = 1;

  audio->bank_dim[2] = 64;
  
  ags_audio_add_recycling_context(audio,
				  parent_recycling_context);
  
  ags_audio_set_audio_channels(audio,
			       1, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  ags_recycling_context_replace(parent_recycling_context,
				audio->output->first_recycling,
				0);

  input = audio->input;
  
  for(i = 0; i < 8 && input != NULL; i++){
    g_object_set(input,
		 "recycling-context", recycling_context,
		 NULL);
    
    ags_channel_add_recall_id(input,
			      recall_id);
    ags_recycling_context_replace(recycling_context,
				  input->first_recycling,
				  i);

    input = input->next;
  }
  
  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);

  fx_pattern_audio = ags_fx_pattern_audio_new(audio);
  
  ags_recall_set_sound_scope(fx_pattern_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_pattern_audio);

  g_object_set(fx_pattern_audio,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio != NULL);

  /* audio processor */  
  fx_pattern_audio_processor = ags_fx_pattern_audio_processor_new(audio);
  ags_recall_set_sound_scope(fx_pattern_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_pattern_audio_processor,
	       "recall-audio", fx_pattern_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_pattern_audio_processor);

  g_object_set(fx_pattern_audio_processor,
	       "recall-id", recall_id,
	       NULL);
  
  CU_ASSERT(fx_pattern_audio_processor != NULL);

  /* attempt #0 */
  input = audio->input;

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
  
  ags_fx_pattern_audio_processor_play(fx_pattern_audio_processor);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);

  /* attempt #1 */
  input = ags_channel_nth(audio->input, 7);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
  
  ags_fx_pattern_audio_processor_play(fx_pattern_audio_processor);

  ags_pattern_toggle_bit(input->pattern->data, 0, 0, 0);
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
  pSuite = CU_add_suite("AgsFxPatternAudioProcessorTest", ags_fx_pattern_audio_processor_test_init_suite, ags_fx_pattern_audio_processor_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxPatternAudioProcessor new", ags_fx_pattern_audio_processor_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxPatternAudioProcessor run inter", ags_fx_pattern_audio_processor_test_run_inter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxPatternAudioProcessor key on", ags_fx_pattern_audio_processor_test_key_on) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxPatternAudioProcessor play", ags_fx_pattern_audio_processor_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxPatternAudioProcessor counter change", ags_fx_pattern_audio_processor_test_counter_change) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
