/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joel Kraehemann
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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_midi_parser_test_init_suite();
int ags_midi_parser_test_clean_suite();

void ags_midi_parser_test_read_gint16();
void ags_midi_parser_test_read_gint24();
void ags_midi_parser_test_read_gint32();
void ags_midi_parser_test_read_varlength();
void ags_midi_parser_test_read_text();
void ags_midi_parser_test_midi_getc();
void ags_midi_parser_test_parse_full();
void ags_midi_parser_test_parse_bytes();
void ags_midi_parser_test_parse_header();
void ags_midi_parser_test_parse_track();
void ags_midi_parser_test_channel_message();
void ags_midi_parser_test_key_on();
void ags_midi_parser_test_key_off();
void ags_midi_parser_test_key_pressure();
void ags_midi_parser_test_change_parameter();
void ags_midi_parser_test_change_pitch_bend();
void ags_midi_parser_test_change_program();
void ags_midi_parser_test_change_channel_pressure();
void ags_midi_parser_test_not_defined();
void ags_midi_parser_test_sysex();
void ags_midi_parser_test_system_common();
void ags_midi_parser_test_meta_event();
void ags_midi_parser_test_sequence_number();
void ags_midi_parser_test_end_of_track();
void ags_midi_parser_test_smtpe();
void ags_midi_parser_test_tempo();
void ags_midi_parser_test_time_signature();
void ags_midi_parser_test_key_signature();
void ags_midi_parser_test_sequencer_meta_event();
void ags_midi_parser_test_text_event();
void ags_midi_parser_test_meta_misc();
void ags_midi_parser_test_midi_channel_prefix();
void ags_midi_parser_test_quarter_frame();
void ags_midi_parser_test_song_position();
void ags_midi_parser_test_song_select();
void ags_midi_parser_test_tune_request();
void ags_midi_parser_test_open_filename();
void ags_midi_parser_test_set_buffer();
void ags_midi_parser_test_set_file_length();

#define AGS_MIDI_PARSER_DEFAULT_BUFFER_SIZE (8192)

static guchar midi_buffer[AGS_MIDI_PARSER_DEFAULT_BUFFER_SIZE];

static gsize midi_file_length = 0;

static gint midi_key_on_000_offset = 0;
static gint midi_key_on_001_offset = 0;
static gint midi_key_on_002_offset = 0;

static gint midi_key_off_000_offset = 0;
static gint midi_key_off_001_offset = 0;
static gint midi_key_off_002_offset = 0;

static gint midi_key_pressure_002_offset = 0;

static gint midi_change_parameter_000_offset = 0;

static gint midi_pitch_bend_000_offset = 0;

static gint midi_change_program_000_offset = 0;

static gint midi_change_pressure_000_offset = 0;

static gint midi_smtpe_000_offset = 0;

static gint midi_tempo_000_offset = 0;

static gint midi_time_signature_000_offset = 0;

static gint midi_key_signature_000_offset = 0;

static gint midi_sequencer_meta_event_000_offset = 0;

static gint midi_quarter_frame_000_offset = 0;
static gint midi_quarter_frame_001_offset = 0;
static gint midi_quarter_frame_002_offset = 0;
static gint midi_quarter_frame_003_offset = 0;
static gint midi_quarter_frame_004_offset = 0;
static gint midi_quarter_frame_005_offset = 0;
static gint midi_quarter_frame_006_offset = 0;
static gint midi_quarter_frame_007_offset = 0;

static gint midi_song_position_000_offset = 0;

static gint midi_song_select_000_offset = 0;

static gint midi_tune_request_000_offset = 0;

static gint midi_end_of_track_000_offset = 0;
static gint midi_end_of_track_001_offset = 0;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_parser_test_init_suite()
{
  gint position;

  static guchar manufacturer_id[8] = {0,};

  manufacturer_id[0] = '\x00';
  manufacturer_id[1] = '\xca';
  manufacturer_id[2] = '\xfe';
  manufacturer_id[3] = '\xbe';
  
  memset(&(midi_buffer[0]), 0, AGS_MIDI_PARSER_DEFAULT_BUFFER_SIZE * sizeof(guchar));

  position = 0;
  
  ags_midi_smf_util_put_header(NULL,
			       midi_buffer,
			       0, 1,
			       1, 96);

  position = 14;

  /* track 000 */
  ags_midi_smf_util_put_track(NULL,
			      midi_buffer + position,
			      0);

  position += 8;
  
  /* key on/off - 000 */
  midi_key_on_000_offset = position;
  
  ags_midi_smf_util_put_key_on(NULL,
			       midi_buffer + position,
			       32,
			       0,
			       61,
			       127);

  position += 4;
  
  midi_key_off_000_offset = position;
  
  ags_midi_smf_util_put_key_off(NULL,
				midi_buffer + position,
				196,
				0,
				61,
				127);

  position += 5;
  
  /* key on/off - 001 */
  midi_key_on_001_offset = position;
  
  ags_midi_smf_util_put_key_on(NULL,
			       midi_buffer + position,
			       228,
			       0,
			       61,
			       127);
  
  position += 5;

  midi_key_off_001_offset = position;
  
  ags_midi_smf_util_put_key_off(NULL,
				midi_buffer + position,
				392,
				0,
				61,
				127);
  
  position += 5;

  /* key on/pressure/off - 002 */
  midi_key_on_002_offset = position;

  ags_midi_smf_util_put_key_on(NULL,
			       midi_buffer + position,
			       408,
			       0,
			       61,
			       127);
  
  position += 5;

  midi_key_pressure_002_offset = position;
  
  ags_midi_smf_util_put_key_pressure(NULL,
				     midi_buffer + position,
				     568,
				     0,
				     61,
				     127);
  
  position += 5;

  midi_key_off_002_offset = position;
  
  ags_midi_smf_util_put_key_off(NULL,
				midi_buffer + position,
				736,
				0,
				61,
				127);
  
  position += 5;

  /* change parameter */
  midi_change_parameter_000_offset = position;
  
  ags_midi_smf_util_put_change_parameter(NULL,
					 midi_buffer + position,
					 752,
					 0,
					 7,
					 64);
  
  position += 5;

  /* pitch bend */
  midi_pitch_bend_000_offset = position;
  
  ags_midi_smf_util_put_pitch_bend(NULL,
				   midi_buffer + position,
				   768,
				   0,
				   16384);
  
  position += 5;

  /* change program */
  midi_change_program_000_offset = position;
  
  ags_midi_smf_util_put_change_program(NULL,
				       midi_buffer + position,
				       784,
				       0,
				       0x0f);
  
  position += 4;

  /* change pressure */
  midi_change_pressure_000_offset = position;
  
  ags_midi_smf_util_put_change_pressure(NULL,
					midi_buffer + position,
					800,
					0,
					127);
  
  position += 4;

  /* end of track */
  midi_end_of_track_000_offset = position;
  
  ags_midi_smf_util_put_end_of_track(NULL,
				     midi_buffer + position,
				     816);

  position += 5;

  /* track 001 */
  ags_midi_smf_util_put_track(NULL,
			      midi_buffer + position,
			      0);

  position += 8;

  /* smtpe 000 */
  midi_smtpe_000_offset = position;
  
  ags_midi_smf_util_put_smtpe(NULL,
			      midi_buffer + position,
			      32,
			      25,
			      0,
			      0,
			      0,
			      0);
  
  position += 9;
  
  /* tempo 000 */
  midi_tempo_000_offset = position;
  
  ags_midi_smf_util_put_tempo(NULL,
			      midi_buffer + position,
			      48,
			      500000);
  
  position += 7;
  
  /* time signature 000 */
  midi_time_signature_000_offset = position;
  
  ags_midi_smf_util_put_time_signature(NULL,
				       midi_buffer + position,
				       64,
				       0x04,
				       0x02,
				       0x18,
				       0x08);
  
  position += 8;
  
  /* key signature 000 */
  midi_key_signature_000_offset = position;
  
  ags_midi_smf_util_put_key_signature(NULL,
				      midi_buffer + position,
				      82,
				      2,
				      0);
  
  position += 6;
  
  /* sequencer meta event 000 */
  midi_sequencer_meta_event_000_offset = position;
  
  ags_midi_smf_util_put_sequencer_meta_event_fixed(NULL,
						   midi_buffer + position,
						   108,
						   4,
						   ((0x00) | (0xaf << 8) | (0xfe << 16) | (0x0c << 24)),
						   (guchar *) &(manufacturer_id[0]));
  
  position += 8;
  
  /* quarter frame 000 */
  midi_quarter_frame_000_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x00,
				      0x0c);
  
  position += 3;
  
  /* quarter frame 001 */
  midi_quarter_frame_001_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x10,
				      0x00);
  
  position += 3;
  
  /* quarter frame 002 */
  midi_quarter_frame_002_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x20,
				      0x00);
  
  position += 3;
  
  /* quarter frame 003 */
  midi_quarter_frame_003_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x30,
				      0x03);
  
  position += 3;
  
  /* quarter frame 004 */
  midi_quarter_frame_004_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x40,
				      0x05);
  
  position += 3;
  
  /* quarter frame 005 */
  midi_quarter_frame_005_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x50,
				      0x01);
  
  position += 3;
  
  /* quarter frame 006 */
  midi_quarter_frame_006_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x60,
				      0x01);
  
  position += 3;
  
  /* quarter frame 007 */
  midi_quarter_frame_007_offset = position;
  
  ags_midi_smf_util_put_quarter_frame(NULL,
				      midi_buffer + position,
				      124,
				      0x70,
				      0x02);
  
  position += 3;
  
  /* song position 000 */
  midi_song_position_000_offset = position;
  
  ags_midi_smf_util_put_song_position(NULL,
				      midi_buffer + position,
				      140,
				      64);
  
  position += 5;
  
  /* song select 000 */
  midi_song_select_000_offset = position;
  
  ags_midi_smf_util_put_song_select(NULL,
				    midi_buffer + position,
				    156,
				    9);
  
  position += 4;

  /* tune request 000 */
  midi_tune_request_000_offset = position;
  
  ags_midi_smf_util_put_tune_request(NULL,
				     midi_buffer + position,
				     172);
  
  position += 3;

  /* end of track */
  midi_end_of_track_001_offset = position;
  
  ags_midi_smf_util_put_end_of_track(NULL,
				     midi_buffer + position,
				     816);

  position += 5;

  /* MIDI file length */
  midi_file_length = position + 1;
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_parser_test_clean_suite()
{
  return(0);
}

void
ags_midi_parser_test_read_gint16()
{
  AgsMidiParser *midi_parser;

  gint format;
  gint track_count;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* MThd - format */
  midi_parser->offset = 8;
  
  format = ags_midi_parser_read_gint16(midi_parser);

  CU_ASSERT(format == 1);

  /* MThd - track count */
  midi_parser->offset = 10;
  
  track_count = ags_midi_parser_read_gint16(midi_parser);

  CU_ASSERT(track_count == 1);
}

void
ags_midi_parser_test_read_gint24()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_read_gint32()
{
  AgsMidiParser *midi_parser;

  gint chunk_length;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* MThd - chunk length */
  midi_parser->offset = 4;
  
  chunk_length = ags_midi_parser_read_gint32(midi_parser);

  CU_ASSERT(chunk_length == 0);
}

void
ags_midi_parser_test_read_varlength()
{
  AgsMidiParser *midi_parser;

  gint delta_time;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* MTrk - note on */
  midi_parser->offset = 22;
  
  delta_time = ags_midi_parser_read_varlength(midi_parser);

  CU_ASSERT(delta_time == 32);
  
  /* MTrk - note off */
  midi_parser->offset = 26;
  
  delta_time = ags_midi_parser_read_varlength(midi_parser);

  CU_ASSERT(delta_time == 196);
}

void
ags_midi_parser_test_read_text()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_midi_getc()
{
  AgsMidiParser *midi_parser;

  gint c;
  guint i;
  gboolean success;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* MThd - midi getc */
  success = TRUE;
  
  for(i = 0; i < 14; i++){
    c = ags_midi_parser_midi_getc(midi_parser);

    if(c != midi_parser->buffer[i]){
      success = FALSE;

      break;
    }
  }    

  CU_ASSERT(success == TRUE && i == 14);
}

void
ags_midi_parser_test_parse_full()
{
  AgsMidiParser *midi_parser;

  xmlDoc *doc;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse full */
  doc = ags_midi_parser_parse_full(midi_parser);

  CU_ASSERT(doc != NULL);
}

void
ags_midi_parser_test_parse_bytes()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_parse_header()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse header */
  node = ags_midi_parser_parse_header(midi_parser);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == 14);
}

void
ags_midi_parser_test_parse_track()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse track */
  midi_parser->offset = 14;
  
  node = ags_midi_parser_parse_track(midi_parser);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == 22);
}

void
ags_midi_parser_test_channel_message()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_key_on()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse key-on */
  midi_parser->offset = midi_key_on_000_offset;
  
  node = ags_midi_parser_key_on(midi_parser, 0x90);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_key_on_000_offset + 4);
}

void
ags_midi_parser_test_key_off()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse key-off */
  midi_parser->offset = midi_key_off_000_offset;
  
  node = ags_midi_parser_key_off(midi_parser, 0x80);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_key_off_000_offset + 5);
}

void
ags_midi_parser_test_key_pressure()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse key-pressure */
  midi_parser->offset = midi_key_pressure_002_offset;
  
  node = ags_midi_parser_key_pressure(midi_parser, 0xa0);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_key_pressure_002_offset + 5);
}

void
ags_midi_parser_test_change_parameter()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse change parameter */
  midi_parser->offset = midi_change_parameter_000_offset;
  
  node = ags_midi_parser_key_pressure(midi_parser, 0xb0);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_change_parameter_000_offset + 5);
}

void
ags_midi_parser_test_change_pitch_bend()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse pitch bend */
  midi_parser->offset = midi_pitch_bend_000_offset;
  
  node = ags_midi_parser_change_pitch_bend(midi_parser, 0xe0);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_pitch_bend_000_offset + 5);
}

void
ags_midi_parser_test_change_program()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse change program */
  midi_parser->offset = midi_change_program_000_offset;
  
  node = ags_midi_parser_change_program(midi_parser, 0xc0);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_change_program_000_offset + 4);
}

void
ags_midi_parser_test_change_channel_pressure()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse change channel pressure */
  midi_parser->offset = midi_change_pressure_000_offset;
  
  node = ags_midi_parser_change_channel_pressure(midi_parser, 0xd0);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_change_pressure_000_offset + 4);
}

void
ags_midi_parser_test_not_defined()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_system_common()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_meta_event()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_sequence_number()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_end_of_track()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse end of track */
  midi_parser->offset = midi_end_of_track_000_offset;
  
  node = ags_midi_parser_end_of_track(midi_parser, 0x2f);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_end_of_track_000_offset + 5);
}

void
ags_midi_parser_test_smtpe()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse smtpe */
  midi_parser->offset = midi_smtpe_000_offset;
  
  node = ags_midi_parser_smtpe(midi_parser, 0x54);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_smtpe_000_offset + 9);
}

void
ags_midi_parser_test_tempo()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse tempo */
  midi_parser->offset = midi_tempo_000_offset;
  
  node = ags_midi_parser_tempo(midi_parser, 0x51);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_tempo_000_offset + 7);
}

void
ags_midi_parser_test_time_signature()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse time signature */
  midi_parser->offset = midi_time_signature_000_offset;
  
  node = ags_midi_parser_time_signature(midi_parser, 0x58);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_time_signature_000_offset + 8);
}

void
ags_midi_parser_test_key_signature()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse key signature */
  midi_parser->offset = midi_key_signature_000_offset;
  
  node = ags_midi_parser_key_signature(midi_parser, 0x59);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_key_signature_000_offset + 6);
}

void
ags_midi_parser_test_sequencer_meta_event()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse sequencer meta event */
  midi_parser->offset = midi_sequencer_meta_event_000_offset;
  
  node = ags_midi_parser_sequencer_meta_event(midi_parser, 0x7f);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_sequencer_meta_event_000_offset + 8);
}

void
ags_midi_parser_test_text_event()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_meta_misc()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_midi_channel_prefix()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_quarter_frame()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse quarter frame 000 */
  midi_parser->offset = midi_quarter_frame_000_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_000_offset + 3);

  /* parse quarter frame 001 */
  midi_parser->offset = midi_quarter_frame_001_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_001_offset + 3);

  /* parse quarter frame 002 */
  midi_parser->offset = midi_quarter_frame_002_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_002_offset + 3);

  /* parse quarter frame 003 */
  midi_parser->offset = midi_quarter_frame_003_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_003_offset + 3);

  /* parse quarter frame 004 */
  midi_parser->offset = midi_quarter_frame_004_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_004_offset + 3);

  /* parse quarter frame 005 */
  midi_parser->offset = midi_quarter_frame_005_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_005_offset + 3);

  /* parse quarter frame 006 */
  midi_parser->offset = midi_quarter_frame_006_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_006_offset + 3);

  /* parse quarter frame 007 */
  midi_parser->offset = midi_quarter_frame_007_offset;
  
  node = ags_midi_parser_quarter_frame(midi_parser, 0xf1);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_quarter_frame_007_offset + 3);
}

void
ags_midi_parser_test_song_position()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse song position */
  midi_parser->offset = midi_song_position_000_offset;
  
  node = ags_midi_parser_song_position(midi_parser, 0xf2);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_song_position_000_offset + 5);
}

void
ags_midi_parser_test_song_select()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse song select */
  midi_parser->offset = midi_song_select_000_offset;
  
  node = ags_midi_parser_song_select(midi_parser, 0xf3);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_song_select_000_offset + 4);
}

void
ags_midi_parser_test_tune_request()
{
  AgsMidiParser *midi_parser;

  xmlNode *node;

  midi_parser = ags_midi_parser_new();

  midi_parser->buffer = midi_buffer;

  midi_parser->file_length = midi_file_length;
  
  /* parse tune request */
  midi_parser->offset = midi_tune_request_000_offset;
  
  node = ags_midi_parser_tune_request(midi_parser, 0xf6);

  CU_ASSERT(node != NULL);
  CU_ASSERT(midi_parser->offset == midi_tune_request_000_offset + 3);
}

void
ags_midi_parser_test_open_filename()
{
  AgsMidiParser *midi_parser;

  midi_parser = ags_midi_parser_new();

  ags_midi_parser_open_filename(midi_parser,
				"/dev/null");
  
  //TODO:JK: implement me
}

void
ags_midi_parser_test_set_buffer()
{
  //TODO:JK: implement me
}

void
ags_midi_parser_test_set_file_length()
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
  pSuite = CU_add_suite("AgsMidiParserTest", ags_midi_parser_test_init_suite, ags_midi_parser_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMidiParser read gint16", ags_midi_parser_test_read_gint16) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser read gint24", ags_midi_parser_test_read_gint24) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser read gint32", ags_midi_parser_test_read_gint32) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser read varlength", ags_midi_parser_test_read_varlength) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser read text", ags_midi_parser_test_read_text) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser midi getc", ags_midi_parser_test_midi_getc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser parse full", ags_midi_parser_test_parse_full) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser parse bytes", ags_midi_parser_test_parse_bytes) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser parse header", ags_midi_parser_test_parse_header) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser parse track", ags_midi_parser_test_parse_track) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser channel message", ags_midi_parser_test_channel_message) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser key-on", ags_midi_parser_test_key_on) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser key-off", ags_midi_parser_test_key_off) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser key-pressure", ags_midi_parser_test_key_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser change parameter", ags_midi_parser_test_change_parameter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser change pitch bend", ags_midi_parser_test_change_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser change program", ags_midi_parser_test_change_program) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser change channel pressure", ags_midi_parser_test_change_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser not defined", ags_midi_parser_test_not_defined) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser sysex", ags_midi_parser_test_sysex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser system common", ags_midi_parser_test_system_common) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser meta event", ags_midi_parser_test_meta_event) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser sequence number", ags_midi_parser_test_sequence_number) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser end of track", ags_midi_parser_test_end_of_track) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser SMTPE", ags_midi_parser_test_smtpe) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser tempo", ags_midi_parser_test_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser time signature", ags_midi_parser_test_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser key signature", ags_midi_parser_test_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser sequencer meta event", ags_midi_parser_test_sequencer_meta_event) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser text event", ags_midi_parser_test_text_event) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser meta misc", ags_midi_parser_test_meta_misc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser midi channel prefix", ags_midi_parser_test_midi_channel_prefix) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser quarter frame", ags_midi_parser_test_quarter_frame) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser song position", ags_midi_parser_test_song_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser song select", ags_midi_parser_test_song_select) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser tune request", ags_midi_parser_test_tune_request) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser open filename", ags_midi_parser_test_open_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser set buffer", ags_midi_parser_test_set_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiParser set file length", ags_midi_parser_test_set_file_length) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
