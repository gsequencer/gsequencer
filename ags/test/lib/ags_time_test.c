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

#include <ags/lib/ags_time.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_time_test_init_suite();
int ags_time_test_clean_suite();

void ags_time_test_get_uptime_from_offset();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_time_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_time_test_clean_suite()
{
  return(0);
}

void
ags_time_test_get_uptime_from_offset()
{
  gdouble bpm = 120.0;
  gdouble delay = (60.0 * ((44100 / 1024) / 120.0)) * ((1.0 / 16.0) * (1.0 / 0.25));
  gdouble delay_factor = 0.25;
  guint offset_0 = 0;
  guint offset_1_times_delay = delay;
  guint offset_4_times_delay = 4 * delay;
  guint offset_16_times_delay = 16 * delay;
  
  gchar *offset_0_str = "0000:00.000\0";
  gchar *offset_1_times_delay_str = "0000:00.625\0";
  gchar *offset_4_times_delay_str = "0000:02.625\0";
  gchar *offset_16_times_delay_str = "0000:10.750\0";
  gchar *str;

  /* assert offset 0 */
  str = ags_time_get_uptime_from_offset(offset_0,
					bpm,
					delay,
					delay_factor);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(offset_0_str,
				 str,
				 12));

  /* assert offset 1 */
  str = ags_time_get_uptime_from_offset(offset_1_times_delay,
					bpm,
					delay,
					delay_factor);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(offset_1_times_delay_str,
				 str,
				 12));

  /* assert offset 4 */
  str = ags_time_get_uptime_from_offset(offset_4_times_delay,
					bpm,
					delay,
					delay_factor);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(offset_4_times_delay_str,
				 str,
				 12));

  /* assert offset 16 */
  str = ags_time_get_uptime_from_offset(offset_16_times_delay,
					bpm,
					delay,
					delay_factor);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(offset_16_times_delay_str,
				 str,
				 12));
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsTimeTest\0", ags_time_test_init_suite, ags_time_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_time.c get uptime from offset\0", ags_time_test_get_uptime_from_offset) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
