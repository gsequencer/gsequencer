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

gboolean stub_change_program = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_plugin_test_init_suite()
{
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

  length[0] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_0;
  length[1] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_1;
  length[2] = AGS_LV2_PLUGIN_TEST_CONCAT_EVENT_BUFFER_LENGTH_2;
  
  event_buffer[0] = ags_lv2_plugin_event_buffer_alloc(length[0]);  
  event_buffer[1] = ags_lv2_plugin_event_buffer_alloc(length[1]);
  event_buffer[2] = ags_lv2_plugin_event_buffer_alloc(length[2]);

  ptr = ags_lv2_plugin_event_buffer_concat(event_buffer[0],
					   event_buffer[1],
					   event_buffer[2],
					   NULL);

  CU_ASSERT(AGS_LV2_EVENT_BUFFER(ptr)->capacity == length[0] &&
	    AGS_LV2_EVENT_BUFFER(ptr + sizeof(LV2_Event_Buffer))->capacity == length[1] &&
	    AGS_LV2_EVENT_BUFFER(ptr + (2 * sizeof(LV2_Event_Buffer)))->capacity == length[2]);
}

void
ags_lv2_plugin_test_event_buffer_append_midi()
{
  LV2_Event_Buffer *event_buffer;

  
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsLv2PluginTest\0", ags_lv2_plugin_test_init_suite, ags_lv2_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2Plugin concat event buffer\0", ags_lv2_plugin_test_concat_event_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer concat\0", ags_lv2_plugin_test_event_buffer_concat) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer append midi\0", ags_lv2_plugin_test_event_buffer_append_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin event buffer remove midi\0", ags_lv2_plugin_test_event_buffer_remove_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin clear event buffer\0", ags_lv2_plugin_test_clear_event_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin concat atom sequence\0", ags_lv2_plugin_test_concat_atom_sequence) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin atom sequence append midi\0", ags_lv2_plugin_test_atom_sequence_append_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin atom sequence remove midi\0", ags_lv2_plugin_test_atom_sequence_remove_midi) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin clear atom sequence\0", ags_lv2_plugin_test_clear_atom_sequence) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin find pname\0", ags_lv2_plugin_test_find_pname) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Plugin change program\0", ags_lv2_plugin_test_change_program) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
