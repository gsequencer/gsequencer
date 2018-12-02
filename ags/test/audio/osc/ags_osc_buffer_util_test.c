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

#include <stdlib.h>

int ags_osc_buffer_util_test_init_suite();
int ags_osc_buffer_util_test_clean_suite();

void ags_osc_buffer_util_test_put_int32();
void ags_osc_buffer_util_test_get_int32();
void ags_osc_buffer_util_test_put_timetag();
void ags_osc_buffer_util_test_get_timetag();
void ags_osc_buffer_util_test_put_float();
void ags_osc_buffer_util_test_get_float();
void ags_osc_buffer_util_test_put_string();
void ags_osc_buffer_util_test_get_string();
void ags_osc_buffer_util_test_put_blob();
void ags_osc_buffer_util_test_get_blob();
void ags_osc_buffer_util_test_put_int64();
void ags_osc_buffer_util_test_get_int64();
void ags_osc_buffer_util_test_put_double();
void ags_osc_buffer_util_test_get_double();
void ags_osc_buffer_util_test_put_char();
void ags_osc_buffer_util_test_get_char();
void ags_osc_buffer_util_test_put_rgba();
void ags_osc_buffer_util_test_get_rgba();
void ags_osc_buffer_util_test_put_midi();
void ags_osc_buffer_util_test_get_midi();
void ags_osc_buffer_util_test_put_packet();
void ags_osc_buffer_util_test_get_packet();
void ags_osc_buffer_util_test_put_packets();
void ags_osc_buffer_util_test_put_message();
void ags_osc_buffer_util_test_get_message();
void ags_osc_buffer_util_test_put_bundle();
void ags_osc_buffer_util_test_get_bundle();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_buffer_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_osc_buffer_util_test_put_int32()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;
  
  static const gint32 val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00",
    "\x00\x00\x00\x01",
    "\x00\x00\x00\xff",
    "\x00\x00\x01\x00",
    "\x00\x00\xff\xff",
    "\x00\x01\x00\x00",
    "\x00\xff\xff\xff",
    "\x01\x00\x00\x00",
    "\xff\xff\xff\xff",
  };

  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_put_int32(buffer,
				   val[i]);
    
    for(j = 0; j < 4; j++){
      if(buffer[j] != val_buffer[i][j]){
	success = FALSE;

	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_int32()
{
  gint32 current;
  guint i;
  gboolean success;
    
  static const gint32 val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00",
    "\x00\x00\x00\x01",
    "\x00\x00\x00\xff",
    "\x00\x00\x01\x00",
    "\x00\x00\xff\xff",
    "\x00\x01\x00\x00",
    "\x00\xff\xff\xff",
    "\x01\x00\x00\x00",
    "\xff\xff\xff\xff",
  };

  success = TRUE;

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_get_int32(val_buffer[i],
				   &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_timetag()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;

  static const gint32 val_tv_secs[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,    
  };

  static const gint32 val_tv_fraction[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,    
  };

  static const gint32 val_immediately[] = {
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
  };
  
  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00\x00\x00\x00\x01",
    "\x00\x00\x00\x01\x00\x00\x00\x00",
    "\x00\x00\x00\xff\x00\x00\x00\xfe",
    "\x00\x00\x01\x00\x00\x00\x01\x01",
    "\x00\x00\xff\xff\x00\x00\xff\xfe",
    "\x00\x01\x00\x00\x00\x01\x00\x01",
    "\x00\xff\xff\xff\x00\xff\xff\xfe",
    "\x01\x00\x00\x00\x01\x00\x00\x01",
    "\xff\xff\xff\xff\xff\xff\xff\xfe",
  };

  success = TRUE;

  buffer = (unsigned char *) malloc(8 * sizeof(unsigned char));

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_put_timetag(buffer,
				    val_tv_secs[i], val_tv_fraction[i], val_immediately[i]);
    
    for(j = 0; j < 8; j++){
      if(buffer[j] != val_buffer[i][j]){
	success = FALSE;

	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_timetag()
{
  gint32 current_tv_secs, current_tv_fraction;
  gboolean current_immediately;
  guint i;
  gboolean success;

  static const gint32 val_tv_secs[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,    
  };

  static const gint32 val_tv_fraction[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,    
  };

  static const gint32 val_immediately[] = {
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    FALSE,
  };
  
  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00\x00\x00\x00\x01",
    "\x00\x00\x00\x01\x00\x00\x00\x00",
    "\x00\x00\x00\xff\x00\x00\x00\xfe",
    "\x00\x00\x01\x00\x00\x00\x01\x01",
    "\x00\x00\xff\xff\x00\x00\xff\xfe",
    "\x00\x01\x00\x00\x00\x01\x00\x01",
    "\x00\xff\xff\xff\x00\xff\xff\xfe",
    "\x01\x00\x00\x00\x01\x00\x00\x01",
    "\xff\xff\xff\xff\xff\xff\xff\xfe",
  };

  success = TRUE;

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_get_timetag(val_buffer[i],
				    &current_tv_secs, &current_tv_fraction, &current_immediately);

    if(current_tv_secs != val_tv_secs[i] ||
       current_tv_fraction != (val_tv_fraction[i] & (~0x1)) ||
       current_immediately != val_immediately[i]){
      success = FALSE;

      g_message("%d", i);
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_float()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_float()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_string()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_string()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_blob()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_blob()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_int64()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_int64()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_double()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_double()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_char()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_char()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_rgba()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_rgba()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_midi()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_midi()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_packet()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_packet()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_packets()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_message()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_message()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_put_bundle()
{
  //TODO:JK: implement me
}

void
ags_osc_buffer_util_test_get_bundle()
{
  //TODO:JK: implement me
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

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsOscBufferUtilTest", ags_osc_buffer_util_test_init_suite, ags_osc_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_osc_buffer_util.c put int32", ags_osc_buffer_util_test_put_int32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get int32", ags_osc_buffer_util_test_get_int32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put timetag", ags_osc_buffer_util_test_put_timetag) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get timetag", ags_osc_buffer_util_test_get_timetag) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put float", ags_osc_buffer_util_test_put_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get float", ags_osc_buffer_util_test_get_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put string", ags_osc_buffer_util_test_put_string) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get string", ags_osc_buffer_util_test_get_string) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put blob", ags_osc_buffer_util_test_put_blob) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get blob", ags_osc_buffer_util_test_get_blob) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put int64", ags_osc_buffer_util_test_put_int64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get int64", ags_osc_buffer_util_test_get_int64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put double", ags_osc_buffer_util_test_put_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get double", ags_osc_buffer_util_test_get_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put char", ags_osc_buffer_util_test_put_char) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get char", ags_osc_buffer_util_test_get_char) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put rgba", ags_osc_buffer_util_test_put_rgba) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get rgba", ags_osc_buffer_util_test_get_rgba) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put midi", ags_osc_buffer_util_test_put_midi) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get midi", ags_osc_buffer_util_test_get_midi) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put packet", ags_osc_buffer_util_test_put_packet) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get packet", ags_osc_buffer_util_test_get_packet) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put packets", ags_osc_buffer_util_test_put_packets) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put message", ags_osc_buffer_util_test_put_message) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get message", ags_osc_buffer_util_test_get_message) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c put bundle", ags_osc_buffer_util_test_put_bundle) == NULL) ||
     (CU_add_test(pSuite, "test of ags_osc_buffer_util.c get bundle", ags_osc_buffer_util_test_get_bundle) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


