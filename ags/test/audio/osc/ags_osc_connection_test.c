/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

int ags_osc_connection_test_init_suite();
int ags_osc_connection_test_clean_suite();

void ags_osc_connection_test_read_bytes();
void ags_osc_connection_test_write_response();
void ags_osc_connection_test_close();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_connection_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_connection_test_clean_suite()
{
  return(0);
}

void
ags_osc_connection_test_read_bytes()
{
  //TODO:JK: implement me
}

void
ags_osc_connection_test_write_response()
{
  AgsOscConnection *osc_connection;
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint packet_size;
  gint64 retval;
  
  static const unsigned char server_info_message[] = "/info\0\0\0,ssss\0\0\0V2.1.0\0\0osc-server\0\0Advanced Gtk+ Sequencer\02.1.0\0\0\0";

  osc_connection = ags_osc_connection_new(NULL);

  osc_connection->fd = open("/dev/null", O_RDWR, 0);

  CU_ASSERT(osc_connection->fd != -1);

  osc_response = ags_osc_response_new();

  /* create packet */
  packet = (unsigned char *) malloc((4 * sizeof(unsigned char)) + sizeof(server_info_message));

  ags_osc_buffer_util_put_int32(packet,
				sizeof(server_info_message));
  memcpy(packet + 4, server_info_message, sizeof(server_info_message));

  /* set response packet */
  g_object_set(osc_response,
	       "packet", packet,
	       "packet-size", (4 * sizeof(unsigned char)) + sizeof(server_info_message),
	       NULL);

  retval = ags_osc_connection_write_response(osc_connection,
					     osc_response);

  CU_ASSERT(retval >= (4 * sizeof(unsigned char)) + sizeof(server_info_message));
}

void
ags_osc_connection_test_close()
{
  AgsOscConnection *osc_connection;

  osc_connection = ags_osc_connection_new(NULL);

  ags_osc_connection_close(osc_connection);

  CU_ASSERT(osc_connection->fd == -1);
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
  pSuite = CU_add_suite("AgsOscConnectionTest", ags_osc_connection_test_init_suite, ags_osc_connection_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscConnection read bytes", ags_osc_connection_test_read_bytes) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscConnection write response", ags_osc_connection_test_write_response) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscConnection close", ags_osc_connection_test_close) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


