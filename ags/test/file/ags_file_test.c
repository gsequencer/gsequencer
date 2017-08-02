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

int ags_file_test_init_suite();
int ags_file_test_clean_suite();

void ags_file_test_str2md5();
void ags_file_test_add_id_ref();
void ags_file_test_find_id_ref_by_node();
void ags_file_test_find_id_ref_by_xpath();
void ags_file_test_find_id_ref_by_reference();
void ags_file_test_add_lookup();
void ags_file_test_add_launch();
void ags_file_test_open();
void ags_file_test_open_from_data();
void ags_file_test_rw_open();
void ags_file_test_open_filename();
void ags_file_test_close();
void ags_file_test_write();
void ags_file_test_write_concurrent();
void ags_file_test_write_resolve();
void ags_file_test_read();
void ags_file_test_read_resolve();
void ags_file_test_read_start();
void ags_file_test_read_config();
void ags_file_test_write_config();
void ags_file_test_read_application_context();
void ags_file_test_write_application_context();

void ags_file_test_stub_open(AgsFile *file,
			     GError **error);
void ags_file_test_stub_open_from_data(AgsFile *file,
				       gchar *data, guint length,
				       GError **error);
void ags_file_test_stub_rw_open(AgsFile *file,
				gboolean create,
				GError **error);
void ags_file_test_stub_write(AgsFile *file);
void ags_file_test_stub_write_concurrent(AgsFile *file);
void ags_file_test_stub_write_resolve(AgsFile *file);
void ags_file_test_stub_read(AgsFile *file);
void ags_file_test_stub_read_resolve(AgsFile *file);
void ags_file_test_stub_read_start(AgsFile *file);

#define AGS_FILE_TEST_STR2MD5_CONTENT "GNU's not linux"

gboolean stub_open = FALSE;
gboolean stub_open_from_data = FALSE;
gboolean stub_rw_open = FALSE;
gboolean stub_write = FALSE;
gboolean stub_write_concurrent = FALSE;
gboolean stub_write_resolve = FALSE;
gboolean stub_read = FALSE;
gboolean stub_read_resolve = FALSE;
gboolean stub_read_start = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_test_clean_suite()
{
  return(0);
}

void
ags_file_test_str2md5()
{
  gchar *md5_checksum;

  md5_checksum = ags_file_str2md5(AGS_FILE_TEST_STR2MD5_CONTENT,
				  strlen(AGS_FILE_TEST_STR2MD5_CONTENT));

  CU_ASSERT(strlen(md5_checksum) == AGS_FILE_CHECKSUM_LENGTH);
}

void
ags_file_test_add_id_ref()
{
  AgsFile *file;
  AgsFileIdRef *file_id_ref[3];
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* add some id refs */
  file_id_ref[0] = g_object_new(AGS_TYPE_FILE_ID_REF,
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[0]);

  file_id_ref[1] = g_object_new(AGS_TYPE_FILE_ID_REF,
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[1]);

  file_id_ref[2] = g_object_new(AGS_TYPE_FILE_ID_REF,
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[2]);

  /* assert to be present */
  CU_ASSERT(g_list_find(file->id_refs,
			file_id_ref[0]) != NULL);

  CU_ASSERT(g_list_find(file->id_refs,
			file_id_ref[1]) != NULL);

  CU_ASSERT(g_list_find(file->id_refs,
			file_id_ref[2]) != NULL);
}

void
ags_file_test_find_id_ref_by_node()
{
  AgsFile *file;
  AgsFileIdRef *file_id_ref[3], *current;

  xmlNode *node[3];

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* add some id refs */
  node[0] = xmlNewNode(NULL,
		       "ags-file-test");

  file_id_ref[0] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"node", node[0],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[0]);

  node[1] = xmlNewNode(NULL,
		       "ags-file-test");

  file_id_ref[1] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"node", node[1],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[1]);

  node[2] = xmlNewNode(NULL,
		       "ags-file-test");

  file_id_ref[2] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"node", node[2],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[2]);  

 /* assert find by node */
  CU_ASSERT((current = ags_file_find_id_ref_by_node(file,
						    node[0])) != NULL &&
	    current == file_id_ref[0]);

  CU_ASSERT((current = ags_file_find_id_ref_by_node(file,
						    node[1])) != NULL &&
	    current == file_id_ref[1]);

  CU_ASSERT((current = ags_file_find_id_ref_by_node(file,
						    node[2])) != NULL &&
	    current == file_id_ref[2]);
}

void
ags_file_test_find_id_ref_by_xpath()
{
  //TODO:JK: implement me
}

void
ags_file_test_find_id_ref_by_reference()
{
  AgsFile *file;
  AgsFileIdRef *file_id_ref[3], *current;

  AgsTurtle *turtle[3];

  file = g_object_new(AGS_TYPE_FILE,
		      NULL);
  
  /* add some id refs */
  turtle[0] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);
  
  file_id_ref[0] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"reference", turtle[0],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[0]);

  turtle[1] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);

  file_id_ref[1] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"reference", turtle[1],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[1]);

  turtle[2] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);

  file_id_ref[2] = g_object_new(AGS_TYPE_FILE_ID_REF,
				"reference", turtle[2],
				NULL);
  ags_file_add_id_ref(file,
		      file_id_ref[2]);  

 /* assert find by reference */
  CU_ASSERT((current = ags_file_find_id_ref_by_reference(file,
							 turtle[0])) != NULL &&
	    current == file_id_ref[0]);

  CU_ASSERT((current = ags_file_find_id_ref_by_reference(file,
							 turtle[1])) != NULL &&
	    current == file_id_ref[1]);

  CU_ASSERT((current = ags_file_find_id_ref_by_reference(file,
							 turtle[2])) != NULL &&
	    current == file_id_ref[2]);
}

void
ags_file_test_add_lookup()
{
  AgsFile *file;
  AgsFileIdRef *file_lookup[3];
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* add some id refs */
  file_lookup[0] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				NULL);
  ags_file_add_lookup(file,
		      file_lookup[0]);

  file_lookup[1] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				NULL);
  ags_file_add_lookup(file,
		      file_lookup[1]);

  file_lookup[2] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				NULL);
  ags_file_add_lookup(file,
		      file_lookup[2]);

  /* assert to be present */
  CU_ASSERT(g_list_find(file->lookup,
			file_lookup[0]) != NULL);

  CU_ASSERT(g_list_find(file->lookup,
			file_lookup[1]) != NULL);

  CU_ASSERT(g_list_find(file->lookup,
			file_lookup[2]) != NULL);
}

void
ags_file_test_add_launch()
{
  AgsFile *file;
  AgsFileIdRef *file_launch[3];
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* add some id refs */
  file_launch[0] = g_object_new(AGS_TYPE_FILE_LAUNCH,
				NULL);
  ags_file_add_launch(file,
		      file_launch[0]);

  file_launch[1] = g_object_new(AGS_TYPE_FILE_LAUNCH,
				NULL);
  ags_file_add_launch(file,
		      file_launch[1]);

  file_launch[2] = g_object_new(AGS_TYPE_FILE_LAUNCH,
				NULL);
  ags_file_add_launch(file,
		      file_launch[2]);

  /* assert to be present */
  CU_ASSERT(g_list_find(file->launch,
			file_launch[0]) != NULL);

  CU_ASSERT(g_list_find(file->launch,
			file_launch[1]) != NULL);

  CU_ASSERT(g_list_find(file->launch,
			file_launch[2]) != NULL);
}

void
ags_file_test_open()
{
  AgsFile *file;

  gpointer ptr;

  GError *error;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub open */
  ptr = AGS_FILE_GET_CLASS(file)->open;
  
  AGS_FILE_GET_CLASS(file)->open = ags_file_test_stub_open;

  /* assert open */
  error = NULL;
  ags_file_open(file,
		&error);

  CU_ASSERT(stub_open == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->open = ptr;
}

void
ags_file_test_open_from_data()
{
  AgsFile *file;

  gpointer ptr;

  GError *error;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub open from data */
  ptr = AGS_FILE_GET_CLASS(file)->open_from_data;
  
  AGS_FILE_GET_CLASS(file)->open_from_data = ags_file_test_stub_open_from_data;

  /* assert open from data */
  error = NULL;
  ags_file_open_from_data(file,
			  NULL, 0,
			  &error);

  CU_ASSERT(stub_open_from_data == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->open_from_data = ptr;
}

void
ags_file_test_rw_open()
{
  AgsFile *file;

  gpointer ptr;

  GError *error;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub read/write open */
  ptr = AGS_FILE_GET_CLASS(file)->rw_open;
  
  AGS_FILE_GET_CLASS(file)->rw_open = ags_file_test_stub_rw_open;

  /* assert read/write open */
  error = NULL;
  ags_file_rw_open(file,
		   FALSE,
		   &error);

  CU_ASSERT(stub_rw_open == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->rw_open = ptr;
}

void
ags_file_test_open_filename()
{
  //TODO:JK: implement me
}

void
ags_file_test_close()
{
  //TODO:JK: implement me
}

void
ags_file_test_write()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub write */
  ptr = AGS_FILE_GET_CLASS(file)->write;
  
  AGS_FILE_GET_CLASS(file)->write = ags_file_test_stub_write;

  /* assert write */
  ags_file_write(file);

  CU_ASSERT(stub_write == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->write = ptr;
}

void
ags_file_test_write_concurrent()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub write concurrent */
  ptr = AGS_FILE_GET_CLASS(file)->write_concurrent;
  
  AGS_FILE_GET_CLASS(file)->write_concurrent = ags_file_test_stub_write_concurrent;

  /* assert write concurrent */
  ags_file_write_concurrent(file);

  CU_ASSERT(stub_write_concurrent == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->write_concurrent = ptr;
}

void
ags_file_test_write_resolve()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub write resolve */
  ptr = AGS_FILE_GET_CLASS(file)->write_resolve;
  
  AGS_FILE_GET_CLASS(file)->write_resolve = ags_file_test_stub_write_resolve;

  /* assert write resolve */
  ags_file_write_resolve(file);

  CU_ASSERT(stub_write_resolve == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->write_resolve = ptr;
}

void
ags_file_test_read()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub read */
  ptr = AGS_FILE_GET_CLASS(file)->read;
  
  AGS_FILE_GET_CLASS(file)->read = ags_file_test_stub_read;

  /* assert read */
  ags_file_read(file);

  CU_ASSERT(stub_read == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->read = ptr;
}

void
ags_file_test_read_resolve()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub read resolve */
  ptr = AGS_FILE_GET_CLASS(file)->read_resolve;
  
  AGS_FILE_GET_CLASS(file)->read_resolve = ags_file_test_stub_read_resolve;

  /* assert read resolve */
  ags_file_read_resolve(file);

  CU_ASSERT(stub_read_resolve == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->read_resolve = ptr;
}

void
ags_file_test_read_start()
{
  AgsFile *file;

  gpointer ptr;
  
  file = g_object_new(AGS_TYPE_FILE,
		      NULL);

  /* stub read start */
  ptr = AGS_FILE_GET_CLASS(file)->read_start;
  
  AGS_FILE_GET_CLASS(file)->read_start = ags_file_test_stub_read_start;

  /* assert read start */
  ags_file_read_start(file);

  CU_ASSERT(stub_read_start == TRUE);
  
  /* reset */
  AGS_FILE_GET_CLASS(file)->read_start = ptr;
}

void
ags_file_test_read_config()
{
  //TODO:JK: implement me
}

void
ags_file_test_write_config()
{
  //TODO:JK: implement me
}

void
ags_file_test_read_application_context()
{
  //TODO:JK: implement me
}

void
ags_file_test_write_application_context()
{
  //TODO:JK: implement me
}

void
ags_file_test_stub_open(AgsFile *file,
			GError **error)
{
  stub_open = TRUE;
}

void
ags_file_test_stub_open_from_data(AgsFile *file,
				  gchar *data, guint length,
				  GError **error)
{
  stub_open_from_data = TRUE;
}

void
ags_file_test_stub_rw_open(AgsFile *file,
			   gboolean create,
			   GError **error)
{
  stub_rw_open = TRUE;
}

void
ags_file_test_stub_write(AgsFile *file)
{
  stub_write = TRUE;
}

void
ags_file_test_stub_write_concurrent(AgsFile *file)
{
  stub_write_concurrent = TRUE;
}

void
ags_file_test_stub_write_resolve(AgsFile *file)
{
  stub_write_resolve = TRUE;
}

void
ags_file_test_stub_read(AgsFile *file)
{
  stub_read = TRUE;
}

void
ags_file_test_stub_read_resolve(AgsFile *file)
{
  stub_read_resolve = TRUE;
}

void
ags_file_test_stub_read_start(AgsFile *file)
{
  stub_read_start = TRUE;
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
  pSuite = CU_add_suite("AgsFileTest\0", ags_file_test_init_suite, ags_file_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFile string to md5 sum\0", ags_file_test_str2md5) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile add id ref\0", ags_file_test_add_id_ref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile find id ref by node\0", ags_file_test_find_id_ref_by_node) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile find id ref by xpath\0", ags_file_test_find_id_ref_by_xpath) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile find id ref by reference\0", ags_file_test_find_id_ref_by_reference) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile add lookup\0", ags_file_test_add_lookup) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile add launch\0", ags_file_test_add_launch) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile open\0", ags_file_test_open) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile open from data\0", ags_file_test_open_from_data) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile rw open\0", ags_file_test_rw_open) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile open filename\0", ags_file_test_open_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile close\0", ags_file_test_close) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile write\0", ags_file_test_write) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile write concurrent\0", ags_file_test_write_concurrent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile write resolve\0", ags_file_test_write_resolve) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile read\0", ags_file_test_read) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile read resolve\0", ags_file_test_read_resolve) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile read start\0", ags_file_test_read_start) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile read config\0", ags_file_test_read_config) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile write config\0", ags_file_test_write_config) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile read application context\0", ags_file_test_read_application_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFile write application context\0", ags_file_test_write_application_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
