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

int ags_connectable_test_init_suite();
int ags_connectable_test_clean_suite();

void ags_connectable_test_get_uuid();
void ags_connectable_test_has_resource();
void ags_connectable_test_is_ready();
void ags_connectable_test_add_to_registry();
void ags_connectable_test_remove_from_registry();
void ags_connectable_test_list_resource();
void ags_connectable_test_xml_compose();
void ags_connectable_test_xml_parse();
void ags_connectable_test_is_connected();
void ags_connectable_test_connect();
void ags_connectable_test_disconnect();
void ags_connectable_test_connect_connection();
void ags_connectable_test_disconnect_connection();

GType connectable_test_types[39];
 
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_connectable_test_init_suite()
{
  guint i;

  ags_audio_application_context_new();
  
  i = 0;
  
  connectable_test_types[i++] = AGS_TYPE_APPLICATION_CONTEXT;
  connectable_test_types[i++] = AGS_TYPE_THREAD;
  connectable_test_types[i++] = AGS_TYPE_AUDIO;
  connectable_test_types[i++] = AGS_TYPE_AUDIO_SIGNAL;
  connectable_test_types[i++] = AGS_TYPE_CHANNEL;
  connectable_test_types[i++] = AGS_TYPE_FIFOOUT;
  connectable_test_types[i++] = AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN;
  connectable_test_types[i++] = AGS_TYPE_GENERIC_RECALL_RECYCLING;
  connectable_test_types[i++] = AGS_TYPE_PATTERN;
  connectable_test_types[i++] = AGS_TYPE_PORT;
  connectable_test_types[i++] = AGS_TYPE_RECALL;
  connectable_test_types[i++] = AGS_TYPE_RECYCLING;
  connectable_test_types[i++] = AGS_TYPE_AUDIO_FILE;
  connectable_test_types[i++] = AGS_TYPE_AUDIO_CONTAINER;
  connectable_test_types[i++] = AGS_TYPE_IPATCH;
  connectable_test_types[i++] = AGS_TYPE_IPATCH_DLS2_READER;
  connectable_test_types[i++] = AGS_TYPE_IPATCH_GIG_READER;
  connectable_test_types[i++] = AGS_TYPE_IPATCH_SF2_READER;
  connectable_test_types[i++] = AGS_TYPE_SNDFILE;
  connectable_test_types[i++] = AGS_TYPE_ALSA_DEVIN;
  connectable_test_types[i++] = AGS_TYPE_ALSA_DEVOUT;
  connectable_test_types[i++] = AGS_TYPE_ALSA_MIDIIN;
  connectable_test_types[i++] = AGS_TYPE_CORE_AUDIO_CLIENT;
  connectable_test_types[i++] = AGS_TYPE_CORE_AUDIO_DEVOUT;
  connectable_test_types[i++] = AGS_TYPE_CORE_AUDIO_MIDIIN;
  connectable_test_types[i++] = AGS_TYPE_CORE_AUDIO_PORT;
  connectable_test_types[i++] = AGS_TYPE_CORE_AUDIO_SERVER;
  connectable_test_types[i++] = AGS_TYPE_JACK_CLIENT;
  connectable_test_types[i++] = AGS_TYPE_JACK_DEVIN;
  connectable_test_types[i++] = AGS_TYPE_JACK_DEVOUT;
  connectable_test_types[i++] = AGS_TYPE_JACK_MIDIIN;
  connectable_test_types[i++] = AGS_TYPE_JACK_PORT;
  connectable_test_types[i++] = AGS_TYPE_JACK_SERVER;
  connectable_test_types[i++] = AGS_TYPE_PULSE_CLIENT;
  connectable_test_types[i++] = AGS_TYPE_PULSE_DEVIN;
  connectable_test_types[i++] = AGS_TYPE_PULSE_DEVOUT;
  connectable_test_types[i++] = AGS_TYPE_PULSE_PORT;
  connectable_test_types[i++] = AGS_TYPE_PULSE_SERVER;
  connectable_test_types[i++] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_connectable_test_clean_suite()
{  
  return(0);
}

void
ags_connectable_test_get_uuid()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);

    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->get_uuid == NULL){
      g_message("AgsConnectable::get-uuid missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_connectable_get_uuid(current);
  }

  CU_ASSERT(success);
}

void
ags_connectable_test_has_resource()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);

    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->has_resource == NULL){
      g_message("AgsConnectable::has-resource missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_connectable_has_resource(current);
  }

  CU_ASSERT(success);
}

void
ags_connectable_test_is_ready()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    ags_connectable_is_ready(current);
  }
}

void
ags_connectable_test_add_to_registry()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    ags_connectable_add_to_registry(current);
  }
}

void
ags_connectable_test_remove_from_registry()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    ags_connectable_add_to_registry(current);
    ags_connectable_remove_from_registry(current);
  }
}

void
ags_connectable_test_list_resource()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    ags_connectable_list_resource(current);
  }
}

void
ags_connectable_test_xml_compose()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    ags_connectable_xml_compose(current);
  }
}

void
ags_connectable_test_xml_parse()
{
  GObject *current;

  xmlNode *node;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    node = ags_connectable_xml_compose(current);
    ags_connectable_xml_parse(current,
			      node);
  }
}

void
ags_connectable_test_is_connected()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);

    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->is_connected == NULL){
      g_message("AgsConnectable::is-connected missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_connectable_is_connected(current);
  }

  CU_ASSERT(success);
}

void
ags_connectable_test_connect()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);

    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->connect == NULL){
      g_message("AgsConnectable::connect missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_connectable_connect(current);
  }

  CU_ASSERT(success);
}

void
ags_connectable_test_disconnect()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);

    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->disconnect == NULL){
      g_message("AgsConnectable::disconnect missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_connectable_connect(current);
    ags_connectable_disconnect(current);
  }

  CU_ASSERT(success);
}

void
ags_connectable_test_connect_connection()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->connect_connection != NULL){
      ags_connectable_connect_connection(current,
					 NULL);
    }
  }
}

void
ags_connectable_test_disconnect_connection()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; connectable_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(connectable_test_types[i],
			   NULL);
    
    if(AGS_CONNECTABLE_GET_INTERFACE(AGS_CONNECTABLE(current))->disconnect_connection != NULL){
      ags_connectable_disconnect_connection(current,
					    NULL);
    }
  }
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
  pSuite = CU_add_suite("AgsConnectableTest", ags_connectable_test_init_suite, ags_connectable_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConnectable get uuid", ags_connectable_test_get_uuid) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable has resource", ags_connectable_test_has_resource) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable is ready", ags_connectable_test_is_ready) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable add to registry", ags_connectable_test_add_to_registry) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable remove from registry", ags_connectable_test_remove_from_registry) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable list resource", ags_connectable_test_list_resource) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable xml compose", ags_connectable_test_xml_compose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable xml parse", ags_connectable_test_xml_parse) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable is connected", ags_connectable_test_is_connected) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable connect", ags_connectable_test_connect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable disconnect", ags_connectable_test_disconnect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable connect connection", ags_connectable_test_connect_connection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectable disconnect connection", ags_connectable_test_disconnect_connection) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
