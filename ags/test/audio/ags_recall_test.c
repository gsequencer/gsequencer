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
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall.h>

int ags_recall_test_init_suite();
int ags_recall_test_clean_suite();

void ags_recall_test_resolve_dependencies();
void ags_recall_test_child_added();
void ags_recall_test_run_init_pre();
void ags_recall_test_run_init_inter();
void ags_recall_test_run_init_post();
void ags_recall_test_stop_persistent();
void ags_recall_test_done();
void ags_recall_test_cancel();
void ags_recall_test_remove();
void ags_recall_test_is_done();
void ags_recall_test_duplicate();
void ags_recall_test_set_recall_id();
void ags_recall_test_set_soundcard_recursive();
void ags_recall_test_notify_dependency();
void ags_recall_test_add_dependency();
void ags_recall_test_remove_dependency();
void ags_recall_test_get_dependencies();
void ags_recall_test_remove_child();
void ags_recall_test_add_child();
void ags_recall_test_get_children();
void ags_recall_test_get_by_effect();
void ags_recall_test_find_by_effect();
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

#define AGS_RECALL_RUN_INIT_PRE_N_CHILDREN (4)
#define AGS_RECALL_RUN_INIT_INTER_N_CHILDREN (4)
#define AGS_RECALL_RUN_INIT_POST_N_CHILDREN (4)

#define AGS_RECALL_RUN_PRE_N_CHILDREN (4)
#define AGS_RECALL_RUN_INTER_N_CHILDREN (4)
#define AGS_RECALL_RUN_POST_N_CHILDREN (4)

AgsApplicationContext *application_context;
AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_clean_suite()
{
  return(0);
}

void
ags_recall_test_callback(AgsRecall *recall,
			 gpointer data)
{
  *((guint *) data) += 1;
}

void
ags_recall_test_resolve_dependencies()
{
  AgsRecall *recall;

  guint data;

  /* create recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "resolve-dependencies\0",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback invoked */
  data = 0;
  ags_recall_resolve_dependencies(recall);
  
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
  g_signal_connect(G_OBJECT(recall), "child-added\0",
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
  g_signal_connect(G_OBJECT(recall), "run-init-pre\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_PRE_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_PRE_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-pre\0",
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
  g_signal_connect(G_OBJECT(recall), "run-init-inter\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_INTER_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_INTER_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-inter\0",
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
  g_signal_connect(G_OBJECT(recall), "run-init-post\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  child = (AgsRecall **) malloc(AGS_RECALL_RUN_INIT_POST_N_CHILDREN * sizeof(AgsRecall *));
  
  for(i = 0; i < AGS_RECALL_RUN_INIT_POST_N_CHILDREN; i++){
    child[i] = ags_recall_new();
    ags_recall_add_child(recall,
			 child[i]);
    g_signal_connect(G_OBJECT(child[i]), "run-init-post\0",
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
  recall->flags |= (AGS_RECALL_PERSISTENT |
		    AGS_RECALL_PERSISTENT_PLAYBACK);
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_RECALL_PERSISTENT & (recall->flags)) == 0) &&
	    ((AGS_RECALL_PERSISTENT_PLAYBACK & (recall->flags)) == 0) &&
	    data == 1);

  /* sequencer */
  recall = ags_recall_new();
  recall->flags |= (AGS_RECALL_PERSISTENT |
		    AGS_RECALL_PERSISTENT_SEQUENCER);
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_RECALL_PERSISTENT & (recall->flags)) == 0) &&
	    ((AGS_RECALL_PERSISTENT_SEQUENCER & (recall->flags)) == 0) &&
	    data == 1);

  /* notation */
  recall = ags_recall_new();
  recall->flags |= (AGS_RECALL_PERSISTENT |
		    AGS_RECALL_PERSISTENT_NOTATION);
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &data);
  
  /* assert callback invoked */
  data = 0;
  ags_recall_stop_persistent(recall);
  
  CU_ASSERT(((AGS_RECALL_PERSISTENT & (recall->flags)) == 0) &&
	    ((AGS_RECALL_PERSISTENT_NOTATION & (recall->flags)) == 0) &&
	    data == 1);
}

void
ags_recall_test_done()
{
  AgsRecall *recall;

  guint data;

  /* recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback invoked */
  data = 0;
  ags_recall_done(recall);
  
  CU_ASSERT(((AGS_RECALL_DONE & (recall->flags)) != 0) &&
	    data == 1);

  /* persistent recall */
  recall = ags_recall_new();
  recall->flags |= AGS_RECALL_PERSISTENT;
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &data);

  /* assert callback not invoked */
  data = 0;
  ags_recall_done(recall);
  
  CU_ASSERT(((AGS_RECALL_DONE & (recall->flags)) == 0) &&
	    data == 0);
}

void
ags_recall_test_cancel()
{
  AgsRecall *recall;

  guint cancel_data, done_data;

  /* recall */
  recall = ags_recall_new();
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &done_data);
  g_signal_connect(G_OBJECT(recall), "cancel\0",
		   G_CALLBACK(ags_recall_test_callback), &cancel_data);

  /* assert callback invoked */
  done_data = 0;
  cancel_data = 0;
  ags_recall_cancel(recall);
  
  CU_ASSERT(((AGS_RECALL_DONE & (recall->flags)) != 0) &&
	    done_data == 1 &&
	    cancel_data == 1);

  /* persistent recall */
  recall = ags_recall_new();
  recall->flags |= AGS_RECALL_PERSISTENT;
  g_signal_connect(G_OBJECT(recall), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &done_data);
  g_signal_connect(G_OBJECT(recall), "cancel\0",
		   G_CALLBACK(ags_recall_test_callback), &cancel_data);

  /* assert callback invoked */
  done_data = 0;
  cancel_data = 0;
  ags_recall_cancel(recall);
  
  CU_ASSERT(((AGS_RECALL_DONE & (recall->flags)) != 0) &&
	    done_data == 1 &&
	    cancel_data == 1);
}

void
ags_recall_test_remove()
{
  AgsRecall *parent, *recall_0, *recall_1;

  guint remove_data, done_data;
  
  /* parent */
  parent = ags_recall_new();
  g_signal_connect(G_OBJECT(parent), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &done_data);

  /* 1st recall */
  recall_0 = ags_recall_new();
  ags_recall_add_child(parent,
		       recall_0);
  g_signal_connect(G_OBJECT(recall_0), "remove\0",
		   G_CALLBACK(ags_recall_test_callback), &remove_data);

  /* 2nd recall */
  recall_1 = ags_recall_new();
  ags_recall_add_child(parent,
		       recall_1);
  g_signal_connect(G_OBJECT(recall_1), "remove\0",
		   G_CALLBACK(ags_recall_test_callback), &remove_data);

  /* assert callback invoked */
  done_data = 0;
  remove_data = 0;

  ags_recall_remove(recall_0);
  
  CU_ASSERT(((AGS_RECALL_DONE & (parent->flags)) == 0) &&
	    remove_data == 1 &&
	    done_data == 0);

  ags_recall_remove(recall_1);
  
  CU_ASSERT(((AGS_RECALL_DONE & (parent->flags)) == 0) &&
	    remove_data == 2 &&
	    done_data == 0);

  /* parent propagate done */
  parent = ags_recall_new();
  parent->flags |= AGS_RECALL_PROPAGATE_DONE;
  g_signal_connect(G_OBJECT(parent), "done\0",
		   G_CALLBACK(ags_recall_test_callback), &done_data);

  /* 1st recall */
  recall_0 = ags_recall_new();
  ags_recall_add_child(parent,
		       recall_0);
  g_signal_connect(G_OBJECT(recall_0), "remove\0",
		   G_CALLBACK(ags_recall_test_callback), &remove_data);

  /* 2nd recall */
  recall_1 = ags_recall_new();
  ags_recall_add_child(parent,
		       recall_1);
  g_signal_connect(G_OBJECT(recall_1), "remove\0",
		   G_CALLBACK(ags_recall_test_callback), &remove_data);

  /* assert callback invoked */
  done_data = 0;
  remove_data = 0;

  ags_recall_remove(recall_0);
  
  CU_ASSERT(((AGS_RECALL_DONE & (parent->flags)) == 0) &&
	    remove_data == 1 &&
	    done_data == 0);

  ags_recall_remove(recall_1);
  
  CU_ASSERT(((AGS_RECALL_DONE & (parent->flags)) != 0) &&
	    remove_data == 2 &&
	    done_data == 1);
}

void
ags_recall_test_is_done()
{
  //TODO:JK: implement me
}

void
ags_recall_test_duplicate()
{
  //TODO:JK: implement me
}

void
ags_recall_test_set_recall_id()
{
  //TODO:JK: implement me
}

void
ags_recall_test_set_soundcard_recursive()
{
  //TODO:JK: implement me
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
ags_recall_test_get_dependencies()
{
  //TODO:JK: implement me
}

void
ags_recall_test_remove_child()
{
  //TODO:JK: implement me
}

void
ags_recall_test_add_child()
{
  //TODO:JK: implement me
}

void
ags_recall_test_get_children()
{
  //TODO:JK: implement me
}

void
ags_recall_test_get_by_effect()
{
  //TODO:JK: implement me
}

void
ags_recall_test_find_by_effect()
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

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsRecallTest\0", ags_recall_test_init_suite, ags_recall_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecall resolve dependencies\0", ags_recall_test_resolve_dependencies) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall child added\0", ags_recall_test_child_added) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init pre\0", ags_recall_test_run_init_pre) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init inter\0", ags_recall_test_run_init_inter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init post\0", ags_recall_test_run_init_post) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall stop persistent\0", ags_recall_test_stop_persistent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall done\0", ags_recall_test_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall cancel\0", ags_recall_test_cancel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove\0", ags_recall_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall is done\0", ags_recall_test_is_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall duplicate\0", ags_recall_test_duplicate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall set recall id\0", ags_recall_test_set_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall set soundcard recursive\0", ags_recall_test_set_soundcard_recursive) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall notify dependency\0", ags_recall_test_notify_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add dependency\0", ags_recall_test_add_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove dependency\0", ags_recall_test_remove_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get dependencies\0", ags_recall_test_get_dependencies) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove child\0", ags_recall_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add child\0", ags_recall_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get children\0", ags_recall_test_get_children) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get by effect\0", ags_recall_test_get_by_effect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type\0", ags_recall_test_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find template\0", ags_recall_test_find_template) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find type\0", ags_recall_test_template_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find all type\0", ags_recall_test_template_find_all_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type with recycling context\0", ags_recall_test_find_type_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find recycling context\0", ags_recall_test_find_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider\0", ags_recall_test_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find provider\0", ags_recall_test_template_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider with recycling context\0", ags_recall_test_find_provider_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init\0", ags_recall_test_run_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add handler\0", ags_recall_test_add_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove handler\0", ags_recall_test_remove_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall lock port\0", ags_recall_test_lock_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall unlock port\0", ags_recall_test_unlock_port) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

