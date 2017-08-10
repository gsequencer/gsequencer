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

#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_0 (32)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_1 (48)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_2 (64)
#define AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE (1024)

#define AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_NOTE_0 (17)
#define AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_SIZE (1024)

#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_0 (32)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_1 (48)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_2 (64)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_SIZE (1024)

#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_0 (32)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_1 (48)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_2 (64)
#define AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE (1024)

#define AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_NOTE_0 (17)
#define AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_SIZE (1024)

#define AGS_LV2_PLUGIN_TEST_FIND_PNAME_SWH "swh"
#define AGS_LV2_PLUGIN_TEST_FIND_PNAME_INVADA "invada"
#define AGS_LV2_PLUGIN_TEST_FIND_PNAME_ZYN "zyn"

AgsLv2UriMapManager *uri_map_manager;
AgsLv2UridManager *urid_manager;

gboolean stub_change_program = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_plugin_test_init_suite()
{
  uri_map_manager = ags_lv2_uri_map_manager_get_instance();
  urid_manager = ags_lv2_urid_manager_get_instance();
  
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
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* note #1 */
  seq_event[1].type = SND_SEQ_EVENT_NOTEON;

  seq_event[1].data.note.channel = 0;
  seq_event[1].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_1;
  seq_event[1].data.note.velocity = 127;

  /* note #2 */
  seq_event[2].type = SND_SEQ_EVENT_NOTEON;

  seq_event[2].data.note.channel = 0;
  seq_event[2].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_2;
  seq_event[2].data.note.velocity = 127;

  /* event buffer and append midi */
  event_buffer = ags_lv2_plugin_alloc_event_buffer(AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE);
  
  ags_lv2_plugin_event_buffer_append_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE,
					  &seq_event,
					  3);

  /* remove midi and assert */
  ags_lv2_plugin_event_buffer_remove_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_1);

  offset = event_buffer->data;

  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_0);
  offset += (sizeof(LV2_Event) + 8);

  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_2);

  ags_lv2_plugin_event_buffer_remove_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_2);

  offset = event_buffer->data;

  CU_ASSERT(AGS_LV2_EVENT(offset)->type == id &&
	    ((uint8_t *) (offset + sizeof(LV2_Event)))[1] == AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_0);
  offset += (sizeof(LV2_Event) + 8);

  CU_ASSERT(((uint8_t *) (offset + sizeof(LV2_Event)))[1] == 0);

  ags_lv2_plugin_event_buffer_remove_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_SIZE,
					  AGS_LV2_PLUGIN_TEST_EVENT_BUFFER_REMOVE_MIDI_NOTE_0);

  offset = event_buffer->data;

  CU_ASSERT(((uint8_t *) (offset + sizeof(LV2_Event)))[1] == 0);
}

void
ags_lv2_plugin_test_clear_event_buffer()
{
  LV2_Event_Buffer *event_buffer;

  snd_seq_event_t seq_event[1];

  void *offset;

  memset(&seq_event, 0, sizeof(snd_seq_event_t));

  /* note #0 */
  seq_event[0].type = SND_SEQ_EVENT_NOTEON;

  seq_event[0].data.note.channel = 0;
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* event buffer and append midi */
  event_buffer = ags_lv2_plugin_alloc_event_buffer(AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_SIZE);
  
  ags_lv2_plugin_event_buffer_append_midi(event_buffer,
					  AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_SIZE,
					  &seq_event,
					  1);

  /* clear and assert */
  ags_lv2_plugin_clear_event_buffer(event_buffer,
				    AGS_LV2_PLUGIN_TEST_CLEAR_EVENT_BUFFER_SIZE);
  
  offset = event_buffer->data;

  CU_ASSERT(((uint8_t *) (offset + sizeof(LV2_Event)))[1] == 0);
}

void
ags_lv2_plugin_test_concat_atom_sequence()
{
  //TODO:JK: not implemented
}

void
ags_lv2_plugin_test_atom_sequence_append_midi()
{
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev;

  snd_seq_event_t seq_event[3];

  uint32_t id;

  id = ags_lv2_urid_manager_map(NULL,
				LV2_MIDI__MidiEvent);

  memset(&seq_event, 0, 3 * sizeof(snd_seq_event_t));

  /* note #0 */
  seq_event[0].type = SND_SEQ_EVENT_NOTEON;

  seq_event[0].data.note.channel = 0;
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* note #1 */
  seq_event[1].type = SND_SEQ_EVENT_NOTEON;

  seq_event[1].data.note.channel = 0;
  seq_event[1].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_1;
  seq_event[1].data.note.velocity = 127;

  /* note #2 */
  seq_event[2].type = SND_SEQ_EVENT_NOTEON;

  seq_event[2].data.note.channel = 0;
  seq_event[2].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_2;
  seq_event[2].data.note.velocity = 127;

  /* atom sequence */
  aseq = ags_lv2_plugin_alloc_atom_sequence(AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_SIZE);
  aev = (LV2_Atom_Event *) ((char *) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));
  
  /* append midi and assert */
  ags_lv2_plugin_atom_sequence_append_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_SIZE,
					   &seq_event,
					   3);
  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_0);
  aev += ((3 + 7) & (~7));

  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_1);
  aev += ((3 + 7) & (~7));
  
  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_APPEND_MIDI_NOTE_2);
}

void
ags_lv2_plugin_test_atom_sequence_remove_midi()
{
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev;

  snd_seq_event_t seq_event[3];
  
  uint32_t id;

  id = ags_lv2_urid_manager_map(NULL,
				LV2_MIDI__MidiEvent);

  memset(&seq_event, 0, 3 * sizeof(snd_seq_event_t));

  /* note #0 */
  seq_event[0].type = SND_SEQ_EVENT_NOTEON;

  seq_event[0].data.note.channel = 0;
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* note #1 */
  seq_event[1].type = SND_SEQ_EVENT_NOTEON;

  seq_event[1].data.note.channel = 0;
  seq_event[1].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_1;
  seq_event[1].data.note.velocity = 127;

  /* note #2 */
  seq_event[2].type = SND_SEQ_EVENT_NOTEON;

  seq_event[2].data.note.channel = 0;
  seq_event[2].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_2;
  seq_event[2].data.note.velocity = 127;

  /* atom sequence */
  aseq = ags_lv2_plugin_alloc_atom_sequence(AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE);
  
  ags_lv2_plugin_atom_sequence_append_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE,
					   &seq_event,
					   3);

  /* remove midi and assert */
  ags_lv2_plugin_atom_sequence_remove_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_1);
  
  aev = (LV2_Atom_Event *) ((char *) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));

  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_0);
  aev += ((3 + 7) & (~7));

  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_2);

  ags_lv2_plugin_atom_sequence_remove_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_2);

  aev = (LV2_Atom_Event *) ((char *) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));

  CU_ASSERT(aev->body.type == id &&
	    ((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_0);
  aev += ((3 + 7) & (~7));

  CU_ASSERT(((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == 0);

  ags_lv2_plugin_atom_sequence_remove_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_SIZE,
					   AGS_LV2_PLUGIN_TEST_ATOM_SEQUENCE_REMOVE_MIDI_NOTE_0);

  aev = (LV2_Atom_Event *) ((char *) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));

  CU_ASSERT(((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == 0);
}

void
ags_lv2_plugin_test_clear_atom_sequence()
{
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev;

  snd_seq_event_t seq_event[1];

  uint32_t id;

  id = ags_lv2_urid_manager_map(NULL,
				LV2_MIDI__MidiEvent);

  memset(&seq_event, 0, sizeof(snd_seq_event_t));

  /* note #0 */
  seq_event[0].type = SND_SEQ_EVENT_NOTEON;

  seq_event[0].data.note.channel = 0;
  seq_event[0].data.note.note = 0x7f & AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_NOTE_0;
  seq_event[0].data.note.velocity = 127;

  /* atom sequence */
  aseq = ags_lv2_plugin_alloc_atom_sequence(AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_SIZE);
  
  ags_lv2_plugin_atom_sequence_append_midi(aseq,
					   AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_SIZE,
					   &seq_event,
					   1);
  
  /* clear and assert */
  ags_lv2_plugin_clear_atom_sequence(aseq,
				     AGS_LV2_PLUGIN_TEST_CLEAR_ATOM_SEQUENCE_SIZE);
  
  aev = (LV2_Atom_Event *) ((char *) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));

  CU_ASSERT(((uint8_t *) (LV2_ATOM_BODY(&(aev->body))))[1] == 0);
}

void
ags_lv2_plugin_test_find_pname()
{
  AgsLv2Plugin *lv2_plugin[3];
  
  GList *list, *current;

  list = NULL;
  
  lv2_plugin[0] = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
						"pname", AGS_LV2_PLUGIN_TEST_FIND_PNAME_SWH,
						NULL);
  list = g_list_prepend(list,
			lv2_plugin[0]);
  
  lv2_plugin[1] = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
						"pname", AGS_LV2_PLUGIN_TEST_FIND_PNAME_INVADA,
						NULL);
  list = g_list_prepend(list,
			lv2_plugin[1]);
  
  lv2_plugin[2] = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
						"pname", AGS_LV2_PLUGIN_TEST_FIND_PNAME_ZYN,
						NULL);
  list = g_list_prepend(list,
			lv2_plugin[2]);

  /* assert */
  CU_ASSERT((current = ags_lv2_plugin_find_pname(list, AGS_LV2_PLUGIN_TEST_FIND_PNAME_SWH)) != NULL &&
	    current->data == lv2_plugin[0]);

  CU_ASSERT((current = ags_lv2_plugin_find_pname(list, AGS_LV2_PLUGIN_TEST_FIND_PNAME_INVADA)) != NULL &&
	    current->data == lv2_plugin[1]);

  CU_ASSERT((current = ags_lv2_plugin_find_pname(list, AGS_LV2_PLUGIN_TEST_FIND_PNAME_ZYN)) != NULL &&
	    current->data == lv2_plugin[2]);
}

void
ags_lv2_plugin_test_change_program()
{
  AgsLv2Plugin *lv2_plugin;

  gpointer ptr;

  lv2_plugin = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
					     NULL);

  /* stub */
  ptr = AGS_LV2_PLUGIN_GET_CLASS(lv2_plugin)->change_program;

  AGS_LV2_PLUGIN_GET_CLASS(lv2_plugin)->change_program = ags_lv2_plugin_test_stub_change_program;

  /* assert */
  ags_lv2_plugin_change_program(lv2_plugin,
				NULL,
				0,
				0);
  
  CU_ASSERT(stub_change_program == TRUE);

  /* reset */
  AGS_LV2_PLUGIN_GET_CLASS(lv2_plugin)->change_program = ptr;
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
