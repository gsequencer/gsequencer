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

int ags_sequencer_test_init_suite();
int ags_sequencer_test_clean_suite();

void ags_sequencer_test_set_device();
void ags_sequencer_test_get_device();
void ags_sequencer_test_is_starting();
void ags_sequencer_test_is_playing();
void ags_sequencer_test_is_recording();
void ags_sequencer_test_play_init();
void ags_sequencer_test_play();
void ags_sequencer_test_record_init();
void ags_sequencer_test_record();
void ags_sequencer_test_stop();
void ags_sequencer_test_tic();
void ags_sequencer_test_offset_changed();
void ags_sequencer_test_get_buffer();
void ags_sequencer_test_get_next_buffer();  
void ags_sequencer_test_set_bpm();
void ags_sequencer_test_get_bpm();
void ags_sequencer_test_set_delay_factor();
void ags_sequencer_test_get_delay_factor();
void ags_sequencer_test_set_start_note_offset();
void ags_sequencer_test_get_start_note_offset();
void ags_sequencer_test_set_note_offset();
void ags_sequencer_test_get_note_offset();

#define AGS_SEQUENCER_TEST_SET_DEVICE_DEVICE "ags-test-default-0"

#define AGS_SEQUENCER_TEST_SET_BPM_BPM (145.0)

#define AGS_SEQUENCER_TEST_SET_DELAY_FACTOR_DELAY_FACTOR (1.0 / 8.0)

#define AGS_SEQUENCER_TEST_SET_START_NOTE_OFFSET_NOTE_OFFSET (16 * 64)
#define AGS_SEQUENCER_TEST_SET_NOTE_OFFSET_NOTE_OFFSET (16 * 64)

GType sequencer_test_types[4];

extern AgsApplicationContext *ags_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sequencer_test_init_suite()
{
  guint i;

  ags_audio_application_context_new();
  
  i = 0;
  
  sequencer_test_types[(i++)] = AGS_TYPE_ALSA_MIDIIN;
  sequencer_test_types[(i++)] = AGS_TYPE_CORE_AUDIO_MIDIIN;
  sequencer_test_types[(i++)] = AGS_TYPE_JACK_MIDIIN;
  sequencer_test_types[(i++)] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sequencer_test_clean_suite()
{  
  return(0);
}

void
ags_sequencer_test_set_device()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    g_message("instantiated %s", G_OBJECT_TYPE_NAME(current));    

    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->set_device == NULL){
      g_message("AgsSequencer::set-device missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_sequencer_set_device(AGS_SEQUENCER(current),
			     AGS_SEQUENCER_TEST_SET_DEVICE_DEVICE);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_device()
{
  GObject *current;
  
  GType current_type;

  gchar *retval;
  
  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);

    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_device == NULL){
      g_message("AgsSequencer::get-device missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_is_starting()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->is_starting == NULL){
      g_message("AgsSequencer::is-starting missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_is_starting(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_is_playing()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->is_playing == NULL){
      g_message("AgsSequencer::is-playing missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_is_playing(AGS_SEQUENCER(current));
  }

  //TODO:JK: fix this
  //  CU_ASSERT(success);
}

void
ags_sequencer_test_is_recording()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->is_recording == NULL){
      g_message("AgsSequencer::is-recording missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_is_recording(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_play_init()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->play_init == NULL){
      g_message("AgsSequencer::play-init missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  //TODO:JK: fix this
  //  CU_ASSERT(success);
}

void
ags_sequencer_test_play()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->play == NULL){
      g_message("AgsSequencer::play missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  //TODO:JK: fix this
  //  CU_ASSERT(success);
}

void
ags_sequencer_test_record_init()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->record_init == NULL){
      g_message("AgsSequencer::record-init missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_record()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->record == NULL){
      g_message("AgsSequencer::record missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_stop()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->stop == NULL){
      g_message("AgsSequencer::stop missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_tic()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->tic == NULL){
      g_message("AgsSequencer::tic missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_tic(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_offset_changed()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->offset_changed == NULL){
      g_message("AgsSequencer::offset-changed missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_offset_changed(AGS_SEQUENCER(current), 0);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_buffer()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint buffer_length;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_buffer == NULL){
      g_message("AgsSequencer::get-buffer missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_get_buffer(AGS_SEQUENCER(current),
			     &buffer_length);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_next_buffer()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint buffer_length;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_next_buffer == NULL){
      g_message("AgsSequencer::get-next-buffer missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_get_next_buffer(AGS_SEQUENCER(current),
				  &buffer_length);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_set_bpm()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->set_bpm == NULL){
      g_message("AgsSequencer::set-bpm missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_set_bpm(AGS_SEQUENCER(current),
			  AGS_SEQUENCER_TEST_SET_BPM_BPM);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_bpm()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_bpm == NULL){
      g_message("AgsSequencer::get-bpm missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_sequencer_get_bpm(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_set_delay_factor()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->set_delay_factor == NULL){
      g_message("AgsSequencer::set-delay-factor missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_set_delay_factor(AGS_SEQUENCER(current),
				   AGS_SEQUENCER_TEST_SET_DELAY_FACTOR_DELAY_FACTOR);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_delay_factor()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_delay_factor == NULL){
      g_message("AgsSequencer::get-delay-factor missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_sequencer_get_delay_factor(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_set_start_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->set_start_note_offset == NULL){
      g_message("AgsSequencer::set-start-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_set_start_note_offset(AGS_SEQUENCER(current),
				  AGS_SEQUENCER_TEST_SET_START_NOTE_OFFSET_NOTE_OFFSET);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_start_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_start_note_offset == NULL){
      g_message("AgsSequencer::get-start-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_sequencer_get_start_note_offset(AGS_SEQUENCER(current));
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_set_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->set_note_offset == NULL){
      g_message("AgsSequencer::set-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_sequencer_set_note_offset(AGS_SEQUENCER(current),
				  AGS_SEQUENCER_TEST_SET_NOTE_OFFSET_NOTE_OFFSET);
  }

  CU_ASSERT(success);
}

void
ags_sequencer_test_get_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  ags_application_context = NULL;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; sequencer_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(sequencer_test_types[i],
			   NULL);
    
    if(AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(current))->get_note_offset == NULL){
      g_message("AgsSequencer::get-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_sequencer_get_note_offset(AGS_SEQUENCER(current));
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
  pSuite = CU_add_suite("AgsSequencerTest", ags_sequencer_test_init_suite, ags_sequencer_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSequencer set device", ags_sequencer_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSequencer get device", ags_sequencer_test_get_device) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
