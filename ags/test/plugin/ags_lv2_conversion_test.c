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

int ags_lv2_conversion_test_init_suite();
int ags_lv2_conversion_test_clean_suite();

void ags_lv2_conversion_test_test_flags();
void ags_lv2_conversion_test_set_flags();
void ags_lv2_conversion_test_unset_flags();
void ags_lv2_conversion_test_convert();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_conversion_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_conversion_test_clean_suite()
{
  return(0);
}

void
ags_lv2_conversion_test_test_flags()
{
  AgsLv2Conversion *lv2_conversion;

  lv2_conversion = ags_lv2_conversion_new();

  /* test flags - no logarithmic */
  CU_ASSERT(!ags_lv2_conversion_test_flags(lv2_conversion, AGS_LV2_CONVERSION_LOGARITHMIC));

  /* set flags */
  lv2_conversion->flags = (AGS_LV2_CONVERSION_LOGARITHMIC);

  /* test flags - logarithmic */  
  CU_ASSERT(ags_lv2_conversion_test_flags(lv2_conversion, AGS_LV2_CONVERSION_LOGARITHMIC));
}

void
ags_lv2_conversion_test_set_flags()
{
  AgsLv2Conversion *lv2_conversion;

  lv2_conversion = ags_lv2_conversion_new();
  lv2_conversion->flags = 0;

  /* set flags - logarithmic */
  ags_lv2_conversion_set_flags(lv2_conversion, AGS_LV2_CONVERSION_LOGARITHMIC);

  CU_ASSERT((AGS_LV2_CONVERSION_LOGARITHMIC & (lv2_conversion->flags)) != 0);
}

void
ags_lv2_conversion_test_unset_flags()
{
  AgsLv2Conversion *lv2_conversion;

  lv2_conversion = ags_lv2_conversion_new();

  lv2_conversion->flags = (AGS_LV2_CONVERSION_LOGARITHMIC);

  /* unset flags - logarithmic */
  ags_lv2_conversion_unset_flags(lv2_conversion, AGS_LV2_CONVERSION_LOGARITHMIC);

  CU_ASSERT((AGS_LV2_CONVERSION_LOGARITHMIC & (lv2_conversion->flags)) == 0);
}

void
ags_lv2_conversion_test_convert()
{
  AgsLv2Conversion *lv2_conversion;

  gdouble retval;
  
  lv2_conversion = ags_lv2_conversion_new();

  /* convert - #0 */
  ags_lv2_conversion_set_flags(lv2_conversion, AGS_LV2_CONVERSION_LOGARITHMIC);

  retval = ags_conversion_convert(lv2_conversion,
				  0.25,
				  FALSE);
  CU_ASSERT(retval != 0.0);

  retval = ags_conversion_convert(lv2_conversion,
				  exp(0.25),
				  TRUE);
  CU_ASSERT(retval != 0.0);
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
  pSuite = CU_add_suite("AgsLv2ConversionTest", ags_lv2_conversion_test_init_suite, ags_lv2_conversion_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2Conversion test flags", ags_lv2_conversion_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Conversion set flags", ags_lv2_conversion_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Conversion unset flags", ags_lv2_conversion_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLv2Conversion convert", ags_lv2_conversion_test_convert) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
