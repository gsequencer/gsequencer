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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_turtle_manager_test_init_suite();
int ags_turtle_manager_test_clean_suite();

void ags_turtle_manager_test_find();
void ags_turtle_manager_test_add();

#define AGS_TURTLE_MANAGER_TEST_FIND_MICHAELANGELO "Michael-Angelo.rdf"
#define AGS_TURTLE_MANAGER_TEST_FIND_RAPHAEL "Raphael.rdf"
#define AGS_TURTLE_MANAGER_TEST_FIND_LEONARDO "Leonardo.rdf"
#define AGS_TURTLE_MANAGER_TEST_FIND_DONATELLO "Donatello.rdf"

#define AGS_TURTLE_MANAGER_TEST_ADD_DEFAULT "Default"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_manager_test_clean_suite()
{
  return(0);
}

void
ags_turtle_manager_test_find()
{
  AgsTurtleManager *turtle_manager;  
  AgsTurtle *turtle[4], *current;
  
  turtle_manager = ags_turtle_manager_get_instance();

  /* turtle #0 */
  turtle[0] = ags_turtle_new(AGS_TURTLE_MANAGER_TEST_FIND_MICHAELANGELO);

  ags_turtle_manager_add(turtle_manager,
			 turtle[0]);

  /* turtle #1 */
  turtle[1] = ags_turtle_new(AGS_TURTLE_MANAGER_TEST_FIND_RAPHAEL);

  ags_turtle_manager_add(turtle_manager,
			 turtle[1]);

  /* turtle #2 */
  turtle[2] = ags_turtle_new(AGS_TURTLE_MANAGER_TEST_FIND_LEONARDO);

  ags_turtle_manager_add(turtle_manager,
			 turtle[2]);

  /* turtle #3 */
  turtle[3] = ags_turtle_new(AGS_TURTLE_MANAGER_TEST_FIND_DONATELLO);

  ags_turtle_manager_add(turtle_manager,
			 turtle[3]);

  /* assert find */
  CU_ASSERT((current = ags_turtle_manager_find(turtle_manager,
					       AGS_TURTLE_MANAGER_TEST_FIND_MICHAELANGELO)) != NULL &&
	    current == turtle[0]);

  CU_ASSERT((current = ags_turtle_manager_find(turtle_manager,
					       AGS_TURTLE_MANAGER_TEST_FIND_RAPHAEL)) != NULL &&
	    current == turtle[1]);

  CU_ASSERT((current = ags_turtle_manager_find(turtle_manager,
					       AGS_TURTLE_MANAGER_TEST_FIND_LEONARDO)) != NULL &&
	    current == turtle[2]);

  CU_ASSERT((current = ags_turtle_manager_find(turtle_manager,
					       AGS_TURTLE_MANAGER_TEST_FIND_DONATELLO)) != NULL &&
	    current == turtle[3]);

  /* unref turtle manager */
  g_object_unref(turtle_manager);
}

void
ags_turtle_manager_test_add()
{
  AgsTurtleManager *turtle_manager;  
  AgsTurtle *turtle;
  
  gchar *str;

  guint i;

  turtle_manager = ags_turtle_manager_get_instance();

  /* add turtle - #0 */
  i = 0;
  
  str = g_strdup_printf("%s %d",
			AGS_TURTLE_MANAGER_TEST_ADD_DEFAULT,
			i);
  i++;

  turtle = ags_turtle_new(str);
  ags_turtle_manager_add(turtle_manager,
			 turtle);

  /* add turtle - #1 */
  str = g_strdup_printf("%s %d",
			AGS_TURTLE_MANAGER_TEST_ADD_DEFAULT,
			i);
  i++;

  turtle = ags_turtle_new(str);
  ags_turtle_manager_add(turtle_manager,
			 turtle);

  /* add turtle - #2 */
  str = g_strdup_printf("%s %d",
			AGS_TURTLE_MANAGER_TEST_ADD_DEFAULT,
			i);
  i++;

  turtle = ags_turtle_new(str);
  ags_turtle_manager_add(turtle_manager,
			 turtle);

  /* assert count equals 3 */
  CU_ASSERT(g_list_length(turtle_manager->turtle) == 3);
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
  pSuite = CU_add_suite("AgsTurtleManagerTest\0", ags_turtle_manager_test_init_suite, ags_turtle_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTurtleManager find\0", ags_turtle_manager_test_find) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtleManager add\0", ags_turtle_manager_test_add) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
