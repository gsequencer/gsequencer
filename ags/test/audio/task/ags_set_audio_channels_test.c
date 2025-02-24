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

int ags_set_audio_channels_test_init_suite();
int ags_set_audio_channels_test_clean_suite();

void ags_set_audio_channels_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_audio_channels_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_audio_channels_test_clean_suite()
{  
  return(0);
}

void
ags_set_audio_channels_test_launch()
{
  AgsAlsaDevout *devout;

  AgsSetAudioChannels *set_audio_channels;

  guint audio_channels;
  
  devout = ags_alsa_devout_new();

  set_audio_channels = ags_set_audio_channels_new(devout,
						  6);
  
  CU_ASSERT(AGS_IS_SET_AUDIO_CHANNELS(set_audio_channels));
  CU_ASSERT(set_audio_channels->soundcard == devout);
  CU_ASSERT(set_audio_channels->audio_channels == 6);

  /* launch */
  ags_task_launch(set_audio_channels);

  ags_soundcard_get_presets(AGS_SOUNDCARD(devout),
			    &audio_channels,
			    NULL,
			    NULL,
			    NULL);
  
  CU_ASSERT(audio_channels == 6);
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
  pSuite = CU_add_suite("AgsSetAudioChannelsTest", ags_set_audio_channels_test_init_suite, ags_set_audio_channels_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSetAudioChannels launch", ags_set_audio_channels_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
