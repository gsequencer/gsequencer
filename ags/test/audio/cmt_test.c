/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2025 Joël Krähemann
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

#include <dlfcn.h>

void cmt_test_dlopen();
void cmt_test_instantiate();

// #define CMT_TEST_DLOPEN_DIRECTORY "/usr/lib/ladspa"
#define CMT_TEST_DLOPEN_DIRECTORY "/home/joelkraehemann/github/cmt_1.18/plugins"
#define CMT_TEST_DLOPEN_FILENAME "cmt.so"
//#define CMT_TEST_DLOPEN_EFFECT "Feedback Delay Line (Maximum Delay 5s)"
#define CMT_TEST_DLOPEN_EFFECT "Echo Delay Line (Maximum Delay 5s)"

gchar **ags_ladspa_default_path;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
cmt_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
cmt_test_clean_suite()
{
  return(0);
}

void
cmt_test_dlopen()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = ags_ladspa_manager_get_instance();
   
  /* test NULL */
  ags_ladspa_manager_load_file(ladspa_manager,
			       CMT_TEST_DLOPEN_DIRECTORY,
			       CMT_TEST_DLOPEN_FILENAME);
}


void
cmt_test_instantiate()
{
#if 1
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  
  ladspa_manager = ags_ladspa_manager_get_instance();
   
  /* test NULL */
  ags_ladspa_manager_load_file(ladspa_manager,
			       CMT_TEST_DLOPEN_DIRECTORY,
			       CMT_TEST_DLOPEN_FILENAME);

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
							"/home/joelkraehemann/github/cmt_1.18/plugins/cmt.so",
							CMT_TEST_DLOPEN_EFFECT);
  
  ags_base_plugin_instantiate((AgsBasePlugin *) ladspa_plugin,
			      44100, 2048);
#endif

  void *plugin_so = dlopen("/home/joelkraehemann/github/cmt_1.18/plugins/cmt.so",
			   RTLD_NOW);

  if(plugin_so == NULL){
    g_message("plugin_so is NULL");
  }
  
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  
  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym((void *) plugin_so,
							 "ladspa_descriptor");

  unsigned long i;
  gboolean success;
  
  for(i = 0; (plugin_descriptor = ladspa_descriptor(i)) != NULL; i++){
    g_message("%s", plugin_descriptor->Name);

    if(!strncmp(plugin_descriptor->Name,
		CMT_TEST_DLOPEN_EFFECT,
		strlen(CMT_TEST_DLOPEN_EFFECT))){
      g_message("!!! found !!!");

       LADSPA_Handle (*instantiate)(const LADSPA_Descriptor *descriptor,
				    unsigned long samplerate);

       instantiate = plugin_descriptor->instantiate;

       void *cmt_plugin = instantiate(plugin_descriptor,
				      44100);

       if(cmt_plugin != NULL){
	 g_message("--- success ---");
       }
    }
  }
}

void
cmt_test_connect_port()
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  AgsPort **ladspa_port;
  
  GType channel_type;
  
  LADSPA_Handle ladspa_handle;

  GList *start_plugin_port, *plugin_port;

  LADSPA_Data *output;
  LADSPA_Data *input;

  guint *output_port;
  guint *input_port;

  guint audio_channel;
  guint pad;
  guint line;
  guint output_port_count, input_port_count;
  guint control_port_count;
  guint nth;
  guint i;
    
  ladspa_manager = ags_ladspa_manager_get_instance();

  line = 0;

  channel_type = AGS_TYPE_INPUT;

  audio_channel = 0;
  pad = 0;
  
  /* test NULL */
  ags_ladspa_manager_load_file(ladspa_manager,
			       CMT_TEST_DLOPEN_DIRECTORY,
			       CMT_TEST_DLOPEN_FILENAME);

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
							"/home/joelkraehemann/github/cmt_1.18/plugins/cmt.so",
							CMT_TEST_DLOPEN_EFFECT);
  
  ladspa_handle = ags_base_plugin_instantiate((AgsBasePlugin *) ladspa_plugin,
					      44100, 2048);
  
  start_plugin_port = NULL;
  
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  output_port = NULL;
  input_port = NULL;
  
  output_port_count = 0;
  input_port_count = 0;

  control_port_count = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }else if(ags_plugin_port_test_flags(plugin_port->data,
					AGS_PLUGIN_PORT_AUDIO)){
      guint port_index;

      g_object_get(plugin_port->data,
		   "port-index", &port_index,
		   NULL);
      
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_INPUT)){
	if(input_port == NULL){
	  input_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  input_port = (guint *) g_realloc(input_port,
					   (input_port_count + 1) * sizeof(guint));
	}

	input_port[input_port_count] = port_index;
	input_port_count++;
      }else if(ags_plugin_port_test_flags(plugin_port->data,
					  AGS_PLUGIN_PORT_OUTPUT)){
	if(output_port == NULL){
	  output_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  output_port = (guint *) g_realloc(output_port,
					    (output_port_count + 1) * sizeof(guint));
	}

	output_port[output_port_count] = port_index;
	output_port_count++;
      }
    }

    plugin_port = plugin_port->next;
  }

  output = (float *) g_malloc(output_port_count * 2048 * sizeof(float));
  
  input = (float *) g_malloc(input_port_count * 2048 * sizeof(float));
  
   /*  */
  if(control_port_count > 0){
    ladspa_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

    plugin_port = start_plugin_port;
    
    for(nth = 0; nth < control_port_count && plugin_port != NULL;){
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_CONTROL)){
	AgsPluginPort *current_plugin_port;

	gchar *plugin_name;
	gchar *specifier;
	gchar *control_port;

	guint port_index;
      
	GValue default_value = {0,};

	GRecMutex *plugin_port_mutex;
      
	current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

	/* get plugin port mutex */
	plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

	/* plugin name, specifier and control port */
	plugin_name = g_strdup_printf("ladspa-%u", ladspa_plugin->unique_id);

	specifier = NULL;
      
	port_index = 0;

	g_object_get(current_plugin_port,
		     "port-name", &specifier,
		     "port-index", &port_index,
		     NULL);

	control_port = g_strdup_printf("%u/%u",
				       nth,
				       control_port_count);

	/* default value */
	g_value_init(&default_value,
		     G_TYPE_FLOAT);
      
	g_rec_mutex_lock(plugin_port_mutex);
      
	g_value_copy(current_plugin_port->default_value,
		     &default_value);
      
	g_rec_mutex_unlock(plugin_port_mutex);

	/* ladspa port */
	ladspa_port[nth] = g_object_new(AGS_TYPE_PORT,
					"line", line,
					"channel-type", channel_type,
					"plugin-name", plugin_name,
					"specifier", specifier,
					"control-port", control_port,
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					NULL);
      
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_OUTPUT)){
	  ags_port_set_flags(ladspa_port[nth], AGS_PORT_IS_OUTPUT);
	  
	  //	  ags_recall_set_flags((AgsRecall *) fx_ladspa_channel,
	  //		       AGS_RECALL_HAS_OUTPUT_PORT);
	
	}else{
	  if(!ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_TOGGLED)){
	    ags_port_set_flags(ladspa_port[nth], AGS_PORT_INFINITE_RANGE);
	  }
	}
	
	g_object_set(ladspa_port[nth],
		     "plugin-port", current_plugin_port,
		     NULL);

	//	ags_port_util_load_ladspa_conversion(ladspa_port[nth],
	//				     current_plugin_port);
	
	//	ags_recall_add_port((AgsRecall *) fx_ladspa_channel,
	//		    ladspa_port[nth]);

	/* connect port */
	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
				       ladspa_handle,
				       port_index,
				       &(ladspa_port[nth]->port_value.ags_port_ladspa));
	}

	ags_port_safe_write_raw(ladspa_port[nth],
				&default_value);

      
	g_free(plugin_name);
	g_free(specifier);
	g_free(control_port);

	g_value_unset(&default_value);

	nth++;
      }else if(ags_plugin_port_test_flags(plugin_port->data,
					  AGS_PLUGIN_PORT_AUDIO)){
	guint channel;
	
	if(ags_plugin_port_test_flags(plugin_port->data,
				      AGS_PLUGIN_PORT_INPUT)){
	  for(channel = 0; channel < input_port_count; channel++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
					 (gpointer) ladspa_handle,
					 (guint) input_port[channel],
					 (gpointer) (input + channel));
	  }
	}else if(ags_plugin_port_test_flags(plugin_port->data,
					    AGS_PLUGIN_PORT_OUTPUT)){
	  for(channel = 0; channel < output_port_count; channel++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
					 (gpointer) ladspa_handle,
					 (guint) output_port[channel],
					 (gpointer) (output + channel));
	  }
	}
      }

      plugin_port = plugin_port->next;
    }
      
    ladspa_port[nth] = NULL;
  }

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
  pSuite = CU_add_suite("CMTTest", cmt_test_init_suite, cmt_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of cmt dlopen", cmt_test_dlopen) == NULL) ||
     (CU_add_test(pSuite, "test of cmt instantiate", cmt_test_instantiate) == NULL) ||
     (CU_add_test(pSuite, "test of cmt connect port", cmt_test_connect_port) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
