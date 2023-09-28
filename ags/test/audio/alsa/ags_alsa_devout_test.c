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

#include <string.h>

int ags_alsa_devout_test_init_suite();
int ags_alsa_devout_test_clean_suite();

void ags_alsa_devout_test_set_device();
void ags_alsa_devout_test_get_device();
void ags_alsa_devout_test_set_presets();
void ags_alsa_devout_test_get_presets();
void ags_alsa_devout_test_list_cards();
void ags_alsa_devout_test_pcm_info();
void ags_alsa_devout_test_get_capability();
void ags_alsa_devout_test_is_available();
void ags_alsa_devout_test_is_starting();
void ags_alsa_devout_test_is_playing();
void ags_alsa_devout_test_is_recording();
void ags_alsa_devout_test_get_uptime();
void ags_alsa_devout_test_play_init();
void ags_alsa_devout_test_play();
void ags_alsa_devout_test_record_init();
void ags_alsa_devout_test_record();
void ags_alsa_devout_test_stop();
void ags_alsa_devout_test_tic();
void ags_alsa_devout_test_offset_changed();
void ags_alsa_devout_test_get_buffer();
void ags_alsa_devout_test_get_next_buffer();
void ags_alsa_devout_test_get_prev_buffer();
void ags_alsa_devout_test_lock_buffer();
void ags_alsa_devout_test_unlock_buffer();
void ags_alsa_devout_test_set_bpm();
void ags_alsa_devout_test_get_bpm();
void ags_alsa_devout_test_set_delay_factor();
void ags_alsa_devout_test_get_delay_factor();
void ags_alsa_devout_test_get_absolute_delay();
void ags_alsa_devout_test_get_delay();
void ags_alsa_devout_test_get_attack();
void ags_alsa_devout_test_get_delay_counter();
void ags_alsa_devout_test_get_start_note_offset();
void ags_alsa_devout_test_set_start_note_offset();
void ags_alsa_devout_test_get_note_offset();
void ags_alsa_devout_test_set_note_offset();
void ags_alsa_devout_test_get_note_offset_absolute();
void ags_alsa_devout_test_set_note_offset_absolute();
void ags_alsa_devout_test_set_loop();
void ags_alsa_devout_test_get_loop();
void ags_alsa_devout_test_get_loop_offset();
void ags_alsa_devout_test_get_sub_block_count();
void ags_alsa_devout_test_trylock_sub_block();
void ags_alsa_devout_test_unlock_sub_block();
void ags_alsa_devout_test_get_note_256th_offset();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devout_test_init_suite()
{
  ags_audio_application_context_new();
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_alsa_devout_test_clean_suite()
{
  return(0);
}

void
ags_alsa_devout_test_set_device()
{
  AgsAlsaDevout *alsa_devout;

  const static gchar *device = "default";
  
  alsa_devout = ags_alsa_devout_new();

  ags_soundcard_set_device(AGS_SOUNDCARD(alsa_devout),
			   device);

  //  CU_ASSERT(!g_ascii_strncasecmp(alsa_devout->device,
  //				 device,
  //				 strlen(device)));
}

void
ags_alsa_devout_test_get_device()
{
  AgsAlsaDevout *alsa_devout;

  gchar *device;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->device = g_strdup("hw:1,0");
  
  device = ags_soundcard_get_device(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(!g_ascii_strncasecmp(alsa_devout->device,
				 device,
				 strlen(device)) == TRUE);

  g_free(device);
}

void
ags_alsa_devout_test_set_presets()
{
  AgsAlsaDevout *alsa_devout;

  const static guint channels = 2;
  const static guint samplerate = 192000;
  const static guint buffer_size = 2048;
  const static AgsSoundcardFormat format = AGS_SOUNDCARD_SIGNED_32_BIT;
  
  alsa_devout = ags_alsa_devout_new();

  ags_soundcard_set_presets(AGS_SOUNDCARD(alsa_devout),
			    channels,
			    samplerate,
			    buffer_size,
			    format);

  CU_ASSERT(channels == alsa_devout->pcm_channels);
  CU_ASSERT(samplerate == alsa_devout->samplerate);
  CU_ASSERT(buffer_size == alsa_devout->buffer_size);
  CU_ASSERT(format == alsa_devout->format);
}

void
ags_alsa_devout_test_get_presets()
{
  AgsAlsaDevout *alsa_devout;

  guint channels;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->pcm_channels = 2;
  alsa_devout->samplerate = 192000;
  alsa_devout->buffer_size = 2048;
  alsa_devout->format = AGS_SOUNDCARD_SIGNED_32_BIT;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(alsa_devout),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  CU_ASSERT(channels == alsa_devout->pcm_channels);
  CU_ASSERT(samplerate == alsa_devout->samplerate);
  CU_ASSERT(buffer_size == alsa_devout->buffer_size);
  CU_ASSERT(format == alsa_devout->format);
}

void
ags_alsa_devout_test_list_cards()
{
  AgsAlsaDevout *alsa_devout;

  GList *card_id, *card_name;

  alsa_devout = ags_alsa_devout_new();

  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(alsa_devout),
			   &card_id, &card_name);

  //TODO:JK: implement me

  g_list_free_full(card_id,
		   g_free);
  g_list_free_full(card_name,
		   g_free);
}

void
ags_alsa_devout_test_pcm_info()
{
  AgsAlsaDevout *alsa_devout;

  GList *card_id, *card_name;
  guint channels_min, channels_max;
  guint rate_min, rate_max;
  guint buffer_size_min, buffer_size_max;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(alsa_devout),
			   &card_id, &card_name);

  CU_ASSERT(card_id != NULL);

  error = NULL;
  ags_soundcard_pcm_info(AGS_SOUNDCARD(alsa_devout),
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
ags_alsa_devout_test_get_capability()
{
  AgsAlsaDevout *alsa_devout;

  guint capability;
  
  alsa_devout = ags_alsa_devout_new();

  capability = ags_soundcard_get_capability(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(capability == AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

void
ags_alsa_devout_test_is_available()
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_available;
  
  alsa_devout = ags_alsa_devout_new();

  is_available = ags_soundcard_is_available(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(is_available == FALSE);
}

void
ags_alsa_devout_test_is_starting()
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_starting;
  
  alsa_devout = ags_alsa_devout_new();

  is_starting = ags_soundcard_is_starting(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(is_starting == FALSE);
}

void
ags_alsa_devout_test_is_playing()
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_playing;
  
  alsa_devout = ags_alsa_devout_new();

  is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(is_playing == FALSE);
}

void
ags_alsa_devout_test_is_recording()
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_recording;
  
  alsa_devout = ags_alsa_devout_new();

  is_recording = ags_soundcard_is_recording(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(is_recording == FALSE);
}

void
ags_alsa_devout_test_get_uptime()
{
  AgsAlsaDevout *alsa_devout;

  gchar *uptime;
  
  alsa_devout = ags_alsa_devout_new();

  uptime = ags_soundcard_get_uptime(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(uptime != NULL);
}

void
ags_alsa_devout_test_play_init()
{
  AgsAlsaDevout *alsa_devout;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devout),
			  &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devout));
}

void
ags_alsa_devout_test_play()
{
  AgsAlsaDevout *alsa_devout;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devout),
			  &error);

  error = NULL;
  ags_soundcard_play(AGS_SOUNDCARD(alsa_devout),
		     &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devout));
}

void
ags_alsa_devout_test_record_init()
{
  AgsAlsaDevout *alsa_devout;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  error = NULL;
  ags_soundcard_record_init(AGS_SOUNDCARD(alsa_devout),
			  &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devout));
}

void
ags_alsa_devout_test_record()
{
  AgsAlsaDevout *alsa_devout;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  error = NULL;
  ags_soundcard_record_init(AGS_SOUNDCARD(alsa_devout),
			  &error);

  error = NULL;
  ags_soundcard_record(AGS_SOUNDCARD(alsa_devout),
		     &error);

  //TODO:JK: implement me

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devout));
}

void
ags_alsa_devout_test_stop()
{
  AgsAlsaDevout *alsa_devout;

  GError *error;
  
  alsa_devout = ags_alsa_devout_new();

  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(alsa_devout),
			  &error);

  ags_soundcard_stop(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_tic()
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset;
  gdouble delay;
  gdouble delay_counter;
  guint i;
  
  alsa_devout = ags_alsa_devout_new();

  delay_counter = alsa_devout->delay_counter;
  delay = alsa_devout->delay[0];

  note_offset = alsa_devout->note_offset;
  
  ags_soundcard_tic(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(delay_counter < alsa_devout->delay_counter);

  for(i = 0; i < (guint) floor(delay) + 1; i++){
    ags_soundcard_tic(AGS_SOUNDCARD(alsa_devout));
  }
  
  CU_ASSERT(note_offset < alsa_devout->note_offset);
}

void
ags_alsa_devout_test_offset_changed()
{
  AgsAlsaDevout *alsa_devout;
  
  alsa_devout = ags_alsa_devout_new();

  ags_soundcard_offset_changed(AGS_SOUNDCARD(alsa_devout),
			       1);
  
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_buffer()
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = ags_alsa_devout_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(buffer != NULL);
}

void
ags_alsa_devout_test_get_next_buffer()
{
  AgsAlsaDevout *alsa_devout;

  void *next_buffer;
  
  alsa_devout = ags_alsa_devout_new();
  
  next_buffer = ags_soundcard_get_next_buffer(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(next_buffer != NULL);
}

void
ags_alsa_devout_test_get_prev_buffer()
{
  AgsAlsaDevout *alsa_devout;

  void *prev_buffer;
  
  alsa_devout = ags_alsa_devout_new();
  
  prev_buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(prev_buffer != NULL);
}

void
ags_alsa_devout_test_lock_buffer()
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = ags_alsa_devout_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(buffer != NULL);

  ags_soundcard_lock_buffer(AGS_SOUNDCARD(alsa_devout),
			    buffer);
  
  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(alsa_devout),
			      buffer);
}

void
ags_alsa_devout_test_unlock_buffer()
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = ags_alsa_devout_new();
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(buffer != NULL);

  ags_soundcard_lock_buffer(AGS_SOUNDCARD(alsa_devout),
			    buffer);
  
  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(alsa_devout),
			      buffer);
}

void
ags_alsa_devout_test_set_bpm()
{
  AgsAlsaDevout *alsa_devout;

  alsa_devout = ags_alsa_devout_new();
  
  ags_soundcard_set_bpm(AGS_SOUNDCARD(alsa_devout),
			138.0);

  CU_ASSERT(alsa_devout->bpm == 138.0);
}

void
ags_alsa_devout_test_get_bpm()
{
  AgsAlsaDevout *alsa_devout;

  gdouble bpm;
  
  alsa_devout = ags_alsa_devout_new();
  
  ags_soundcard_set_bpm(AGS_SOUNDCARD(alsa_devout),
			138.0);

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(alsa_devout->bpm == 138.0);
}

void
ags_alsa_devout_test_set_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay_factor()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_absolute_delay()
{
  AgsAlsaDevout *alsa_devout;

  gdouble absolute_delay;
  
  alsa_devout = ags_alsa_devout_new();
  
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay()
{
  AgsAlsaDevout *alsa_devout;

  gdouble delay;
  
  alsa_devout = ags_alsa_devout_new();
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_attack()
{
  AgsAlsaDevout *alsa_devout;

  guint attack;
  
  alsa_devout = ags_alsa_devout_new();
  
  attack = ags_soundcard_get_attack(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_delay_counter()
{
  AgsAlsaDevout *alsa_devout;

  guint delay_counter;
  
  alsa_devout = ags_alsa_devout_new();
  
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_start_note_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint start_note_offset;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->start_note_offset = 64;
  
  start_note_offset = ags_soundcard_get_start_note_offset(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(start_note_offset == 64);
}

void
ags_alsa_devout_test_set_start_note_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint start_note_offset;
  
  alsa_devout = ags_alsa_devout_new();

  start_note_offset = 64;

  ags_soundcard_set_start_note_offset(AGS_SOUNDCARD(alsa_devout),
				      start_note_offset);

  CU_ASSERT(alsa_devout->start_note_offset == 64);
}

void
ags_alsa_devout_test_get_note_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->note_offset = 64;
  
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(note_offset == 64);
}

void
ags_alsa_devout_test_set_note_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset;
  
  alsa_devout = ags_alsa_devout_new();

  note_offset = 64;

  ags_soundcard_set_note_offset(AGS_SOUNDCARD(alsa_devout),
				      note_offset);

  CU_ASSERT(alsa_devout->note_offset == 64);
}

void
ags_alsa_devout_test_get_note_offset_absolute()
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset_absolute;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->note_offset_absolute = 64;
  
  note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(alsa_devout));

  CU_ASSERT(note_offset_absolute == 64);
}

void
ags_alsa_devout_test_set_note_offset_absolute_absolute()
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset_absolute;
  
  alsa_devout = ags_alsa_devout_new();

  note_offset_absolute = 64;

  ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(alsa_devout),
					 note_offset_absolute);
  
  CU_ASSERT(alsa_devout->note_offset_absolute == 64);
}

void
ags_alsa_devout_test_set_loop()
{
  AgsAlsaDevout *alsa_devout;

  guint loop_left, loop_right;
  gboolean do_loop;
  
  alsa_devout = ags_alsa_devout_new();

  alsa_devout->loop_left = 64;
  alsa_devout->loop_right = 128;
  
  alsa_devout->do_loop = TRUE;
  
  ags_soundcard_get_loop(AGS_SOUNDCARD(alsa_devout),
			 &loop_left, &loop_right,
			 &do_loop);

  CU_ASSERT(loop_left == 64);
  CU_ASSERT(loop_right == 128);
  CU_ASSERT(do_loop == TRUE);
}

void
ags_alsa_devout_test_get_loop()
{
  AgsAlsaDevout *alsa_devout;

  guint loop_left, loop_right;
  gboolean do_loop;
  
  alsa_devout = ags_alsa_devout_new();

  loop_left = 64;
  loop_right = 128;
  
  do_loop = TRUE;

  ags_soundcard_set_loop(AGS_SOUNDCARD(alsa_devout),
			 loop_left, loop_right,
			 do_loop);

  CU_ASSERT(alsa_devout->loop_left == 64);
  CU_ASSERT(alsa_devout->loop_right == 128);
  CU_ASSERT(alsa_devout->do_loop == TRUE);
}

void
ags_alsa_devout_test_get_loop_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint loop_offset;
  
  alsa_devout = ags_alsa_devout_new();

  loop_offset = ags_soundcard_get_loop_offset(AGS_SOUNDCARD(alsa_devout));
  
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_sub_block_count()
{
  AgsAlsaDevout *alsa_devout;

  guint sub_block_count;
  
  alsa_devout = ags_alsa_devout_new();

  sub_block_count = ags_soundcard_get_sub_block_count(AGS_SOUNDCARD(alsa_devout));

  //TODO:JK: implement me
}

void
ags_alsa_devout_test_trylock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_unlock_sub_block()
{
  //TODO:JK: implement me
}

void
ags_alsa_devout_test_get_note_256th_offset()
{
  AgsAlsaDevout *alsa_devout;

  guint note_256th_offset_lower, note_256th_offset_upper;
  
  alsa_devout = ags_alsa_devout_new();

  ags_soundcard_get_note_256th_offset(AGS_SOUNDCARD(alsa_devout),
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
  pSuite = CU_add_suite("AgsAlsaDevoutTest", ags_alsa_devout_test_init_suite, ags_alsa_devout_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAlsaDevout set device", ags_alsa_devout_test_set_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get device", ags_alsa_devout_test_get_device) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set presets", ags_alsa_devout_test_set_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get presets", ags_alsa_devout_test_get_presets) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout list cards", ags_alsa_devout_test_list_cards) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout PCM info", ags_alsa_devout_test_pcm_info) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get capability", ags_alsa_devout_test_get_capability) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is available", ags_alsa_devout_test_is_available) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is starting", ags_alsa_devout_test_is_starting) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is playing", ags_alsa_devout_test_is_playing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout is recording", ags_alsa_devout_test_is_recording) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get uptime", ags_alsa_devout_test_get_uptime) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout play init", ags_alsa_devout_test_play_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout play", ags_alsa_devout_test_play) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout record init", ags_alsa_devout_test_record_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout record", ags_alsa_devout_test_record) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout stop", ags_alsa_devout_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout tic", ags_alsa_devout_test_tic) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout offset changed", ags_alsa_devout_test_offset_changed) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get buffer", ags_alsa_devout_test_get_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get next buffer", ags_alsa_devout_test_get_next_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get prev buffer", ags_alsa_devout_test_get_prev_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout lock buffer", ags_alsa_devout_test_lock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout unlock buffer", ags_alsa_devout_test_unlock_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set bpm", ags_alsa_devout_test_set_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get bpm", ags_alsa_devout_test_get_bpm) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set delay factor", ags_alsa_devout_test_set_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay factor", ags_alsa_devout_test_get_delay_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get absolute delay", ags_alsa_devout_test_get_absolute_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay", ags_alsa_devout_test_get_delay) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get attack", ags_alsa_devout_test_get_attack) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get delay counter", ags_alsa_devout_test_get_delay_counter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set start note offset", ags_alsa_devout_test_set_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get start note offset", ags_alsa_devout_test_get_start_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set note offset", ags_alsa_devout_test_set_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get note offset", ags_alsa_devout_test_get_note_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout set loop", ags_alsa_devout_test_set_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get loop", ags_alsa_devout_test_get_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get loop offset", ags_alsa_devout_test_get_loop_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout get sub block count", ags_alsa_devout_test_get_sub_block_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout trylock sub block", ags_alsa_devout_test_trylock_sub_block) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAlsaDevout unock sub block", ags_alsa_devout_test_unlock_sub_block) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

