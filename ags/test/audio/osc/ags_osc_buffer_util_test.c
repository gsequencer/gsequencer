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
#include <string.h>

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
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_float()
{
  unsigned char *buffer;

  gfloat current;
  
  guint i, j;
  gboolean success;

  static const gfloat val[] = {
    0.0,
    1.0,
    4.125,
    5.25,
    7.8,
    9.87,
    16.99,
    23.5,
    31.75,
  };

  success = TRUE;

  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_put_float(buffer,
				  val[i]);
    
    ags_osc_buffer_util_get_float(buffer,
				  &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_float()
{
  //NOTE:JK: see above
}

void
ags_osc_buffer_util_test_put_string()
{
  unsigned char *buffer;
  static const gchar *str = "I bytes OSC";

  guint i;

  buffer = (unsigned char *) malloc(16 * sizeof(unsigned char));
  
  ags_osc_buffer_util_put_string(buffer,
				 str, -1);

  CU_ASSERT(!g_strcmp0(buffer, str));
}

void
ags_osc_buffer_util_test_get_string()
{
  gchar *str;
  
  static const unsigned char *str_buffer = "Every bytes OSC\0";

  str = NULL;
  
  ags_osc_buffer_util_get_string(str_buffer,
				 &str, NULL);

  CU_ASSERT(!g_strcmp0(str_buffer, str));
}

void
ags_osc_buffer_util_test_put_blob()
{
  unsigned char *buffer;

  guint i;
  gboolean success;
  
  static const unsigned char *blob = "\x00\x00\x00\x00\x00\x00\x00\x01\x0e\x00\x00\x00\x00\x00\x00\x00";
  static const unsigned char *blob_length_data = "\x00\x00\x00\x10";
  
  buffer = (unsigned char *) malloc(32 * sizeof(unsigned char));

  ags_osc_buffer_util_put_blob(buffer,
			       16, blob);

  success = TRUE;

  for(i = 0; i < 4; i++){
    if(blob_length_data[i] != buffer[i]){
      success = FALSE;

      break;
    }
  }
  
  for(i = 0; i < 16; i++){
    if(blob[i] != buffer[i + 4]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_osc_buffer_util_test_get_blob()
{
  unsigned char *data;

  gint32 data_size;
  guint i;
  gboolean success;
  
  static const unsigned char *buffer = "\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x01\x0e\x00\x00\x00\x00\x00\x00\x00";

  ags_osc_buffer_util_get_blob(buffer,
			       &data_size, &data);

  CU_ASSERT(data_size == 16);
  
  success = TRUE;
  
  for(i = 0; i < 16; i++){
    if(data[i] != buffer[i + 4]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_osc_buffer_util_test_put_int64()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;
  
  static const gint64 val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,
    9223372036854775807,
    18446744073709551615,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00\x00\x00\x00\x00",
    "\x00\x00\x00\x00\x00\x00\x00\x01",
    "\x00\x00\x00\x00\x00\x00\x00\xff",
    "\x00\x00\x00\x00\x00\x00\x01\x00",
    "\x00\x00\x00\x00\x00\x00\xff\xff",
    "\x00\x00\x00\x00\x00\x01\x00\x00",
    "\x00\x00\x00\x00\x00\xff\xff\xff",
    "\x00\x00\x00\x00\x01\x00\x00\x00",
    "\x00\x00\x00\x00\xff\xff\xff\xff",
    "\x7f\xff\xff\xff\xff\xff\xff\xff",
    "\xff\xff\xff\xff\xff\xff\xff\xff",
  };

  buffer = (unsigned char *) malloc(8 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 11 && success; i++){
    ags_osc_buffer_util_put_int64(buffer,
				  val[i]);
    
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
ags_osc_buffer_util_test_get_int64()
{
  gint64 current;
  guint i;
  gboolean success;

  static const gint64 val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
    16777216,
    4294967295,
    9223372036854775807,
    18446744073709551615,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x00\x00\x00\x00\x00",
    "\x00\x00\x00\x00\x00\x00\x00\x01",
    "\x00\x00\x00\x00\x00\x00\x00\xff",
    "\x00\x00\x00\x00\x00\x00\x01\x00",
    "\x00\x00\x00\x00\x00\x00\xff\xff",
    "\x00\x00\x00\x00\x00\x01\x00\x00",
    "\x00\x00\x00\x00\x00\xff\xff\xff",
    "\x00\x00\x00\x00\x01\x00\x00\x00",
    "\x00\x00\x00\x00\xff\xff\xff\xff",
    "\x7f\xff\xff\xff\xff\xff\xff\xff",
    "\xff\xff\xff\xff\xff\xff\xff\xff",
  };

  success = TRUE;

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_get_int64(val_buffer[i],
				  &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_double()
{
  unsigned char *buffer;

  gdouble current;
  
  guint i;
  gboolean success;

  static const gdouble val[] = {
    0.0,
    1.0,
    4.125,
    5.25,
    7.8,
    9.87,
    16.99,
    23.5,
    31.75,
  };

  success = TRUE;

  buffer = (unsigned char *) malloc(8 * sizeof(unsigned char));

  for(i = 0; i < 9 && success; i++){
    ags_osc_buffer_util_put_double(buffer,
				   val[i]);
    
    ags_osc_buffer_util_get_double(buffer,
				   &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_double()
{
  //NOTE:JK: see above
}

void
ags_osc_buffer_util_test_put_char()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;
  
  static const gchar *str = "abcdefghijklmnopqrstuvwxyz0123456789 ";
  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x61",
    "\x00\x00\x00\x62",
    "\x00\x00\x00\x63",
    "\x00\x00\x00\x64",
    "\x00\x00\x00\x65",
    "\x00\x00\x00\x66",
    "\x00\x00\x00\x67",
    "\x00\x00\x00\x68",
    "\x00\x00\x00\x69",
    "\x00\x00\x00\x6a",
    "\x00\x00\x00\x6b",
    "\x00\x00\x00\x6c",
    "\x00\x00\x00\x6d",
    "\x00\x00\x00\x6e",
    "\x00\x00\x00\x6f",
    "\x00\x00\x00\x70",
    "\x00\x00\x00\x71",
    "\x00\x00\x00\x72",
    "\x00\x00\x00\x73",
    "\x00\x00\x00\x74",
    "\x00\x00\x00\x75",
    "\x00\x00\x00\x76",
    "\x00\x00\x00\x77",
    "\x00\x00\x00\x78",
    "\x00\x00\x00\x79",
    "\x00\x00\x00\x7a",
    "\x00\x00\x00\x30",
    "\x00\x00\x00\x31",
    "\x00\x00\x00\x32",
    "\x00\x00\x00\x33",
    "\x00\x00\x00\x34",
    "\x00\x00\x00\x35",
    "\x00\x00\x00\x36",
    "\x00\x00\x00\x37",
    "\x00\x00\x00\x38",
    "\x00\x00\x00\x39",
    "\x00\x00\x00\x20",
  };
    
  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  success = TRUE;

  for(i = 0; i < 37 && success; i++){
    ags_osc_buffer_util_put_char(buffer,
				 str[i]);
    
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
ags_osc_buffer_util_test_get_char()
{
  gchar current;
  guint i;
  gboolean success;
  
  static const gchar *str = "abcdefghijklmnopqrstuvwxyz0123456789 ";
  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00\x61",
    "\x00\x00\x00\x62",
    "\x00\x00\x00\x63",
    "\x00\x00\x00\x64",
    "\x00\x00\x00\x65",
    "\x00\x00\x00\x66",
    "\x00\x00\x00\x67",
    "\x00\x00\x00\x68",
    "\x00\x00\x00\x69",
    "\x00\x00\x00\x6a",
    "\x00\x00\x00\x6b",
    "\x00\x00\x00\x6c",
    "\x00\x00\x00\x6d",
    "\x00\x00\x00\x6e",
    "\x00\x00\x00\x6f",
    "\x00\x00\x00\x70",
    "\x00\x00\x00\x71",
    "\x00\x00\x00\x72",
    "\x00\x00\x00\x73",
    "\x00\x00\x00\x74",
    "\x00\x00\x00\x75",
    "\x00\x00\x00\x76",
    "\x00\x00\x00\x77",
    "\x00\x00\x00\x78",
    "\x00\x00\x00\x79",
    "\x00\x00\x00\x7a",
    "\x00\x00\x00\x30",
    "\x00\x00\x00\x31",
    "\x00\x00\x00\x32",
    "\x00\x00\x00\x33",
    "\x00\x00\x00\x34",
    "\x00\x00\x00\x35",
    "\x00\x00\x00\x36",
    "\x00\x00\x00\x37",
    "\x00\x00\x00\x38",
    "\x00\x00\x00\x39",
    "\x00\x00\x00\x20",
  };

  success = TRUE;

  for(i = 0; i < 37 && success; i++){
    ags_osc_buffer_util_get_char(val_buffer[i],
				 &current);

    if(current != str[i]){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_rgba()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;
  
  static const guint8 rgba[14][4] = {
    {0x0, 0x0, 0x0, 0xff},
    {0x0, 0x0, 0xff, 0xff},
    {0x0, 0xff, 0x0, 0xff},
    {0xff, 0x0, 0x0, 0xff},
    {0x0, 0xff, 0xff, 0xff},
    {0xff, 0x0, 0xff, 0xff},
    {0xff, 0xff, 0x0, 0xff},
    {0xff, 0xff, 0xff, 0xff},
    {0x0, 0x0, 0x0, 0x7f},
    {0x0, 0x0, 0xff, 0x7f},
    {0x0, 0xff, 0x0, 0x7f},
    {0xff, 0x0, 0x0, 0x7f},
    {0x0, 0xff, 0xff, 0x7f},
    {0xff, 0x0, 0xff, 0x7f},
    {0xff, 0xff, 0x0, 0x7f},
    {0xff, 0xff, 0xff, 0x7f},
  };

  static const unsigned char *val_rgba[] = {
    "\x00\x00\x00\xff",
    "\x00\x00\xff\xff",
    "\x00\xff\x00\xff",
    "\xff\x00\x00\xff",
    "\x00\xff\xff\xff",
    "\xff\x00\xff\xff",
    "\xff\xff\x00\xff",
    "\xff\xff\xff\xff",
    "\x00\x00\x00\x7f",
    "\x00\x00\xff\x7f",
    "\x00\xff\x00\x7f",
    "\xff\x00\x00\x7f",
    "\x00\xff\xff\x7f",
    "\xff\x00\xff\x7f",
    "\xff\xff\x00\x7f",
    "\xff\xff\xff\x7f",
  };
  
  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  success = TRUE;

  for(i = 0; i < 14 && success; i++){
    ags_osc_buffer_util_put_rgba(buffer,
				 rgba[i][0], rgba[i][1], rgba[i][2], rgba[i][3]);

    for(j = 0; j < 4; j++){
      if(buffer[j] != val_rgba[i][j]){
	success = FALSE;
	
	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_rgba()
{
  guint8 r, g, b, a;
  guint i;
  gboolean success;
  
  static const guint8 rgba[14][4] = {
    {0x0, 0x0, 0x0, 0xff},
    {0x0, 0x0, 0xff, 0xff},
    {0x0, 0xff, 0x0, 0xff},
    {0xff, 0x0, 0x0, 0xff},
    {0x0, 0xff, 0xff, 0xff},
    {0xff, 0x0, 0xff, 0xff},
    {0xff, 0xff, 0x0, 0xff},
    {0xff, 0xff, 0xff, 0xff},
    {0x0, 0x0, 0x0, 0x7f},
    {0x0, 0x0, 0xff, 0x7f},
    {0x0, 0xff, 0x0, 0x7f},
    {0xff, 0x0, 0x0, 0x7f},
    {0x0, 0xff, 0xff, 0x7f},
    {0xff, 0x0, 0xff, 0x7f},
    {0xff, 0xff, 0x0, 0x7f},
    {0xff, 0xff, 0xff, 0x7f},
  };

  static const unsigned char *val_rgba[] = {
    "\x00\x00\x00\xff",
    "\x00\x00\xff\xff",
    "\x00\xff\x00\xff",
    "\xff\x00\x00\xff",
    "\x00\xff\xff\xff",
    "\xff\x00\xff\xff",
    "\xff\xff\x00\xff",
    "\xff\xff\xff\xff",
    "\x00\x00\x00\x7f",
    "\x00\x00\xff\x7f",
    "\x00\xff\x00\x7f",
    "\xff\x00\x00\x7f",
    "\x00\xff\xff\x7f",
    "\xff\x00\xff\x7f",
    "\xff\xff\x00\x7f",
    "\xff\xff\xff\x7f",
  };

  success = TRUE;

  for(i = 0; i < 14 && success; i++){
    ags_osc_buffer_util_get_rgba(val_rgba[i],
				 &r, &g, &b, &a);

    if(r != rgba[i][0] ||
       g != rgba[i][1] ||
       b != rgba[i][2] ||
       a != rgba[i][3]){
      success = FALSE;
      
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_put_midi()
{
  unsigned char *buffer;

  guint i, j;
  gboolean success;
  
  static const guint8 midi[2][4] = {
    {0x0, 0x7f, 0xff, 0x0},
    {0x0, 0x80, 0xff, 0x0},
  };

  static const unsigned char *val_midi[] = {
    "\x00\x7f\xff\x00",
    "\x00\x80\xff\x00",
  };
  
  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    ags_osc_buffer_util_put_midi(buffer,
				 midi[i][0], midi[i][1], midi[i][2], midi[i][3]);

    for(j = 0; j < 4; j++){
      if(buffer[j] != val_midi[i][j]){
	success = FALSE;
	
	break;
      }
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_osc_buffer_util_test_get_midi()
{
  guint8 port, status_byte, data0, data1;
  
  guint i;
  gboolean success;

  static const guint8 midi[2][4] = {
    {0x0, 0x7f, 0xff, 0x0},
    {0x0, 0x80, 0xff, 0x0},
  };

  static const unsigned char *val_midi[] = {
    "\x00\x7f\xff\x00",
    "\x00\x80\xff\x00",
  };

  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    ags_osc_buffer_util_get_midi(val_midi[i],
				 &port, &status_byte, &data0, &data1);

    if(port != midi[i][0] ||
       status_byte != midi[i][1] ||
       data0 != midi[i][2] ||
       data1 != midi[i][3]){
      success = FALSE;
	
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
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


