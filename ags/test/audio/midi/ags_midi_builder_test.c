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

#include <libags.h>
#include <libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_midi_builder_test_init_suite();
int ags_midi_builder_test_clean_suite();

AgsMidiBuilder* ags_midi_builder_test_create_default(guint n_tracks);

void ags_midi_builder_test_append_header();
void ags_midi_builder_test_append_track();
void ags_midi_builder_test_append_key_on();
void ags_midi_builder_test_append_key_off();
void ags_midi_builder_test_append_key_pressure();
void ags_midi_builder_test_append_change_parameter();
void ags_midi_builder_test_append_change_pitch_bend();
void ags_midi_builder_test_append_change_program();
void ags_midi_builder_test_append_change_pressure();
void ags_midi_builder_test_append_sysex();
void ags_midi_builder_test_append_quarter_frame();
void ags_midi_builder_test_append_song_position();
void ags_midi_builder_test_append_song_select();
void ags_midi_builder_test_append_tune_request();
void ags_midi_builder_test_append_sequence_number();
void ags_midi_builder_test_append_smtpe();
void ags_midi_builder_test_append_time_signature();
void ags_midi_builder_test_append_key_signature();
void ags_midi_builder_test_append_sequencer_meta_event();
void ags_midi_builder_test_append_text_event();

#define AGS_MIDI_BUILDER_TEST_DEFAULT_SEQUENCE_PREFIX "ags-sequence-\0"

#define AGS_MIDI_BUILDER_TEST_DEFAULT_FORMAT (1)
#define AGS_MIDI_BUILDER_TEST_DEFAULT_TRACK_COUNT (0)
#define AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION (0xe250)
#define AGS_MIDI_BUILDER_TEST_DEFAULT_TIMES (30)
#define AGS_MIDI_BUILDER_TEST_DEFAULT_BPM (120)
#define AGS_MIDI_BUILDER_TEST_DEFAULT_CLICKS (4)

#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_FORMAT (1)
#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TRACK_COUNT (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_DIVISION (0xe250)
#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TIMES (30)
#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_BPM (120)
#define AGS_MIDI_BUILDER_TEST_APPEND_HEADER_CLICKS (4)

#define AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0 "ags-sequence-0\0"
#define AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1 "ags-sequence-1\0"

#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_NOTE_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_CHANNEL (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_VELOCITY (127)

#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_NOTE_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_CHANNEL (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_VELOCITY (127)

#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_NOTE_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_CHANNEL (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_VELOCITY (127)

#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CONTROL_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CHANNEL (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_VALUE (127)

#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PITCH_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_CHANNEL (0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRANSMITTER (127)

#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PROGRAM_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_CHANNEL (0)

#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_TRACK_COUNT (7)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PRESSURE_COUNT (64)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PULSE_UNIT (16.0 * AGS_MIDI_BUILDER_TEST_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION) * 1000000.0)
#define AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_CHANNEL (0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_builder_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_builder_test_clean_suite()
{
  return(0);
}

AgsMidiBuilder*
ags_midi_builder_test_create_default(guint n_tracks)
{
  AgsMidiBuilder *midi_builder;

  gchar *str;
  
  guint i;

  midi_builder = ags_midi_builder_new(NULL);

  /* default header */
  ags_midi_builder_append_header(midi_builder,
				 6, AGS_MIDI_BUILDER_TEST_DEFAULT_FORMAT,
				 n_tracks, AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION,
				 AGS_MIDI_BUILDER_TEST_DEFAULT_TIMES, AGS_MIDI_BUILDER_TEST_DEFAULT_BPM,
				 AGS_MIDI_BUILDER_TEST_DEFAULT_CLICKS);

  for(i = 0; i < n_tracks; i++){
    str = g_strdup_printf("%s%d\0",
			  AGS_MIDI_BUILDER_TEST_DEFAULT_SEQUENCE_PREFIX,
			  i);
    
    ags_midi_builder_append_track(midi_builder,
				  str);

    free(str);
  }

  midi_builder->current_midi_track = midi_builder->midi_track->data;

  return(midi_builder);
}

void
ags_midi_builder_test_append_header()
{
  AgsMidiBuilder *midi_builder;

  glong offset, format;
  glong track_count, division;
    
  midi_builder = ags_midi_builder_new(NULL);

  /* append header */
  ags_midi_builder_append_header(midi_builder,
				 6, AGS_MIDI_BUILDER_TEST_APPEND_HEADER_FORMAT,
				 AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TRACK_COUNT, AGS_MIDI_BUILDER_TEST_APPEND_HEADER_DIVISION,
				 AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TIMES, AGS_MIDI_BUILDER_TEST_APPEND_HEADER_BPM,
				 AGS_MIDI_BUILDER_TEST_APPEND_HEADER_CLICKS);

  /* assert struct present */
  CU_ASSERT(midi_builder->midi_header != NULL);

  CU_ASSERT(midi_builder->midi_header->offset == 6);
  CU_ASSERT(midi_builder->midi_header->format == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_FORMAT);
  CU_ASSERT(midi_builder->midi_header->count == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TRACK_COUNT);
  CU_ASSERT(midi_builder->midi_header->division == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_DIVISION);
  CU_ASSERT(midi_builder->midi_header->times == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TIMES);
  CU_ASSERT(midi_builder->midi_header->beat == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_BPM);
  CU_ASSERT(midi_builder->midi_header->clicks == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_CLICKS);

  /* build */
  ags_midi_builder_build(midi_builder);

  CU_ASSERT(midi_builder->data != NULL);
  CU_ASSERT(midi_builder->length == 14);

  /* assert data */  
  ags_midi_buffer_util_get_header(midi_builder->data,
				  &offset, &format,
				  &track_count, &division);

  CU_ASSERT(offset == 6);
  CU_ASSERT(format == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_FORMAT);
  CU_ASSERT(track_count == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_TRACK_COUNT);
  CU_ASSERT(division == AGS_MIDI_BUILDER_TEST_APPEND_HEADER_DIVISION);
}

void
ags_midi_builder_test_append_track()
{
  AgsMidiBuilder *midi_builder;

  unsigned char *offset;

  gchar *text;

  guint length;
  glong delta_time;
  glong text_length;
  
  midi_builder = ags_midi_builder_new(NULL);

  /* default header */
  ags_midi_builder_append_header(midi_builder,
				 6, AGS_MIDI_BUILDER_TEST_DEFAULT_FORMAT,
				 AGS_MIDI_BUILDER_TEST_DEFAULT_TRACK_COUNT, AGS_MIDI_BUILDER_TEST_DEFAULT_DIVISION,
				 AGS_MIDI_BUILDER_TEST_DEFAULT_TIMES, AGS_MIDI_BUILDER_TEST_DEFAULT_BPM,
				 AGS_MIDI_BUILDER_TEST_DEFAULT_CLICKS);

  /* add track 0 */
  ags_midi_builder_append_track(midi_builder,
				AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0);

  CU_ASSERT(g_list_length(midi_builder->midi_track) == 1);

  CU_ASSERT(strcmp(midi_builder->current_midi_track->track_name,
		   AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0) == 0);

  /* add track 1 */
  ags_midi_builder_append_track(midi_builder,
				AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1);

  CU_ASSERT(g_list_length(midi_builder->midi_track) == 2);

  CU_ASSERT(strcmp(midi_builder->current_midi_track->track_name,
		   AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1) == 0);

  /* build */
  ags_midi_builder_build(midi_builder);

  length = (14 +
	    (2 * (4 + 4 + 1 + 3)) +
	    (2 * (1 + 3)) +
	    strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0) +
	    strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1));
  
  CU_ASSERT(midi_builder->data != NULL);
  CU_ASSERT(midi_builder->length == length);

  /* assert sequence 0 */
  offset = midi_builder->data;
  offset += (14 + 8);

  ags_midi_buffer_util_get_text_event(offset,
				      &delta_time,
				      &text, &text_length);
  
  
  CU_ASSERT(delta_time == 0);
  CU_ASSERT(text != NULL &&
	    strncmp(text,
		    AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0,
		    strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0)) == 0);
  CU_ASSERT(text_length == strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0));

  /* assert sequence 1 */
  offset += ((4 + 4 + 1 + 3) +
	     (1 + 3) +
	     strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0));

  ags_midi_buffer_util_get_text_event(offset,
				      &delta_time,
				      &text, &text_length);
  
  
  CU_ASSERT(delta_time == 0);
  CU_ASSERT(text != NULL &&
	    strncmp(text,
		    AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1,
		    strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_0)) == 0);
  CU_ASSERT(text_length == strlen(AGS_MIDI_BUILDER_TEST_APPEND_TRACK_SEQUENCE_1));
}

void
ags_midi_builder_test_append_key_on()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;
  guint keys[AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_NOTE_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong key;
  glong velocity;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_NOTE_COUNT; j++){
      keys[i][j] = rand() % 128;
      
      ags_midi_builder_append_key_on(midi_builder,
				     AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_PULSE_UNIT,
				     AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_CHANNEL,
				     keys[i][j],
				     AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_VELOCITY);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_ON_NOTE_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_key_on(offset,
						 &delta_time,
						 &channel,
						 &key,
						 &velocity);

      if(key != keys[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_key_off()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;
  guint keys[AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_NOTE_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong key;
  glong velocity;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_NOTE_COUNT; j++){
      keys[i][j] = rand() % 128;
      
      ags_midi_builder_append_key_off(midi_builder,
				      AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_PULSE_UNIT,
				      AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_CHANNEL,
				      keys[i][j],
				      AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_VELOCITY);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_OFF_NOTE_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_key_off(offset,
						  &delta_time,
						  &channel,
						  &key,
						  &velocity);

      if(key != keys[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_key_pressure()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;
  guint keys[AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_NOTE_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong key;
  glong velocity;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_NOTE_COUNT; j++){
      keys[i][j] = rand() % 128;
      
      ags_midi_builder_append_key_pressure(midi_builder,
					   AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_PULSE_UNIT,
					   AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_CHANNEL,
					   keys[i][j],
					   AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_VELOCITY);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_KEY_PRESSURE_NOTE_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_key_pressure(offset,
						       &delta_time,
						       &channel,
						       &key,
						       &velocity);

      if(key != keys[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_change_parameter()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;

  guint controls[AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CONTROL_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong control;
  glong value;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CONTROL_COUNT; j++){
      controls[i][j] = rand() % 128;
      
      ags_midi_builder_append_change_parameter(midi_builder,
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_PULSE_UNIT,
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CHANNEL,
					       controls[i][j],
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_VALUE);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PARAMETER_CONTROL_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_change_parameter(offset,
							   &delta_time,
							   &channel,
							   &control,
							   &value);

      if(control != controls[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_change_pitch_bend()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;

  guint pitchs[AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PITCH_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong pitch;
  glong transmitter;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PITCH_COUNT; j++){
      pitchs[i][j] = rand() % 128;
      
      ags_midi_builder_append_change_pitch_bend(midi_builder,
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PULSE_UNIT,
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_CHANNEL,
					       pitchs[i][j],
					       AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRANSMITTER);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PITCH_BEND_PITCH_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_pitch_bend(offset,
						     &delta_time,
						     &channel,
						     &pitch,
						     &transmitter);

      if(pitch != pitchs[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_change_program()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;

  guint programs[AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PROGRAM_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong program;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PROGRAM_COUNT; j++){
      programs[i][j] = rand() % 128;
      
      ags_midi_builder_append_change_program(midi_builder,
					     AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PULSE_UNIT,
					     AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_CHANNEL,
					     programs[i][j]);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PROGRAM_PROGRAM_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_change_program(offset,
							 &delta_time,
							 &channel,
							 &program);

      if(program != programs[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_change_pressure()
{
  AgsMidiBuilder *midi_builder;

  GList *current_midi_track;

  unsigned char *offset;

  guint pressures[AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_TRACK_COUNT][AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PRESSURE_COUNT];

  guint ret_size;
  glong delta_time;
  glong channel;
  glong pressure;
  guint i, j;
  gboolean success;
  
  midi_builder = ags_midi_builder_test_create_default(AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_TRACK_COUNT);

  current_midi_track = midi_builder->midi_track;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_TRACK_COUNT; i++){
    midi_builder->current_midi_track = current_midi_track->data;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PRESSURE_COUNT; j++){
      pressures[i][j] = rand() % 128;
      
      ags_midi_builder_append_change_pressure(midi_builder,
					      AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PULSE_UNIT,
					      AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_CHANNEL,
					      pressures[i][j]);
    }

    current_midi_track = current_midi_track->next;
  }

  /* build */
  ags_midi_builder_build(midi_builder);

  /* assert */
  offset = midi_builder->data;
  offset += 14;
  
  success = TRUE;

  for(i = 0; i < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_TRACK_COUNT; i++){
    offset += 8;

    ret_size = ags_midi_buffer_util_get_text_event(offset,
						   &delta_time,
						   NULL, NULL);

    if(delta_time != 0){
      success = FALSE;
      
      break;
    }
    
    offset += ret_size;
    
    for(j = 0; j < AGS_MIDI_BUILDER_TEST_APPEND_CHANGE_PRESSURE_PRESSURE_COUNT; j++){
      ret_size = ags_midi_buffer_util_get_change_pressure(offset,
							  &delta_time,
							  &channel,
							  &pressure);

      if(pressure != pressures[i][j]){
	success = FALSE;

	break;
      }

      offset += ret_size;
    }
    
    ret_size = ags_midi_buffer_util_get_end_of_track(offset,
						     NULL);
    offset += ret_size;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_builder_test_append_sysex()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_quarter_frame()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_song_position()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_song_select()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_tune_request()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_sequence_number()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_smtpe()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_time_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_key_signature()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_sequencer_meta_event()
{
  //TODO:JK: implement me
}

void
ags_midi_builder_test_append_text_event()
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
  pSuite = CU_add_suite("AgsMidiBuilderTest\0", ags_midi_builder_test_init_suite, ags_midi_builder_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMidiBuilder to append header\0", ags_midi_builder_test_append_header) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append track\0", ags_midi_builder_test_append_track) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append key on\0", ags_midi_builder_test_append_key_on) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append key off\0", ags_midi_builder_test_append_key_off) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append key pressure\0", ags_midi_builder_test_append_key_pressure) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append change parameter\0", ags_midi_builder_test_append_change_parameter) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append change pitch bend\0", ags_midi_builder_test_append_change_pitch_bend) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append change program\0", ags_midi_builder_test_append_change_program) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append change pressure\0", ags_midi_builder_test_append_change_pressure) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append sysex\0", ags_midi_builder_test_append_sysex) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append quarter frame\0", ags_midi_builder_test_append_quarter_frame) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append song position\0", ags_midi_builder_test_append_song_position) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append song select\0", ags_midi_builder_test_append_song_select) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append tune request\0", ags_midi_builder_test_append_tune_request) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append sequence number\0", ags_midi_builder_test_append_sequence_number) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append smtpe\0", ags_midi_builder_test_append_smtpe) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append time signature\0", ags_midi_builder_test_append_time_signature) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append key signature\0", ags_midi_builder_test_append_key_signature) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append sequencer meta event\0", ags_midi_builder_test_append_sequencer_meta_event) == NULL) || 
     (CU_add_test(pSuite, "test of AgsMidiBuilder to append text event\0", ags_midi_builder_test_append_text_event) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


