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

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

int ags_lv2_option_manager_test_init_suite();
int ags_lv2_option_manager_test_clean_suite();

void ags_lv2_option_manager_test_ressource_insert();
void ags_lv2_option_manager_test_ressource_remove();
void ags_lv2_option_manager_test_ressource_lookup();
void ags_lv2_option_manager_test_get_option();
void ags_lv2_option_manager_test_set_option();
void ags_lv2_option_manager_test_lv2_options_get();
void ags_lv2_option_manager_test_lv2_options_set();

void ags_lv2_option_manager_test_stub_get_option(AgsLv2OptionManager *lv2_option_manager,
						 gpointer instance,
						 gpointer option,
						 gpointer retval);
void ags_lv2_option_manager_test_stub_set_option(AgsLv2OptionManager *lv2_option_manager,
						 gpointer instance,
						 gpointer option,
						 gpointer retval);

#define AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_FILENAME "/usr/lib/lv2/delay-swh.lv2/plugin-linux.so"
#define AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_EFFECT "Simple delay line, noninterpolating"
#define AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SAMPLERATE (44100)
#define AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SUBJECT (3)
#define AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SIZE (4)

#define AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT (3)

#define AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_SET_SUBJECT (3)

AgsLv2Manager *lv2_manager;
AgsLv2UridManager *lv2_urid_manager;

guint stub_get_option = 0;
guint stub_set_option = 0;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_option_manager_test_init_suite()
{
  lv2_manager = ags_lv2_manager_get_instance();
  ags_lv2_manager_load_default_directory(lv2_manager);

  lv2_urid_manager = ags_lv2_urid_manager_get_instance();

  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_option_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2_option_manager_test_ressource_insert()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
  
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  CU_ASSERT(g_hash_table_lookup(lv2_option_manager->ressource,
				lv2_option_ressource) == &value);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_ressource_remove()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
    
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  ags_lv2_option_manager_ressource_remove(lv2_option_manager,
					  lv2_option_ressource);
  
  CU_ASSERT(g_hash_table_lookup(lv2_option_manager->ressource,
				lv2_option_ressource) == NULL);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_ressource_lookup()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);
    
  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  CU_ASSERT(ags_lv2_option_manager_ressource_lookup(lv2_option_manager,
						    lv2_option_ressource) == &value);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_get_option()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  AgsLv2Plugin *lv2_plugin;

  LV2_Handle instance;
  LV2_Options_Option *option, *current;

  uint32_t retval;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);

  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  /* creat plugin instance */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
					       AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_FILENAME,
					       AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_EFFECT);
  instance = ags_base_plugin_instantiate(AGS_BASE_PLUGIN(lv2_plugin),
					 AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SAMPLERATE);
  
  lv2_option_ressource = ags_lv2_option_ressource_alloc();
  lv2_option_ressource->instance = instance;
  option = lv2_option_ressource->option;

  option->context = LV2_OPTIONS_RESOURCE;
  option->subject = AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SUBJECT;
  option->key = ags_lv2_urid_manager_map(NULL,
					 LV2_MIDI_URI);
  option->type = ags_lv2_urid_manager_map(NULL,
					 LV2_ATOM_URI);
  option->size = AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SIZE;
  option->value = &value;

  ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					  lv2_option_ressource, &value);

  /* assert */
  current = (LV2_Options_Option *) malloc(sizeof(LV2_Options_Option));
  current->context = LV2_OPTIONS_RESOURCE;
  current->subject = option->subject;
  current->key = option->key;

  ags_lv2_option_manager_get_option(lv2_option_manager,
				    (gpointer) instance,
				    (gpointer) current,
				    (gpointer) &retval);

  CU_ASSERT(retval == 0 &&
	    current->type == option->type &&
	    current->size == option->size &&
	    current->value == option->value);

  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_set_option()
{
  AgsLv2OptionManager *lv2_option_manager;
  
  AgsLv2OptionRessource *lv2_option_ressource;

  AgsLv2Plugin *lv2_plugin;

  LV2_Handle instance;
  LV2_Options_Option *option;

  uint32_t retval;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   0);

  lv2_option_manager = ags_lv2_option_manager_get_instance();
  
  /* creat plugin instance */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
					       AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_FILENAME,
					       AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_PLUGIN_EFFECT);
  instance = ags_base_plugin_instantiate(AGS_BASE_PLUGIN(lv2_plugin),
					 AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SAMPLERATE);
  
  option = (LV2_Options_Option *) malloc(sizeof(LV2_Options_Option));

  option->context = LV2_OPTIONS_RESOURCE;
  option->subject = AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SUBJECT;
  option->key = ags_lv2_urid_manager_map(NULL,
					 LV2_MIDI_URI);
  option->type = ags_lv2_urid_manager_map(NULL,
					 LV2_ATOM_URI);
  option->size = AGS_LV2_OPTION_MANAGER_TEST_GET_OPTION_SIZE;
  option->value = &value;

  /* assert */
  ags_lv2_option_manager_set_option(lv2_option_manager,
				    (gpointer) instance,
				    (gpointer) option,
				    (gpointer) &retval);

  lv2_option_ressource = ags_lv2_option_ressource_alloc();
  lv2_option_ressource->instance = instance;
  lv2_option_ressource->option = option;

  CU_ASSERT(retval == 0 &&
	    ags_lv2_option_manager_ressource_lookup(lv2_option_manager,
						    lv2_option_ressource) == &value);
  
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_lv2_options_get()
{
  AgsLv2OptionManager *lv2_option_manager;

  LV2_Options_Option options[4];

  gpointer ptr;
  
  lv2_option_manager = ags_lv2_option_manager_get_instance();

  ptr = AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->get_option;
  AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->get_option = ags_lv2_option_manager_test_stub_get_option;
  
  options[0].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;
  options[1].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;
  options[2].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;

  options[3].subject = 0;
  options[3].key = 0;
  options[3].type = 0;
  options[3].size = 0;
  options[3].value = NULL;
  
  ags_lv2_option_manager_lv2_options_get(NULL,
					 &options);

  CU_ASSERT(stub_get_option == 3);
  
  AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->get_option = ptr;
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_stub_get_option(AgsLv2OptionManager *lv2_option_manager,
					    gpointer instance,
					    gpointer option,
					    gpointer retval)
{
  stub_get_option++;
}

void
ags_lv2_option_manager_test_lv2_options_set()
{
  AgsLv2OptionManager *lv2_option_manager;

  LV2_Options_Option options[4];

  gpointer ptr;

  lv2_option_manager = ags_lv2_option_manager_get_instance();

  ptr = AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->set_option;
  AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->set_option = ags_lv2_option_manager_test_stub_set_option;

  options[0].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;
  options[1].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;
  options[2].subject = AGS_LV2_OPTION_MANAGER_TEST_LV2_OPTIONS_GET_SUBJECT;

  options[3].subject = 0;
  options[3].key = 0;
  options[3].type = 0;
  options[3].size = 0;
  options[3].value = NULL;
  
  ags_lv2_option_manager_lv2_options_set(NULL,
					 &options);

  CU_ASSERT(stub_set_option == 3);
  
  AGS_LV2_OPTION_MANAGER_GET_CLASS(lv2_option_manager)->set_option = ptr;
  g_object_unref(lv2_option_manager);
}

void
ags_lv2_option_manager_test_stub_set_option(AgsLv2OptionManager *lv2_option_manager,
					    gpointer instance,
					    gpointer option,
					    gpointer retval)
{
  stub_set_option++;
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
  pSuite = CU_add_suite("AgsLv2OptionManagerTest\0", ags_lv2_option_manager_test_init_suite, ags_lv2_option_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2OptionManager ressource insert\0", ags_lv2_option_manager_test_ressource_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager ressource remove\0", ags_lv2_option_manager_test_ressource_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager ressource lookup\0", ags_lv2_option_manager_test_ressource_lookup) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager get option\0", ags_lv2_option_manager_test_get_option) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager set option\0", ags_lv2_option_manager_test_set_option) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager lv2 options get\0", ags_lv2_option_manager_test_lv2_options_get) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2OptionManager lv2 options set\0", ags_lv2_option_manager_test_lv2_options_set) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
