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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <stdlib.h>

int ags_recycling_context_test_init_suite();
int ags_recycling_context_test_clean_suite();

void ags_recycling_context_test_find_scope();
void ags_recycling_context_test_replace();
void ags_recycling_context_test_add();
void ags_recycling_context_test_remove();
void ags_recycling_context_test_insert();
void ags_recycling_context_test_get_toplevel();
void ags_recycling_context_test_find();
void ags_recycling_context_test_find_child();
void ags_recycling_context_test_find_parent();
void ags_recycling_context_test_add_child();
void ags_recycling_context_test_remove_child();
void ags_recycling_context_test_get_child_recall_id();
void ags_recycling_context_test_reset_recycling();

#define AGS_RECYCLING_CONTEXT_TEST_REPLACE_RECYCLING_COUNT (9)

#define AGS_RECYCLING_CONTEXT_TEST_ADD_RECYCLING_COUNT (7)

#define AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT (7)

#define AGS_RECYCLING_CONTEXT_TEST_INSERT_RECYCLING_COUNT (88)

#define RECYCLING_CONTEXT_TEST_GET_TOPLEVEL_NESTING_LEVEL (5)

#define AGS_RECYCLING_CONTEXT_TEST_FIND_RECYCLING_COUNT (88)

#define AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_CHILD_COUNT (5)
#define AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_RECYCLING_COUNT (4)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_context_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_context_test_clean_suite()
{  
  return(0);
}

void
ags_recycling_context_test_find_scope()
{
  AgsRecyclingContext **recycling_context;

  GList *list;
  GList *current;

  recycling_context = (AgsRecyclingContext **) malloc(AGS_SOUND_SCOPE_LAST * sizeof(AgsRecyclingContext *));

  list = NULL;
  
  recycling_context[AGS_SOUND_SCOPE_PLAYBACK] = ags_recycling_context_new(0);
  recycling_context[AGS_SOUND_SCOPE_PLAYBACK]->sound_scope = AGS_SOUND_SCOPE_PLAYBACK;
  list = g_list_prepend(list,
			recycling_context[AGS_SOUND_SCOPE_PLAYBACK]);
  
  recycling_context[AGS_SOUND_SCOPE_SEQUENCER] = ags_recycling_context_new(0);
  recycling_context[AGS_SOUND_SCOPE_SEQUENCER]->sound_scope = AGS_SOUND_SCOPE_SEQUENCER;
  list = g_list_prepend(list,
			recycling_context[AGS_SOUND_SCOPE_SEQUENCER]);

  recycling_context[AGS_SOUND_SCOPE_NOTATION] = ags_recycling_context_new(0);
  recycling_context[AGS_SOUND_SCOPE_NOTATION]->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  list = g_list_prepend(list,
			recycling_context[AGS_SOUND_SCOPE_NOTATION]);

  recycling_context[AGS_SOUND_SCOPE_WAVE] = ags_recycling_context_new(0);
  recycling_context[AGS_SOUND_SCOPE_WAVE]->sound_scope = AGS_SOUND_SCOPE_WAVE;
  list = g_list_prepend(list,
			recycling_context[AGS_SOUND_SCOPE_WAVE]);

  recycling_context[AGS_SOUND_SCOPE_MIDI] = ags_recycling_context_new(0);
  recycling_context[AGS_SOUND_SCOPE_MIDI]->sound_scope = AGS_SOUND_SCOPE_MIDI;
  list = g_list_prepend(list,
			recycling_context[AGS_SOUND_SCOPE_MIDI]);

  list = g_list_reverse(list);

  /* test */
  current = ags_recycling_context_find_scope(list,
					     AGS_SOUND_SCOPE_PLAYBACK);
  CU_ASSERT(current->data == recycling_context[AGS_SOUND_SCOPE_PLAYBACK]);

  current = ags_recycling_context_find_scope(list,
					     AGS_SOUND_SCOPE_SEQUENCER);
  CU_ASSERT(current->data == recycling_context[AGS_SOUND_SCOPE_SEQUENCER]);

  current = ags_recycling_context_find_scope(list,
					     AGS_SOUND_SCOPE_NOTATION);
  CU_ASSERT(current->data == recycling_context[AGS_SOUND_SCOPE_NOTATION]);
  
  current = ags_recycling_context_find_scope(list,
					     AGS_SOUND_SCOPE_WAVE);
  CU_ASSERT(current->data == recycling_context[AGS_SOUND_SCOPE_WAVE]);

  current = ags_recycling_context_find_scope(list,
					     AGS_SOUND_SCOPE_MIDI);
  CU_ASSERT(current->data == recycling_context[AGS_SOUND_SCOPE_MIDI]);
}

void
ags_recycling_context_test_replace()
{
  AgsRecycling **recycling;
  AgsRecyclingContext *recycling_context;

  guint i;
  gboolean success;
  
  recycling = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_REPLACE_RECYCLING_COUNT * sizeof(AgsRecycling *));
  
  recycling_context = ags_recycling_context_new(AGS_RECYCLING_CONTEXT_TEST_REPLACE_RECYCLING_COUNT);

  success = TRUE;
  
  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_REPLACE_RECYCLING_COUNT; i++){
    recycling[i] = ags_recycling_new(NULL);
    ags_recycling_context_replace(recycling_context,
				  recycling[i],
				  i);

    if(recycling_context->recycling[i] != recycling[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_recycling_context_test_add()
{
  AgsRecycling **recycling;
  AgsRecyclingContext *recycling_context;

  guint i;
  gboolean success;
  
  recycling = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_ADD_RECYCLING_COUNT * sizeof(AgsRecycling *));
  
  recycling_context = ags_recycling_context_new(0);

  success = TRUE;
  
  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_ADD_RECYCLING_COUNT; i++){
    recycling[i] = ags_recycling_new(NULL);
    ags_recycling_context_add(recycling_context,
			      recycling[i]);

    if(recycling_context->recycling[i] != recycling[i] ||
       recycling_context->length != i + 1){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_recycling_context_test_remove()
{
  AgsRecycling **recycling;
  AgsRecyclingContext *recycling_context;

  guint i, j;
  gboolean success;
  
  recycling = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT * sizeof(AgsRecycling *));
  
  recycling_context = ags_recycling_context_new(AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT);

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT; i++){
    recycling[i] = ags_recycling_new(NULL);
    ags_recycling_context_replace(recycling_context,
				  recycling[i],
				  i);
  }

  /* test */
  success = TRUE;

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT && success; i++){
    ags_recycling_context_remove(recycling_context,
				 recycling[i]);

    if(recycling_context->length != 0){
      for(j = 0; AGS_RECYCLING_CONTEXT_TEST_REMOVE_RECYCLING_COUNT - (j + i + 1) > 1; j++){
	if(recycling_context->recycling[j] != recycling[i + j + 1]){
	  success = FALSE;
	
	  break;
	}
      }
    }
    
    if(recycling_context->length == 0){
      if(recycling_context->recycling != NULL){
	success = FALSE;

	break;
      }
    }
  }
  
  CU_ASSERT(success);
}

void
ags_recycling_context_test_insert()
{
  AgsRecycling **recycling;
  AgsRecyclingContext *recycling_context;

  gint position;
  guint i, j;
  gboolean success;
  
  recycling = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_INSERT_RECYCLING_COUNT * sizeof(AgsRecycling *));
  
  recycling_context = ags_recycling_context_new(0);

  success = TRUE;
  
  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_INSERT_RECYCLING_COUNT; i++){
    recycling[i] = ags_recycling_new(NULL);
  }

  /* test */
  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_INSERT_RECYCLING_COUNT; i++){
    if(i > 0){
      position = rand() % i;
    }else{
      position = 0;
    }
    
    ags_recycling_context_insert(recycling_context,
				 recycling[i],
				 position);

    
    if(recycling_context->recycling[position] != recycling[i]){
      success = FALSE;
      
      break;
    }
  }
  
  CU_ASSERT(success);
}

void
ags_recycling_context_test_get_toplevel()
{
  AgsRecyclingContext *toplevel;
  AgsRecyclingContext *recycling_context, *current;

  guint i;
  
  toplevel = ags_recycling_context_new(0);

  current = toplevel;

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_GET_TOPLEVEL_NESTING_LEVEL; i++){
    recycling_context = ags_recycling_context_new(0);
    ags_recycling_context_add_child(current,
				    recycling_context);

    current = recycling_context;
  }

  CU_ASSERT(ags_recycling_context_get_toplevel(toplevel) == toplevel);
  CU_ASSERT(ags_recycling_context_get_toplevel(current) == toplevel);
}

void
ags_recycling_context_test_find()
{
  AgsRecycling **recycling;
  AgsRecyclingContext *recycling_context;

  gint position;
  guint i;
  gboolean success;
  
  recycling = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_FIND_RECYCLING_COUNT * sizeof(AgsRecycling *));
  
  recycling_context = ags_recycling_context_new(AGS_RECYCLING_CONTEXT_TEST_FIND_RECYCLING_COUNT);

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_FIND_RECYCLING_COUNT; i++){
    recycling[i] = ags_recycling_new(NULL);
    ags_recycling_context_replace(recycling_context,
				  recycling[i],
				  i);
  }

  /* test */
  success = TRUE;

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_FIND_RECYCLING_COUNT; i++){
    position = ags_recycling_context_find(recycling_context,
					  recycling[i]);

    if(position != i){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success);

  CU_ASSERT(ags_recycling_context_find(recycling_context, NULL) == -1);
}

void
ags_recycling_context_test_find_child()
{
  AgsRecycling ***recycling;
  AgsRecyclingContext *recycling_context;
  AgsRecyclingContext **child;

  gint position;
  guint i, j;
  gboolean success;
  
  recycling_context = ags_recycling_context_new(0);
  child = (AgsRecyclingContext **) malloc(AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_CHILD_COUNT * sizeof(AgsRecyclingContext *));

  recycling = (AgsRecycling ***) malloc(AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_CHILD_COUNT * sizeof(AgsRecycling **));

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_CHILD_COUNT; i++){
    child[i] = ags_recycling_context_new(0);
    ags_recycling_context_add_child(recycling_context,
				    child[i]);

    recycling[i] = (AgsRecycling **) malloc(AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_RECYCLING_COUNT * sizeof(AgsRecycling *));
    
    for(j = 0; j < AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_RECYCLING_COUNT; j++){
      recycling[i][j] = ags_recycling_new(NULL);
      ags_recycling_context_replace(child[i],
				    recycling[i],
				    i);
    }
  }

  /* test */
  success = TRUE;

  for(i = 0; i < AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_CHILD_COUNT; i++){
    for(j = 0; j < AGS_RECYCLING_CONTEXT_TEST_FIND_CHILD_RECYCLING_COUNT; j++){
      position = ags_recycling_context_find_child(recycling_context,
						  recycling[i][j]);

      if(position != i){
	success = FALSE;

	break;
      }
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_recycling_context_test_find_parent()
{
  //TODO:JK: implement me
}

void
ags_recycling_context_test_add_child()
{
  //TODO:JK: implement me
}

void
ags_recycling_context_test_remove_child()
{
  //TODO:JK: implement me
}

void
ags_recycling_context_test_get_child_recall_id()
{
  //TODO:JK: implement me
}

void
ags_recycling_context_test_reset_recycling()
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
  pSuite = CU_add_suite("AgsRecyclingContextTest", ags_recycling_context_test_init_suite, ags_recycling_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecyclingContext find scope", ags_recycling_context_test_find_scope) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext replace", ags_recycling_context_test_replace) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext add", ags_recycling_context_test_add) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext remove", ags_recycling_context_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext insert", ags_recycling_context_test_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext get toplevel", ags_recycling_context_test_get_toplevel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext find", ags_recycling_context_test_find) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext find child", ags_recycling_context_test_find_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext find parent", ags_recycling_context_test_find_parent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext add child", ags_recycling_context_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext remove child", ags_recycling_context_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext get child recall id", ags_recycling_context_test_get_child_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecyclingContext reset recycling", ags_recycling_context_test_reset_recycling) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
