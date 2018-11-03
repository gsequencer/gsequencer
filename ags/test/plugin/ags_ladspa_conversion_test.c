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

int ags_ladspa_conversion_test_init_suite();
int ags_ladspa_conversion_test_clean_suite();

void ags_ladspa_conversion_test_test_flags();
void ags_ladspa_conversion_test_set_flags();
void ags_ladspa_conversion_test_unset_flags();
void ags_ladspa_conversion_test_convert();

#define AGS_LADSPA_CONVERSION_TEST_CONVERT_SAMPLERATE (44100)

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_ladspa_conversion_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_ladspa_conversion_test_clean_suite()
{
  return(0);
}

void
ags_ladspa_conversion_test_test_flags()
{
  AgsLadspaConversion *ladspa_conversion;

  ladspa_conversion = ags_ladspa_conversion_new();
  ladspa_conversion->flags = 0;
  
  /* test flags - samplerate */
  CU_ASSERT(!ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE));

  /* test flags - bounded below */
  CU_ASSERT(!ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_BELOW));

  /* test flags - bounded above */
  CU_ASSERT(!ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_ABOVE));

  /* test flags - logarithmic */
  CU_ASSERT(!ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC));

  /* set flags */
  ladspa_conversion->flags = (AGS_LADSPA_CONVERSION_SAMPLERATE |
			      AGS_LADSPA_CONVERSION_BOUNDED_BELOW |
			      AGS_LADSPA_CONVERSION_BOUNDED_ABOVE |
			      AGS_LADSPA_CONVERSION_LOGARITHMIC);
  
  /* test flags - samplerate */
  CU_ASSERT(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE));

  /* test flags - bounded below */
  CU_ASSERT(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_BELOW));

  /* test flags - bounded above */
  CU_ASSERT(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_ABOVE));

  /* test flags - logarithmic */
  CU_ASSERT(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC));
}

void
ags_ladspa_conversion_test_set_flags()
{
  AgsLadspaConversion *ladspa_conversion;

  ladspa_conversion = ags_ladspa_conversion_new();
  ladspa_conversion->flags = 0;

  /* set flags - samplerate */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE);

  CU_ASSERT((AGS_LADSPA_CONVERSION_SAMPLERATE & (ladspa_conversion->flags)) != 0);

  /* set flags - bounded below */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_BELOW);

  CU_ASSERT((AGS_LADSPA_CONVERSION_BOUNDED_BELOW & (ladspa_conversion->flags)) != 0);

  /* set flags - bounded above */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_ABOVE);

  CU_ASSERT((AGS_LADSPA_CONVERSION_BOUNDED_ABOVE & (ladspa_conversion->flags)) != 0);

  /* set flags - logarithmic */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC);

  CU_ASSERT((AGS_LADSPA_CONVERSION_LOGARITHMIC & (ladspa_conversion->flags)) != 0);
}

void
ags_ladspa_conversion_test_unset_flags()
{
  AgsLadspaConversion *ladspa_conversion;

  ladspa_conversion = ags_ladspa_conversion_new();

  ladspa_conversion->flags = (AGS_LADSPA_CONVERSION_SAMPLERATE |
			      AGS_LADSPA_CONVERSION_BOUNDED_BELOW |
			      AGS_LADSPA_CONVERSION_BOUNDED_ABOVE |
			      AGS_LADSPA_CONVERSION_LOGARITHMIC);

  /* unset flags - samplerate */
  ags_ladspa_conversion_unset_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE);

  CU_ASSERT((AGS_LADSPA_CONVERSION_SAMPLERATE & (ladspa_conversion->flags)) == 0);

  /* unset flags - bounded below */
  ags_ladspa_conversion_unset_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_BELOW);

  CU_ASSERT((AGS_LADSPA_CONVERSION_BOUNDED_BELOW & (ladspa_conversion->flags)) == 0);

  /* unset flags - bounded above */
  ags_ladspa_conversion_unset_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_BOUNDED_ABOVE);

  CU_ASSERT((AGS_LADSPA_CONVERSION_BOUNDED_ABOVE & (ladspa_conversion->flags)) == 0);

  /* unset flags - logarithmic */
  ags_ladspa_conversion_unset_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC);

  CU_ASSERT((AGS_LADSPA_CONVERSION_LOGARITHMIC & (ladspa_conversion->flags)) == 0);
}

void
ags_ladspa_conversion_test_convert()
{
  AgsLadspaConversion *ladspa_conversion;

  gdouble retval;
  
  ladspa_conversion = ags_ladspa_conversion_new();
  g_object_set(ladspa_conversion,
	       "samplerate", AGS_LADSPA_CONVERSION_TEST_CONVERT_SAMPLERATE,
	       NULL);

  /* convert - #0 */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC);

  retval = ags_conversion_convert(ladspa_conversion,
				  0.25,
				  FALSE);
  CU_ASSERT(retval != 0.0);

  retval = ags_conversion_convert(ladspa_conversion,
				  exp(0.25),
				  TRUE);
  CU_ASSERT(retval != 0.0);
  
  /* convert - #1 */
  ags_ladspa_conversion_set_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE);

  retval = ags_conversion_convert(ladspa_conversion,
				  0.25,
				  FALSE);
  CU_ASSERT(retval != 0.0);

  retval = ags_conversion_convert(ladspa_conversion,
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
  pSuite = CU_add_suite("AgsLadspaConversionTest", ags_ladspa_conversion_test_init_suite, ags_ladspa_conversion_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLadspaConversion test flags", ags_ladspa_conversion_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaConversion set flags", ags_ladspa_conversion_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaConversion unset flags", ags_ladspa_conversion_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLadspaConversion convert", ags_ladspa_conversion_test_convert) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
