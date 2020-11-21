/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

int ags_message_envelope_test_init_suite();
int ags_message_envelope_test_clean_suite();

void ags_message_envelope_test_get_sender();
void ags_message_envelope_test_get_recipient();
void ags_message_envelope_test_get_doc();
void ags_message_envelope_test_get_parameter();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_envelope_test_init_suite()
{  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_envelope_test_clean_suite()
{
  return(0);
}

void
ags_message_envelope_test_get_sender()
{
  AgsMessageEnvelope *message_envelope;
  AgsThread *thread;
  
  message_envelope = ags_message_envelope_new((thread = ags_thread_new()),
					      NULL,
					      NULL);

  CU_ASSERT(ags_message_envelope_get_sender(message_envelope) == thread);
}

void
ags_message_envelope_test_get_recipient()
{
  AgsMessageEnvelope *message_envelope;
  AgsThread *thread;
  
  message_envelope = ags_message_envelope_new(NULL,
					      (thread = ags_thread_new()),
					      NULL);

  CU_ASSERT(ags_message_envelope_get_recipient(message_envelope) == thread);
}

void
ags_message_envelope_test_get_doc()
{
  AgsMessageEnvelope *message_envelope;

  xmlDoc *doc;
  xmlNode *root_node;

  /* specify message body */
  doc = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 "ags-command");
  xmlDocSetRootElement(doc, root_node);    

  xmlNewProp(root_node,
	     "method",
	     "AgsMessageEnvelopeTest::test-signal");

  message_envelope = ags_message_envelope_new(NULL,
					      NULL,
					      doc);

  CU_ASSERT(ags_message_envelope_get_doc(message_envelope) == doc);
}

void
ags_message_envelope_test_get_parameter()
{
  AgsMessageEnvelope *message_envelope;
  AgsThread *thread;

  GValue *value, *ret_value;
  
  gchar **parameter_name, **ret_parameter_name;
  
  guint n_params, ret_n_params;

  n_params = 2;
  
  value = g_new0(GValue,
		 2);

  parameter_name = (gchar **) g_malloc(3 * sizeof(gchar *));

  parameter_name[0] = "test-0";
  parameter_name[1] = "test-1";
  parameter_name[2] = NULL;
  
  message_envelope = ags_message_envelope_new_with_params(NULL,
							  NULL,
							  NULL,
							  n_params,
							  parameter_name,
							  value);

  ags_message_envelope_get_parameter(message_envelope,
				     &ret_n_params,
				     &ret_parameter_name,
				     &ret_value);
  
  CU_ASSERT(n_params == ret_n_params);
  CU_ASSERT(parameter_name == ret_parameter_name);
  CU_ASSERT(value == ret_value);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsMessageEnvelopeTest", ags_message_envelope_test_init_suite, ags_message_envelope_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMessageEnvelope get sender", ags_message_envelope_test_get_sender) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageEnvelope get recipient", ags_message_envelope_test_get_recipient) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageEnvelope get doc", ags_message_envelope_test_get_doc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageEnvelope get parameter", ags_message_envelope_test_get_parameter) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

