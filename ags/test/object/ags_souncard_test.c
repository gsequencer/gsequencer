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

int ags_soundcard_test_init_suite();
int ags_soundcard_test_clean_suite();

void ags_soundcard_test_set_application_context();
void ags_soundcard_test_get_application_context();
void ags_soundcard_test_set_device();
void ags_soundcard_test_get_device();
void ags_soundcard_test_pcm_info();
void ags_soundcard_test_get_capability();
void ags_soundcard_test_set_presets();
void ags_soundcard_test_get_presets();
void ags_soundcard_test_list_cards();
void ags_soundcard_test_get_poll_fd();
void ags_soundcard_test_is_available();
void ags_soundcard_test_is_starting();
void ags_soundcard_test_is_playing();
void ags_soundcard_test_is_recording();
void ags_soundcard_test_get_uptime();
void ags_soundcard_test_play_init();
void ags_soundcard_test_play();
void ags_soundcard_test_record_init();
void ags_soundcard_test_record();
void ags_soundcard_test_stop();
void ags_soundcard_test_tic();
void ags_soundcard_test_offset_changed();
void ags_soundcard_test_get_buffer();
void ags_soundcard_test_get_next_buffer();
void ags_soundcard_test_get_prev_buffer();
void ags_soundcard_test_lock_buffer();
void ags_soundcard_test_unlock_buffer();
void ags_soundcard_test_set_bpm();
void ags_soundcard_test_get_bpm();
void ags_soundcard_test_set_delay_factor();
void ags_soundcard_test_get_delay_factor();
void ags_soundcard_test_get_absolute_delay();
void ags_soundcard_test_get_delay();
void ags_soundcard_test_get_attack();
void ags_soundcard_test_get_delay_counter();
void ags_soundcard_test_set_note_offset();
void ags_soundcard_test_get_note_offset();
void ags_soundcard_test_set_note_offset_absolute();
void ags_soundcard_test_get_note_offset_absolute();
void ags_soundcard_test_set_loop();
void ags_soundcard_test_get_loop();
void ags_soundcard_test_get_loop_offset();

#define AGS_SOUNDCARD_TEST_SET_DEVICE_DEVICE "ags-test-default-0"

#define AGS_SOUNDCARD_TEST_SET_PRESETS_CHANNELS (2)
#define AGS_SOUNDCARD_TEST_SET_PRESETS_SAMPLERATE (44100)
#define AGS_SOUNDCARD_TEST_SET_PRESETS_BUFFER_SIZE (512)
#define AGS_SOUNDCARD_TEST_SET_PRESETS_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_SOUNDCARD_TEST_SET_BPM_BPM (145.0)

#define AGS_SOUNDCARD_TEST_SET_DELAY_FACTOR_DELAY_FACTOR (1.0 / 8.0)

#define AGS_SOUNDCARD_TEST_SET_NOTE_OFFSET_NOTE_OFFSET (16 * 64)

#define AGS_SOUNDCARD_TEST_SET_NOTE_OFFSET_ABSOLUTE_NOTE_OFFSET (16 * 64)

#define AGS_SOUNDCARD_TEST_SET_LOOP_LOOP_LEFT (0)
#define AGS_SOUNDCARD_TEST_SET_LOOP_LOOP_RIGHT (16 * 4)
#define AGS_SOUNDCARD_TEST_SET_DO_LOOP (TRUE)
  
GType soundcard_test_types[10];
 
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_test_init_suite()
{
  guint i;

  i = 0;
  
  soundcard_test_types[i++] = AGS_TYPE_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_FIFOOUT;
  soundcard_test_types[i++] = AGS_TYPE_CORE_AUDIO_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_CORE_AUDIO_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_JACK_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_JACK_DEVIN;
  soundcard_test_types[i++] = AGS_TYPE_PULSE_DEVOUT;
  soundcard_test_types[i++] = AGS_TYPE_PULSE_DEVIN;
  soundcard_test_types[i++] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_soundcard_test_clean_suite()
{  
  return(0);
}

void
ags_soundcard_test_set_application_context()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  application_context = ags_thread_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_application_context == NULL){
      g_message("AgsSoundcard::set-application-context missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_soundcard_set_application_context(AGS_SOUNDCARD(current),
					  application_context);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_application_context()
{
  AgsApplicationContext *application_context;
  AgsApplicationContext *retval;
  
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  application_context = ags_thread_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    ags_soundcard_set_application_context(AGS_SOUNDCARD(current),
					  application_context);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_application_context == NULL){
      g_message("AgsSoundcard::get-application-context missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_application_context(AGS_SOUNDCARD(current));

    if(retval != application_context){
      g_message("AgsSoundcard::get-application-context failed: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_device()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_device == NULL){
      g_message("AgsSoundcard::set-device missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_soundcard_set_device(AGS_SOUNDCARD(current),
			     AGS_SOUNDCARD_TEST_SET_DEVICE_DEVICE);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_device()
{
  GObject *current;
  
  GType current_type;

  gchar *retval;
  
  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_device == NULL){
      g_message("AgsSoundcard::get-device missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    retval = ags_soundcard_get_device(AGS_SOUNDCARD(current));

    if(retval == NULL){
      g_message("AgsSoundcard::get-device returns NULL: %s", G_OBJECT_TYPE_NAME(current));

      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_pcm_info()
{
  GObject *current;
  
  GType current_type;

  gchar *retval;
  
  guint i;
  gboolean success;

  GError *error;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->pcm_info == NULL){
      g_message("AgsSoundcard::pcm-info missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    error = NULL;
    retval = ags_soundcard_pcm_info(AGS_SOUNDCARD(current),
				    NULL,
				    NULL, NULL,
				    NULL, NULL,
				    NULL, NULL,
				    &error);

    if(error != NULL){
      g_message("%s", error->message);
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_capability()
{
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_capability == NULL){
      g_message("AgsSoundcard::get-capability missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_capability(AGS_SOUNDCARD(current));

    if(!(retval == AGS_SOUNDCARD_CAPABILITY_PLAYBACK ||
	 retval == AGS_SOUNDCARD_CAPABILITY_CAPTURE)){
      g_message("AgsSoundcard::get-capability failed: %s", G_OBJECT_TYPE_NAME(current));

      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_presets()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_presets == NULL){
      g_message("AgsSoundcard::set-presets missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_presets(AGS_SOUNDCARD(current),
			      AGS_SOUNDCARD_TEST_SET_PRESETS_CHANNELS,
			      AGS_SOUNDCARD_TEST_SET_PRESETS_SAMPLERATE,
			      AGS_SOUNDCARD_TEST_SET_PRESETS_BUFFER_SIZE,
			      AGS_SOUNDCARD_TEST_SET_PRESETS_FORMAT);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_presets()
{
  GObject *current;
  
  GType current_type;

  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;
    
  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_presets == NULL){
      g_message("AgsSoundcard::get-presets missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_presets(AGS_SOUNDCARD(current),
			      &channels,
			      &samplerate,
			      &buffer_size,
			      &format);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_list_cards()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  GList *card_id, *card_name;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);

    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->list_cards == NULL){
      g_message("AgsSoundcard::list-cards missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    card_id = NULL;
    card_name = NULL;
    
    ags_soundcard_list_cards(AGS_SOUNDCARD(current),
			     &card_id, &card_name);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_poll_fd()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    ags_soundcard_get_poll_fd(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_is_available()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    ags_soundcard_is_available(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_is_starting()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->is_starting == NULL){
      g_message("AgsSoundcard::is-starting missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_is_starting(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_is_playing()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_PLAYBACK &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->is_playing == NULL){
      g_message("AgsSoundcard::is-playing missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_is_playing(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_is_recording()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_CAPTURE &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->is_recording == NULL){
      g_message("AgsSoundcard::is-recording missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_is_recording(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_uptime()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_uptime == NULL){
      g_message("AgsSoundcard::get-uptime missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_uptime(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_play_init()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_PLAYBACK &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->play_init == NULL){
      g_message("AgsSoundcard::play-init missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_play()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_PLAYBACK &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->play == NULL){
      g_message("AgsSoundcard::play missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_record_init()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_CAPTURE &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->record_init == NULL){
      g_message("AgsSoundcard::record-init missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_record()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(ags_soundcard_get_capability(AGS_SOUNDCARD(current)) == AGS_SOUNDCARD_CAPABILITY_CAPTURE &&
       AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->record == NULL){
      g_message("AgsSoundcard::record missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_stop()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->stop == NULL){
      g_message("AgsSoundcard::stop missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_tic()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->tic == NULL){
      g_message("AgsSoundcard::tic missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_tic(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_offset_changed()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->offset_changed == NULL){
      g_message("AgsSoundcard::offset-changed missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_offset_changed(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_buffer()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_buffer == NULL){
      g_message("AgsSoundcard::get-buffer missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_buffer(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_next_buffer()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_next_buffer == NULL){
      g_message("AgsSoundcard::get-next-buffer missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_next_buffer(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_prev_buffer()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_prev_buffer == NULL){
      g_message("AgsSoundcard::get-prev-buffer missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_lock_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_unlock_buffer()
{
  //TODO:JK: implement me
}

void
ags_soundcard_test_set_bpm()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_bpm == NULL){
      g_message("AgsSoundcard::set-bpm missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_bpm(AGS_SOUNDCARD(current),
			  AGS_SOUNDCARD_TEST_SET_BPM_BPM);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_bpm()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_bpm == NULL){
      g_message("AgsSoundcard::get-bpm missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_bpm(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_delay_factor()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_delay_factor == NULL){
      g_message("AgsSoundcard::set-delay-factor missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_delay_factor(AGS_SOUNDCARD(current),
				   AGS_SOUNDCARD_TEST_SET_DELAY_FACTOR_DELAY_FACTOR);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_delay_factor()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_delay_factor == NULL){
      g_message("AgsSoundcard::get-delay-factor missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_absolute_delay()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_absolute_delay == NULL){
      g_message("AgsSoundcard::get-absolute-delay missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_delay()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  gdouble retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_delay == NULL){
      g_message("AgsSoundcard::get-delay missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_delay(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_attack()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_attack == NULL){
      g_message("AgsSoundcard::get-attack missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_attack(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_delay_counter()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_delay_counter == NULL){
      g_message("AgsSoundcard::get-delay-counter missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_note_offset == NULL){
      g_message("AgsSoundcard::set-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_note_offset(AGS_SOUNDCARD(current),
				  AGS_SOUNDCARD_TEST_SET_NOTE_OFFSET_NOTE_OFFSET);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_note_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_note_offset == NULL){
      g_message("AgsSoundcard::get-note-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_note_offset(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_note_offset_absolute()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;
  
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_note_offset_absolute == NULL){
      g_message("AgsSoundcard::set-note-offset-absolute missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(current),
					   AGS_SOUNDCARD_TEST_SET_NOTE_OFFSET_ABSOLUTE_NOTE_OFFSET);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_note_offset_absolute()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_note_offset_absolute == NULL){
      g_message("AgsSoundcard::get-note-offset-absolute missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(current));
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_set_loop()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint loop_left, loop_right;
  gboolean do_loop;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->set_loop == NULL){
      g_message("AgsSoundcard::set-loop missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_set_loop(AGS_SOUNDCARD(current),
			   AGS_SOUNDCARD_TEST_SET_LOOP_LOOP_LEFT, AGS_SOUNDCARD_TEST_SET_LOOP_LOOP_RIGHT,
			   AGS_SOUNDCARD_TEST_SET_DO_LOOP);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_loop()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint loop_left, loop_right;
  gboolean do_loop;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_loop == NULL){
      g_message("AgsSoundcard::get-loop missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    ags_soundcard_get_loop(AGS_SOUNDCARD(current),
			   &loop_left, &loop_right,
			   &do_loop);
  }

  CU_ASSERT(success);
}

void
ags_soundcard_test_get_loop_offset()
{
  AgsApplicationContext *application_context;
  
  GObject *current;
  
  GType current_type;

  guint retval;
  guint i;
  gboolean success;

  application_context = ags_audio_application_context_new();

  success = TRUE;
  
  for(i = 0; soundcard_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(soundcard_test_types[i],
			   NULL);
    
    if(AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(current))->get_loop_offset == NULL){
      g_message("AgsSoundcard::get-loop-offset missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }

    retval = ags_soundcard_get_loop_offset(AGS_SOUNDCARD(current));
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
  pSuite = CU_add_suite("AgsSoundcardTest", ags_soundcard_test_init_suite, ags_soundcard_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSoundcard set application context", ags_soundcard_test_set_application_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get application context", ags_soundcard_test_get_application_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set device", ags_soundcard_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get device", ags_soundcard_test_get_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard pcm info", ags_soundcard_test_pcm_info) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get capability", ags_soundcard_test_get_capability) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set presets", ags_soundcard_test_set_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get presets", ags_soundcard_test_get_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard list cards", ags_soundcard_test_list_cards) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get poll fd", ags_soundcard_test_get_poll_fd) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is available", ags_soundcard_test_is_available) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is starting", ags_soundcard_test_is_starting) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is playing", ags_soundcard_test_is_playing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard is recording", ags_soundcard_test_is_recording) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get uptime", ags_soundcard_test_get_uptime) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard play init", ags_soundcard_test_play_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard play", ags_soundcard_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard record init", ags_soundcard_test_record_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard record", ags_soundcard_test_record) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard stop", ags_soundcard_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard tic", ags_soundcard_test_tic) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard offset changed", ags_soundcard_test_offset_changed) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get buffer", ags_soundcard_test_get_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get next buffer", ags_soundcard_test_get_next_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get prev buffer", ags_soundcard_test_get_prev_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard lock buffer", ags_soundcard_test_lock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard unlock buffer", ags_soundcard_test_unlock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set bpm", ags_soundcard_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get bpm", ags_soundcard_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set delay factor", ags_soundcard_test_set_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay factor", ags_soundcard_test_get_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get absolute delay", ags_soundcard_test_get_absolute_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay", ags_soundcard_test_get_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get attack", ags_soundcard_test_get_attack) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get delay counter", ags_soundcard_test_get_delay_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set note offset", ags_soundcard_test_set_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get note offset", ags_soundcard_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set note offset absolute", ags_soundcard_test_set_note_offset_absolute) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get note offset absolute", ags_soundcard_test_get_note_offset_absolute) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard set loop", ags_soundcard_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get loop", ags_soundcard_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSoundcard get loop offset", ags_soundcard_test_get_loop_offset) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
