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

int ags_base_plugin_test_init_suite();
int ags_base_plugin_test_clean_suite();

void ags_port_descriptor_test_find_symbol();
void ags_base_plugin_test_find_filename();
void ags_base_plugin_test_find_effect();
void ags_base_plugin_test_find_ui_effect_index();
void ags_base_plugin_test_sort();
void ags_base_plugin_test_apply_port_group_by_prefix();
void ags_base_plugin_test_instantiate();
void ags_base_plugin_test_activate();
void ags_base_plugin_test_deactivate();
void ags_base_plugin_test_run();
void ags_base_plugin_test_load_plugin();

#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY "delay"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY "dry"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET "wet"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK "feedback"

#define AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT "default"

#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_DEFAULT "default"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_base_plugin_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_base_plugin_test_clean_suite()
{
  return(0);
}

void
ags_port_descriptor_test_find_symbol()
{
  AgsPortDescriptor *port_descriptor;

  GList *list, *current;

  list = NULL;
  
  /* delay */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY;

  list = g_list_prepend(list,
			port_descriptor);
  
  /* dry */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY;

  list = g_list_prepend(list,
			port_descriptor);

  /* wet */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET;

  list = g_list_prepend(list,
			port_descriptor);

  /* feedback */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK;

  list = g_list_prepend(list,
			port_descriptor);

  /* assert find */
  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK));
}

void
ags_base_plugin_test_find_filename()
{
  AgsBasePlugin *base_plugin;

  GList *list, *current;
  
  gchar *str[3];

  guint i;

  list = NULL;
  i = 0;

  /* base plugin #0 */
  str[0] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[0],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);
  
  /* base plugin #1 */
  str[1] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[1],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* base plugin #2 */
  str[2] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[2],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* assert find filename */
  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[0])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[0]));
  
  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[1])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[1]));

  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[2])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[2]));
}

void
ags_base_plugin_test_find_effect()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_find_ui_effect_index()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_sort()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_apply_port_group_by_prefix()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_instantiate()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_activate()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_deactivate()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_run()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_load_plugin()
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
  pSuite = CU_add_suite("AgsBasePluginTest\0", ags_base_plugin_test_init_suite, ags_base_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPortDescriptor find symbol\0", ags_port_descriptor_test_find_symbol) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find filename\0", ags_base_plugin_test_find_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find effect\0", ags_base_plugin_test_find_effect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find UI effect index\0", ags_base_plugin_test_find_ui_effect_index) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin sort\0", ags_base_plugin_test_sort) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin apply port group by prefix\0", ags_base_plugin_test_apply_port_group_by_prefix) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin instantiate\0", ags_base_plugin_test_instantiate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin activate\0", ags_base_plugin_test_activate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin deactivate\0", ags_base_plugin_test_deactivate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin run\0", ags_base_plugin_test_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin load plugin\0", ags_base_plugin_test_load_plugin) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
