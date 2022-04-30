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

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_recall_test_init_suite();
int ags_recall_test_clean_suite();

void ags_recall_test_resolve_dependency();
void ags_recall_test_child_added();
void ags_recall_test_run_init_pre();
void ags_recall_test_run_init_inter();
void ags_recall_test_run_init_post();
void ags_recall_test_stop_persistent();
void ags_recall_test_done();
void ags_recall_test_cancel();
void ags_recall_test_is_done();
void ags_recall_test_duplicate();
void ags_recall_test_set_recall_id();
void ags_recall_test_notify_dependency();
void ags_recall_test_add_dependency();
void ags_recall_test_remove_dependency();
void ags_recall_test_get_dependency();
void ags_recall_test_remove_child();
void ags_recall_test_add_child();
void ags_recall_test_get_by_effect();
void ags_recall_test_find_recall_id_with_effect();
void ags_recall_test_find_type();
void ags_recall_test_find_template();
void ags_recall_test_template_find_type();
void ags_recall_test_template_find_all_type();
void ags_recall_test_find_type_with_recycling_context();
void ags_recall_test_find_recycling_context();
void ags_recall_test_find_provider();
void ags_recall_test_template_find_provider();
void ags_recall_test_find_provider_with_recycling_context();
void ags_recall_test_run_init();
void ags_recall_test_add_handler();
void ags_recall_test_remove_handler();
void ags_recall_test_lock_port();
void ags_recall_test_unlock_port();

void ags_recall_test_callback(AgsRecall *recall,
			      gpointer data);
void ags_recall_test_child_added_callback(AgsRecall *recall, AgsRecall *child,
					  gpointer data);
AgsRecall* ags_recall_test_duplicate_callback(AgsRecall *recall, AgsRecallID *recall_id,
					      guint *n_params,
					      gchar *parameter_name, GValue *value,
					      gpointer data);

#define AGS_RECALL_RUN_INIT_PRE_N_CHILDREN (4)
#define AGS_RECALL_RUN_INIT_INTER_N_CHILDREN (4)
#define AGS_RECALL_RUN_INIT_POST_N_CHILDREN (4)

#define AGS_RECALL_RUN_PRE_N_CHILDREN (4)
#define AGS_RECALL_RUN_INTER_N_CHILDREN (4)
#define AGS_RECALL_RUN_POST_N_CHILDREN (4)

#define AGS_RECALL_TEST_IS_DONE_RECYCLING_CONTEXT_COUNT (2)
#define AGS_RECALL_TEST_IS_DONE_RECALL_COUNT (16)

#define AGS_RECALL_TEST_DUPLICATE_TEMPLATE_COUNT (5)
#define AGS_RECALL_TEST_DUPLICATE_RECALL_ID_COUNT (2)

#define AGS_RECALL_TEST_SET_RECALL_ID_CHILDREN_COUNT (16)

#define AGS_RECALL_TEST_REMOVE_CHILD_CHILDREN_COUNT (16)

#define AGS_RECALL_TEST_ADD_CHILD_CHILDREN_COUNT (16)

#define AGS_RECALL_TEST_GET_BY_EFFECT_RECALL_COUNT (4)
#define AGS_RECALL_TEST_GET_BY_EFFECT_LADSPA_RECALL_COUNT (16)

AgsAlsaDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_init_suite()
{
  devout = ags_alsa_devout_new();
  g_object_ref(devout);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_clean_suite()
{
  g_object_unref(devout);
  
  return(0);
}

void
ags_recall_test_callback(AgsRecall *recall,
			 gpointer data)
{
  *((guint *) data) += 1;
}

void
ags_recall_test_resolve_dependency()
{
  AgsRecall *recall;

  guint data;

  /* create recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "resolve-dependency",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback invoked */
  data = 0;
  ags_recall_resolve_dependency(recall);
  
  CU_ASSERT(data == 1);
}

void
ags_recall_test_child_added_callback(AgsRecall *recall, AgsRecall *child,
				     gpointer data)
{
  *((guint *) data) += 1;
}

void
ags_recall_test_child_added()
{
  AgsRecall *recall;

  guint data;

  /* create recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "child-added",
		   G_CALLBACK(ags_recall_test_child_added_callback), &data);

  /* assert callback invoked */
  data = 0;
  ags_recall_child_added(recall,
			 NULL);
  
  CU_ASSERT(data == 1);
}

void
ags_recall_test_run_init_pre()
{
  AgsRecall *recall;
  AgsRecall **child;

  guint data;
  guint i;

  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "run-init-pre",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_PRE_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_PRE_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-pre",
		     G_CALLBACK(ags_recall_test_callback), &data);
  }
  
  /* assert callback invoked */
  data = 0;
  ags_recall_run_init_pre(recall);
  
  CU_ASSERT(data == (AGS_RECALL_RUN_INIT_PRE_N_CHILDREN + 1));
}

void
ags_recall_test_run_init_inter()
{
  AgsRecall *recall;
  AgsRecall **child;

  guint data;
  guint i;

  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "run-init-inter",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_INTER_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_INTER_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-inter",
		     G_CALLBACK(ags_recall_test_callback), &data);
  }
  
  /* assert callback invoked */
  data = 0;
  ags_recall_run_init_inter(recall);
  
  CU_ASSERT(data == (AGS_RECALL_RUN_INIT_INTER_N_CHILDREN + 1));
}

void
ags_recall_test_run_init_post()
{
  AgsRecall *recall;
  AgsRecall **child;

  guint data;
  guint i;

  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "run-init-post",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_POST_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_POST_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-post",
		     G_CALLBACK(ags_recall_test_callback), &data);
  }
  
  /* assert callback invoked */
  data = 0;
  ags_recall_run_init_post(recall);
  
  CU_ASSERT(data == (AGS_RECALL_RUN_INIT_POST_N_CHILDREN + 1));
}

void
ags_recall_test_stop_persistent()
{
  AgsRecall *recall;

  guint data;

  /* playback */
  recall = ags_recall_new();
  recall->behaviour_flags |= (AGS_SOUND_BEHAVIOUR_PERSISTENT |
			      AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK);
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_SOUND_BEHAVIOUR_PERSISTENT & (recall->behaviour_flags)) == 0) &&
	    ((AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK & (recall->behaviour_flags)) == 0) &&
	    data == 1);

  /* sequencer */
  recall = ags_recall_new();
  recall->behaviour_flags |= (AGS_SOUND_BEHAVIOUR_PERSISTENT |
			      AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER);
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_SOUND_BEHAVIOUR_PERSISTENT & (recall->behaviour_flags)) == 0) &&
	    ((AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER & (recall->behaviour_flags)) == 0) &&
	    data == 1);

  /* notation */
  recall = ags_recall_new();
  recall->behaviour_flags |= (AGS_SOUND_BEHAVIOUR_PERSISTENT |
			      AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION);
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_SOUND_BEHAVIOUR_PERSISTENT & (recall->behaviour_flags)) == 0) &&
	    ((AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION & (recall->behaviour_flags)) == 0) &&
	    data == 1);
}

void
ags_recall_test_done()
{
  AgsRecall *recall;

  guint data;

  /* recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback invoked */
  data = 0;
  ags_recall_set_staging_flags(recall,
			       AGS_SOUND_STAGING_DONE);
  
  CU_ASSERT(((AGS_SOUND_STAGING_DONE & (recall->staging_flags)) != 0) &&
	    data == 1);
 
  /* persistent recall */
  //FIXME:JK: doesn't pass
  recall = ags_recall_new();
  recall->behaviour_flags |= AGS_SOUND_BEHAVIOUR_PERSISTENT;
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback not invoked */
  data = 0;
  ags_recall_set_staging_flags(recall,
			       AGS_SOUND_STAGING_DONE);
  
  CU_ASSERT(((AGS_SOUND_STAGING_DONE & (recall->staging_flags)) == 0) &&
	    data == 0);
}

void
ags_recall_test_cancel()
{
  AgsRecall *recall;

  guint cancel_data, done_data;

  /* recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &done_data);
  g_signal_connect(G_OBJECT(recall), "cancel",
		   G_CALLBACK(ags_recall_test_callback), &cancel_data);

  /* assert callback invoked */
  done_data = 0;
  cancel_data = 0;
  ags_recall_cancel(recall);
  
  CU_ASSERT(((AGS_SOUND_STAGING_DONE & (recall->staging_flags)) != 0) &&
	    done_data == 1 &&
	    cancel_data == 1);

  /* persistent recall */
  recall = ags_recall_new();
  recall->behaviour_flags |= AGS_SOUND_BEHAVIOUR_PERSISTENT;
  g_signal_connect(G_OBJECT(recall), "done",
		   G_CALLBACK(ags_recall_test_callback), &done_data);
  g_signal_connect(G_OBJECT(recall), "cancel",
		   G_CALLBACK(ags_recall_test_callback), &cancel_data);

  /* assert callback invoked */
  done_data = 0;
  cancel_data = 0;
  ags_recall_cancel(recall);
  
  CU_ASSERT(((AGS_SOUND_STAGING_DONE & (recall->staging_flags)) != 0) &&
	    done_data == 1 &&
	    cancel_data == 1);
}

void
ags_recall_test_is_done()
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  AgsRecyclingContext *test_recycling_context, *recycling_context;
  
  GList *list;

  guint i, j;
  gboolean is_done;

  /* recalls not done */
  test_recycling_context = NULL;
  list = NULL;

  for(i = 0; i < AGS_RECALL_TEST_IS_DONE_RECYCLING_CONTEXT_COUNT; i++){
    recycling_context = ags_recycling_context_new(0);

    if(i == 0){
      test_recycling_context = recycling_context;
    }
    
    for(j = 0; j < AGS_RECALL_TEST_IS_DONE_RECALL_COUNT; j++){
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling-context", recycling_context,
			       NULL);

      recall = g_object_new(AGS_TYPE_RECALL,
			    "recall-id", recall_id,
			    NULL);

      if(i != 0){
	recall->staging_flags |= AGS_SOUND_STAGING_DONE;
      }
      
      list = g_list_prepend(list,
			    recall);
    }
  }

  is_done = ags_recall_is_done(list,
			       test_recycling_context);

  CU_ASSERT(is_done == FALSE);
  
  /* recalls done */
  test_recycling_context = NULL;
  list = NULL;

  for(i = 0; i < AGS_RECALL_TEST_IS_DONE_RECYCLING_CONTEXT_COUNT; i++){
    recycling_context = ags_recycling_context_new(0);

    if(i == 0){
      test_recycling_context = recycling_context;
    }
    
    for(j = 0; j < AGS_RECALL_TEST_IS_DONE_RECALL_COUNT; j++){
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling-context", recycling_context,
			       NULL);

      recall = g_object_new(AGS_TYPE_RECALL,
			    "recall-id", recall_id,
			    NULL);

      if(i == 0){
	recall->staging_flags |= AGS_SOUND_STAGING_DONE;
      }
      
      list = g_list_prepend(list,
			    recall);
    }
  }

  is_done = ags_recall_is_done(list,
			       test_recycling_context);

  CU_ASSERT(is_done == TRUE);
}

AgsRecall*
ags_recall_test_duplicate_callback(AgsRecall *recall, AgsRecallID *recall_id,
				   guint *n_params,
				   gchar *parameter_name, GValue *value,
				   gpointer data)
{
  *((guint *) data) += 1;

  return(NULL);
}

void
ags_recall_test_duplicate()
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;
  AgsRecyclingContext *recycling_context;
  AgsRecallID *recall_id;
  
  GList *list, *start;

  guint data;
  guint i;

  /* create recall templates */
  start = NULL;

  for(i = 0; i < AGS_RECALL_TEST_DUPLICATE_TEMPLATE_COUNT; i++){
    recall = g_object_new(AGS_TYPE_RECALL,
			  NULL);
    recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				    NULL);
    
    g_object_set(recall,
		 "output-soundcard", devout,
		 "recall-container", recall_container,
		 NULL);
    recall->flags |= AGS_RECALL_TEMPLATE;
    
    g_signal_connect(G_OBJECT(recall), "duplicate",
		     G_CALLBACK(ags_recall_test_duplicate_callback), &data);

    start = g_list_prepend(start,
			   recall);
    g_object_ref(recall);
  }

  /* assert duplicate */
  data = 0;
  
  for(i = 0; i < AGS_RECALL_TEST_DUPLICATE_RECALL_ID_COUNT; i++){
    recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				     NULL);
    recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			     "recycling-context", recycling_context,
			     NULL);
    
    list = start;

    while(list != NULL){
      ags_recall_duplicate(list->data,
			   recall_id,
			   NULL, NULL, NULL);
      
      list = list->next;
    }
  }
  
  CU_ASSERT(data == (AGS_RECALL_TEST_DUPLICATE_RECALL_ID_COUNT * AGS_RECALL_TEST_DUPLICATE_TEMPLATE_COUNT));
}

void
ags_recall_test_set_recall_id()
{
  AgsRecall *recall, *child;
  AgsRecallID *recall_id;

  GList *list;
  
  guint i;
  gboolean success;

  recall = g_object_new(AGS_TYPE_RECALL,
			NULL);
  
  for(i = 0; i < AGS_RECALL_TEST_SET_RECALL_ID_CHILDREN_COUNT; i++){
    child = g_object_new(AGS_TYPE_RECALL,
			 NULL);
    ags_recall_add_child(recall,
			 child);
  }

  /* assert recall id set */
  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			   NULL);
  success = TRUE;

  ags_recall_set_recall_id(recall,
			   recall_id);

  CU_ASSERT(recall->recall_id == recall_id);

  list = recall->children;

  while(list != NULL){
    if(AGS_RECALL(list->data)->recall_id != recall_id){
      success = FALSE;
	
      break;
    }
    
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_recall_test_notify_dependency()
{
  //TODO:JK: implement me
}

void
ags_recall_test_add_dependency()
{
  //TODO:JK: implement me
}

void
ags_recall_test_remove_dependency()
{
  //TODO:JK: implement me
}

void
ags_recall_test_get_dependency()
{
  //TODO:JK: implement me
}

void
ags_recall_test_remove_child()
{
  AgsRecall *recall, *child;

  GList *list, *list_next;
  
  gboolean success;

  guint i;

  /* create recall */
  recall = g_object_new(AGS_TYPE_RECALL,
			NULL);
  
  for(i = 0; i < AGS_RECALL_TEST_REMOVE_CHILD_CHILDREN_COUNT; i++){
    child = g_object_new(AGS_TYPE_RECALL,
			 NULL);
    ags_recall_add_child(recall,
			 child);
  }

  /* assert removed */
  list = recall->children;

  success = TRUE;

  while(list != NULL){
    list_next = list->next;

    child = list->data;
    ags_recall_remove_child(recall,
			    child);

    if(g_list_find(recall->children,
		   child) != NULL){
      success = FALSE;

      break;
    }
    
    list = list_next;
  }

  CU_ASSERT(success == TRUE &&
	    recall->children == NULL);
}

void
ags_recall_test_add_child()
{
  AgsRecall *recall, *child;

  gboolean success;

  guint i;

  /* create recall and assert child to be present */
  recall = g_object_new(AGS_TYPE_RECALL,
			NULL);

  success = TRUE;
  
  for(i = 0; i < AGS_RECALL_TEST_ADD_CHILD_CHILDREN_COUNT; i++){
    child = g_object_new(AGS_TYPE_RECALL,
			 NULL);
    ags_recall_add_child(recall,
			 child);

    if(g_list_find(recall->children,
		   child) == NULL){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE &&
	    recall->children != NULL);
}

void
ags_recall_test_get_by_effect()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_recall_id_with_effect()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_type()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_template()
{
  //TODO:JK: implement me
}

void
ags_recall_test_template_find_type()
{
  //TODO:JK: implement me
}

void
ags_recall_test_template_find_all_type()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_type_with_recycling_context()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_recycling_context()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_provider()
{
  //TODO:JK: implement me
}

void
ags_recall_test_template_find_provider()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_provider_with_recycling_context()
{
  //TODO:JK: implement me
}

void
ags_recall_test_run_init()
{
  //TODO:JK: implement me
}

void
ags_recall_test_add_handler()
{
  //TODO:JK: implement me
}

void
ags_recall_test_remove_handler()
{
  //TODO:JK: implement me
}

void
ags_recall_test_lock_port()
{
  //TODO:JK: implement me
}

void
ags_recall_test_unlock_port()
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
  pSuite = CU_add_suite("AgsRecallTest", ags_recall_test_init_suite, ags_recall_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecall resolve dependency", ags_recall_test_resolve_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall child added", ags_recall_test_child_added) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init pre", ags_recall_test_run_init_pre) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init inter", ags_recall_test_run_init_inter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init post", ags_recall_test_run_init_post) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall stop persistent", ags_recall_test_stop_persistent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall done", ags_recall_test_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall cancel", ags_recall_test_cancel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall is done", ags_recall_test_is_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall duplicate", ags_recall_test_duplicate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall set recall id", ags_recall_test_set_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall notify dependency", ags_recall_test_notify_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add dependency", ags_recall_test_add_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove dependency", ags_recall_test_remove_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get dependency", ags_recall_test_get_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove child", ags_recall_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add child", ags_recall_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get by effect", ags_recall_test_get_by_effect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type", ags_recall_test_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find template", ags_recall_test_find_template) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find type", ags_recall_test_template_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find all type", ags_recall_test_template_find_all_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type with recycling context", ags_recall_test_find_type_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find recycling context", ags_recall_test_find_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider", ags_recall_test_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find provider", ags_recall_test_template_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider with recycling context", ags_recall_test_find_provider_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init", ags_recall_test_run_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add handler", ags_recall_test_add_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove handler", ags_recall_test_remove_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall lock port", ags_recall_test_lock_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall unlock port", ags_recall_test_unlock_port) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

