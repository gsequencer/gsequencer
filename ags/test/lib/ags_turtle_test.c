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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_turtle_test_init_suite();
int ags_turtle_test_clean_suite();

void ags_turtle_test_read_iriref();
void ags_turtle_test_read_pname_ns();
void ags_turtle_test_read_pname_ln();
void ags_turtle_test_read_blank_node_label();
void ags_turtle_test_read_langtag();
void ags_turtle_test_read_boolean();
void ags_turtle_test_read_integer();
void ags_turtle_test_read_decimal();
void ags_turtle_test_read_double();
void ags_turtle_test_read_exponent();
void ags_turtle_test_read_string();
void ags_turtle_test_read_string_literal_quote();
void ags_turtle_test_read_string_literal_single_quote();
void ags_turtle_test_read_string_literal_long_quote();
void ags_turtle_test_read_string_literal_long_single_quote();
void ags_turtle_test_read_uchar();
void ags_turtle_test_read_echar();
void ags_turtle_test_read_ws();
void ags_turtle_test_read_anon();
void ags_turtle_test_read_pn_chars_base();
void ags_turtle_test_read_pn_chars_u();
void ags_turtle_test_read_pn_chars();
void ags_turtle_test_read_pn_prefix();
void ags_turtle_test_read_pn_local();
void ags_turtle_test_read_plx();
void ags_turtle_test_read_percent();
void ags_turtle_test_read_hex();
void ags_turtle_test_read_pn_local_esc();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_test_clean_suite()
{
  return(0);
}

void
ags_turtle_test_read_iriref()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pname_ns()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pname_ln()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_blank_node_label()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_langtag()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_boolean()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_integer()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_decimal()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_double()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_exponent()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_string()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_string_literal_quote()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_string_literal_single_quote()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_string_literal_long_quote()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_string_literal_long_single_quote()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_uchar()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_echar()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_ws()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_anon()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_chars_base()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_chars_u()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_chars()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_prefix()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_local()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_plx()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_percent()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_hex()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_local_esc()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsTurtleTest\0", ags_turtle_test_init_suite, ags_turtle_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTurtle read iriref\0", ags_turtle_test_read_iriref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read prefixed name namespace\0", ags_turtle_test_read_pname_ns) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read prefixed name localized name\0", ags_turtle_test_read_prefixed_name_localized_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read blank node label\0", ags_turtle_test_read_blank_node_label) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read language tag\0", ags_turtle_test_read_lang_tag) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read boolean\0", ags_turtle_test_read_boolean) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read integer\0", ags_turtle_test_read_integer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read decimal\0", ags_turtle_test_read_decimal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read double\0", ags_turtle_test_read_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read exponent\0", ags_turtle_test_read_exponent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string\0", ags_turtle_test_read_string) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal quote\0", ags_turtle_test_read_string_literal_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal single quote\0", ags_turtle_test_read_string_literal_single_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal long quote\0", ags_turtle_test_read_string_literal_long_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal long single quote\0", ags_turtle_test_read_string_literal_long_single_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read uchar\0", ags_turtle_test_read_uchar) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read echar\0", ags_turtle_test_read_echar) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read ws\0", ags_turtle_test_read_ws) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read anon\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-chars u\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-chars\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-prefix\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-local\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read plx\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read percent\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read hex\0", ags_turtle_test_read_) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-local esc\0", ags_turtle_test_read_) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
