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

int ags_message_queue_test_init_suite();
int ags_message_queue_test_clean_suite();

void ags_message_queue_test_set_sender_namespace();
void ags_message_queue_test_get_sender_namespace();
void ags_message_queue_test_set_recipient_namespace();
void ags_message_queue_test_get_recipient_namespace();
void ags_message_queue_test_set_message_envelope();
void ags_message_queue_test_get_message_envelope();
void ags_message_queue_test_add_message_envelope();
void ags_message_queue_test_remove_message_envelope();
void ags_message_queue_test_find_sender();
void ags_message_queue_test_find_recipient();
void ags_message_queue_test_query_message();

#define AGS_MESSAGE_QUEUE_TEST_SENDER_NAMESPACE "test-sender"

#define AGS_MESSAGE_QUEUE_TEST_RECIPIENT_NAMESPACE "test-recipient"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_queue_test_init_suite()
{  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_message_queue_test_clean_suite()
{
  return(0);
}

void
ags_message_queue_test_set_sender_namespace()
{
  AgsMessageQueue *message_queue;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  CU_ASSERT(message_queue->sender_namespace == NULL);

  ags_message_queue_set_sender_namespace(message_queue,
					 AGS_MESSAGE_QUEUE_TEST_SENDER_NAMESPACE);

  CU_ASSERT(!g_strcmp0(message_queue->sender_namespace,
		       AGS_MESSAGE_QUEUE_TEST_SENDER_NAMESPACE));
}

void
ags_message_queue_test_get_sender_namespace()
{
  AgsMessageQueue *message_queue;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  message_queue->sender_namespace = g_strdup(AGS_MESSAGE_QUEUE_TEST_SENDER_NAMESPACE);

  CU_ASSERT(!g_strcmp0(ags_message_queue_get_sender_namespace(message_queue),
		       AGS_MESSAGE_QUEUE_TEST_SENDER_NAMESPACE));
}

void
ags_message_queue_test_set_recipient_namespace()
{
  AgsMessageQueue *message_queue;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  CU_ASSERT(message_queue->recipient_namespace == NULL);

  ags_message_queue_set_recipient_namespace(message_queue,
					    AGS_MESSAGE_QUEUE_TEST_RECIPIENT_NAMESPACE);

  CU_ASSERT(!g_strcmp0(message_queue->recipient_namespace,
		       AGS_MESSAGE_QUEUE_TEST_RECIPIENT_NAMESPACE));
}

void
ags_message_queue_test_get_recipient_namespace()
{
  AgsMessageQueue *message_queue;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  message_queue->recipient_namespace = g_strdup(AGS_MESSAGE_QUEUE_TEST_RECIPIENT_NAMESPACE);

  CU_ASSERT(!g_strcmp0(ags_message_queue_get_recipient_namespace(message_queue),
		       AGS_MESSAGE_QUEUE_TEST_RECIPIENT_NAMESPACE));
}

void
ags_message_queue_test_set_message_envelope()
{
  AgsMessageQueue *message_queue;

  GList *start_message_envelope;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  start_message_envelope = NULL;

  start_message_envelope = g_list_prepend(start_message_envelope,
					  g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL));
  start_message_envelope = g_list_prepend(start_message_envelope,
					  g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL));

  CU_ASSERT(message_queue->message_envelope == NULL);

  ags_message_queue_set_message_envelope(message_queue,
					 start_message_envelope);

  CU_ASSERT(message_queue->message_envelope == start_message_envelope);
}

void
ags_message_queue_test_get_message_envelope()
{
  AgsMessageQueue *message_queue;

  GList *start_message_envelope;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  start_message_envelope = NULL;

  start_message_envelope = g_list_prepend(start_message_envelope,
					  g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL));
  start_message_envelope = g_list_prepend(start_message_envelope,
					  g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL));

  message_queue->message_envelope = start_message_envelope;
  
  CU_ASSERT(ags_message_queue_get_message_envelope(message_queue) == start_message_envelope);
}

void
ags_message_queue_test_add_message_envelope()
{
  AgsMessageQueue *message_queue;

  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  
  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  message_envelope_0 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL);

  ags_message_queue_add_message_envelope(message_queue,
					 message_envelope_0);
  
  message_envelope_1 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL);

  ags_message_queue_add_message_envelope(message_queue,
					 message_envelope_1);

  CU_ASSERT(g_list_find(message_queue->message_envelope, message_envelope_0) != NULL);
  CU_ASSERT(g_list_find(message_queue->message_envelope, message_envelope_1) != NULL);
}

void
ags_message_queue_test_remove_message_envelope()
{
  AgsMessageQueue *message_queue;

  GList *start_message_envelope;

  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;

  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  start_message_envelope = NULL;

  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_0 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL)));
  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_1 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, NULL)));

  message_queue->message_envelope = start_message_envelope;
  
  ags_message_queue_remove_message_envelope(message_queue,
					    message_envelope_0);
  
  CU_ASSERT(g_list_find(message_queue->message_envelope, message_envelope_0) == NULL);
  CU_ASSERT(g_list_find(message_queue->message_envelope, message_envelope_1) != NULL);

  ags_message_queue_remove_message_envelope(message_queue,
					    message_envelope_1);

  CU_ASSERT(g_list_find(message_queue->message_envelope, message_envelope_1) == NULL);
}

void
ags_message_queue_test_find_sender()
{
  AgsMessageQueue *message_queue;

  GList *start_message_envelope;

  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  AgsThread *sender_0, *sender_1, *sender_2;
  
  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  start_message_envelope = NULL;

  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_0 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, "sender", (sender_0 = ags_thread_new()), NULL)));
  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_1 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, "sender", (sender_1 = ags_thread_new()), NULL)));

  sender_2 = ags_thread_new();
  
  message_queue->message_envelope = start_message_envelope;

  CU_ASSERT(ags_message_queue_find_sender(message_queue, sender_0) != NULL);
  CU_ASSERT(ags_message_queue_find_sender(message_queue, sender_1) != NULL);
  CU_ASSERT(ags_message_queue_find_sender(message_queue, sender_2) == NULL);
}

void
ags_message_queue_test_find_recipient()
{
  AgsMessageQueue *message_queue;

  GList *start_message_envelope;

  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;
  AgsThread *recipient_0, *recipient_1, *recipient_2;
  
  message_queue = g_object_new(AGS_TYPE_MESSAGE_QUEUE,
			       NULL);

  start_message_envelope = NULL;

  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_0 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, "recipient", (recipient_0 = ags_thread_new()), NULL)));
  start_message_envelope = g_list_prepend(start_message_envelope,
					  (message_envelope_1 = g_object_new(AGS_TYPE_MESSAGE_ENVELOPE, "recipient", (recipient_1 = ags_thread_new()), NULL)));

  recipient_2 = ags_thread_new();
  
  message_queue->message_envelope = start_message_envelope;

  CU_ASSERT(ags_message_queue_find_recipient(message_queue, recipient_0) != NULL);
  CU_ASSERT(ags_message_queue_find_recipient(message_queue, recipient_1) != NULL);
  CU_ASSERT(ags_message_queue_find_recipient(message_queue, recipient_2) == NULL);
}

void
ags_message_queue_test_query_message()
{
  AgsMessageQueue *message_queue_0;
  AgsMessageEnvelope *message_envelope_0, *message_envelope_1;

  xmlDoc *doc_0, *doc_1;
  xmlNode *root_node;
  
  message_queue_0 = ags_message_queue_new("ags-test-000");

  /* specify message body */
  doc_0 = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 "ags-command");
  xmlDocSetRootElement(doc_0, root_node);    

  xmlNewProp(root_node,
	     "method",
	     "AgsMessageQueueTest::test-signal");

  message_envelope_0 = ags_message_envelope_new(NULL,
						NULL,
						doc_0);
  ags_message_queue_add_message_envelope(message_queue_0,
					 message_envelope_0);

  /* specify message body */
  doc_1 = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 "ags-command");
  xmlDocSetRootElement(doc_1, root_node);    

  xmlNewProp(root_node,
	     "method",
	     "AgsMessageQueueTest::test-signal");

  message_envelope_1 = ags_message_envelope_new(NULL,
						NULL,
						doc_1);
  ags_message_queue_add_message_envelope(message_queue_0,
					 message_envelope_1);

  CU_ASSERT(ags_message_queue_query_message(message_queue_0,
					    "/ags-command[@method = 'AgsMessageQueueTest::test-signal']") != NULL);  
  CU_ASSERT(ags_message_queue_query_message(message_queue_0,
					    "/ags-command[@method = 'AgsMessageQueueTest::test-signal']") != NULL);
  
  CU_ASSERT(g_list_length(ags_message_queue_query_message(message_queue_0,
							  "/ags-command[@method = 'AgsMessageQueueTest::test-signal']")) == 2);
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
  pSuite = CU_add_suite("AgsMessageQueueTest", ags_message_queue_test_init_suite, ags_message_queue_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMessageQueue set sender namespace", ags_message_queue_test_set_sender_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue get sender namespace", ags_message_queue_test_get_sender_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue set recipient namespace", ags_message_queue_test_set_recipient_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue get recipient namespace", ags_message_queue_test_get_recipient_namespace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue set message envelope", ags_message_queue_test_set_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue get message envelope", ags_message_queue_test_get_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue add message envelope", ags_message_queue_test_add_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue remove message envelope", ags_message_queue_test_remove_message_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue find sender", ags_message_queue_test_find_sender) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue find recipient", ags_message_queue_test_find_recipient) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMessageQueue query message", ags_message_queue_test_query_message) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

