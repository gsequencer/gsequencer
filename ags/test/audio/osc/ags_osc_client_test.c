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

int ags_osc_client_test_init_suite();
int ags_osc_client_test_clean_suite();

void ags_osc_client_test_resolve();
void ags_osc_client_test_connect();
void ags_osc_client_test_write_bytes();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_client_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_client_test_clean_suite()
{
  return(0);
}

void
ags_osc_client_test_resolve()
{
  AgsOscClient *osc_client;

  osc_client = ags_osc_client_new();

  g_object_set(osc_client,
	       "ip4", NULL,
	       "ip6", NULL,
	       NULL);

  ags_osc_client_resolve(osc_client);

  CU_ASSERT(osc_client->ip4 != NULL);
  CU_ASSERT(osc_client->ip6 != NULL);
}

void
ags_osc_client_test_connect()
{
  AgsOscClient *osc_client;

  osc_client = ags_osc_client_new();
  ags_osc_client_set_flags(osc_client,
			   (AGS_OSC_CLIENT_INET4 |
			    AGS_OSC_CLIENT_INET6 |
			    AGS_OSC_CLIENT_TCP));

  ags_osc_client_connect(osc_client);

  CU_ASSERT(osc_client->ip4_fd != -1);
  CU_ASSERT(osc_client->ip6_fd != -1);
}

void
ags_osc_client_test_write_bytes()
{
  AgsOscClient *osc_client;

  gboolean retval;
  
  static const unsigned char *status_message = "/status\x00,\x00\x00\x00";

  static const guint status_message_size = 12;

  osc_client = ags_osc_client_new();
  ags_osc_client_set_flags(osc_client,
			   AGS_OSC_CLIENT_INET4);

  osc_client->ip4_fd = open("/dev/null", O_RDWR, 0);

  CU_ASSERT(osc_client->ip4_fd != -1);

  retval = ags_osc_client_write_bytes(osc_client,
				      status_message, status_message_size);

  CU_ASSERT(retval == TRUE);
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
  pSuite = CU_add_suite("AgsOscClientTest", ags_osc_client_test_init_suite, ags_osc_client_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscClient resolve", ags_osc_client_test_resolve) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscClient connect", ags_osc_client_test_connect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscClient write bytes", ags_osc_client_test_write_bytes) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


