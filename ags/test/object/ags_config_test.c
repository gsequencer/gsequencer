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

int ags_config_test_init_suite();
int ags_config_test_clean_suite();

void ags_config_test_load_defaults();
void ags_config_test_load_from_file();
void ags_config_test_load_from_data();
void ags_config_test_set_value();
void ags_config_test_get_value();
void ags_config_test_to_data();
void ags_config_test_save();
void ags_config_test_clear();

void ags_config_test_stub_load_defaults();
void ags_config_test_stub_set_value();
gchar* ags_config_test_stub_get_value();

#define AGS_CONFIG_TEST_SAMPLE_CONF "ags_config_test_sample.conf"
#define AGS_CONFIG_TEST_SAMPLE_CONF_DATA "[the-group]\n" \
  "subject=very first value\n\n"

gboolean stub_load_defaults = FALSE;
gboolean stub_set_value = FALSE;
gboolean stub_get_value = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_config_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_config_test_clean_suite()
{
  return(0);
}

void
ags_config_test_load_defaults()
{
  AgsConfig *config;

  gpointer ptr;
  
  config = ags_config_get_instance();

  /* stub */
  ptr = AGS_CONFIG_GET_CLASS(config)->load_defaults;
  AGS_CONFIG_GET_CLASS(config)->load_defaults = ags_config_test_stub_load_defaults;

  ags_config_load_defaults(config);

  CU_ASSERT(stub_load_defaults == TRUE);
  AGS_CONFIG_GET_CLASS(config)->load_defaults = ptr;
  
  g_object_unref(config);
}

void
ags_config_test_load_from_file()
{
  AgsConfig *config;

  /* test not existing file */
  config = ags_config_get_instance();

  ags_config_load_from_file(config,
			    NULL);

  CU_ASSERT(config->key_file != NULL);

  g_object_unref(config);
  
  /* test sample configuration */
  config = ags_config_get_instance();

  ags_config_load_from_file(config,
			    AGS_CONFIG_TEST_SAMPLE_CONF);

  CU_ASSERT(config->key_file != NULL);
  
  g_object_unref(config);
}

void
ags_config_test_load_from_data()
{
  AgsConfig *config;

  /* test NULL */
  config = ags_config_get_instance();

  ags_config_load_from_data(config,
			    NULL, 0);

  CU_ASSERT(config->key_file != NULL);

  g_object_unref(config);
  
  /* test sample configuration */
  config = ags_config_get_instance();

  ags_config_load_from_data(config,
			    AGS_CONFIG_TEST_SAMPLE_CONF_DATA, sizeof(AGS_CONFIG_TEST_SAMPLE_CONF_DATA));

  CU_ASSERT(config->key_file != NULL);
  
  g_object_unref(config);
}

void
ags_config_test_set_value()
{
  AgsConfig *config;

  gpointer ptr;
  
  config = ags_config_get_instance();

  /* stub */
  ptr = AGS_CONFIG_GET_CLASS(config)->set_value;
  AGS_CONFIG_GET_CLASS(config)->set_value = ags_config_test_stub_set_value;

  ags_config_set_value(config,
		       NULL, NULL, NULL);

  CU_ASSERT(stub_set_value == TRUE);

  AGS_CONFIG_GET_CLASS(config)->set_value = ptr;
  
  g_object_unref(config);
}

void
ags_config_test_get_value()
{
  AgsConfig *config;

  gpointer ptr;
  
  config = ags_config_get_instance();

  /* stub */
  ptr = AGS_CONFIG_GET_CLASS(config)->get_value;
  AGS_CONFIG_GET_CLASS(config)->get_value = ags_config_test_stub_get_value;
  
  ags_config_get_value(config,
		       NULL, NULL);
   
  CU_ASSERT(stub_get_value == TRUE);

  AGS_CONFIG_GET_CLASS(config)->get_value = ptr;
  
  g_object_unref(config);
}

void
ags_config_test_to_data()
{
  AgsConfig *config;

  char *buffer;
  
  gsize buffer_length;
  
  /* test NULL */
  config = ags_config_get_instance();

  ags_config_to_data(config,
		     NULL, 0);

  g_object_unref(config);
  
  /* test default configuration */
  config = ags_config_get_instance();
  ags_config_load_defaults(config);
  g_message("%s", g_key_file_to_data(config->key_file, NULL, NULL));
  
  buffer = NULL;
  buffer_length = 0;
  
  ags_config_to_data(config,
		     &buffer, &buffer_length);
  
  CU_ASSERT(buffer != NULL);
  CU_ASSERT(buffer_length > 0);
  
  g_object_unref(config);
}

void
ags_config_test_save()
{
  //TODO:JK: implement me
}

void
ags_config_test_clear()
{
  //TODO:JK: implement me
}

void
ags_config_test_stub_load_defaults()
{
  stub_load_defaults = TRUE;
}

void
ags_config_test_stub_set_value()
{
  stub_set_value = TRUE;
}

gchar*
ags_config_test_stub_get_value()
{
  stub_get_value = TRUE;

  return(NULL);
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
  pSuite = CU_add_suite("AgsConfigTest\0", ags_config_test_init_suite, ags_config_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConfig load defaults\0", ags_config_test_load_defaults) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig load from file\0", ags_config_test_load_from_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig load from data\0", ags_config_test_load_from_data) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig set value\0", ags_config_test_set_value) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig get value\0", ags_config_test_get_value) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig to data\0", ags_config_test_to_data) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig save\0", ags_config_test_save) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConfig clear\0", ags_config_test_clear) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
