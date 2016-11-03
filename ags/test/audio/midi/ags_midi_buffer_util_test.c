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
void ags_midi_buffer_util_test_put_pitch_bend();
void ags_midi_buffer_util_test_get_pitch_bend();
void ags_midi_buffer_util_test_put_change_program();
void ags_midi_buffer_util_test_get_change_program();
void ags_midi_buffer_util_test_put_change_pressure();
void ags_midi_buffer_util_test_get_change_pressure();
void ags_midi_buffer_util_test_put_sysex();
void ags_midi_buffer_util_test_get_sysex();
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

  CU_ASSERT(success == TRUE);

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

  CU_ASSERT(success == TRUE);
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
  unsigned char *buffer;
  unsigned char *track = "MTrk\x00\x00\x00\x00";

  guint i;
  gboolean success;
  
  buffer = (unsigned char *) malloc(8 * sizeof(unsigned char));

  /* track */
  success = TRUE;

  ags_midi_buffer_util_put_track(buffer,
				 0);
  
  for(i = 0; i < 8; i++){
    if(buffer[i] != track[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_track()
{
  unsigned char *track = "MTrk\x00\x00\x00\x00";

  glong offset;

  /* invoke with no return location */
  ags_midi_buffer_util_get_track(track,
				 NULL);
  
  /* track */
  ags_midi_buffer_util_get_track(track,
				 &offset);

  CU_ASSERT(offset == 0);
}

void
ags_midi_buffer_util_test_put_key_on()
{
  unsigned char *buffer;
  unsigned char *key_on = "\x90\x36\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_key_on(buffer,
				    varlength[i][1],
				    0,
				    54,
				    127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], key_on, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_key_on()
{
  unsigned char *buffer;
  unsigned char *key_on = "\x90\x36\x7f";

  guint i;
  glong delta_time, channel, key, velocity;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], key_on, 3);

  ags_midi_buffer_util_get_key_on(buffer,
				  NULL,
				  NULL,
				  NULL,
				  NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], key_on, 3);
    
    ags_midi_buffer_util_get_key_on(buffer,
				    &delta_time,
				    &channel,
				    &key,
				    &velocity);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       key != 54 ||
       velocity != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_key_off()
{
  unsigned char *buffer;
  unsigned char *key_off = "\x80\x36\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_key_off(buffer,
				     varlength[i][1],
				     0,
				     54,
				     127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], key_off, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_key_off()
{
  unsigned char *buffer;
  unsigned char *key_off = "\x80\x36\x7f";

  guint i;
  glong delta_time, channel, key, velocity;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], key_off, 3);

  ags_midi_buffer_util_get_key_off(buffer,
				   NULL,
				   NULL,
				   NULL,
				   NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], key_off, 3);
    
    ags_midi_buffer_util_get_key_off(buffer,
				     &delta_time,
				     &channel,
				     &key,
				     &velocity);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       key != 54 ||
       velocity != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_key_pressure()
{
  unsigned char *buffer;
  unsigned char *key_pressure = "\xa0\x36\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_key_pressure(buffer,
					  varlength[i][1],
					  0,
					  54,
					  127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], key_pressure, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_key_pressure()
{
  unsigned char *buffer;
  unsigned char *key_pressure = "\xa0\x36\x7f";

  guint i;
  glong delta_time, channel, key, velocity;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], key_pressure, 3);
  
  ags_midi_buffer_util_get_key_pressure(buffer,
					NULL,
					NULL,
					NULL,
					NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], key_pressure, 3);
    
    ags_midi_buffer_util_get_key_pressure(buffer,
					  &delta_time,
					  &channel,
					  &key,
					  &velocity);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       key != 54 ||
       velocity != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_change_parameter()
{
  unsigned char *buffer;
  unsigned char *change_parameter = "\xb0\x00\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_change_parameter(buffer,
					      varlength[i][1],
					      0,
					      0,
					      127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], change_parameter, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_change_parameter()
{
  unsigned char *buffer;
  unsigned char *change_parameter = "\xb0\x00\x7f";

  guint i;
  glong delta_time, channel, control, value;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], change_parameter, 3);
  
  ags_midi_buffer_util_get_change_parameter(buffer,
					    NULL,
					    NULL,
					    NULL,
					    NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], change_parameter, 3);
    
    ags_midi_buffer_util_get_change_parameter(buffer,
					      &delta_time,
					      &channel,
					      &control,
					      &value);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       control != 0 ||
       value != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_pitch_bend()
{
  unsigned char *buffer;
  unsigned char *pitch_bend = "\xe0\x00\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_pitch_bend(buffer,
					varlength[i][1],
					0,
					0,
					127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], pitch_bend, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_pitch_bend()
{
  unsigned char *buffer;
  unsigned char *pitch_bend = "\xe0\x00\x7f";

  guint i;
  glong delta_time, channel, pitch, transmitter;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], pitch_bend, 3);
  
  ags_midi_buffer_util_get_pitch_bend(buffer,
				      NULL,
				      NULL,
				      NULL,
				      NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], pitch_bend, 3);
    
    ags_midi_buffer_util_get_pitch_bend(buffer,
					&delta_time,
					&channel,
					&pitch,
					&transmitter);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       pitch != 0 ||
       transmitter != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_change_program()
{
  unsigned char *buffer;
  unsigned char *change_program = "\xc0\x00";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_change_program(buffer,
					    varlength[i][1],
					    0,
					    0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], change_program, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_change_program()
{
  unsigned char *buffer;
  unsigned char *change_program = "\xc0\x00";

  guint i;
  glong delta_time, channel, program;
  gboolean success;

  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], change_program, 3);
  
  ags_midi_buffer_util_get_change_program(buffer,
					  NULL,
					  NULL,
					  NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], change_program, 3);
    
    ags_midi_buffer_util_get_change_program(buffer,
					    &delta_time,
					    &channel,
					    &program);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       program != 0){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_change_pressure()
{
  unsigned char *buffer;
  unsigned char *change_pressure = "\xd0\x7f";
  
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    ags_midi_buffer_util_put_change_pressure(buffer,
					    varlength[i][1],
					    0,
					    127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], change_pressure, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_change_pressure()
{
  unsigned char *buffer;
  unsigned char *change_pressure = "\xd0\x7f";

  guint i;
  glong delta_time, channel, pressure;
  gboolean success;

  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], change_pressure, 3);
  
  ags_midi_buffer_util_get_change_pressure(buffer,
					  NULL,
					  NULL,
					  NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], change_pressure, 3);
    
    ags_midi_buffer_util_get_change_pressure(buffer,
					    &delta_time,
					    &channel,
					    &pressure);

    if(delta_time != varlength[i][1] ||
       channel != 0 ||
       pressure != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_get_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_buffer_util_test_put_quarter_frame()
{
  unsigned char *buffer;
  unsigned char *quarter_frame;
  
  unsigned char *quarter_frame_frame_number_0_lsb = "\xf1\x00";
  unsigned char *quarter_frame_frame_number_0_msb = "\xf1\x10";

  unsigned char *quarter_frame_frame_number_30_lsb = "\xf1\x0e";
  unsigned char *quarter_frame_frame_number_30_msb = "\xf1\x11";

  unsigned char *quarter_frame_seconds_0_lsb = "\xf1\x20";
  unsigned char *quarter_frame_seconds_0_msb = "\xf1\x30";

  unsigned char *quarter_frame_seconds_59_lsb = "\xf1\x2c";
  unsigned char *quarter_frame_seconds_59_msb = "\xf1\x33";

  unsigned char *quarter_frame_minutes_0_lsb = "\xf1\x40";
  unsigned char *quarter_frame_minutes_0_msb = "\xf1\x50";

  unsigned char *quarter_frame_minutes_59_lsb = "\xf1\x4c";
  unsigned char *quarter_frame_minutes_59_msb = "\xf1\x53";

  unsigned char *quarter_frame_hours_0_lsb = "\xf1\x60";
  unsigned char *quarter_frame_hours_0_msb = "\xf1\x70";

  unsigned char *quarter_frame_hours_23_lsb = "\xf1\x67";
  unsigned char *quarter_frame_hours_23_msb = "\xf1\x71";

  guint i;
  
  gboolean success;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));
  quarter_frame = (unsigned char *) malloc(7 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 12; i++){
    /* frame number 0 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_frame_number_0_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* frame number 0 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_frame_number_0_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }

    /* frame number 30 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_frame_number_30_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB, (0x0f & 0x1e));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* frame number 30 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_frame_number_30_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB, ((0x10 & 0x1e) >> 4));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }

    /* seconds 0 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_seconds_0_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* seconds 0 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_seconds_0_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }

    /* seconds 59 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_seconds_59_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB, (0x0f & 0x3c));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* seconds 59 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_seconds_59_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB, ((0x30 & (0x3c)) >> 4));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;
      
      break;
    }
    
    /* minutes 0 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_minutes_0_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* minutes 0 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_minutes_0_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }

    /* minutes 59 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_minutes_59_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB, (0x0f & 0x3c));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* minutes 59 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_minutes_59_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB, ((0x30 & (0x3c)) >> 4));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;
      
      break;
    }

    /* hours 0 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_hours_0_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_LSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* hours 0 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_hours_0_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_MSB, 0);

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }

    /* hours 23 - lsb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_hours_23_lsb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_LSB, (0x0f & 0x17));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
    
    /* hours 23 - msb */
    memcpy(quarter_frame, varlength_buffer[i], varlength[i][2]);
    memcpy(quarter_frame + varlength[i][2], quarter_frame_hours_23_msb, 2);

    ags_midi_buffer_util_put_quarter_frame(buffer,
					   varlength[i][1],
					   AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_MSB, ((0x10 & 0x17) >> 4));

    if(memcmp(buffer,
	      quarter_frame,
	      varlength[i][2] + 2)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_quarter_frame()
{
  unsigned char *buffer;
  
  unsigned char *quarter_frame_frame_number_0_lsb = "\xf1\x00";
  unsigned char *quarter_frame_frame_number_0_msb = "\xf1\x10";

  unsigned char *quarter_frame_frame_number_30_lsb = "\xf1\x0e";
  unsigned char *quarter_frame_frame_number_30_msb = "\xf1\x11";

  unsigned char *quarter_frame_seconds_0_lsb = "\xf1\x20";
  unsigned char *quarter_frame_seconds_0_msb = "\xf1\x30";

  unsigned char *quarter_frame_seconds_59_lsb = "\xf1\x2c";
  unsigned char *quarter_frame_seconds_59_msb = "\xf1\x33";

  unsigned char *quarter_frame_minutes_0_lsb = "\xf1\x40";
  unsigned char *quarter_frame_minutes_0_msb = "\xf1\x50";

  unsigned char *quarter_frame_minutes_59_lsb = "\xf1\x4c";
  unsigned char *quarter_frame_minutes_59_msb = "\xf1\x53";

  unsigned char *quarter_frame_hours_0_lsb = "\xf1\x60";
  unsigned char *quarter_frame_hours_0_msb = "\xf1\x70";

  unsigned char *quarter_frame_hours_23_lsb = "\xf1\x67";
  unsigned char *quarter_frame_hours_23_msb = "\xf1\x71";

  glong delta_time, message_type, values;
  guint i;
  gboolean success;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  success = TRUE;
  
  for(i = 0; i < 12; i++){
    /* frame number 0 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_frame_number_0_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB ||
       values != 0){
      success = FALSE;

      break;
    }
    
    /* frame number 0 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_frame_number_0_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB ||
       values != 0){
      success = FALSE;

      break;
    }

    /* frame number 30 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_frame_number_30_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB ||
       values != (0x0f & 0x1e)){
      success = FALSE;

      break;
    }
    
    /* frame number 30 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_frame_number_30_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB ||
       values != ((0x10 & 0x1e) >> 4)){
      success = FALSE;

      break;
    }

    /* seconds 0 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_seconds_0_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB ||
       values != 0){
      success = FALSE;

      break;
    }
    
    /* seconds 0 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_seconds_0_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB ||
       values != 0){
      success = FALSE;

      break;
    }

    /* seconds 59 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_seconds_59_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB ||
       values != (0x0f & 0x3c)){
      success = FALSE;

      break;
    }
    
    /* seconds 59 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_seconds_59_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB ||
       values != ((0x30 & 0x3c) >> 4)){
      success = FALSE;
      
      break;
    }
    
    /* minutes 0 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_minutes_0_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB ||
       values != 0){
      success = FALSE;

      break;
    }
    
    /* minutes 0 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_minutes_0_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB ||
       values != 0){
      success = FALSE;

      break;
    }

    /* minutes 59 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_minutes_59_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB ||
       values != (0x0f & 0x3c)){
      success = FALSE;

      break;
    }
    
    /* minutes 59 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_minutes_59_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB ||
       values != ((0x30 & 0x3c) >> 4)){
      success = FALSE;
      
      break;
    }

    /* hours 0 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_hours_0_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_LSB ||
       values != 0){
      success = FALSE;

      break;
    }
    
    /* hours 0 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_hours_0_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_MSB ||
       values != 0){
      success = FALSE;

      break;
    }

    /* hours 23 - lsb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_hours_23_lsb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_LSB ||
       values != (0x0f & 0x17)){
      success = FALSE;

      break;
    }
    
    /* hours 23 - msb */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], quarter_frame_hours_23_msb, 2);

    ags_midi_buffer_util_get_quarter_frame(buffer,
					   &delta_time,
					   &message_type, &values);

    if(message_type != AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_MSB ||
       values != ((0x10 & 0x17) >> 4)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_song_position()
{
  unsigned char *buffer;
  unsigned char *song_position_0 = "\xf2\x00\x00";
  unsigned char *song_position_16 = "\xf2\x10\x00";
  unsigned char *song_position_16383 = "\xf2\x7f\x7f";
 
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    /* position 0 */
    ags_midi_buffer_util_put_song_position(buffer,
					   varlength[i][1],
					   0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], song_position_0, 3)){
      success = FALSE;

      break;
    }

    /* position 16 */
    ags_midi_buffer_util_put_song_position(buffer,
					   varlength[i][1],
					   16);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], song_position_16, 3)){
      success = FALSE;

      break;
    }

    /* position 16383 (maximum) */
    ags_midi_buffer_util_put_song_position(buffer,
					   varlength[i][1],
					   16383);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], song_position_16383, 3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_song_position()
{
  unsigned char *buffer;
  unsigned char *song_position_0 = "\xf2\x00\x00";
  unsigned char *song_position_16 = "\xf2\x10\x00";
  unsigned char *song_position_16383 = "\xf2\x7f\x7f";

  guint i;
  glong delta_time, song_position;
  gboolean success;

  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], song_position, 3);
  
  ags_midi_buffer_util_get_song_position(buffer,
					 NULL,
					 NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* position 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], song_position_0, 3);
    
    ags_midi_buffer_util_get_song_position(buffer,
					   &delta_time,
					   &song_position);

    if(delta_time != varlength[i][1] ||
       song_position != 0){
      success = FALSE;

      break;
    }

    /* position 16 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], song_position_16, 3);
    
    ags_midi_buffer_util_get_song_position(buffer,
					   &delta_time,
					   &song_position);

    if(delta_time != varlength[i][1] ||
       song_position != 16){
      success = FALSE;

      break;
    }

    /* position 16383 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], song_position_16383, 3);
    
    ags_midi_buffer_util_get_song_position(buffer,
					   &delta_time,
					   &song_position);

    if(delta_time != varlength[i][1] ||
       song_position != 16383){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_song_select()
{
  unsigned char *buffer;
  unsigned char *song_select_0 = "\xf3\x00";
  unsigned char *song_select_127 = "\xf3\x7f";
 
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    /* select 0 */
    ags_midi_buffer_util_put_song_select(buffer,
					 varlength[i][1],
					 0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], song_select_0, 2)){
      success = FALSE;

      break;
    }

    /* select 127 */
    ags_midi_buffer_util_put_song_select(buffer,
					 varlength[i][1],
					 127);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], song_select_127, 2)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_song_select()
{
  unsigned char *buffer;
  unsigned char *song_select_0 = "\xf3\x00";
  unsigned char *song_select_127 = "\xf3\x7f";

  guint i;
  glong delta_time, song_select;
  gboolean success;

  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], song_select, 2);
  
  ags_midi_buffer_util_get_song_select(buffer,
				       NULL,
				       NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* select 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], song_select_0, 2);
    
    ags_midi_buffer_util_get_song_select(buffer,
					 &delta_time,
					 &song_select);

    if(delta_time != varlength[i][1] ||
       song_select != 0){
      success = FALSE;

      break;
    }

    /* select 127 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], song_select_127, 2);
    
    ags_midi_buffer_util_get_song_select(buffer,
					 &delta_time,
					 &song_select);

    if(delta_time != varlength[i][1] ||
       song_select != 127){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_tune_request()
{
  unsigned char *buffer;
  unsigned char *tune_request = "\xf6";
 
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    /* select 0 */
    ags_midi_buffer_util_put_tune_request(buffer,
					  varlength[i][1]);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tune_request, 1)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_tune_request()
{
  unsigned char *buffer;
  unsigned char *tune_request = "\xf6";

  guint i;
  glong delta_time;
  gboolean success;

  buffer = (unsigned char *) malloc(6 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], tune_request, 1);
  
  ags_midi_buffer_util_get_tune_request(buffer,
					NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* select 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tune_request, 1);
    
    ags_midi_buffer_util_get_tune_request(buffer,
					  &delta_time);

    if(delta_time != varlength[i][1]){
      success = FALSE;

      break;
    }
  }  

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_sequence_number()
{
  unsigned char *buffer;
  unsigned char *sequence_number_0 = "\xff\x00\x02\x00\x00";
  unsigned char *sequence_number_255 = "\xff\x00\x02\x00\xff";
  unsigned char *sequence_number_256 = "\xff\x00\x02\x01\x00";
  unsigned char *sequence_number_65535 = "\xff\x00\x02\xff\xff";
 
  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(9 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){
    /* select 0 */
    ags_midi_buffer_util_put_sequence_number(buffer,
					     varlength[i][1],
					     0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], sequence_number_0, 5)){
      success = FALSE;

      break;
    }

    /* select 255 */
    ags_midi_buffer_util_put_sequence_number(buffer,
					     varlength[i][1],
					     255);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], sequence_number_255, 5)){
      success = FALSE;

      break;
    }

    /* select 256 */
    ags_midi_buffer_util_put_sequence_number(buffer,
					     varlength[i][1],
					     256);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], sequence_number_256, 5)){
      success = FALSE;

      break;
    }

    /* select 65535 */
    ags_midi_buffer_util_put_sequence_number(buffer,
					     varlength[i][1],
					     65535);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], sequence_number_65535, 5)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_sequence_number()
{
  unsigned char *buffer;
  unsigned char *sequence_number_0 = "\xff\x00\x02\x00\x00";
  unsigned char *sequence_number_255 = "\xff\x00\x02\x00\xff";
  unsigned char *sequence_number_256 = "\xff\x00\x02\x01\x00";
  unsigned char *sequence_number_65535 = "\xff\x00\x02\xff\xff";
 
  guint i;
  glong delta_time, sequence_number;
  gboolean success;

  buffer = (unsigned char *) malloc(9 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], sequence_number, 5);
  
  ags_midi_buffer_util_get_sequence_number(buffer,
					   NULL,
					   NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* select 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], sequence_number_0, 5);
    
    ags_midi_buffer_util_get_sequence_number(buffer,
					     &delta_time,
					     &sequence_number);

    if(delta_time != varlength[i][1] ||
       sequence_number != 0){
      success = FALSE;

      break;
    }

    /* select 255 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], sequence_number_255, 5);
    
    ags_midi_buffer_util_get_sequence_number(buffer,
					     &delta_time,
					     &sequence_number);

    if(delta_time != varlength[i][1] ||
       sequence_number != 255){
      success = FALSE;

      break;
    }

    /* select 256 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], sequence_number_256, 5);
    
    ags_midi_buffer_util_get_sequence_number(buffer,
					     &delta_time,
					     &sequence_number);

    if(delta_time != varlength[i][1] ||
       sequence_number != 256){
      success = FALSE;

      break;
    }

    /* select 65535 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], sequence_number_65535, 5);
    
    ags_midi_buffer_util_get_sequence_number(buffer,
					     &delta_time,
					     &sequence_number);

    if(delta_time != varlength[i][1] ||
       sequence_number != 65535){
      success = FALSE;

      break;
    }
  }  
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_smtpe()
{
  unsigned char *buffer;
  unsigned char *smtpe_0_0_0_0 = "\xff\x54\x05\x80\x00\x00\x00";
  unsigned char *smtpe_0_0_0_30 = "\xff\x54\x05\x80\x00\x00\x1e";
  unsigned char *smtpe_0_0_59_30 = "\xff\x54\x05\x80\x00\x3b\x1e";
  unsigned char *smtpe_0_59_59_30 = "\xff\x54\x05\x80\x3b\x3b\x1e";
  unsigned char *smtpe_23_59_59_30 = "\xff\x54\x05\x97\x3b\x3b\x1e";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(11 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* smtpe 0 */
    ags_midi_buffer_util_put_smtpe(buffer,
				   varlength[i][1],
				   AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS,
				   0,
				   0,
				   0,
				   0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], smtpe_0_0_0_0, 7)){
      success = FALSE;

      break;
    }

    /* smtpe 30 fr */
    ags_midi_buffer_util_put_smtpe(buffer,
				   varlength[i][1],
				   AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS,
				   0,
				   0,
				   0,
				   30);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], smtpe_0_0_0_30, 7)){
      success = FALSE;

      break;
    }

    /* smtpe 59 s 30 fr */
    ags_midi_buffer_util_put_smtpe(buffer,
				   varlength[i][1],
				   AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS,
				   0,
				   0,
				   59,
				   30);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], smtpe_0_0_59_30, 7)){
      success = FALSE;

      break;
    }

    /* smtpe 59 m 59 s 30 fr */
    ags_midi_buffer_util_put_smtpe(buffer,
				   varlength[i][1],
				   AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS,
				   0,
				   59,
				   59,
				   30);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], smtpe_0_59_59_30, 7)){
      success = FALSE;

      break;
    }

    /* smtpe 23 h 59 m 59 s 30 fr */
    ags_midi_buffer_util_put_smtpe(buffer,
				   varlength[i][1],
				   AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS,
				   23,
				   59,
				   59,
				   30);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], smtpe_23_59_59_30, 7)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_smtpe()
{
  unsigned char *buffer;
  unsigned char *smtpe_0_0_0_0 = "\xff\x54\x05\x80\x00\x00\x00";
  unsigned char *smtpe_0_0_0_30 = "\xff\x54\x05\x80\x00\x00\x1e";
  unsigned char *smtpe_0_0_59_30 = "\xff\x54\x05\x80\x00\x3b\x1e";
  unsigned char *smtpe_0_59_59_30 = "\xff\x54\x05\x80\x3b\x3b\x1e";
  unsigned char *smtpe_23_59_59_30 = "\xff\x54\x05\x97\x3b\x3b\x1e";

  guint i;
  glong delta_time, rr, hr, mn, se, fr;
  gboolean success;

  buffer = (unsigned char *) malloc(11 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], smtpe_0_0_0_0, 7);
  
  ags_midi_buffer_util_get_smtpe(buffer,
				 NULL,
				 NULL,
				 NULL,
				 NULL,
				 NULL,
				 NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* smtpe 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], smtpe_0_0_0_0, 7);
    
    ags_midi_buffer_util_get_smtpe(buffer,
				   &delta_time,
				   &rr,
				   &hr,
				   &mn,
				   &se,
				   &fr);

    if(delta_time != varlength[i][1] ||
       rr != AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS ||
       hr != 0 ||
       mn != 0 ||
       se != 0 ||
       fr != 0){
      success = FALSE;

      break;
    }

    /* smtpe 30 fr */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], smtpe_0_0_0_30, 7);
    
    ags_midi_buffer_util_get_smtpe(buffer,
				   &delta_time,
				   &rr,
				   &hr,
				   &mn,
				   &se,
				   &fr);

    if(delta_time != varlength[i][1] ||
       rr != AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS ||
       hr != 0 ||
       mn != 0 ||
       se != 0 ||
       fr != 30){
      success = FALSE;

      break;
    }

    /* smtpe 59 s 30 fr */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], smtpe_0_0_59_30, 7);
    
    ags_midi_buffer_util_get_smtpe(buffer,
				   &delta_time,
				   &rr,
				   &hr,
				   &mn,
				   &se,
				   &fr);

    if(delta_time != varlength[i][1] ||
       rr != AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS ||
       hr != 0 ||
       mn != 0 ||
       se != 59 ||
       fr != 30){
      success = FALSE;

      break;
    }

    /* smtpe 59 m 59 s 30 fr */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], smtpe_0_59_59_30, 7);
    
    ags_midi_buffer_util_get_smtpe(buffer,
				   &delta_time,
				   &rr,
				   &hr,
				   &mn,
				   &se,
				   &fr);

    if(delta_time != varlength[i][1] ||
       rr != AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS ||
       hr != 0 ||
       mn != 59 ||
       se != 59 ||
       fr != 30){
      success = FALSE;

      break;
    }

    /* smtpe 23 h 59 m 59 s 30 fr */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], smtpe_23_59_59_30, 7);
    
    ags_midi_buffer_util_get_smtpe(buffer,
				   &delta_time,
				   &rr,
				   &hr,
				   &mn,
				   &se,
				   &fr);

    if(delta_time != varlength[i][1] ||
       rr != AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS ||
       hr != 23 ||
       mn != 59 ||
       se != 59 ||
       fr != 30){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_tempo()
{
  unsigned char *buffer;
  unsigned char *tempo_0 = "\xff\x51\x03\x00\x00\x00";
  unsigned char *tempo_1 = "\xff\x51\x03\x00\x00\x01";
  unsigned char *tempo_255 = "\xff\x51\x03\x00\x00\xff";
  unsigned char *tempo_256 = "\xff\x51\x03\x00\x01\x00";
  unsigned char *tempo_65535 = "\xff\x51\x03\x00\xff\xff";
  unsigned char *tempo_65536 = "\xff\x51\x03\x01\x00\x00";
  unsigned char *tempo_16777215 = "\xff\x51\x03\xff\xff\xff";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(10 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* tempo 0 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_0, 6)){
      success = FALSE;

      break;
    }

    /* tempo 1 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   1);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_1, 6)){
      success = FALSE;

      break;
    }

    /* tempo 255 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   255);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_255, 6)){
      success = FALSE;

      break;
    }

    /* tempo 256 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   256);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_256, 6)){
      success = FALSE;

      break;
    }

    /* tempo 65535 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   65535);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_65535, 6)){
      success = FALSE;

      break;
    }

    /* tempo 65536 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   65536);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_65536, 6)){
      success = FALSE;

      break;
    }

    /* tempo 16777215 */
    ags_midi_buffer_util_put_tempo(buffer,
				   varlength[i][1],
				   16777215);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], tempo_16777215, 6)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_tempo()
{
  unsigned char *buffer;
  unsigned char *tempo_0 = "\xff\x51\x03\x00\x00\x00";
  unsigned char *tempo_1 = "\xff\x51\x03\x00\x00\x01";
  unsigned char *tempo_255 = "\xff\x51\x03\x00\x00\xff";
  unsigned char *tempo_256 = "\xff\x51\x03\x00\x01\x00";
  unsigned char *tempo_65535 = "\xff\x51\x03\x00\xff\xff";
  unsigned char *tempo_65536 = "\xff\x51\x03\x01\x00\x00";
  unsigned char *tempo_16777215 = "\xff\x51\x03\xff\xff\xff";

  guint i;
  glong delta_time, tempo;
  gboolean success;

  buffer = (unsigned char *) malloc(10 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], tempo_0, 6);
  
  ags_midi_buffer_util_get_tempo(buffer,
				 NULL,
				 NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* tempo 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_0, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 0){
      success = FALSE;

      break;
    }

    /* tempo 1 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_1, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 1){
      success = FALSE;

      break;
    }

    /* tempo 255 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_255, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 255){
      success = FALSE;

      break;
    }

    /* tempo 256 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_256, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 256){
      success = FALSE;

      break;
    }

    /* tempo 65535 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_65535, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 65535){
      success = FALSE;

      break;
    }

    /* tempo 65536 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_65536, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 65536){
      success = FALSE;

      break;
    }

    /* tempo 16777215 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], tempo_16777215, 6);
    
    ags_midi_buffer_util_get_tempo(buffer,
				   &delta_time,
				   &tempo);

    if(delta_time != varlength[i][1] ||
       tempo != 16777215){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_time_signature()
{
  unsigned char *buffer;
  unsigned char *time_signature_0_0_0_0 = "\xff\x58\x04\x00\x00\x00\x00";
  unsigned char *time_signature_1_1_96_8 = "\xff\x58\x04\x01\x01\x60\x08";
  unsigned char *time_signature_4_4_24_8 = "\xff\x58\x04\x04\x04\x18\x08";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(11 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* time signature 0 0 0 0 */
    ags_midi_buffer_util_put_time_signature(buffer,
					    varlength[i][1],
					    0,
					    0,
					    0,
					    0);
    
    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], time_signature_0_0_0_0, 7)){
      success = FALSE;

      break;
    }

    /* time signature 1 1 96 8 */
    ags_midi_buffer_util_put_time_signature(buffer,
					    varlength[i][1],
					    1,
					    1,
					    96,
					    8);
    
    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], time_signature_1_1_96_8, 7)){
      success = FALSE;

      break;
    }

    /* time signature 4 4 24 8 */
    ags_midi_buffer_util_put_time_signature(buffer,
					    varlength[i][1],
					    4,
					    4,
					    24,
					    8);
    
    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], time_signature_4_4_24_8, 7)){
      success = FALSE;

      break;
    }
  }  
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_time_signature()
{
  unsigned char *buffer;
  unsigned char *time_signature_0_0_0_0 = "\xff\x58\x04\x00\x00\x00\x00";
  unsigned char *time_signature_1_1_96_8 = "\xff\x58\x04\x01\x01\x60\x08";
  unsigned char *time_signature_4_4_24_8 = "\xff\x58\x04\x04\x04\x18\x08";

  guint i;
  glong delta_time, nn, dd, cc, bb;
  gboolean success;

  buffer = (unsigned char *) malloc(11 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], time_signature_0_0_0_0, 7);
  
  ags_midi_buffer_util_get_time_signature(buffer,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* time signature 0 0 0 0 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], time_signature_0_0_0_0, 7);
    
    ags_midi_buffer_util_get_time_signature(buffer,
					    &delta_time,
					    &nn,
					    &dd,
					    &cc,
					    &bb);
    
    if(delta_time != varlength[i][1] ||
       nn != 0 ||
       dd != 0 ||
       cc != 0 ||
       bb != 0){
      success = FALSE;

      break;
    }

    /* time signature 1 1 96 8 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], time_signature_1_1_96_8, 7);
    
    ags_midi_buffer_util_get_time_signature(buffer,
					    &delta_time,
					    &nn,
					    &dd,
					    &cc,
					    &bb);
    
    if(delta_time != varlength[i][1] ||
       nn != 1 ||
       dd != 1 ||
       cc != 96 ||
       bb != 8){
      success = FALSE;

      break;
    }

    /* time signature 4 4 24 8 */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], time_signature_4_4_24_8, 7);
    
    ags_midi_buffer_util_get_time_signature(buffer,
					    &delta_time,
					    &nn,
					    &dd,
					    &cc,
					    &bb);
    
    if(delta_time != varlength[i][1] ||
       nn != 4 ||
       dd != 4 ||
       cc != 24 ||
       bb != 8){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_key_signature()
{
  unsigned char *buffer;
  unsigned char *key_signature_3_minor = "\xff\x59\x02\x03\x01";
  unsigned char *key_signature_3_major = "\xff\x59\x02\x03\x00";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(9 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* key signature 3 minor */
    ags_midi_buffer_util_put_key_signature(buffer,
					   varlength[i][1],
					   3, 1);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], key_signature_3_minor, 5)){
      success = FALSE;

      break;
    }

    /* key signature 3 major */
    ags_midi_buffer_util_put_key_signature(buffer,
					   varlength[i][1],
					   3, 0);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], key_signature_3_major, 5)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_key_signature()
{
  unsigned char *buffer;
  unsigned char *key_signature_3_minor = "\xff\x59\x02\x03\x01";
  unsigned char *key_signature_3_major = "\xff\x59\x02\x03\x00";

  guint i;
  glong delta_time, sharp_flats, is_minor;
  gboolean success;

  buffer = (unsigned char *) malloc(10 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], key_signature_3_minor, 5);
  
  ags_midi_buffer_util_get_key_signature(buffer,
					 NULL,
					 NULL,
					 NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* key signature 3 minor */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], key_signature_3_minor, 5);
    
    ags_midi_buffer_util_get_key_signature(buffer,
					   &delta_time,
					   &sharp_flats,
					   &is_minor);
    
    if(delta_time != varlength[i][1] ||
       sharp_flats != 3 ||
       is_minor != 1){
      success = FALSE;

      break;
    }

    /* key signature 3 major */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], key_signature_3_major, 5);
    
    ags_midi_buffer_util_get_key_signature(buffer,
					   &delta_time,
					   &sharp_flats,
					   &is_minor);
    
    if(delta_time != varlength[i][1] ||
       sharp_flats != 3 ||
       is_minor != 0){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
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
  unsigned char *buffer;
  unsigned char *text_event_MY_TEXT = "\xff\x01\x07MY TEXT\0";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(14 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* text event */
    ags_midi_buffer_util_put_text_event(buffer,
					varlength[i][1],
					"MY TEXT\0", 7);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], text_event_MY_TEXT, 10)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_text_event()
{
  unsigned char *buffer;
  unsigned char *text_event_MY_TEXT = "\xff\x01\x07MY TEXT\0";

  guint i;
  unsigned char *text_event;
  glong delta_time, length;
  gboolean success;

  buffer = (unsigned char *) malloc(14 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], text_event_MY_TEXT, 10);
  
  ags_midi_buffer_util_get_text_event(buffer,
				      NULL,
				      NULL,
				      NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* text event */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], text_event_MY_TEXT, 10);
    
    ags_midi_buffer_util_get_text_event(buffer,
					&delta_time,
					&text_event,
					&length);
    
    if(delta_time != varlength[i][1] ||
       memcmp(text_event, "MY TEXT\0", 7) ||
       length != 7){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_put_end_of_track()
{
  unsigned char *buffer;
  unsigned char *end_of_track = "\xff\x2f\x00\0";

  guint i;
  gboolean success;

  /* test different delta-time */
  success = TRUE;
  
  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  for(i = 0; i < 12; i++){    
    /* text event */
    ags_midi_buffer_util_put_end_of_track(buffer,
					  varlength[i][1]);

    if(memcmp(buffer, varlength_buffer[i], varlength[i][2]) ||
       memcmp(buffer + varlength[i][2], end_of_track, 3)){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_buffer_util_test_get_end_of_track()
{
  unsigned char *buffer;
  unsigned char *end_of_track = "\xff\x2f\x00\0";

  guint i;
  glong delta_time;
  gboolean success;

  buffer = (unsigned char *) malloc(7 * sizeof(unsigned char));

  /* invoke without return location */
  memcpy(buffer, varlength_buffer[0], varlength[0][2]);
  memcpy(buffer + varlength[0][2], end_of_track, 3);
  
  ags_midi_buffer_util_get_end_of_track(buffer,
					NULL);
  
  /* test different delta-time */
  success = TRUE;

  for(i = 0; i < 12; i++){
    /* text event */
    memcpy(buffer, varlength_buffer[i], varlength[i][2]);
    memcpy(buffer + varlength[i][2], end_of_track, 3);
    
    ags_midi_buffer_util_get_end_of_track(buffer,
					  &delta_time);
    
    if(delta_time != varlength[i][1]){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
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
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change pitch bend\0", ags_midi_buffer_util_test_put_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change pitch bend\0", ags_midi_buffer_util_test_get_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change program\0", ags_midi_buffer_util_test_put_change_program) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change program\0", ags_midi_buffer_util_test_get_change_program) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put change pressure\0", ags_midi_buffer_util_test_put_change_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get change pressure\0", ags_midi_buffer_util_test_get_change_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c put sysex\0", ags_midi_buffer_util_test_put_sysex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_buffer_util.c get sysex\0", ags_midi_buffer_util_test_get_sysex) == NULL) ||
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


