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

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_port.h>

int ags_automation_test_init_suite();
int ags_automation_test_clean_suite();

void ags_automation_test_find_port();
void ags_automation_test_find_near_timestamp();
void ags_automation_test_add_acceleration();
void ags_automation_test_remove_accleration_at_position();
void ags_automation_test_is_acceleration_selected();
void ags_automation_test_find_point();
void ags_automation_test_find_region();
void ags_automation_test_free_selection();
void ags_automation_test_add_all_to_selection();
void ags_automation_test_add_point_to_selection();
void ags_automation_test_remove_point_from_selection();
void ags_automation_test_add_region_to_selection();
void ags_automation_test_remove_region_from_selection();
void ags_automation_test_copy_selection();
void ags_automation_test_cut_selection();
void ags_automation_test_insert_from_clipboard();
void ags_automation_test_get_current();
void ags_automation_test_get_specifier_unique();
void ags_automation_test_find_specifier();
void ags_automation_test_find_specifier_with_type_and_line();
void ags_automation_test_get_value();

#define AGS_AUTOMATION_TEST_CONTROL_NAME "./ags-test-control\0"

#define AGS_AUTOMATION_TEST_FIND_PORT_N_AUTOMATION (8)

#define AGS_AUTOMATION_TEST_FIND_NEAR_TIMESTAMP_N_AUTOMATION (8)

AgsAudio *audio;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_automation_test_init_suite()
{
  audio = ags_audio_new(NULL);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_automation_test_clean_suite()
{
  g_object_unref(audio);
  
  return(0);
}

void
ags_automation_test_find_port()
{
  AgsAutomation **automation;
  AgsPort **port;
  
  GList *list, *current;

  guint i;
  
  automation = (AgsAutomation **) malloc(AGS_AUTOMATION_TEST_FIND_PORT_N_AUTOMATION * sizeof(AgsAutomation *));
  port = (AgsAutomation **) malloc(AGS_AUTOMATION_TEST_FIND_PORT_N_AUTOMATION * sizeof(AgsPort *));
  list = NULL;

  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_PORT_N_AUTOMATION; i++){    
    /* nth automation */
    automation[i] = ags_automation_new(audio,
				       0,
				       AGS_TYPE_INPUT,
				       AGS_AUTOMATION_TEST_CONTROL_NAME);

    port[i] = ags_port_new();
    g_object_set(automation[i],
		 "port\0", port[i],
		 NULL);

    list = g_list_prepend(list,
			  automation[i]);
  }
  
  /* assert find */
  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_PORT_N_AUTOMATION; i++){
    current = list;
    current = ags_automation_find_port(current,
				       port[i]);

    CU_ASSERT(current != NULL && AGS_AUTOMATION(current->data)->port == port[i]);
  }  
}

void
ags_automation_test_find_near_timestamp()
{
  AgsAutomation **automation;
  AgsTimestamp *timestamp;

  GList *list, *current;

  guint i;
  
  automation = (AgsAutomation **) malloc(AGS_AUTOMATION_TEST_FIND_NEAR_TIMESTAMP_N_AUTOMATION * sizeof(AgsAutomation *));
  list = NULL;

  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_NEAR_TIMESTAMP_N_AUTOMATION; i++){
    /* nth automation */
    automation[i] = ags_automation_new(audio,
				       0,
				       AGS_TYPE_INPUT,
				       AGS_AUTOMATION_TEST_CONTROL_NAME);
    timestamp = ags_timestamp_new();
    g_object_set(automation[i],
		 "timestamp\0", timestamp,
		 NULL);

    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(automation[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_AUTOMATION_DEFAULT_DURATION);

    list = g_list_prepend(list,
			  automation[i]);
  }

  /* instantiate timestamp to check against */
  timestamp = ags_timestamp_new();
  
  /* assert find */
  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_NEAR_TIMESTAMP_N_AUTOMATION; i++){
    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(automation[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_AUTOMATION_DEFAULT_DURATION);
    current = ags_automation_find_near_timestamp(list, 0,
						 timestamp);

    CU_ASSERT(current != NULL && current->data == automation[i]);
  }  
}

void
ags_automation_test_add_acceleration()
{
  //TODO:JK: implement me
}

void
ags_automation_test_remove_accleration_at_position()
{
  //TODO:JK: implement me
}

void
ags_automation_test_is_acceleration_selected()
{
  //TODO:JK: implement me
}

void
ags_automation_test_find_point()
{
  //TODO:JK: implement me
}

void
ags_automation_test_find_region()
{
  //TODO:JK: implement me
}

void
ags_automation_test_free_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_add_all_to_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_add_point_to_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_remove_point_from_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_add_region_to_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_remove_region_from_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_copy_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_cut_selection()
{
  //TODO:JK: implement me
}

void
ags_automation_test_insert_from_clipboard()
{
  //TODO:JK: implement me
}

void
ags_automation_test_get_current()
{
  //TODO:JK: implement me
}

void
ags_automation_test_get_specifier_unique()
{
  //TODO:JK: implement me
}

void
ags_automation_test_find_specifier()
{
  //TODO:JK: implement me
}

void
ags_automation_test_find_specifier_with_type_and_line()
{
  //TODO:JK: implement me
}

void
ags_automation_test_get_value()
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
  pSuite = CU_add_suite("AgsAutomationTest\0", ags_automation_test_init_suite, ags_automation_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAutomation find port\0", ags_automation_test_find_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation find near timestamp\0", ags_automation_test_find_near_timestamp) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

