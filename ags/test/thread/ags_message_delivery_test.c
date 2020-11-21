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

int ags_message_delivery_test_init_suite();
int ags_message_delivery_test_clean_suite();

void ags_message_delivery_test_add_message_queue();
void ags_message_delivery_test_remove_message_queue();
void ags_message_delivery_test_find_sender_namespace();
void ags_message_delivery_test_find_recipient_namespace();
void ags_message_delivery_test_add_message_envelope();
void ags_message_delivery_test_remove_message_envelope();
void ags_message_delivery_test_find_sender();
void ags_message_delivery_test_find_recipient();
void ags_message_delivery_test_query_message();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_delivery_test_init_suite()
{  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_delivery_test_clean_suite()
{
  return(0);
}

void
ags_message_delivery_test_add_message_queue()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  
  message_delivery = ags_message_delivery_new();

  message_queue = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue);

  CU_ASSERT(g_list_find(message_delivery->message_queue, message_queue) != NULL);
  
  message_queue = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue);

  CU_ASSERT(g_list_find(message_delivery->message_queue, message_queue) != NULL);
}

void
ags_message_delivery_test_remove_message_queue()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  message_delivery->message_queue = g_list_prepend(message_delivery->message_queue,
						   message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  message_delivery->message_queue = g_list_prepend(message_delivery->message_queue,
						   message_queue_1);

  ags_message_delivery_remove_message_queue(message_delivery,
					    message_queue_0);

  CU_ASSERT(g_list_find(message_delivery->message_queue, message_queue_0) == NULL);
  
  ags_message_delivery_remove_message_queue(message_delivery,
					    message_queue_1);

  CU_ASSERT(g_list_find(message_delivery->message_queue, message_queue_1) == NULL);
}

void
ags_message_delivery_test_find_sender_namespace()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  
  message_delivery = ags_message_delivery_new();

  CU_ASSERT(ags_message_delivery_find_sender_namespace(message_delivery, "ags-test-000") == NULL);  
  CU_ASSERT(ags_message_delivery_find_sender_namespace(message_delivery, "ags-test-001") == NULL);  

  message_queue = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue);

  message_queue = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue);

  CU_ASSERT(ags_message_delivery_find_sender_namespace(message_delivery, "ags-test-000") != NULL);  
  CU_ASSERT(ags_message_delivery_find_sender_namespace(message_delivery, "ags-test-001") != NULL);  
  CU_ASSERT(ags_message_delivery_find_sender_namespace(message_delivery, "ags-test-002") == NULL);  
}

void
ags_message_delivery_test_find_recipient_namespace()
{
  //TODO:JK: implement me
}

void
ags_message_delivery_test_add_message_envelope()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  AgsMessageEnvelope *message_envelope;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_1);

  message_envelope = ags_message_envelope_new(NULL,
					      NULL,
					      NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-000",
					    message_envelope);

  CU_ASSERT(g_list_find(message_queue_0->message_envelope, message_envelope) != NULL);

  message_envelope = ags_message_envelope_new(NULL,
					      NULL,
					      NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-001",
					    message_envelope);

  CU_ASSERT(g_list_find(message_queue_1->message_envelope, message_envelope) != NULL);

  message_envelope = ags_message_envelope_new(NULL,
					      NULL,
					      NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-002",
					    message_envelope);
}

void
ags_message_delivery_test_remove_message_envelope()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_1);

  message_envelope_0 = ags_message_envelope_new(NULL,
						NULL,
						NULL);
  message_queue_0->message_envelope = g_list_prepend(message_queue_0->message_envelope,
						     message_envelope_0);
  
  message_envelope_1 = ags_message_envelope_new(NULL,
						NULL,
						NULL);
  message_queue_1->message_envelope = g_list_prepend(message_queue_1->message_envelope,
						     message_envelope_1);


  ags_message_delivery_remove_message_envelope(message_delivery,
					       "ags-test-000",
					       message_envelope_0);

  CU_ASSERT(g_list_find(message_queue_0->message_envelope, message_envelope_0) == NULL);
  
  ags_message_delivery_remove_message_envelope(message_delivery,
					       "ags-test-001",
					       message_envelope_1);

  CU_ASSERT(g_list_find(message_queue_1->message_envelope, message_envelope_1) == NULL);
}

void
ags_message_delivery_test_find_sender()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  AgsThread *thread_0, *thread_1;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_1);

  message_envelope_0 = ags_message_envelope_new((thread_0 = ags_thread_new()),
						NULL,
						NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-000",
					    message_envelope_0);

  message_envelope_1 = ags_message_envelope_new((thread_1 = ags_thread_new()),
						NULL,
						NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-001",
					    message_envelope_1);

  CU_ASSERT(ags_message_delivery_find_sender(message_delivery,
					     NULL,
					     thread_0) != NULL);
  CU_ASSERT(ags_message_delivery_find_sender(message_delivery,
					     "ags-test-000",
					     thread_0) == NULL);

  CU_ASSERT(ags_message_delivery_find_sender(message_delivery,
					     NULL,
					     thread_1) != NULL);
  CU_ASSERT(ags_message_delivery_find_sender(message_delivery,
					     "ags-test-001",
					     thread_1) == NULL);
}

void
ags_message_delivery_test_find_recipient()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  AgsThread *thread_0, *thread_1;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_1);

  message_envelope_0 = ags_message_envelope_new(NULL,
						(thread_0 = ags_thread_new()),
						NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-000",
					    message_envelope_0);

  message_envelope_1 = ags_message_envelope_new(NULL,
						(thread_1 = ags_thread_new()),
						NULL);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-001",
					    message_envelope_1);

  CU_ASSERT(ags_message_delivery_find_recipient(message_delivery,
						NULL,
						thread_0) != NULL);
  CU_ASSERT(ags_message_delivery_find_recipient(message_delivery,
						"ags-test-000",
						thread_0) == NULL);

  CU_ASSERT(ags_message_delivery_find_recipient(message_delivery,
						NULL,
						thread_1) != NULL);
  CU_ASSERT(ags_message_delivery_find_recipient(message_delivery,
						"ags-test-001",
						thread_1) == NULL);
}

void
ags_message_delivery_test_query_message()
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue_0, *message_queue_1;
  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;

  xmlDoc *doc_0, *doc_1;
  xmlNode *root_node;
  
  message_delivery = ags_message_delivery_new();

  message_queue_0 = ags_message_queue_new("ags-test-000");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_0);

  message_queue_1 = ags_message_queue_new("ags-test-001");
  ags_message_delivery_add_message_queue(message_delivery,
					 message_queue_1);

  /* specify message body */
  doc_0 = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 "ags-command");
  xmlDocSetRootElement(doc_0, root_node);    

  xmlNewProp(root_node,
	     "method",
	     "AgsMessageDeliveryTest::test-signal");

  message_envelope_0 = ags_message_envelope_new(NULL,
						NULL,
						doc_0);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-000",
					    message_envelope_0);

  /* specify message body */
  doc_1 = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 "ags-command");
  xmlDocSetRootElement(doc_1, root_node);    

  xmlNewProp(root_node,
	     "method",
	     "AgsMessageDeliveryTest::test-signal");

  message_envelope_1 = ags_message_envelope_new(NULL,
						NULL,
						doc_1);
  ags_message_delivery_add_message_envelope(message_delivery,
					    "ags-test-001",
					    message_envelope_1);

  CU_ASSERT(ags_message_delivery_query_message(message_delivery,
					       NULL,
					       "/ags-command[@method = 'AgsMessageDeliveryTest::test-signal']") != NULL);  
  CU_ASSERT(ags_message_delivery_query_message(message_delivery,
					       NULL,
					       "/ags-command[@method = 'AgsMessageDeliveryTest::test-signal']") != NULL);

  CU_ASSERT(g_list_length(ags_message_delivery_query_message(message_delivery,
							     NULL,
							     "/ags-command[@method = 'AgsMessageDeliveryTest::test-signal']")) == 2);
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
  pSuite = CU_add_suite("AgsMessageDeliveryTest", ags_message_delivery_test_init_suite, ags_message_delivery_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMessageDelivery add message queue", ags_message_delivery_test_add_message_queue) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery remove message queue", ags_message_delivery_test_remove_message_queue) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery find sender namespace", ags_message_delivery_test_find_sender_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery find recipient namespace", ags_message_delivery_test_find_recipient_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery add message envelope", ags_message_delivery_test_add_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery remove message envelope", ags_message_delivery_test_remove_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery find sender", ags_message_delivery_test_find_sender) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery find recipient", ags_message_delivery_test_find_recipient) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageDelivery query message", ags_message_delivery_test_query_message) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

