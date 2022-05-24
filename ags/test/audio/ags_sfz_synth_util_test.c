/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2020 Joël Krähemann
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

int ags_sfz_synth_util_test_init_suite();
int ags_sfz_synth_util_test_clean_suite();

guint ags_sfz_synth_util_test_stub_read(AgsSoundResource *sound_resource,
					void *dbuffer, guint daudio_channels,
					guint audio_channel,
					guint frame_count, guint format);

#define AGS_SFZ_SYNTH_UTIL_TEST_BUFFER_SIZE (1024)
#define AGS_SFZ_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_SFZ_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SFZ_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_SFZ_SYNTH_UTIL_TEST_FRAME_COUNT (1024)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_MODE (AGS_SFZ_SYNTH_UTIL_LOOP_NONE)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_START (0)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOOP_END (0)

#define AGS_SFZ_SYNTH_UTIL_TEST_NOTE_COUNT (128)
#define AGS_SFZ_SYNTH_UTIL_TEST_LOWER (-70.0)
#define AGS_SFZ_SYNTH_UTIL_TEST_UPPER (57.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sfz_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_sfz_synth_util_test_clean_suite()
{
  return(0);
}

guint
ags_sfz_synth_util_test_stub_read(AgsSoundResource *sound_resource,
				  void *dbuffer, guint daudio_channels,
				  guint audio_channel,
				  guint frame_count, guint format)
{
  AgsSFZSample *sfz_sample;

  guint copy_mode;
  guint read_count;
  
  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sfz_sample->format));

  read_count = sfz_sample->buffer_size;

  ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, 0,
					      sfz_sample->full_buffer, sfz_sample->info->channels, audio_channel,
					      read_count, copy_mode);
  
  return(read_count);
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
  pSuite = CU_add_suite("AgsSFZSynthUtilTest", ags_sfz_synth_util_test_init_suite, ags_sfz_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if(0){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
