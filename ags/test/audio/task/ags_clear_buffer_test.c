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

int ags_clear_buffer_test_init_suite();
int ags_clear_buffer_test_clean_suite();

void ags_clear_buffer_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_clear_buffer_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_clear_buffer_test_clean_suite()
{  
  return(0);
}

void
ags_clear_buffer_test_launch()
{
  AgsAlsaDevout *devout;

  AgsClearBuffer *clear_buffer;

  gint16 *buffer;
  
  guint i;
  gboolean success;
  
  /* create soundcard */
  devout = g_object_new(AGS_TYPE_ALSA_DEVOUT,
			NULL);
  g_object_set(devout,
	       "format", AGS_SOUNDCARD_SIGNED_16_BIT,
	       NULL);
  g_object_ref(devout);

  /* fill random data - attempt #0 */
  devout->backend_buffer_mode = AGS_ALSA_DEVOUT_BACKEND_BUFFER_0;
  devout->app_buffer_mode = AGS_ALSA_DEVOUT_APP_BUFFER_0;

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));
  
  for(i = 0; i < devout->buffer_size; i++){
    buffer[i] = rand() % G_MAXINT16;
  }

  /* create clear buffer */
  clear_buffer = ags_clear_buffer_new(devout);

  CU_ASSERT(AGS_IS_CLEAR_BUFFER(clear_buffer));
  CU_ASSERT(clear_buffer->device == devout);

  /* launch */
  ags_task_launch(clear_buffer);

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));

  success = TRUE;
  
  for(i = 0; i < devout->buffer_size; i++){
    if(buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);

  /* fill random data - attempt #1 */
  devout->backend_buffer_mode = AGS_ALSA_DEVOUT_BACKEND_BUFFER_1;
  devout->app_buffer_mode = AGS_ALSA_DEVOUT_APP_BUFFER_1;
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));
  
  for(i = 0; i < devout->buffer_size; i++){
    buffer[i] = rand() % G_MAXINT16;
  }

  /* create clear buffer */
  clear_buffer = ags_clear_buffer_new(devout);

  CU_ASSERT(AGS_IS_CLEAR_BUFFER(clear_buffer));
  CU_ASSERT(clear_buffer->device == devout);

  /* launch */
  ags_task_launch(clear_buffer);

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));

  success = TRUE;
  
  for(i = 0; i < devout->buffer_size; i++){
    if(buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);

  /* fill random data - attempt #2 */
  devout->backend_buffer_mode = AGS_ALSA_DEVOUT_BACKEND_BUFFER_2;
  devout->app_buffer_mode = AGS_ALSA_DEVOUT_APP_BUFFER_2;
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));
  
  for(i = 0; i < devout->buffer_size; i++){
    buffer[i] = rand() % G_MAXINT16;
  }

  /* create clear buffer */
  clear_buffer = ags_clear_buffer_new(devout);

  CU_ASSERT(AGS_IS_CLEAR_BUFFER(clear_buffer));
  CU_ASSERT(clear_buffer->device == devout);

  /* launch */
  ags_task_launch(clear_buffer);

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));

  success = TRUE;
  
  for(i = 0; i < devout->buffer_size; i++){
    if(buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);

  /* fill random data - attempt #3 */
  devout->backend_buffer_mode = AGS_ALSA_DEVOUT_BACKEND_BUFFER_3;
  devout->app_buffer_mode = AGS_ALSA_DEVOUT_APP_BUFFER_3;
  
  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));
  
  for(i = 0; i < devout->buffer_size; i++){
    buffer[i] = rand() % G_MAXINT16;
  }

  /* create clear buffer */
  clear_buffer = ags_clear_buffer_new(devout);

  CU_ASSERT(AGS_IS_CLEAR_BUFFER(clear_buffer));
  CU_ASSERT(clear_buffer->device == devout);

  /* launch */
  ags_task_launch(clear_buffer);

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));

  success = TRUE;
  
  for(i = 0; i < devout->buffer_size; i++){
    if(buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
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
  pSuite = CU_add_suite("AgsClearBufferTest", ags_clear_buffer_test_init_suite, ags_clear_buffer_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsClearBuffer launch", ags_clear_buffer_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
