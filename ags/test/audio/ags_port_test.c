/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <libags.h>
#include <libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_port_test_init_suite();
int ags_port_test_clean_suite();

void ags_port_test_safe_read();
void ags_port_test_safe_write();
void ags_port_test_safe_write_raw();
void ags_port_test_safe_get_property();
void ags_port_test_safe_set_property();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_port_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_port_test_clean_suite()
{  
  return(0);
}

void
ags_port_test_safe_read()
{
  AgsPort *port;

  GValue *value;

  /*
   * check boolean
   */
  port = ags_port_new();

  port->port_value_is_pointer = FALSE;
  port->port_value_type = G_TYPE_BOOLEAN;

  port->port_value_size = sizeof(gboolean);
  port->port_value_length = 1;
  
  /* assert false */
  port->port_value.ags_port_boolean = FALSE;

  value = g_new0(GValue,
		 1);
  
  g_value_init(value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_boolean(value) == FALSE);

  /* assert true */
  port->port_value.ags_port_boolean = TRUE;

  g_value_reset(value);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_boolean(value) == TRUE);

  /*
   * check unsigned int 64
   */
  port = ags_port_new();

  port->port_value_is_pointer = FALSE;
  port->port_value_type = G_TYPE_UINT64;

  port->port_value_size = sizeof(guint64);
  port->port_value_length = 1;
  
  /* assert 0 */
  port->port_value.ags_port_uint = 0;

  value = g_new0(GValue,
		 1);
  
  g_value_init(value,
	       G_TYPE_UINT64);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_uint64(value) == 0);

  /* assert max unsigned int 64 */
  port->port_value.ags_port_uint = G_MAXUINT64;

  g_value_reset(value);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_uint64(value) == G_MAXUINT64);

  /*
   * check double
   */
  port = ags_port_new();

  port->port_value_is_pointer = FALSE;
  port->port_value_type = G_TYPE_DOUBLE;

  port->port_value_size = sizeof(gdouble);
  port->port_value_length = 1;
  
  /* assert 0.0 */
  port->port_value.ags_port_double = 0.0;

  value = g_new0(GValue,
		 1);
  
  g_value_init(value,
	       G_TYPE_DOUBLE);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_double(value) == 0.0);

  /* assert 1.0 */
  port->port_value.ags_port_double = 1.0;

  g_value_reset(value);
  ags_port_safe_read(port,
		     value);

  CU_ASSERT(g_value_get_double(value) == 1.0);
}

void
ags_port_test_safe_write()
{
}

void
ags_port_test_safe_write_raw()
{
}

void
ags_port_test_safe_get_property()
{
}

void
ags_port_test_safe_set_property()
{
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsPortTest\0", ags_port_test_init_suite, ags_port_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPort safe read\0", ags_port_test_safe_read) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPort safe write\0", ags_port_test_safe_write) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPort safe write raw\0", ags_port_test_safe_write_raw) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPort safe get property\0", ags_port_test_safe_get_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPort safe set property\0", ags_port_test_safe_set_property) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

