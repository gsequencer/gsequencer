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

#include <ags/audio/midi/ags_midi_buffer_util.h>

int ags_midi_buffer_util_test_init_suite();
int ags_midi_buffer_util_test_clean_suite();

void ags_midi_buffer_util_test_get_varlength_size();
void ags_midi_buffer_util_test_put_varlength();
void ags_midi_buffer_util_test_get_varlength();
void ags_midi_buffer_util_test_put_int16();
void ags_midi_buffer_util_test_get_int16();
void ags_midi_buffer_util_test_put_int24();
void ags_midi_buffer_util_test_get_int24();
void ags_midi_buffer_util_test_put_int32();
void ags_midi_buffer_util_test_get_int32();
void ags_midi_buffer_util_test_put_header();
void ags_midi_buffer_util_test_get_header();
void ags_midi_buffer_util_test_put_track();
void ags_midi_buffer_util_test_get_track();
void ags_midi_buffer_util_test_put_key_on();
void ags_midi_buffer_util_test_get_key_on();
void ags_midi_buffer_util_test_put_key_off();
void ags_midi_buffer_util_test_get_key_off();
void ags_midi_buffer_util_test_put_key_pressure();
void ags_midi_buffer_util_test_get_key_pressure();
void ags_midi_buffer_util_test_put_change_parameter();
void ags_midi_buffer_util_test_get_change_parameter();
void ags_midi_buffer_util_test_put_change_pitch_bend();
void ags_midi_buffer_util_test_get_change_pitch_bend();
void ags_midi_buffer_util_test_put_change_program();
void ags_midi_buffer_util_test_get_change_program();
void ags_midi_buffer_util_test_put_change_pressure();
void ags_midi_buffer_util_test_get_change_pressure();
void ags_midi_buffer_util_test_put_put_sysex();
void ags_midi_buffer_util_test_get_get_sysex();
void ags_midi_buffer_util_test_put_quarter_frame();
void ags_midi_buffer_util_test_get_quarter_frame();
void ags_midi_buffer_util_test_put_song_position();
void ags_midi_buffer_util_test_get_song_position();
void ags_midi_buffer_util_test_put_song_select();
void ags_midi_buffer_util_test_get_song_select();
void ags_midi_buffer_util_test_put_tune_request();
void ags_midi_buffer_util_test_get_tune_request();
void ags_midi_buffer_util_test_put_sequence_number();
void ags_midi_buffer_util_test_get_sequence_number();
void ags_midi_buffer_util_test_put_smtpe();
void ags_midi_buffer_util_test_get_smtpe();
void ags_midi_buffer_util_test_put_tempo();
void ags_midi_buffer_util_test_get_tempo();
void ags_midi_buffer_util_test_put_time_signature();
void ags_midi_buffer_util_test_get_time_signature();
void ags_midi_buffer_util_test_put_key_signature();
void ags_midi_buffer_util_test_get_key_signature();
void ags_midi_buffer_util_test_put_sequencer_meta_event();
void ags_midi_buffer_util_test_get_sequencer_meta_event();
void ags_midi_buffer_util_test_put_text_event();
void ags_midi_buffer_util_test_get_text_event();
void ags_midi_buffer_util_test_put_end_of_track();
void ags_midi_buffer_util_test_get_end_of_track();
void ags_midi_buffer_util_test_put_seek_message();
void ags_midi_buffer_util_test_decode();

static const glong varlength[12][3] = {
  {0x0, 0, 1},
  {0x1, 1, 1},
  {0x7f, 128 - 1, 1},
  {0x8100, 128, 2},
  {0x8101, 128 + 1, 2},
  {0xff7f, 128 * 128 - 1, 2},
  {0x818000, 128 * 128, 3},
  {0x818001, 128 * 128 + 1, 3},
  {0xffff7f, 128 * 128 * 128 - 1, 3},
  {0x81808000, 128 * 128 * 128, 4},
  {0x81808001, 128 * 128 * 128 + 1, 4},
  {0xffffff7f, 128 * 128 * 128 * 128 - 1, 4},
};

static const unsigned char *varlength_buffer[] = {
  "\x00",
  "\x01",
  "\x7f",
  "\x81\x00",
  "\x81\x01",
  "\xff\x7f",
  "\x81\x80\x00",
  "\x81\x80\x01",
  "\xff\xff\x7f",
  "\x81\x80\x80\x00",
  "\x81\x80\x80\x01",
  "\xff\xff\xff\x7f",
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_buffer_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_midi_buffer_util_test_get_varlength_size()
{
  guint i;
  gboolean success;
  
  /*  */
  success = TRUE;

  for(i = 0; i < 12; i++){
    if(ags_midi_buffer_util_get_varlength_size(varlength[i][1]) != varlength[i][2]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_varlength()
{
  unsigned char *buffer;

  guint mask;
  guint i, j;
  gboolean success;
  
  /*  */
  buffer = (unsigned char *) malloc(4 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 12 && success; i++){
    ags_midi_buffer_util_put_varlength(buffer,
				       varlength[i][1]);

    mask = 0xff;
    
    for(j = 0; j < varlength[i][2]; j++){
      if(buffer[j] != ((mask << (8 * (varlength[i][2] - j - 1))) & varlength[i][0]) >> (8 * (varlength[i][2] - j - 1))){
	success = FALSE;
	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_varlength()
{
  unsigned char **iter;

  glong val;
  guint n_read;
  guint i;
  gboolean success;

  iter = varlength_buffer;
  success = TRUE;
  
  for(i = 0; i < 12; i++){
    n_read = ags_midi_buffer_util_get_varlength(*iter,
						&val);

    if(n_read != varlength[i][2] ||
       val != varlength[i][1]){
      success = FALSE;
      break;
    }

    iter++;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_int16()
{
  unsigned char *buffer;
  
  guint i, j;
  gboolean success;
  
  static const glong val[] = {
    0,
    1,
    255,
    256,
    65535,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00",
    "\x00\x01",
    "\x00\xff",
    "\x01\x00",
    "\xff\xff",
  };

  buffer = (unsigned char *) malloc(2 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 5 && success; i++){
    ags_midi_buffer_util_put_int16(buffer,
				   val[i]);
    
    for(j = 0; j < 2; j++){
      if(buffer[j] != val_buffer[i][j]){
	success = FALSE;

	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_int16()
{
  glong current;
  guint i;
  gboolean success;
  
  static const glong val[] = {
    0,
    1,
    255,
    256,
    65535,
  };

  static const gchar *val_buffer[] = {
    "\x00\x00",
    "\x00\x01",
    "\x00\xff",
    "\x01\x00",
    "\xff\xff",
  };

  success = TRUE;

  for(i = 0; i < 5 && success; i++){
    ags_midi_buffer_util_get_int16(val_buffer[i],
				   &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_int24()
{
  unsigned char *buffer;
  
  guint i, j;
  gboolean success;
  
  static const glong val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00",
    "\x00\x00\x01",
    "\x00\x00\xff",
    "\x00\x01\x00",
    "\x00\xff\xff",
    "\x01\x00\x00",
    "\xff\xff\xff",
  };

  buffer = (unsigned char *) malloc(3 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 7 && success; i++){
    ags_midi_buffer_util_put_int24(buffer,
				   val[i]);
    
    for(j = 0; j < 3; j++){
      if(buffer[j] != val_buffer[i][j]){
	success = FALSE;

	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_int24()
{
  glong current;
  guint i;
  gboolean success;
  
  static const glong val[] = {
    0,
    1,
    255,
    256,
    65535,
    65536,
    16777215,
  };

  static const unsigned char *val_buffer[] = {
    "\x00\x00\x00",
    "\x00\x00\x01",
    "\x00\x00\xff",
    "\x00\x01\x00",
    "\x00\xff\xff",
    "\x01\x00\x00",
    "\xff\xff\xff",
  };

  success = TRUE;

  for(i = 0; i < 7 && success; i++){
    ags_midi_buffer_util_get_int24(val_buffer[i],
				   &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_int32()
{
  unsigned char *buffer;
  
  guint i, j;
  gboolean success;
  
  static const glong val[] = {
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
    ags_midi_buffer_util_put_int32(buffer,
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
ags_midi_buffer_util_test_get_int32()
{
  glong current;
  guint i;
  gboolean success;
    
  static const glong val[] = {
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
    ags_midi_buffer_util_get_int32(val_buffer[i],
				   &current);

    if(current != val[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_header()
{
  unsigned char *buffer;
  unsigned char *header_format_0 = "MThd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x60";
  unsigned char *header_format_1 = "MThd\x00\x00\x00\x00\x00\x01\x00\x00\x00\x60";

  guint i;
  gboolean success;
  
  buffer = malloc(14 * sizeof(unsigned char));

  /* format 0 */
  success = TRUE;

  ags_midi_buffer_util_put_header(buffer,
				  0, 0,
				  0, 96);

  for(i = 0; i < 14; i++){
    if(buffer[i] != header_format_0[i]){
      success = FALSE;

      break;
    }
  }

  /* format 1 */
  success = TRUE;

  ags_midi_buffer_util_put_header(buffer,
				  0, 1,
				  0, 96);

  for(i = 0; i < 14; i++){
    if(buffer[i] != header_format_1[i]){
      success = FALSE;

      break;
    }
  }
}

void
ags_midi_buffer_util_test_get_header()
{
  unsigned char *header_format_0 = "MThd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x60";
  unsigned char *header_format_1 = "MThd\x00\x00\x00\x00\x00\x01\x00\x00\x00\x60";
  
  glong offset, format, track_count, division;

  /* invoke with no return location */
  ags_midi_buffer_util_get_header(header_format_0,
				  NULL, NULL,
				  NULL, NULL);
  
  /* format 0 */
  ags_midi_buffer_util_get_header(header_format_0,
				  &offset, &format,
				  &track_count, &division);
  CU_ASSERT(offset == 0 &&
	    format == 0 &&
	    track_count == 0 &&
	    division == 96);
  
  /* format 1 */
  ags_midi_buffer_util_get_header(header_format_1,
				  &offset, &format,
				  &track_count, &division);
  CU_ASSERT(offset == 0 &&
	    format == 1 &&
	    track_count == 0 &&
	    division == 96);
}

void
ags_midi_buffer_util_test_put_track()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_track()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_key_on()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_key_on()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_key_off()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_key_off()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_key_pressure()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_key_pressure()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_change_parameter()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_change_parameter()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_change_pitch_bend()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_change_pitch_bend()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_change_program()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_change_program()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_change_pressure()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_change_pressure()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_put_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_get_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_quarter_frame()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_quarter_frame()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_song_position()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_song_position()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_song_select()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_song_select()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_tune_request()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_tune_request()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_sequence_number()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_sequence_number()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_smtpe()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_smtpe()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_tempo()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_tempo()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_time_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_time_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_key_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_key_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_sequencer_meta_event()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_sequencer_meta_event()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_text_event()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_text_event()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_end_of_track()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_end_of_track()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_seek_message()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_decode()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsMidiBufferUtilTest\0", ags_midi_buffer_util_test_init_suite, ags_midi_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_midi_buffer_util.c get varlength size\0", ags_midi_buffer_util_test_get_varlength_size) == NULL) || 
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put varlength\0", ags_midi_buffer_util_test_put_varlength) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get varlength\0", ags_midi_buffer_util_test_get_varlength) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put int16\0", ags_midi_buffer_util_test_put_int16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get int16\0", ags_midi_buffer_util_test_get_int16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put int24\0", ags_midi_buffer_util_test_put_int24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get int24\0", ags_midi_buffer_util_test_get_int24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put int32\0", ags_midi_buffer_util_test_put_int32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get int32\0", ags_midi_buffer_util_test_get_int32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put header\0", ags_midi_buffer_util_test_put_header) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get header\0", ags_midi_buffer_util_test_get_header) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put track\0", ags_midi_buffer_util_test_put_track) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get track\0", ags_midi_buffer_util_test_get_track) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put key on\0", ags_midi_buffer_util_test_put_key_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get key on\0", ags_midi_buffer_util_test_get_key_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put key off\0", ags_midi_buffer_util_test_put_key_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get key off\0", ags_midi_buffer_util_test_get_key_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put key pressure\0", ags_midi_buffer_util_test_put_key_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get key pressure\0", ags_midi_buffer_util_test_get_key_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change parameter\0", ags_midi_buffer_util_test_put_change_parameter) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change parameter\0", ags_midi_buffer_util_test_get_change_parameter) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change pitch bend\0", ags_midi_buffer_util_test_put_change_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change pitch bend\0", ags_midi_buffer_util_test_get_change_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change program\0", ags_midi_buffer_util_test_put_change_program) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change program\0", ags_midi_buffer_util_test_get_change_program) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change pressure\0", ags_midi_buffer_util_test_put_change_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change pressure\0", ags_midi_buffer_util_test_get_change_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put sysex\0", ags_midi_buffer_util_test_put_put_sysex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get sysex\0", ags_midi_buffer_util_test_get_get_sysex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put quarter frame\0", ags_midi_buffer_util_test_put_quarter_frame) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get quarter frame\0", ags_midi_buffer_util_test_get_quarter_frame) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put song position\0", ags_midi_buffer_util_test_put_song_position) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get song position\0", ags_midi_buffer_util_test_get_song_position) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put song select\0", ags_midi_buffer_util_test_put_song_select) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get song select\0", ags_midi_buffer_util_test_get_song_select) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put tune request\0", ags_midi_buffer_util_test_put_tune_request) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get tune request\0", ags_midi_buffer_util_test_get_tune_request) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put sequence number\0", ags_midi_buffer_util_test_put_sequence_number) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get sequence number\0", ags_midi_buffer_util_test_get_sequence_number) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put smtpe\0", ags_midi_buffer_util_test_put_smtpe) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get smtpe\0", ags_midi_buffer_util_test_get_smtpe) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put tempo\0", ags_midi_buffer_util_test_put_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get tempo\0", ags_midi_buffer_util_test_get_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put time signature\0", ags_midi_buffer_util_test_put_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get time signature\0", ags_midi_buffer_util_test_get_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put key signature\0", ags_midi_buffer_util_test_put_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get key signature\0", ags_midi_buffer_util_test_get_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put sequencer meta event\0", ags_midi_buffer_util_test_put_sequencer_meta_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get sequencer meta event\0", ags_midi_buffer_util_test_get_sequencer_meta_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put text event\0", ags_midi_buffer_util_test_put_text_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get text event\0", ags_midi_buffer_util_test_get_text_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put end of track\0", ags_midi_buffer_util_test_put_end_of_track) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get end of track\0", ags_midi_buffer_util_test_get_end_of_track) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c seek message\0", ags_midi_buffer_util_test_put_seek_message) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c decode\0", ags_midi_buffer_util_test_decode) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


