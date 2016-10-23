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
void ags_automation_test_remove_acceleration_at_position();
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

#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_HEIGHT (128)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_STEPS (127)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_ADD_ACCELERATION_COUNT (1024)

#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_HEIGHT (128)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_STEPS (127)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT (1024)

#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_WIDTH (4096)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_HEIGHT (88)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_STEPS (127)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_LOWER (0.0)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_UPPER (127.0)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_COUNT (1024)
#define AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_SELECTION_COUNT (128)

#define AGS_AUTOMATION_TEST_FIND_POINT_WIDTH (4096)
#define AGS_AUTOMATION_TEST_FIND_POINT_HEIGHT (128)
#define AGS_AUTOMATION_TEST_FIND_POINT_STEPS (127)
#define AGS_AUTOMATION_TEST_FIND_POINT_LOWER (0.0)
#define AGS_AUTOMATION_TEST_FIND_POINT_UPPER (127.0)
#define AGS_AUTOMATION_TEST_FIND_POINT_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_FIND_POINT_COUNT (1024)
#define AGS_AUTOMATION_TEST_FIND_POINT_N_ATTEMPTS (128)

#define AGS_AUTOMATION_TEST_FIND_REGION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_FIND_REGION_HEIGHT (88)
#define AGS_AUTOMATION_TEST_FIND_REGION_STEPS (127)
#define AGS_AUTOMATION_TEST_FIND_REGION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_FIND_REGION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_FIND_REGION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_FIND_REGION_COUNT (1024)
#define AGS_AUTOMATION_TEST_FIND_REGION_N_ATTEMPTS (128)
#define AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_WIDTH (128)
#define AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_HEIGHT (24)
  
#define AGS_AUTOMATION_TEST_FREE_SELECTION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_HEIGHT (88)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_STEPS (127)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_COUNT (1024)
#define AGS_AUTOMATION_TEST_FREE_SELECTION_SELECTION_COUNT (64)

#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_HEIGHT (88)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_STEPS (127)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_COUNT (1024)

#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_HEIGHT (88)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_STEPS (127)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_COUNT (1024)
#define AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_N_ATTEMPTS (128)

#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_WIDTH (4096)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_HEIGHT (88)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_STEPS (127)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_LOWER (0.0)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_UPPER (127.0)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_DEFAULT_VALUE (1.0)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_COUNT (1024)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_SELECTION_COUNT (128)
#define AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_N_ATTEMPTS (64)

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
  for(i = 0; i + 1 < AGS_AUTOMATION_TEST_FIND_NEAR_TIMESTAMP_N_AUTOMATION; i++){
    timestamp->timer.unix_time.time_val = AGS_TIMESTAMP(automation[0]->timestamp)->timer.unix_time.time_val + ((i + 1) * AGS_AUTOMATION_DEFAULT_DURATION + 1);
    current = ags_automation_find_near_timestamp(list, 0,
						 timestamp);

    CU_ASSERT(current != NULL && current->data == automation[i + 1]);
  }  
}

void
ags_automation_test_add_acceleration()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list;

  gdouble range;
  guint x, y;
  guint i;
  gboolean success;

  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_ADD_ACCELERATION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_ADD_ACCELERATION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_ADD_ACCELERATION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_ADD_ACCELERATION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_ADD_ACCELERATION_UPPER - AGS_AUTOMATION_TEST_ADD_ACCELERATION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_ADD_ACCELERATION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_ADD_ACCELERATION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_ADD_ACCELERATION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_ADD_ACCELERATION_LOWER + ((y / AGS_AUTOMATION_TEST_ADD_ACCELERATION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* assert position */
  list = automation->acceleration;
  success = TRUE;
  
  for(i = 0; i < AGS_AUTOMATION_TEST_ADD_ACCELERATION_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_ACCELERATION(list->prev->data)->x > AGS_ACCELERATION(list->data)->x){
	success = FALSE;
	
	break;
      }
    }
    
    list = list->next;
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_remove_acceleration_at_position()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current;

  gdouble range;
  guint x, y;
  guint nth;
  guint i;
  gboolean success;

  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_UPPER - AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_LOWER + ((y / AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT; i++){
    nth = rand() % (AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT - i);
    current = g_list_nth(automation->acceleration,
			 nth);
    
    ags_automation_remove_acceleration_at_position(automation,
						   AGS_ACCELERATION(current->data)->x,
						   AGS_ACCELERATION(current->data)->y);
  }

  /* assert position */
  list = automation->acceleration;
  success = TRUE;
  
  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT - AGS_AUTOMATION_TEST_REMOVE_ACCELERATION_AT_POSITION_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_ACCELERATION(list->prev->data)->x > AGS_ACCELERATION(list->data)->x){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_is_acceleration_selected()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current;
  
  gdouble range;
  guint x, y;
  guint nth;
  guint i;
  gboolean success;

  /* create automation */
  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_UPPER - AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_LOWER + ((y / AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* select acceleration */
  for(i = 0; i < AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_SELECTION_COUNT; i++){
    nth = rand() % (AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_COUNT - i);
    current = g_list_nth(automation->acceleration,
			 nth);

    ags_automation_add_acceleration(automation,
				    current->data,
				    TRUE);
  }

  /* assert position */
  list = automation->selection;
  success = TRUE;
  
  for(i = 0; i < AGS_AUTOMATION_TEST_IS_ACCELERATION_SELECTED_SELECTION_COUNT; i++){
    if(list->prev != NULL){
      if(AGS_ACCELERATION(list->prev->data)->x > AGS_ACCELERATION(list->data)->x){
	success = FALSE;

	break;
      }
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_find_point()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current;

  gdouble range;
  guint nth;
  guint x, y;
  guint i;
  gboolean success;

  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_FIND_POINT_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_FIND_POINT_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_FIND_POINT_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_FIND_POINT_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_FIND_POINT_UPPER - AGS_AUTOMATION_TEST_FIND_POINT_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_POINT_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_FIND_POINT_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_FIND_POINT_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_FIND_POINT_LOWER + ((y / AGS_AUTOMATION_TEST_FIND_POINT_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* assert find point */
  success = TRUE;

  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_POINT_N_ATTEMPTS; i++){
    nth = rand() % AGS_AUTOMATION_TEST_FIND_POINT_COUNT;
    current = g_list_nth(automation->acceleration,
			 nth);
    
    acceleration = ags_automation_find_point(automation,
					     AGS_ACCELERATION(current->data)->x,
					     AGS_ACCELERATION(current->data)->y,
					     FALSE);

    if(acceleration->x != AGS_ACCELERATION(current->data)->x ||
       acceleration->y != AGS_ACCELERATION(current->data)->y){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_find_region()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current, *region;

  gdouble range;
  guint nth;
  guint x, y;
  guint i;
  gboolean success;

  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_FIND_REGION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_FIND_REGION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_FIND_REGION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_FIND_REGION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_FIND_REGION_UPPER - AGS_AUTOMATION_TEST_FIND_REGION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_FIND_REGION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_FIND_REGION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_FIND_REGION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_FIND_REGION_LOWER + ((y / AGS_AUTOMATION_TEST_FIND_REGION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* assert find region */
  success = TRUE;

  for(i = 0;
      i < AGS_AUTOMATION_TEST_FIND_REGION_N_ATTEMPTS &&
	success;
      i++){
    nth = rand() % AGS_AUTOMATION_TEST_FIND_REGION_COUNT;
    current = g_list_nth(automation->acceleration,
			 nth);
    
    region = ags_automation_find_region(automation,
				      AGS_ACCELERATION(current->data)->x,
				      AGS_ACCELERATION(current->data)->y,
				      AGS_ACCELERATION(current->data)->x + AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_WIDTH,
				      AGS_ACCELERATION(current->data)->y + AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_HEIGHT,
				      FALSE);
    while(region != NULL){
      if(!(AGS_ACCELERATION(region->data)->x >= AGS_ACCELERATION(current->data)->x &&
	   AGS_ACCELERATION(current->data)->x < AGS_ACCELERATION(current->data)->x + AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_WIDTH &&
	   AGS_ACCELERATION(region->data)->y >= AGS_ACCELERATION(current->data)->y &&
	   AGS_ACCELERATION(current->data)->y < AGS_ACCELERATION(current->data)->y + AGS_AUTOMATION_TEST_FIND_REGION_SELECTION_HEIGHT)){
	success = FALSE;
	
	break;
      }
      
      region = region->next;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_free_selection()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current;
  
  gdouble range;
  guint x, y;
  guint nth;
  guint i;

  /* create automation */
  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_FREE_SELECTION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_FREE_SELECTION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_FREE_SELECTION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_FREE_SELECTION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_FREE_SELECTION_UPPER - AGS_AUTOMATION_TEST_FREE_SELECTION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_FREE_SELECTION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_FREE_SELECTION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_FREE_SELECTION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_FREE_SELECTION_LOWER + ((y / AGS_AUTOMATION_TEST_FREE_SELECTION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* select acceleration */
  for(i = 0; i < AGS_AUTOMATION_TEST_FREE_SELECTION_SELECTION_COUNT; i++){
    nth = rand() % (AGS_AUTOMATION_TEST_FREE_SELECTION_SELECTION_COUNT - i);
    current = g_list_nth(automation->acceleration,
			 nth);

    ags_automation_add_acceleration(automation,
				    current->data,
				    TRUE);
  }

  /* assert free slection */
  CU_ASSERT(automation->selection != NULL);

  ags_automation_free_selection(automation);
  
  CU_ASSERT(automation->selection == NULL);
}

void
ags_automation_test_add_all_to_selection()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current, *current_selection;
  
  gdouble range;
  guint x, y;
  guint nth;
  guint i;
  gboolean success;

  /* create automation */
  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_UPPER - AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_LOWER + ((y / AGS_AUTOMATION_TEST_ADD_ALL_TO_SELECTION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* assert all present */
  current = automation->acceleration;
  current_selection = automation->selection;

  success = TRUE;
  
  while(current != NULL &&
	current_selection != NULL){
    if(current->data != current_selection->data){
      success = FALSE;
      
      break;
    }
    
    current = current->next;
    current_selection = current_selection->next;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_add_point_to_selection()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration;

  GList *list, *current;
  
  gdouble range;
  guint x, y;
  guint nth;
  guint i;
  gboolean success;

  /* create automation */
  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_UPPER - AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_LOWER + ((y / AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* assert add point to selection */
  success = TRUE;
  
  for(i = 0; i < AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_N_ATTEMPTS; i++){
    nth = rand() % AGS_AUTOMATION_TEST_ADD_POINT_TO_SELECTION_COUNT;
    current = g_list_nth(automation->acceleration,
			 nth);

    if(current->prev != NULL &&
       current->next != NULL){
      ags_automation_add_point_to_selection(automation,
					    AGS_ACCELERATION(current->data)->x, AGS_ACCELERATION(current->data)->y,
					    FALSE);
      
      if(ags_automation_find_point(automation,
				   AGS_ACCELERATION(current->data)->x, AGS_ACCELERATION(current->data)->y,
				   TRUE) == NULL){
	success = FALSE;
	
	break;
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_automation_test_remove_point_from_selection()
{
  AgsAutomation *automation;
  AgsAcceleration *acceleration, *match;

  GList *list, *current, *iter, *next;
  
  gdouble range;
  guint x, y;
  guint nth;
  guint n_matches;
  guint i;
  
  gboolean success;

  /* create automation */
  automation = ags_automation_new(audio,
				  0,
				  AGS_TYPE_INPUT,
				  AGS_AUTOMATION_TEST_CONTROL_NAME);
  automation->steps = AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_STEPS;
  automation->lower = AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_LOWER;
  automation->upper = AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_UPPER;
  automation->default_value = AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_DEFAULT_VALUE;
  
  range = (AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_UPPER - AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_LOWER);

  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_COUNT; i++){
    x = rand() % AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_WIDTH;
    y = rand() % AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_HEIGHT;

    acceleration = ags_acceleration_new();
    acceleration->x = x;
    acceleration->y = AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_LOWER + ((y / AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_HEIGHT) * range);

    ags_automation_add_acceleration(automation,
				    acceleration,
				    FALSE);
  }

  /* add point to selection */
  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_SELECTION_COUNT; i++){
    nth = rand() % AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_COUNT;
    current = g_list_nth(automation->acceleration,
			 nth);

    ags_automation_add_point_to_selection(automation,
					  AGS_ACCELERATION(current->data)->x, AGS_ACCELERATION(current->data)->y,
					  FALSE);
  }

  /* assert remove point from selection */
  success = TRUE;
  
  for(i = 0; i < AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_N_ATTEMPTS; i++){
    nth = rand() % (AGS_AUTOMATION_TEST_REMOVE_POINT_FROM_SELECTION_N_ATTEMPTS);
    current = g_list_nth(automation->selection,
			 nth);

    if(current == NULL){
      continue;
    }
    
    iter = current->next;
    n_matches = 0;

    while(iter != NULL &&
	  AGS_ACCELERATION(iter->data)->x == AGS_ACCELERATION(current->data)->x &&
	  AGS_ACCELERATION(iter->data)->y == AGS_ACCELERATION(current->data)->y){
      n_matches++;

      iter = iter->next;
    }

    acceleration = AGS_ACCELERATION(current->data);
    ags_automation_remove_point_from_selection(automation,
					       acceleration->x, acceleration->y);

    
    if((match = ags_automation_find_point(automation,
					  acceleration->x, acceleration->y,
					  TRUE)) != NULL){
      if(n_matches == 0){
	success = FALSE;
	
	break;
      }else{
	next = g_list_find(automation->selection,
			   match);
	
	while(next != NULL &&
	      AGS_ACCELERATION(next->data)->x == acceleration->x &&
	      AGS_ACCELERATION(next->data)->y == acceleration->y){
	  n_matches--;
	  
	  next = next->next;
	}
	
	if(n_matches != 0){
	  success = FALSE;
	  
	  break;
	}
      }
    }
  }
  
  CU_ASSERT(success == TRUE);
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
     (CU_add_test(pSuite, "test of AgsAutomation find near timestamp\0", ags_automation_test_find_near_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation add acceleration\0", ags_automation_test_add_acceleration) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation remove acceleration at position\0", ags_automation_test_remove_acceleration_at_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation is acceleration selected\0", ags_automation_test_is_acceleration_selected) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation find point\0", ags_automation_test_find_point) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation find region\0", ags_automation_test_find_region) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation free selection\0", ags_automation_test_free_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation add all to selection\0", ags_automation_test_add_all_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation add point to selection\0", ags_automation_test_add_point_to_selection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAutomation remove point from selection\0", ags_automation_test_remove_point_from_selection) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

