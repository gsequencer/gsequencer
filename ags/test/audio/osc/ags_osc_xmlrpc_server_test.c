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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int ags_osc_xmlrpc_server_test_init_suite();
int ags_osc_xmlrpc_server_test_clean_suite();

void ags_osc_xmlrpc_server_test_find_xmlrpc_connection();
void ags_osc_xmlrpc_server_test_find_websocket_connection();
void ags_osc_xmlrpc_server_test_add_websocket_handler();
void ags_osc_xmlrpc_server_test_add_default_controller();
void ags_osc_xmlrpc_server_test_start();
void ags_osc_xmlrpc_server_test_stop();
void ags_osc_xmlrpc_server_test_listen();
void ags_osc_xmlrpc_server_test_dispatch();

void ags_osc_xmlrpc_server_test_add_websocket_handler_callback(SoupServer *server,
							       SoupWebsocketConnection *connection,
							       const char *path,
							       SoupClientContext *client,
							       gpointer user_data);

#define AGS_OSC_XMLRPC_SERVER_TEST_FIND_XMLRPC_CONNECTION_CONNECTION_COUNT (7)

#define AGS_OSC_XMLRPC_SERVER_TEST_FIND_WEBSOCKET_CONNECTION_CONNECTION_COUNT (9)

#define AGS_OSC_XMLRPC_SERVER_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=false\n"				       \
  "disable-feature=experimental\n"		       \
  "segmentation=4/4\n"				       \
  "\n"						       \
  "[thread]\n"					       \
  "model=super-threaded\n"			       \
  "super-threaded-scope=channel\n"		       \
  "lock-global=ags-thread\n"			       \
  "lock-parent=ags-recycling-thread\n"		       \
  "\n"						       \
  "[soundcard-0]\n"				       \
  "backend=alsa\n"                                     \
  "device=default\n"				       \
  "samplerate=44100\n"				       \
  "buffer-size=1024\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

AgsApplicationContext *application_context;

GObject *default_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_server_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_OSC_XMLRPC_SERVER_TEST_CONFIG,
			    strlen(AGS_OSC_XMLRPC_SERVER_TEST_CONFIG));

  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_server_test_clean_suite()
{
  return(0);
}

void
ags_osc_xmlrpc_server_test_find_xmlrpc_connection()
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
  AgsOscXmlrpcConnection **osc_xmlrpc_connection;
  
  SoupClientContext **client;

  guint i;
  gboolean success;
  
  osc_xmlrpc_server = ags_osc_xmlrpc_server_new();

  osc_xmlrpc_connection = (AgsOscXmlrpcConnection **) malloc(AGS_OSC_XMLRPC_SERVER_TEST_FIND_XMLRPC_CONNECTION_CONNECTION_COUNT * sizeof(AgsOscXmlrpcConnection *));
  client = (SoupClientContext **) malloc(AGS_OSC_XMLRPC_SERVER_TEST_FIND_XMLRPC_CONNECTION_CONNECTION_COUNT * sizeof(SoupClientContext *));

  for(i = 0; i < AGS_OSC_XMLRPC_SERVER_TEST_FIND_XMLRPC_CONNECTION_CONNECTION_COUNT; i++){
    osc_xmlrpc_connection[i] = ags_osc_xmlrpc_connection_new(NULL);
    
    client[i] =
      osc_xmlrpc_connection[i]->client = (SoupClientContext *) malloc(sizeof(gpointer));

    AGS_OSC_SERVER(osc_xmlrpc_server)->connection = g_list_prepend(AGS_OSC_SERVER(osc_xmlrpc_server)->connection,
								   osc_xmlrpc_connection[i]);
  }

  /* find */
  success = TRUE;
  
  for(i = 0; i < AGS_OSC_XMLRPC_SERVER_TEST_FIND_XMLRPC_CONNECTION_CONNECTION_COUNT; i++){
    GObject *current;

    current = ags_osc_xmlrpc_server_find_xmlrpc_connection(osc_xmlrpc_server,
							   client[i]);

    if(current != osc_xmlrpc_connection[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_xmlrpc_server_test_find_websocket_connection()
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
  AgsOscWebsocketConnection **osc_websocket_connection;
  
  SoupWebsocketConnection **websocket_connection;

  guint i;
  gboolean success;
  
  osc_xmlrpc_server = ags_osc_xmlrpc_server_new();

  osc_websocket_connection = (AgsOscWebsocketConnection **) malloc(AGS_OSC_XMLRPC_SERVER_TEST_FIND_WEBSOCKET_CONNECTION_CONNECTION_COUNT * sizeof(AgsOscWebsocketConnection *));
  websocket_connection = (SoupWebsocketConnection **) malloc(AGS_OSC_XMLRPC_SERVER_TEST_FIND_WEBSOCKET_CONNECTION_CONNECTION_COUNT * sizeof(SoupWebsocketConnection *));

  for(i = 0; i < AGS_OSC_XMLRPC_SERVER_TEST_FIND_WEBSOCKET_CONNECTION_CONNECTION_COUNT; i++){
    osc_websocket_connection[i] = ags_osc_websocket_connection_new(NULL);
    
    websocket_connection[i] =
      osc_websocket_connection[i]->websocket_connection = soup_websocket_connection_new(g_simple_io_stream_new(g_memory_input_stream_new(),
													       g_memory_output_stream_new(NULL, 0, g_realloc, g_free)),
											soup_uri_new("http://localhost/ags-test-osc-xmlrpc"),
											SOUP_WEBSOCKET_CONNECTION_SERVER,
											NULL,
											NULL);

    AGS_OSC_SERVER(osc_xmlrpc_server)->connection = g_list_prepend(AGS_OSC_SERVER(osc_xmlrpc_server)->connection,
								   osc_websocket_connection[i]);
  }

  /* find */
  success = TRUE;
  
  for(i = 0; i < AGS_OSC_XMLRPC_SERVER_TEST_FIND_WEBSOCKET_CONNECTION_CONNECTION_COUNT; i++){
    GObject *current;

    current = ags_osc_xmlrpc_server_find_websocket_connection(osc_xmlrpc_server,
							      websocket_connection[i]);

    if(current != osc_websocket_connection[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void ags_osc_xmlrpc_server_test_add_websocket_handler_callback(SoupServer *server,
							       SoupWebsocketConnection *connection,
							       const char *path,
							       SoupClientContext *client,
							       gpointer user_data)
{
}

void
ags_osc_xmlrpc_server_test_add_websocket_handler()
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;

  AgsServer *server;

  osc_xmlrpc_server = ags_osc_xmlrpc_server_new();

  server = ags_server_new();

  server->soup_server = soup_server_new(NULL);
  
  g_object_set(osc_xmlrpc_server,
	       "xmlrpc-server", server,
	       NULL);

  ags_osc_xmlrpc_server_add_websocket_handler(osc_xmlrpc_server,
					      "/ags-test-osc-xmlrpc",
					      NULL,
					      NULL,
					      ags_osc_xmlrpc_server_test_add_websocket_handler_callback,
					      osc_xmlrpc_server,
					      NULL);
}

void
ags_osc_xmlrpc_server_test_add_default_controller()
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;

  AgsServer *server;
  
  osc_xmlrpc_server = ags_osc_xmlrpc_server_new();

  server = ags_server_new();

  server->soup_server = soup_server_new(NULL);

  g_object_set(osc_xmlrpc_server,
	       "xmlrpc-server", server,
	       NULL);
  
  ags_osc_xmlrpc_server_add_default_controller(osc_xmlrpc_server);

  CU_ASSERT(server->controller != NULL);
  CU_ASSERT(AGS_OSC_SERVER(osc_xmlrpc_server)->controller != NULL);
}

void
ags_osc_xmlrpc_server_test_start()
{
  //TODO:JK: implement me
}

void
ags_osc_xmlrpc_server_test_stop()
{
  //TODO:JK: implement me
}

void
ags_osc_xmlrpc_server_test_listen()
{
  //TODO:JK: implement me
}

void
ags_osc_xmlrpc_server_test_dispatch()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsOscXmlrpcServerTest", ags_osc_xmlrpc_server_test_init_suite, ags_osc_xmlrpc_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscXmlrpcServer find XMLRPC connection", ags_osc_xmlrpc_server_test_find_xmlrpc_connection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer find websocket connection", ags_osc_xmlrpc_server_test_find_websocket_connection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer add websocket handler", ags_osc_xmlrpc_server_test_add_websocket_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer add default controller", ags_osc_xmlrpc_server_test_add_default_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer start", ags_osc_xmlrpc_server_test_start) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer stop", ags_osc_xmlrpc_server_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer listen", ags_osc_xmlrpc_server_test_listen) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer dispatch", ags_osc_xmlrpc_server_test_dispatch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
  
