/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_thread_file_xml_test_init_suite();
int ags_thread_file_xml_test_clean_suite();

void ags_file_test_read_thread();
void ags_file_test_write_thread();
void ags_file_test_read_thread_list();
void ags_file_test_write_thread_list();
void ags_file_test_read_thread_pool();
void ags_file_test_write_thread_pool();
void ags_file_test_read_timestamp();
void ags_file_test_write_timestamp();
void ags_file_test_read_timestamp_list();
void ags_file_test_write_timestamp_list();

#define AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
#define AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n"							\
  "</ags-test>\n"

#define AGS_FILE_TEST_READ_THREAD_DOC AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n"							\
  "<ags-thread type=\"AgsThread\" id=\"test-id-thread-0\" flags=\"0\" frequency=\"125.0\"/>\n" \
  "</ags-test>\n"

#define AGS_FILE_TEST_READ_THREAD_LIST_DOC AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n"							\
  "<ags-thread-list id=\"test-id-thread-list-0\">\n"			\
  "<ags-thread type=\"AgsThread\" id=\"test-id-thread-0\" flags=\"0\" frequency=\"125.0\"/>\n" \
  "<ags-thread type=\"AgsThread\" id=\"test-id-thread-1\" flags=\"0\" frequency=\"250.0\"/>\n" \
  "<ags-thread type=\"AgsThread\" id=\"test-id-thread-2\" flags=\"0\" frequency=\"48.0\"/>\n" \
  "</ags-thread-list>\n"						\
  "</ags-test>\n"

#define AGS_FILE_TEST_READ_THREAD_POOL_DOC AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n"							\
  "<ags-thread-pool id=\"test-id-thread-pool-0\" flags=\"0\"/>\n"	\
  "</ags-test>\n"

#define AGS_FILE_TEST_READ_TIMESTAMP_DOC AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n" \
  "<ags-timestamp id=\"test-id-timestamp-0\" flags=\"0\" delay=\"0.0\" attack=\"0\">0</ags-timestamp>\n" \
  "</ags-test>\n"

#define AGS_FILE_TEST_READ_TIMESTAMP_LIST_DOC AGS_THREAD_XML_FILE_TEST_DOCTYPE_TEMPLATE \
  "<ags-test>\n"							\
  "<ags-timestamp-list id=\"test-id-timestamp-list-0\">\n"		\
  "<ags-timestamp id=\"test-id-timestamp-0\" flags=\"0\" delay=\"0.0\" attack=\"0\">0</ags-timestamp>\n" \
  "<ags-timestamp id=\"test-id-timestamp-0\" flags=\"0\" delay=\"0.0\" attack=\"0\">0</ags-timestamp>\n" \
  "<ags-timestamp id=\"test-id-timestamp-0\" flags=\"0\" delay=\"0.0\" attack=\"0\">0</ags-timestamp>\n" \
  "</ags-timestamp-list>\n"						\
  "</ags-test>\n"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_file_xml_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_file_xml_test_clean_suite()
{
  return(0);
}

void
ags_file_test_read_thread()
{
  AgsThread *thread;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;
  
  thread = NULL;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  doc = xmlReadMemory(AGS_FILE_TEST_READ_THREAD_DOC, strlen(AGS_FILE_TEST_READ_THREAD_DOC),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);
  
  g_object_set(file,
	       "xml-doc", doc,
	       NULL);
  
  /* child */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-thread",
		     child->name,
		     11)){
	ags_file_read_thread(file, child, &thread);

	break;
      }
    }

    child = child->next;
  }

  CU_ASSERT(AGS_IS_THREAD(thread) &&
	    thread->freq == 125.0);
}

void
ags_file_test_write_thread()
{
  AgsThread *thread;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *node;

  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  thread->freq = 250.0;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  doc = xmlReadMemory(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE, strlen(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);

  g_object_set(file,
	       "xml-doc", doc,
	       NULL);

  /* node */
  node = ags_file_write_thread(file, root_node, thread);

  CU_ASSERT(node != NULL &&
	    node->parent == root_node &&
	    !xmlStrncmp(node->name,
			"ags-thread",
			11) &&
	    !xmlStrncmp(xmlGetProp(node,
				   "frequency"),
			"250.0",
			6));
}

void
ags_file_test_read_thread_list()
{
  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  GList *list;
  
  list = NULL;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  doc = xmlReadMemory(AGS_FILE_TEST_READ_THREAD_LIST_DOC, strlen(AGS_FILE_TEST_READ_THREAD_LIST_DOC),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);
  
  g_object_set(file,
	       "xml-doc", doc,
	       NULL);
  
  /* child */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-thread-list",
		     child->name,
		     16)){
	ags_file_read_thread_list(file, child, &list);

	break;
      }
    }

    child = child->next;
  }

  CU_ASSERT(list != NULL &&
	    g_list_length(list) == 3);
}

void
ags_file_test_write_thread_list()
{
  AgsThread *thread;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  GList *list;

  list = NULL;

  /* create some threads */
  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  thread->freq = 250.0;
  list = g_list_prepend(list,
			thread);

  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  thread->freq = 125.0;
  list = g_list_prepend(list,
			thread);

  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  thread->freq = 48.0;
  list = g_list_prepend(list,
			thread);

  /* create file object */
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  doc = xmlReadMemory(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE, strlen(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);

  g_object_set(file,
	       "xml-doc", doc,
	       NULL);

  /* write thread list */
  child = ags_file_write_thread_list(file, root_node, list);

  CU_ASSERT(child != NULL &&
	    child->parent == root_node &&
	    !xmlStrncmp(child->name,
			"ags-thread-list",
			16));
}

void
ags_file_test_read_thread_pool()
{
  AgsThreadPool *thread_pool;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;
  
  thread_pool = NULL;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  doc = xmlReadMemory(AGS_FILE_TEST_READ_THREAD_POOL_DOC, strlen(AGS_FILE_TEST_READ_THREAD_POOL_DOC),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);
  
  g_object_set(file,
	       "xml-doc", doc,
	       NULL);
  
  /* child */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-thread-pool",
		     child->name,
		     16)){
	ags_file_read_thread_pool(file, child, &thread_pool);

	break;
      }
    }

    child = child->next;
  }

  CU_ASSERT(AGS_IS_THREAD_POOL(thread_pool));
}

void
ags_file_test_write_thread_pool()
{
  AgsThreadPool *thread_pool;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  thread_pool = g_object_new(AGS_TYPE_THREAD_POOL,
			     NULL);

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  doc = xmlReadMemory(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE, strlen(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);

  g_object_set(file,
	       "xml-doc", doc,
	       NULL);

  /* node */
  child = ags_file_write_thread_pool(file, root_node, thread_pool);

  CU_ASSERT(child != NULL &&
	    child->parent == root_node &&
	    !xmlStrncmp(child->name,
			"ags-thread-pool",
			16));
}

void
ags_file_test_read_timestamp()
{
  AgsTimestamp *timestamp;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;
  
  timestamp = NULL;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  doc = xmlReadMemory(AGS_FILE_TEST_READ_TIMESTAMP_DOC, strlen(AGS_FILE_TEST_READ_TIMESTAMP_DOC),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);
  
  g_object_set(file,
	       "xml-doc", doc,
	       NULL);
  
  /* child */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-timestamp",
		     child->name,
		     14)){
	ags_file_read_timestamp(file, child, &timestamp);

	break;
      }
    }

    child = child->next;
  }

  CU_ASSERT(AGS_IS_TIMESTAMP(timestamp) &&
	    timestamp->delay == 0.0 &&
	    timestamp->attack == 0);
}

void
ags_file_test_write_timestamp()
{
  AgsTimestamp *timestamp;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  timestamp = g_object_new(AGS_TYPE_TIMESTAMP,
			   NULL);

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  doc = xmlReadMemory(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE, strlen(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);

  g_object_set(file,
	       "xml-doc", doc,
	       NULL);

  /* node */
  child = ags_file_write_timestamp(file, root_node, timestamp);

  CU_ASSERT(child != NULL &&
	    child->parent == root_node &&
	    !xmlStrncmp(child->name,
			"ags-timestamp",
			14));
}

void
ags_file_test_read_timestamp_list()
{
  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  GList *list;
  
  list = NULL;

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  doc = xmlReadMemory(AGS_FILE_TEST_READ_TIMESTAMP_LIST_DOC, strlen(AGS_FILE_TEST_READ_TIMESTAMP_LIST_DOC),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);
  
  g_object_set(file,
	       "xml-doc", doc,
	       NULL);
  
  /* child */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-timestamp-list",
		     child->name,
		     18)){
	ags_file_read_timestamp_list(file, child, &list);

	break;
      }
    }

    child = child->next;
  }

  CU_ASSERT(list != NULL &&
	    g_list_length(list) == 3);
}

void
ags_file_test_write_timestamp_list()
{
  AgsTimestamp *timestamp;

  AgsFile *file;

  xmlDoc *doc;
  xmlNode *root_node, *child;

  GList *list;

  list = NULL;

  /* create some timestamps */
  timestamp = g_object_new(AGS_TYPE_TIMESTAMP,
			NULL);
  list = g_list_prepend(list,
			timestamp);

  timestamp = g_object_new(AGS_TYPE_TIMESTAMP,
			NULL);
  list = g_list_prepend(list,
			timestamp);

  timestamp = g_object_new(AGS_TYPE_TIMESTAMP,
			NULL);
  list = g_list_prepend(list,
			timestamp);

  /* create file object */
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  doc = xmlReadMemory(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE, strlen(AGS_THREAD_XML_FILE_TEST_DOC_TEMPLATE),
		      NULL, NULL, 0);
  root_node = xmlDocGetRootElement(doc);

  g_object_set(file,
	       "xml-doc", doc,
	       NULL);

  /* write timestamp list */
  child = ags_file_write_timestamp_list(file, root_node, list);

  CU_ASSERT(child != NULL &&
	    child->parent == root_node &&
	    !xmlStrncmp(child->name,
			"ags-timestamp-list",
			18));
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
  pSuite = CU_add_suite("AgsThreadFileXmlTest", ags_thread_file_xml_test_init_suite, ags_thread_file_xml_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsThreadFileXml read thread", ags_file_test_read_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml write thread", ags_file_test_write_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml read thread list", ags_file_test_read_thread_list) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml write thread list", ags_file_test_write_thread_list) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml read thread pool", ags_file_test_read_thread_pool) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml write thread pool", ags_file_test_write_thread_pool) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml read timestamp", ags_file_test_read_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml write timestamp", ags_file_test_write_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml read timestamp list", ags_file_test_read_timestamp_list) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadFileXml write timestamp list", ags_file_test_write_timestamp_list) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

