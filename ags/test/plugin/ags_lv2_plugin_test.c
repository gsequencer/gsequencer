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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_lv2_plugin_test_init_suite();
int ags_lv2_plugin_test_clean_suite();

void ags_lv2_plugin_test_concat_event_buffer();
void ags_lv2_plugin_test_event_buffer_concat();
void ags_lv2_plugin_test_event_buffer_append_midi();
void ags_lv2_plugin_test_event_buffer_remove_midi();
void ags_lv2_plugin_test_clear_event_buffer();
void ags_lv2_plugin_test_concat_atom_sequence();
void ags_lv2_plugin_test_atom_sequence_append_midi();
void ags_lv2_plugin_test_atom_sequence_remove_midi();
void ags_lv2_plugin_test_clear_atom_sequence();
void ags_lv2_plugin_test_find_pname();
void ags_lv2_plugin_test_change_program();

void ags_lv2_plugin_test_stub_change_program(AgsLv2Plugin *lv2_plugin,
					     gpointer lv2_handle,
					     guint bank_index,
					     guint program_index);

#define AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_0 (1024 * sizeof(char))
#define AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_1 (2048 * sizeof(char))
#define AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_2 (512 * sizeof(char))

#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_0 (1024 * sizeof(char))
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_1 (2048 * sizeof(char))
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_2 (512 * sizeof(char))

#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_0 (32)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_1 (48)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_2 (64)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_SIZE (1024)

AgsLv2UriMapManager *uri_map_manager;

gboolean stub_change_program = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_plugin_test_init_suite()
{
  uri_map_manager = ags_lv2_uri_map_manager_get_instance();
  
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_plugin_test_clean_suite()
{
  return(0);
}

void
ags_lv2_plugin_test_concat_event_buffer()
{
  LV2_Event_Buffer *event_buffer[3];
  guint length[3];
  void *ptr;

  length[0] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_0;
  length[1] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_1;
  length[2] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_2;
  
  event_buffer[0] = ags_lv2_plugin_alloc_event_buffer(length[0]);  
  event_buffer[1] = ags_lv2_plugin_alloc_event_buffer(length[1]);
  event_buffer[2] = ags_lv2_plugin_alloc_event_buffer(length[2]);
  
  ptr = ags_lv2_plugin_concat_event_buffer(event_buffer[0],
					   event_buffer[1],
					   event_buffer[2],
					   NULL);

  CU_ASSERT(AGS_LV2_EVENT_BUFFER(ptr)->capacity == length[0] &&
	    AGS_LV2_EVENT_BUFFER(ptr + length[0] + sizeof(LV2_Event_Buffer))->capacity == length[1] &&
	    AGS_LV2_EVENT_BUFFER(ptr + length[0] + length[1] + (2 * sizeof(LV2_Event_Buffer)))->capacity == length[2]);
}

void
ags_lv2_plugin_test_event_buffer_concat()
{
  LV2_Event_Buffer *event_buffer[3];
  guint length[3];
  void *ptr;

  length[0] = AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_0;
  length[1] = AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_1;
  length[2] = AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_CONCAT_LENGTH_2;
  
  event_buffer[0] = ags_lv2_plugin_event_buffer_alloc(length[0]);  
  event_buffer[1] = ags_lv2_plugin_event_buffer_alloc(length[1]);
  event_buffer[2] = ags_lv2_plugin_event_buffer_alloc(length[2]);

  ptr = ags_lv2_plugin_event_buffer_concat(event_buffer[0],
					   event_buffer[1],
					   event_buffer[2],
					   NULL);

  CU_ASSERT(AGS_LV2_EVENT_BUFFER(ptr)->capacity == length[0]);
  ptr += sizeof(LV2_Event_Buffer);
  
  CU_ASSERT(AGS_LV2_EVENT_BUFFER(ptr)->capacity == length[1]);
  ptr += sizeof(LV2_Event_Buffer);

  CU_ASSERT(AGS_LV2_EVENT_BUFFER(ptr)->capacity == length[2]);
}

void
ags_lv2_plugin_test_event_buffer_append_midi()
{
  LV2_Event_Buffer *event_buffer;

  snd_seq_event_t seq_event[3];

  void *offset;
  
  uint32_t id;

  id = ags_lv2_uri_map_manager_uri_to_id(NULL,
					 LV2_EVENT_URI,
					 LV2_MIDI__MidiEvent);
  
  memset(&seq_event, 0, 3 * sizeof(snd_seq_event_t));

  /* note #0 */
  seq_event[0].type = SND_SEQ_EVENT_NOTEON;

  seq_event[0].data.note.channel = 0;
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* note #1 */
  seq_event[1].type = SND_SEQ_EVENT_NOTEON;

  seq_event[1].data.note.channel = 0;
  seq_event[1].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_1;
  seq_event[1].data.note.velocity = 127;

  /* note #2 */
  seq_event[2].type = SND_SEQ_EVENT_NOTEON;

  seq_event[2].data.note.channel = 0;
  seq_event[2].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_2;
  seq_event[2].data.note.velocity = 127;

  /* event buffer */
  event_buffer = ags_lv2_plugin_alloc_event_buffer(AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_SIZE);
  offset = event_buffer->data;
  
  /* append midi and assert */
  ags_lv2_plugin_event_buffer_append_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_SIZE,
					  &seq_event,
					  3);
  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_0);
  offset += (sizeof(LV2_Event) + 8);
  
  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_1);
  offset += (sizeof(LV2_Event) + 8);

  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_APPEND_MIDI_NOTE_2);
}

void
ags_lv2_plugin_test_event_buffer_remove_midi()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_clear_event_buffer()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_concat_atom_sequence()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_atom_sequence_append_midi()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_atom_sequence_remove_midi()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_clear_atom_sequence()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_find_pname()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_change_program()
{
  //TODO:JK: implement me
}

void
ags_lv2_plugin_test_stub_change_program(AgsLv2Plugin *lv2_plugin,
					gpointer lv2_handle,
					guint bank_index,
					guint program_index)
{
  stub_change_program = TRUE;
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
  pSuite = CU_add_suite("AgsLv2PluginTest", ags_lv2_plugin_test_init_suite, ags_lv2_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2Plugin concat event buffer", ags_lv2_plugin_test_concat_event_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer concat", ags_lv2_plugin_test_event_buffer_concat) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer append midi", ags_lv2_plugin_test_event_buffer_append_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer remove midi", ags_lv2_plugin_test_event_buffer_remove_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin clear event buffer", ags_lv2_plugin_test_clear_event_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin concat atom sequence", ags_lv2_plugin_test_concat_atom_sequence) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin atom sequence append midi", ags_lv2_plugin_test_atom_sequence_append_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin atom sequence remove midi", ags_lv2_plugin_test_atom_sequence_remove_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin clear atom sequence", ags_lv2_plugin_test_clear_atom_sequence) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin find pname", ags_lv2_plugin_test_find_pname) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin change program", ags_lv2_plugin_test_change_program) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
