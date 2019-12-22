/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2019 Joël Krähemann
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

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int ags_osc_message_test_init_suite();
int ags_osc_message_test_clean_suite();

void ags_osc_message_test_set_property();
void ags_osc_message_test_get_property();

#define AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_SEC (32)
#define AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_FRACTION (25000)

#define AGS_OSC_MESSAGE_TEST_GET_PROPERTY_TV_SEC (32)
#define AGS_OSC_MESSAGE_TEST_GET_PROPERTY_TV_FRACTION (25000)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_message_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_message_test_clean_suite()
{
  return(0);
}

void
ags_osc_message_test_set_property()
{
  AgsOscConnection *osc_connection;
  AgsOscMessage *osc_message;
  
  guint i;
  gboolean success;
  
  static const guchar *info_message = "/info\x00\x00\x00";

  static const guint info_message_size = 8;

  osc_message = ags_osc_message_new();

  osc_connection = ags_osc_connection_new(NULL);
  
  g_object_set(osc_message,
	       "osc-connection", osc_connection,
	       "tv-sec", AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_SEC,
    	       "tv-fraction", AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_FRACTION,
	       "immediately", FALSE,
	       "message-size", info_message_size,
	       "message", info_message,
	       NULL);

  CU_ASSERT(osc_message->osc_connection == osc_connection);
  CU_ASSERT(osc_message->tv_sec == AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_SEC);
  CU_ASSERT(osc_message->tv_fraction == AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_FRACTION);
  CU_ASSERT(osc_message->immediately == FALSE);
  CU_ASSERT(osc_message->message_size == info_message_size);

  success = TRUE;

  for(i = 0; i < info_message_size; i++){
    if(osc_message->message[i] != info_message[i]){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);

  g_object_set(osc_message,
	       "immediately", TRUE,
	       NULL);

  CU_ASSERT(osc_message->immediately == TRUE);
}

void
ags_osc_message_test_get_property()
{
  AgsOscConnection *osc_connection;
  AgsOscMessage *osc_message;

  gpointer message;
  
  guint message_size;
  gint32 tv_sec;
  gint32 tv_fraction;
  gboolean immediately;
  
  static const guchar *info_message = "/info\x00\x00\x00";

  static const guint info_message_size = 8;

  osc_message = ags_osc_message_new();

  osc_message->osc_connection = ags_osc_connection_new(NULL);
  osc_message->tv_sec = AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_SEC;
  osc_message->tv_fraction = AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_FRACTION;
  osc_message->immediately = FALSE;
  osc_message->message_size = info_message_size;
  osc_message->message = info_message;

  g_object_get(osc_message,
	       "osc-connection", &osc_connection,
	       "tv-sec", &tv_sec,
	       "tv-fraction", &tv_fraction,
	       "immediately", &immediately,
	       "message-size", &message_size,
	       "message", &message,
	       NULL);

  CU_ASSERT(AGS_IS_OSC_CONNECTION(osc_connection) == TRUE);
  CU_ASSERT(osc_message->osc_connection == osc_connection);

  CU_ASSERT(tv_sec == AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_SEC);
  CU_ASSERT(tv_fraction == AGS_OSC_MESSAGE_TEST_SET_PROPERTY_TV_FRACTION);
  
  CU_ASSERT(immediately == FALSE);

  CU_ASSERT(message_size == info_message_size);

  CU_ASSERT(message == info_message);

  osc_message->immediately = TRUE;
  
  g_object_get(osc_message,
	       "immediately", &immediately,
	       NULL);
  
  CU_ASSERT(immediately == TRUE);
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
  pSuite = CU_add_suite("AgsOscMessageTest", ags_osc_message_test_init_suite, ags_osc_message_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscMessage set property", ags_osc_message_test_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMessage get property", ags_osc_message_test_get_property) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


