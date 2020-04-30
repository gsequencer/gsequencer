/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <math.h>

int ags_fx_analyse_channel_test_init_suite();
int ags_fx_analyse_channel_test_clean_suite();

void ags_fx_analyse_channel_stub_finalize();

void ags_fx_analyse_channel_test_new();
void ags_fx_analyse_channel_test_dispose();
void ags_fx_analyse_channel_test_finalize();
void ags_fx_analyse_channel_test_input_data_alloc();
void ags_fx_analyse_channel_test_input_data_free();

gboolean finalized;

void
ags_fx_analyse_channel_stub_finalize(GObject *gobject)
{
  finalized = TRUE;
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_analyse_channel_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_analyse_channel_test_clean_suite()
{  
  return(0);
}

void
ags_fx_analyse_channel_test_new()
{
  AgsChannel *channel;
  AgsFxAnalyseChannel *fx_analyse_channel;

  guint i;
  gboolean success;
  
  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);
  
  fx_analyse_channel = ags_fx_analyse_channel_new(channel);

  CU_ASSERT(fx_analyse_channel != NULL);

  CU_ASSERT(AGS_RECALL_CHANNEL(fx_analyse_channel)->source == channel);

  CU_ASSERT(fx_analyse_channel->frequency != NULL);
  CU_ASSERT(fx_analyse_channel->magnitude != NULL);

  success = TRUE;
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(fx_analyse_channel->input_data[i]->parent == NULL){
      success = FALSE;
      
      break;
    }

    if(fx_analyse_channel->input_data[i]->plan == NULL){
      success = FALSE;
      
      break;
    }

    if(fx_analyse_channel->input_data[i]->comout == NULL){
      success = FALSE;
      
      break;
    }

    if(fx_analyse_channel->input_data[i]->in == NULL){
      success = FALSE;
      
      break;
    }

    if(fx_analyse_channel->input_data[i]->out == NULL){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fx_analyse_channel_test_dispose()
{
  AgsChannel *channel;
  AgsFxAnalyseChannel *fx_analyse_channel;

  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);
  
  fx_analyse_channel = ags_fx_analyse_channel_new(channel);

  g_object_run_dispose(fx_analyse_channel);

  CU_ASSERT(fx_analyse_channel->frequency == NULL);
  CU_ASSERT(fx_analyse_channel->magnitude == NULL);

  g_object_unref(fx_analyse_channel);
}

void
ags_fx_analyse_channel_test_finalize()
{
  AgsChannel *channel;
  AgsFxAnalyseChannel *fx_analyse_channel;

  GObjectClass *gobject_class;
  
  gpointer stub;
  
  /* attempt #0 */
  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);
  
  fx_analyse_channel = ags_fx_analyse_channel_new(channel);

  gobject_class = (GObjectClass *) g_type_class_ref(AGS_TYPE_FX_ANALYSE_CHANNEL);
  
  finalized = FALSE;

  stub = gobject_class->finalize;
  gobject_class->finalize = ags_fx_analyse_channel_stub_finalize;
  
  g_object_unref(fx_analyse_channel);

  CU_ASSERT(finalized == TRUE);

  gobject_class->finalize = stub;

  /* attempt #1 */
  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);
  
  fx_analyse_channel = ags_fx_analyse_channel_new(channel);

  g_object_unref(fx_analyse_channel);
}

void
ags_fx_analyse_channel_test_input_data_alloc()
{
  AgsFxAnalyseChannelInputData *input_data;

  input_data = ags_fx_analyse_channel_input_data_alloc();

  CU_ASSERT(input_data != NULL);
}

void
ags_fx_analyse_channel_test_input_data_free()
{
  AgsFxAnalyseChannelInputData *input_data;

  input_data = ags_fx_analyse_channel_input_data_alloc();

  CU_ASSERT(input_data != NULL);

  ags_fx_analyse_channel_input_data_free(input_data);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFxAnalyseChannelTest", ags_fx_analyse_channel_test_init_suite, ags_fx_analyse_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxAnalyseChannel new", ags_fx_analyse_channel_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxAnalyseChannel dispose", ags_fx_analyse_channel_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxAnalyseChannel finalize", ags_fx_analyse_channel_test_finalize) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxAnalyseChannel input data alloc", ags_fx_analyse_channel_test_input_data_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxAnalyseChannel input data free", ags_fx_analyse_channel_test_input_data_free) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
