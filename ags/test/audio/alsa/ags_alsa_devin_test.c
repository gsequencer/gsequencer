/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2023 Joël Krähemann
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

int ags_alsa_devin_test_init_suite();
int ags_alsa_devin_test_clean_suite();

void ags_alsa_devin_test_set_device();
void ags_alsa_devin_test_get_device();
void ags_alsa_devin_test_set_presets();
void ags_alsa_devin_test_get_presets();
void ags_alsa_devin_test_list_cards();
void ags_alsa_devin_test_pcm_info();
void ags_alsa_devin_test_get_capability();
void ags_alsa_devin_test_is_available();
void ags_alsa_devin_test_is_starting();
void ags_alsa_devin_test_is_playing();
void ags_alsa_devin_test_is_recording();
void ags_alsa_devin_test_get_uptime();
void ags_alsa_devin_test_play_init();
void ags_alsa_devin_test_play();
void ags_alsa_devin_test_record_init();
void ags_alsa_devin_test_record();
void ags_alsa_devin_test_stop();
void ags_alsa_devin_test_tic();
void ags_alsa_devin_test_offset_changed();
void ags_alsa_devin_test_get_buffer();
void ags_alsa_devin_test_get_next_buffer();
void ags_alsa_devin_test_get_prev_buffer();
void ags_alsa_devin_test_lock_buffer();
void ags_alsa_devin_test_unlock_buffer();
void ags_alsa_devin_test_set_bpm();
void ags_alsa_devin_test_get_bpm();
void ags_alsa_devin_test_set_delay_factor();
void ags_alsa_devin_test_get_delay_factor();
void ags_alsa_devin_test_get_absolute_delay();
void ags_alsa_devin_test_get_delay();
void ags_alsa_devin_test_get_attack();
void ags_alsa_devin_test_get_delay_counter();
void ags_alsa_devin_test_get_start_note_offset();
void ags_alsa_devin_test_set_start_note_offset();
void ags_alsa_devin_test_get_note_offset();
void ags_alsa_devin_test_set_note_offset();
void ags_alsa_devin_test_get_note_offset_absolute();
void ags_alsa_devin_test_set_note_offset_absolute();
void ags_alsa_devin_test_set_loop();
void ags_alsa_devin_test_get_loop();
void ags_alsa_devin_test_get_loop_offset();
void ags_alsa_devin_test_get_sub_block_count();
void ags_alsa_devin_test_trylock_sub_block();
void ags_alsa_devin_test_unlock_sub_block();
void ags_alsa_devin_test_get_note_256th_offset();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devin_test_init_suite()
{
  ags_audio_application_context_new();

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devin_test_clean_suite()
{
  return(0);
}

void
ags_alsa_devin_test_set_device()
{
  AgsAlsaDevin *alsa_devin;

  const static gchar *device = "default";
  
  alsa_devin = ags_alsa_devin_new();

  ags_soundcard_set_device(AGS_SOUNDCARD(alsa_devin),
			   device);

  //  CU_ASSERT(!g_ascii_strncasecmp(alsa_devin->device,
  //				 device,
  //				 strlen(device)));
}

void
ags_alsa_devin_test_get_device()
{
  AgsAlsaDevin *alsa_devin;

  gchar *device;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->device = g_strdup("hw:1,0");
  
  device = ags_soundcard_get_device(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(!g_ascii_strncasecmp(alsa_devin->device,
				 device,
				 strlen(device)) == TRUE);

  g_free(device);
}

void
ags_alsa_devin_test_set_presets()
{
  AgsAlsaDevin *alsa_devin;

  const static guint channels = 2;
  const static guint samplerate = 192000;
  const static guint buffer_size = 2048;
  const static AgsSoundcardFormat format = AGS_SOUNDCARD_SIGNED_32_BIT;
  
  alsa_devin = ags_alsa_devin_new();

  ags_soundcard_set_presets(AGS_SOUNDCARD(alsa_devin),
			    channels,
			    samplerate,
			    buffer_size,
			    format);

  CU_ASSERT(channels == alsa_devin->pcm_channels);
  CU_ASSERT(samplerate == alsa_devin->samplerate);
  CU_ASSERT(buffer_size == alsa_devin->buffer_size);
  CU_ASSERT(format == alsa_devin->format);
}

void
ags_alsa_devin_test_get_presets()
{
  AgsAlsaDevin *alsa_devin;

  guint channels;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->pcm_channels = 2;
  alsa_devin->samplerate = 192000;
  alsa_devin->buffer_size = 2048;
  alsa_devin->format = AGS_SOUNDCARD_SIGNED_32_BIT;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(alsa_devin),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  CU_ASSERT(channels == alsa_devin->pcm_channels);
  CU_ASSERT(samplerate == alsa_devin->samplerate);
  CU_ASSERT(buffer_size == alsa_devin->buffer_size);
  CU_ASSERT(format == alsa_devin->format);
}

void
ags_alsa_devin_test_list_cards()
{
  AgsAlsaDevin *alsa_devin;

  GList *card_id, *card_name;

  alsa_devin = ags_alsa_devin_new();

  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(alsa_devin),
			   &card_id, &card_name);

  //TODO:JK: implement me

  g_list_free_full(card_id,
		   g_free);
  g_list_free_full(card_name,
		   g_free);
}

void
ags_alsa_devin_test_pcm_info()
{
  AgsAlsaDevin *alsa_devin;

  GList *card_id, *card_name;
  guint channels_min, channels_max;
  guint rate_min, rate_max;
  guint buffer_size_min, buffer_size_max;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(alsa_devin),
			   &card_id, &card_name);

  CU_ASSERT(card_id != NULL);

  error = NULL;
  ags_soundcard_pcm_info(AGS_SOUNDCARD(alsa_devin),
			 card_id->data,
			 &channels_min, &channels_max,
			 &rate_min, &rate_max,
			 &buffer_size_min, &buffer_size_max,
			 &error);
  
  //TODO:JK: implement me

  g_list_free_full(card_id,
		   g_free);
  g_list_free_full(card_name,
		   g_free);
}

void
ags_alsa_devin_test_get_capability()
{
  AgsAlsaDevin *alsa_devin;

  guint capability;
  
  alsa_devin = ags_alsa_devin_new();

  capability = ags_soundcard_get_capability(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(capability == AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

void
ags_alsa_devin_test_is_available()
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_available;
  
  alsa_devin = ags_alsa_devin_new();

  is_available = ags_soundcard_is_available(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(is_available == FALSE);
}

void
ags_alsa_devin_test_is_starting()
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_starting;
  
  alsa_devin = ags_alsa_devin_new();

  is_starting = ags_soundcard_is_starting(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(is_starting == FALSE);
}

void
ags_alsa_devin_test_is_playing()
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_playing;
  
  alsa_devin = ags_alsa_devin_new();

  is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(is_playing == FALSE);
}

void
ags_alsa_devin_test_is_recording()
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_recording;
  
  alsa_devin = ags_alsa_devin_new();

  is_recording = ags_soundcard_is_recording(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(is_recording == FALSE);
}

void
ags_alsa_devin_test_get_uptime()
{
  AgsAlsaDevin *alsa_devin;

  gchar *uptime;
  
  alsa_devin = ags_alsa_devin_new();

  uptime = ags_soundcard_get_uptime(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(uptime != NULL);
}

void
ags_alsa_devin_test_play_init()
{
  AgsAlsaDevin *alsa_devin;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devin),
			  &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devin));
}

void
ags_alsa_devin_test_play()
{
  AgsAlsaDevin *alsa_devin;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devin),
			  &error);

  error = NULL;
  ags_soundcard_play(AGS_SOUNDCARD(alsa_devin),
		     &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devin));
}

void
ags_alsa_devin_test_record_init()
{
  AgsAlsaDevin *alsa_devin;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  error = NULL;
  ags_soundcard_record_init(AGS_SOUNDCARD(alsa_devin),
			    &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devin));
}

void
ags_alsa_devin_test_record()
{
  AgsAlsaDevin *alsa_devin;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  error = NULL;
  ags_soundcard_record_init(AGS_SOUNDCARD(alsa_devin),
			    &error);

  error = NULL;
  ags_soundcard_record(AGS_SOUNDCARD(alsa_devin),
		       &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devin));
}

void
ags_alsa_devin_test_stop()
{
  AgsAlsaDevin *alsa_devin;

  GError *error;
  
  alsa_devin = ags_alsa_devin_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devin),
			  &error);

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_tic()
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset;
  gdouble delay;
  gdouble delay_counter;
  guint i;
  
  alsa_devin = ags_alsa_devin_new();

  delay_counter = alsa_devin->delay_counter;
  delay = alsa_devin->delay[0];

  note_offset = alsa_devin->note_offset;
  
  ags_soundcard_tic(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(delay_counter < alsa_devin->delay_counter);

  for(i = 0; i < (guint) floor(delay) + 1; i++){
    ags_soundcard_tic(AGS_SOUNDCARD(alsa_devin));
  }
  
  CU_ASSERT(note_offset < alsa_devin->note_offset);
}

void
ags_alsa_devin_test_offset_changed()
{
  AgsAlsaDevin *alsa_devin;
  
  alsa_devin = ags_alsa_devin_new();

  ags_soundcard_offset_changed(AGS_SOUNDCARD(alsa_devin),
			       1);
  
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_buffer()
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = ags_alsa_devin_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(buffer != NULL);
}

void
ags_alsa_devin_test_get_next_buffer()
{
  AgsAlsaDevin *alsa_devin;

  void *next_buffer;
  
  alsa_devin = ags_alsa_devin_new();
  
  next_buffer = ags_soundcard_get_next_buffer(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(next_buffer != NULL);
}

void
ags_alsa_devin_test_get_prev_buffer()
{
  AgsAlsaDevin *alsa_devin;

  void *prev_buffer;
  
  alsa_devin = ags_alsa_devin_new();
  
  prev_buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(prev_buffer != NULL);
}

void
ags_alsa_devin_test_set_bpm()
{
  AgsAlsaDevin *alsa_devin;

  alsa_devin = ags_alsa_devin_new();
  
  ags_soundcard_set_bpm(AGS_SOUNDCARD(alsa_devin),
			138.0);

  CU_ASSERT(alsa_devin->bpm == 138.0);
}

void
ags_alsa_devin_test_lock_buffer()
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = ags_alsa_devin_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(buffer != NULL);

  ags_soundcard_lock_buffer(AGS_SOUNDCARD(alsa_devin),
			    buffer);
  
  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(alsa_devin),
			      buffer);
}

void
ags_alsa_devin_test_unlock_buffer()
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = ags_alsa_devin_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(buffer != NULL);

  ags_soundcard_lock_buffer(AGS_SOUNDCARD(alsa_devin),
			    buffer);
  
  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(alsa_devin),
			      buffer);
}

void
ags_alsa_devin_test_get_bpm()
{
  AgsAlsaDevin *alsa_devin;

  gdouble bpm;
  
  alsa_devin = ags_alsa_devin_new();
  
  ags_soundcard_set_bpm(AGS_SOUNDCARD(alsa_devin),
			138.0);

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(alsa_devin->bpm == 138.0);
}

void
ags_alsa_devin_test_set_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_absolute_delay()
{
  AgsAlsaDevin *alsa_devin;

  gdouble absolute_delay;
  
  alsa_devin = ags_alsa_devin_new();
  
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_delay()
{
  AgsAlsaDevin *alsa_devin;

  gdouble delay;
  
  alsa_devin = ags_alsa_devin_new();
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_attack()
{
  AgsAlsaDevin *alsa_devin;

  guint attack;
  
  alsa_devin = ags_alsa_devin_new();
  
  attack = ags_soundcard_get_attack(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_delay_counter()
{
  AgsAlsaDevin *alsa_devin;

  guint delay_counter;
  
  alsa_devin = ags_alsa_devin_new();
  
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_start_note_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint start_note_offset;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->start_note_offset = 64;
  
  start_note_offset = ags_soundcard_get_start_note_offset(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(start_note_offset == 64);
}

void
ags_alsa_devin_test_set_start_note_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint start_note_offset;
  
  alsa_devin = ags_alsa_devin_new();

  start_note_offset = 64;

  ags_soundcard_set_start_note_offset(AGS_SOUNDCARD(alsa_devin),
				      start_note_offset);

  CU_ASSERT(alsa_devin->start_note_offset == 64);
}

void
ags_alsa_devin_test_get_note_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->note_offset = 64;
  
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(note_offset == 64);
}

void
ags_alsa_devin_test_set_note_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset;
  
  alsa_devin = ags_alsa_devin_new();

  note_offset = 64;

  ags_soundcard_set_note_offset(AGS_SOUNDCARD(alsa_devin),
				note_offset);

  CU_ASSERT(alsa_devin->note_offset == 64);
}

void
ags_alsa_devin_test_get_note_offset_absolute()
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset_absolute;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->note_offset_absolute = 64;
  
  note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(alsa_devin));

  CU_ASSERT(note_offset_absolute == 64);
}

void
ags_alsa_devin_test_set_note_offset_absolute()
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset_absolute;
  
  alsa_devin = ags_alsa_devin_new();

  note_offset_absolute = 64;

  ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(alsa_devin),
					 note_offset_absolute);
  
  CU_ASSERT(alsa_devin->note_offset_absolute == 64);
}

void
ags_alsa_devin_test_set_loop()
{
  AgsAlsaDevin *alsa_devin;

  guint loop_left, loop_right;
  gboolean do_loop;
  
  alsa_devin = ags_alsa_devin_new();

  alsa_devin->loop_left = 64;
  alsa_devin->loop_right = 128;
  
  alsa_devin->do_loop = TRUE;
  
  ags_soundcard_get_loop(AGS_SOUNDCARD(alsa_devin),
			 &loop_left, &loop_right,
			 &do_loop);

  CU_ASSERT(loop_left == 64);
  CU_ASSERT(loop_right == 128);
  CU_ASSERT(do_loop == TRUE);
}

void
ags_alsa_devin_test_get_loop()
{
  AgsAlsaDevin *alsa_devin;

  guint loop_left, loop_right;
  gboolean do_loop;
  
  alsa_devin = ags_alsa_devin_new();

  loop_left = 64;
  loop_right = 128;
  
  do_loop = TRUE;

  ags_soundcard_set_loop(AGS_SOUNDCARD(alsa_devin),
			 loop_left, loop_right,
			 do_loop);

  CU_ASSERT(alsa_devin->loop_left == 64);
  CU_ASSERT(alsa_devin->loop_right == 128);
  CU_ASSERT(alsa_devin->do_loop == TRUE);
}

void
ags_alsa_devin_test_get_loop_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint loop_offset;
  
  alsa_devin = ags_alsa_devin_new();

  loop_offset = ags_soundcard_get_loop_offset(AGS_SOUNDCARD(alsa_devin));
  
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_sub_block_count()
{
  AgsAlsaDevin *alsa_devin;

  guint sub_block_count;
  
  alsa_devin = ags_alsa_devin_new();

  sub_block_count = ags_soundcard_get_sub_block_count(AGS_SOUNDCARD(alsa_devin));

  //TODO:JK: implement me
}

void
ags_alsa_devin_test_trylock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_unlock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devin_test_get_note_256th_offset()
{
  AgsAlsaDevin *alsa_devin;

  guint note_256th_offset_lower, note_256th_offset_upper;
  
  alsa_devin = ags_alsa_devin_new();

  ags_soundcard_get_note_256th_offset(AGS_SOUNDCARD(alsa_devin),
				      &note_256th_offset_lower, &note_256th_offset_upper);

  //TODO:JK: implement me
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

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsAlsaDevinTest", ags_alsa_devin_test_init_suite, ags_alsa_devin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAlsaDevin set device", ags_alsa_devin_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get device", ags_alsa_devin_test_get_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set presets", ags_alsa_devin_test_set_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get presets", ags_alsa_devin_test_get_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin list cards", ags_alsa_devin_test_list_cards) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin PCM info", ags_alsa_devin_test_pcm_info) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get capability", ags_alsa_devin_test_get_capability) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin is available", ags_alsa_devin_test_is_available) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin is starting", ags_alsa_devin_test_is_starting) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin is playing", ags_alsa_devin_test_is_playing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin is recording", ags_alsa_devin_test_is_recording) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get uptime", ags_alsa_devin_test_get_uptime) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin play init", ags_alsa_devin_test_play_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin play", ags_alsa_devin_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin record init", ags_alsa_devin_test_record_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin record", ags_alsa_devin_test_record) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin stop", ags_alsa_devin_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin tic", ags_alsa_devin_test_tic) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin offset changed", ags_alsa_devin_test_offset_changed) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get buffer", ags_alsa_devin_test_get_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get next buffer", ags_alsa_devin_test_get_next_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get prev buffer", ags_alsa_devin_test_get_prev_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin lock buffer", ags_alsa_devin_test_lock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin unlock buffer", ags_alsa_devin_test_unlock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set bpm", ags_alsa_devin_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get bpm", ags_alsa_devin_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set delay factor", ags_alsa_devin_test_set_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get delay factor", ags_alsa_devin_test_get_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get absolute delay", ags_alsa_devin_test_get_absolute_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get delay", ags_alsa_devin_test_get_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get attack", ags_alsa_devin_test_get_attack) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get delay counter", ags_alsa_devin_test_get_delay_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set start note offset", ags_alsa_devin_test_set_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get start note offset", ags_alsa_devin_test_get_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set note offset", ags_alsa_devin_test_set_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get note offset", ags_alsa_devin_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin set loop", ags_alsa_devin_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get loop", ags_alsa_devin_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get loop offset", ags_alsa_devin_test_get_loop_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin get sub block count", ags_alsa_devin_test_get_sub_block_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin trylock sub block", ags_alsa_devin_test_trylock_sub_block) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevin unock sub block", ags_alsa_devin_test_unlock_sub_block) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

