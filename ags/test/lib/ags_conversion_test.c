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

#include <ags/libags.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_conversion_test_init_suite();
int ags_conversion_test_clean_suite();

void ags_conversion_test_property();
void ags_conversion_test_finalize();
void ags_conversion_test_convert();

void ags_conversion_test_stub_finalize(GObject *gobject);
gdouble ags_conversion_test_stub_convert(AgsConversion *conversion,
					 gdouble value,
					 gboolean reverse);

#define AGS_CONVERSION_TEST_PROPERTY_NAME_0 "logarithmic transform"
#define AGS_CONVERSION_TEST_PROPERTY_NAME_1 "linear transform"
#define AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_0 "transform using log(), see `man -s3 log` or `man -s3 exp` for further information"
#define AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_1 "transform using multiplication"

#define AGS_CONVERSION_TEST_CONVERT_VALUE_0 (8.0)
#define AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_0 (FALSE)
#define AGS_CONVERSION_TEST_CONVERT_RETVAL_0 (19.0)
#define AGS_CONVERSION_TEST_CONVERT_VALUE_1 (17.0)
#define AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_1 (TRUE)
#define AGS_CONVERSION_TEST_CONVERT_RETVAL_1 (3.0)

gboolean ags_conversion_test_finalize_invoked = FALSE;
guint ags_conversion_test_convert_invoked = 0;

void
ags_conversion_test_property()
{
  AgsConversion *conversion;
  
  conversion = ags_conversion_new();
  g_object_ref(conversion);
  
  CU_ASSERT(conversion != NULL);

  /* test AgsConversion:name - #0 */
  g_object_set(conversion,
	       "name", AGS_CONVERSION_TEST_PROPERTY_NAME_0,
	       NULL);
  CU_ASSERT(!g_strcmp0(conversion->name,
		       AGS_CONVERSION_TEST_PROPERTY_NAME_0));
  
  /* test AgsConversion:name - #1 */
  g_object_set(conversion,
	       "name", AGS_CONVERSION_TEST_PROPERTY_NAME_1,
	       NULL);
  CU_ASSERT(!g_strcmp0(conversion->name,
		       AGS_CONVERSION_TEST_PROPERTY_NAME_1));

  /* test AgsConversion:description - #0 */
  g_object_set(conversion,
	       "description", AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_0,
	       NULL);
  CU_ASSERT(!g_strcmp0(conversion->description,
		       AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_0));
  
  /* test AgsConversion:description - #1 */
  g_object_set(conversion,
	       "description", AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_1,
	       NULL);
  CU_ASSERT(!g_strcmp0(conversion->description,
		       AGS_CONVERSION_TEST_PROPERTY_DESCRIPTION_1));
}

void
ags_conversion_test_stub_finalize(GObject *gobject)
{
  ags_conversion_test_finalize_invoked = TRUE;
}

void
ags_conversion_test_finalize()
{
  AgsConversion *conversion;

  gpointer stubbed_class;
  gpointer orig_finalize;
  
  conversion = ags_conversion_new();
  g_object_ref(conversion);

  CU_ASSERT(conversion != NULL);

  /* stub finalize */
  stubbed_class = AGS_CONVERSION_GET_CLASS(conversion);

  orig_finalize = G_OBJECT_CLASS(stubbed_class)->finalize;
  G_OBJECT_CLASS(stubbed_class)->finalize = ags_conversion_test_stub_finalize;
  
  /* test finalize */
  g_object_unref(conversion);
  CU_ASSERT(ags_conversion_test_finalize_invoked == TRUE);
  
  /* undo stub */
  G_OBJECT_CLASS(stubbed_class)->finalize = orig_finalize;
}

gdouble
ags_conversion_test_stub_convert(AgsConversion *conversion,
				 gdouble value,
				 gboolean reverse)
{
  gdouble retval;

  switch(ags_conversion_test_convert_invoked){
  case 0:
    {
      CU_ASSERT(value == AGS_CONVERSION_TEST_CONVERT_VALUE_0);
      CU_ASSERT(reverse == AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_0);
      
      retval = AGS_CONVERSION_TEST_CONVERT_RETVAL_0;
    }
    break;
  case 1:
    {
      CU_ASSERT(value == AGS_CONVERSION_TEST_CONVERT_VALUE_1);
      CU_ASSERT(reverse == AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_1);

      retval = AGS_CONVERSION_TEST_CONVERT_RETVAL_1;
    }
    break;
  }
  
  ags_conversion_test_convert_invoked++;

  return(retval);
}

void
ags_conversion_test_convert()
{
  AgsConversion *conversion;

  gpointer stubbed_class;
  gpointer orig_convert;

  gdouble retval;
  
  conversion = ags_conversion_new();
  g_object_ref(conversion);

  CU_ASSERT(conversion != NULL);

  /* stub convert */
  stubbed_class = AGS_CONVERSION_GET_CLASS(conversion);

  orig_convert = AGS_CONVERSION_CLASS(stubbed_class)->convert;
  AGS_CONVERSION_CLASS(stubbed_class)->convert = ags_conversion_test_stub_convert;
  
  /* test convert - #0 */
  retval = ags_conversion_convert(conversion,
				  AGS_CONVERSION_TEST_CONVERT_VALUE_0,
				  AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_0);
  CU_ASSERT(retval == AGS_CONVERSION_TEST_CONVERT_RETVAL_0);

  CU_ASSERT(ags_conversion_test_convert_invoked == 1);
 
  /* test convert - #1 */
  retval = ags_conversion_convert(conversion,
				  AGS_CONVERSION_TEST_CONVERT_VALUE_1,
				  AGS_CONVERSION_TEST_CONVERT_IS_REVERSE_1);
  CU_ASSERT(retval == AGS_CONVERSION_TEST_CONVERT_RETVAL_1);

  CU_ASSERT(ags_conversion_test_convert_invoked == 2);
  
  /* undo stub */
  AGS_CONVERSION_CLASS(stubbed_class)->convert = orig_convert;
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
  pSuite = CU_add_suite("AgsConversionTest", ags_conversion_test_init_suite, ags_conversion_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConversion property", ags_conversion_test_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConversion finalize", ags_conversion_test_finalize) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConversion convert", ags_conversion_test_convert) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
