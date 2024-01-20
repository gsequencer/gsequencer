/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/file/ags_file_util.h>

#include <ags/lib/ags_regex_util.h>

#include <gmodule.h>

#include <stdlib.h>
#include <string.h>

/**
 * SECTION:ags_file_util
 * @short_description: file util
 * @title: AgsFileUtil
 * @section_id:
 * @include: ags/lib/ags_file_util.h
 *
 * Common utility functions related to text or XML files.
 */

GType
ags_file_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_file_util = 0;

    ags_type_file_util =
      g_boxed_type_register_static("AgsFileUtil",
				   (GBoxedCopyFunc) ags_file_util_copy,
				   (GBoxedFreeFunc) ags_file_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_file_util_alloc:
 * @app_encoding: the application encoding
 * @encoding: the file encoding
 *
 * Allocate #AgsFileUtil-struct
 *
 * Returns: a new #AgsFileUtil-struct
 *
 * Since: 6.3.0
 */
AgsFileUtil*
ags_file_util_alloc(gchar *app_encoding,
		    gchar *encoding)
{
  AgsFileUtil *ptr;

  ptr = (AgsFileUtil *) g_malloc(sizeof(AgsFileUtil));
  
  ptr->app_encoding = g_strdup(app_encoding);
  ptr->encoding = g_strdup(encoding);

  /* iconv */  
  ptr->converter = NULL;
  ptr->reverse_converter = NULL;
  
  if(ptr->encoding != NULL &&
     ptr->app_encoding != NULL &&
     (!g_strcmp0(ptr->app_encoding, ptr->encoding)) == FALSE){
    ptr->converter = g_iconv_open(ptr->encoding,
				  ptr->app_encoding);
  
    ptr->reverse_converter = g_iconv_open(ptr->app_encoding,
					  ptr->encoding);
  }
  
  return(ptr);
}

/**
 * ags_file_util_copy:
 * @ptr: the original #AgsFileUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFileUtil-struct
 *
 * Since: 6.3.0
 */
gpointer
ags_file_util_copy(AgsFileUtil *ptr)
{
  AgsFileUtil *new_ptr;
  
  new_ptr = ags_file_util_alloc(ptr->app_encoding,
				ptr->encoding);

  return(new_ptr);
}

/**
 * ags_file_util_free:
 * @ptr: the #AgsFileUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 6.3.0
 */
void
ags_file_util_free(AgsFileUtil *ptr)
{
  if(ptr == NULL){
    return;
  }
  
  g_free(ptr->app_encoding);
  g_free(ptr->encoding);

  if(ptr->converter != NULL){
    g_iconv_close(ptr->converter);
  
    g_iconv_close(ptr->reverse_converter);
  }
  
  g_free(ptr);
}

/**
 * ags_file_util_get_app_encoding:
 * @file_util: the #AgsFileUtil-struct
 * 
 * Get application encoding of @file_util.
 *
 * Returns: (transfer full): the application encoding
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_get_app_encoding(AgsFileUtil *file_util)
{
  gchar *app_encoding;
  
  if(file_util == NULL){
    return(NULL);
  }

  app_encoding = g_strdup(file_util->app_encoding);

  return(app_encoding);
}

/**
 * ags_file_util_get_encoding:
 * @file_util: the #AgsFileUtil-struct
 * 
 * Get encoding of @file_util.
 *
 * Returns: (transfer full): the encoding
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_get_encoding(AgsFileUtil *file_util)
{
  gchar *encoding;
  
  if(file_util == NULL){
    return(NULL);
  }

  encoding = g_strdup(file_util->encoding);

  return(encoding);
}

/**
 * ags_file_util_get_file_version:
 * @file_util: the #AgsFileUtil-struct
 * 
 * Get file version of @file_util.
 *
 * Returns: (transfer full): the file version
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_get_file_version(AgsFileUtil *file_util)
{
  gchar *file_version;
  
  if(file_util == NULL){
    return(NULL);
  }

  file_version = g_strdup(file_util->file_version);

  return(file_version);
}

/**
 * ags_file_util_get_file_version:
 * @file_util: the #AgsFileUtil-struct
 * @file_version: the file version
 * 
 * set file version of @file_util.
 *
 * Since: 6.3.0
 */
void
ags_file_util_set_file_version(AgsFileUtil *file_util,
			       gchar *file_version)
{  
  if(file_util == NULL ||
     file_version == NULL){
    return;
  }

  file_util->file_version = g_strdup(file_version);
}

/**
 * ags_file_util_get_int:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string to convert
 * 
 * Get integer from string.
 *
 * Returns: the converted integer
 * 
 * Since: 6.3.0
 */
gint
ags_file_util_get_int(AgsFileUtil *file_util,
		      gchar *str)
{
  gint value;
  
  if(file_util == NULL ||
     str == NULL){
    return(0);
  }

  value = 0;
  sscanf(str, "%d",
	 &value);
  
  return(value);
}

/**
 * ags_file_util_put_int:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put integer to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_int(AgsFileUtil *file_util,
		      gint value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH,
	       "%d",
	       value);
#else
  str = g_strdup_printf("%d",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_uint:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string to convert
 * 
 * Get unsigned integer from string.
 *
 * Returns: the converted unsigned integer
 * 
 * Since: 6.3.0
 */
guint
ags_file_util_get_uint(AgsFileUtil *file_util,
		       gchar *str)
{
  guint value;
  
  if(file_util == NULL ||
     str == NULL){
    return(0);
  }

  value = (gint) g_ascii_strtoull(str,
				  NULL,
				  10);
  
  return(value);
}

/**
 * ags_file_util_put_uint:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put unsigned integer to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_uint(AgsFileUtil *file_util,
		       guint value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH,
	       "%u",
	       value);
#else
  str = g_strdup_printf("%u",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_int64:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string to convert
 * 
 * Get long long integer from string.
 *
 * Returns: the converted long long integer
 * 
 * Since: 6.3.0
 */
gint64
ags_file_util_get_int64(AgsFileUtil *file_util,
			gchar *str)
{
  gint64 value;
  
  if(file_util == NULL ||
     str == NULL){
    return(0);
  }

  value = (gint64) g_ascii_strtoll(str,
				   NULL,
				   10);
  
  return(value);
}

/**
 * ags_file_util_put_int64:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put long long integer to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_int64(AgsFileUtil *file_util,
			gint64 value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH,
	       "%li",
	       value);
#else
  str = g_strdup_printf("%li",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_uint64:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string to convert
 * 
 * Get unsigned long long integer.
 *
 * Returns: the converted unsigned long long integer
 * 
 * Since: 6.3.0
 */
guint64
ags_file_util_get_uint64(AgsFileUtil *file_util,
			 gchar *str)
{
  guint64 value;
  
  if(file_util == NULL ||
     str == NULL){
    return(0);
  }

  value = (guint64) g_ascii_strtoull(str,
				     NULL,
				     10);
  
  return(value);
}

/**
 * ags_file_util_put_uint64:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put unsigned long long integer to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_uint64(AgsFileUtil *file_util,
			 guint64 value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH,
	       "%lu",
	       value);
#else
  str = g_strdup_printf("%lu",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_float:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string
 *
 * Get arbitary size floating point number from string.
 *
 * Returns: the arbitary size floating point number
 * 
 * Since: 6.3.0
 */
gfloat
ags_file_util_get_float(AgsFileUtil *file_util,
			gchar *str)
{
  gchar *tmp_str;
  
  gfloat value;  

  AgsRegexMatch match_arr[6];
    
  gboolean success;
  
  GError *error;

  static AgsRegexUtil *float_with_comma_regex_util = NULL;

  static gboolean regex_compiled = FALSE;

  static const char *float_with_comma_pattern = "^(([+-]?)([0-9]*)(,)([0-9]+))";

  static GMutex regex_mutex;
  
  static const size_t max_matches = 6;

  if(file_util == NULL ||
     str == NULL){
    return(0.0);
  }

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    float_with_comma_regex_util = ags_regex_util_alloc(file_util->app_encoding,
						       file_util->encoding,
						       FALSE, FALSE);

    error = NULL;
    ags_regex_util_compile(float_with_comma_regex_util,
			   float_with_comma_pattern, AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
			   &error);

    if(error != NULL){
      g_warning("failed to compile regex");

      g_error_free(error);
    }
  }
  
  g_mutex_unlock(&regex_mutex);

  tmp_str = NULL;

  /*  */
  error = NULL;
  success = ags_regex_util_execute(float_with_comma_regex_util,
				   str, max_matches,
				   match_arr,
				   0,
				   &error);
  
  if(error != NULL){
    g_warning("failed to execute regex");

    g_error_free(error);
  }

  if(success){
    str = 
      tmp_str = g_strndup(str,
			  match_arr[0].end_match_offset);

    str[match_arr[4].start_match_offset] = '.';
  }
  
  value = (gfloat) g_ascii_strtod(str,
				  NULL);

  if(tmp_str != NULL){
    g_free(tmp_str);
  }
  
  return(value);
}

/**
 * ags_file_util_put_float:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put arbitary size floating point number to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_float(AgsFileUtil *file_util,
			gfloat value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH,
	       "%f",
	       value);
#else
  str = g_strdup_printf("%f",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_double:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string
 *
 * Get double precision size floating point number from string.
 * 
 * Returns: the double precision size floating point number
 *
 * Since: 6.3.0
 */
gdouble
ags_file_util_get_double(AgsFileUtil *file_util,
			 gchar *str)
{
  gchar *tmp_str;
  
  gdouble value;

  AgsRegexMatch match_arr[6];
    
  gboolean success;
  
  GError *error;

  static AgsRegexUtil *double_with_comma_regex_util = NULL;

  static gboolean regex_compiled = FALSE;

  static const char *double_with_comma_pattern = "^(([+-]?)([0-9]*)(,)([0-9]+))";

  static GMutex regex_mutex;
  
  static const size_t max_matches = 6;

  if(file_util == NULL ||
     str == NULL){
    return(0.0);
  }

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    double_with_comma_regex_util = ags_regex_util_alloc(file_util->app_encoding,
							file_util->encoding,
							FALSE, FALSE);
    
    error = NULL;
    ags_regex_util_compile(double_with_comma_regex_util,
			   double_with_comma_pattern, AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
			   &error);

    if(error != NULL){
      g_warning("failed to compile regex");

      g_error_free(error);
    }
  }
  
  g_mutex_unlock(&regex_mutex);

  tmp_str = NULL;

  /*  */
  error = NULL;
  success = ags_regex_util_execute(double_with_comma_regex_util,
				   str, max_matches,
				   match_arr,
				   0,
				   &error);
  
  if(error != NULL){
    g_warning("failed to execute regex");

    g_error_free(error);
  }

  if(success){
    str = 
      tmp_str = g_strndup(str,
			  match_arr[0].end_match_offset);

    str[match_arr[4].start_match_offset] = '.';
  }
  
  value = g_ascii_strtod(str,
			 NULL);

  if(tmp_str != NULL){
    g_free(tmp_str);
  }
  
  return(value);
}

/**
 * ags_file_util_put_double:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put double precision size floating point number to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_double(AgsFileUtil *file_util,
			 gdouble value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH * sizeof(gchar));

  xmlStrprintf(str,
	       AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH,
	       "%lf",
	       value);
#else
  str = g_strdup_printf("%lf",
			value);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_complex:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string
 *
 * Get complex floating point number from string.
 * 
 * Returns: the complex floating point number
 *
 * Since: 6.3.0
 */
AgsComplex*
ags_file_util_get_complex(AgsFileUtil *file_util,
			  gchar *str)
{
  gchar *tmp_str;
  gchar *end_str;
  
  AgsComplex *value;

  AgsRegexMatch match_arr[12];

  gboolean success;
  
  GError *error;

  static AgsRegexUtil *complex_with_comma_regex_util = NULL;
  static AgsRegexUtil *legacy_complex_with_comma_regex_util = NULL;

  static gboolean regex_compiled = FALSE;

  static const char *complex_with_comma_pattern = "^(([+-]?)([0-9]*)(,)([0-9]+)) + I * (([+-]?)([0-9]*)(,)([0-9]+))";
  static const char *legacy_complex_with_comma_pattern = "^(([+-]?)([0-9]*)(,)([0-9]+)) (([+-]?)([0-9]*)(,)([0-9]+))";

  static GMutex regex_mutex;
  
  static const size_t max_matches = 12;

  if(file_util == NULL ||
     str == NULL){
    return(NULL);
  }

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;

    /* complex */
    complex_with_comma_regex_util = ags_regex_util_alloc(file_util->app_encoding,
							 file_util->encoding,
							 FALSE, FALSE);

    error = NULL;
    ags_regex_util_compile(complex_with_comma_regex_util,
			   complex_with_comma_pattern, AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
			   &error);

    if(error != NULL){
      g_warning("failed to compile regex");

      g_error_free(error);
    }

    /* legacy complex */
    legacy_complex_with_comma_regex_util = ags_regex_util_alloc(file_util->app_encoding,
								file_util->encoding,
								FALSE, FALSE);

    error = NULL;
    ags_regex_util_compile(legacy_complex_with_comma_regex_util,
			   legacy_complex_with_comma_pattern, AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
			   &error);

    if(error != NULL){
      g_warning("failed to compile regex");

      g_error_free(error);
    }
  }
  
  g_mutex_unlock(&regex_mutex);

  tmp_str = NULL;

  /* complex */
  error = NULL;
  success = ags_regex_util_execute(complex_with_comma_regex_util,
				   str, max_matches,
				   match_arr,
				   0,
				   &error);
  
  if(error != NULL){
    g_warning("failed to execute regex");

    g_error_free(error);
  }

  if(success){
    str = 
      tmp_str = g_strndup(str,
			  match_arr[0].end_match_offset);


    if(str[match_arr[4].start_match_offset] == ','){
      str[match_arr[4].start_match_offset] = '.';
    }

    if(str[match_arr[9].start_match_offset] == ','){
      str[match_arr[9].start_match_offset] = '.';
    }
  }

  /* legacy complex */
  error = NULL;
  success = ags_regex_util_execute(legacy_complex_with_comma_regex_util,
				   str, max_matches,
				   match_arr,
				   0,
				   &error);
  
  if(error != NULL){
    g_warning("failed to execute regex");

    g_error_free(error);
  }
  
  if(success){
    str = 
      tmp_str = g_strndup(str,
			  match_arr[0].end_match_offset);


    if(str[match_arr[4].start_match_offset] == ','){
      str[match_arr[4].start_match_offset] = '.';
    }

    if(str[match_arr[9].start_match_offset] == ','){
      str[match_arr[9].start_match_offset] = '.';
    }
  }

  value = ags_complex_alloc();

  /* check legacy */
  if(strstr(str, " + I * ") == NULL){
    end_str = NULL;
  
    value->real = g_ascii_strtod(str,
				 &end_str);

    if(strlen(end_str) > 1){
      value->imag = g_ascii_strtod(end_str + 1,
				   NULL);
    }
  }else{      
    end_str = NULL;
  
    value->real = g_ascii_strtod(str,
				 &end_str);

    if(strlen(end_str) > 7){
      value->imag = g_ascii_strtod(end_str + 7,
				   NULL);
    }
  }
  
  if(tmp_str != NULL){
    g_free(tmp_str);
  }
  
  return(value);
}

/**
 * ags_file_util_put_complex:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * Put complex floating point number to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_complex(AgsFileUtil *file_util,
			  AgsComplex *value)
{
  gchar *str;
  
  if(file_util == NULL){
    return(NULL);
  }

#if 0
  str = g_malloc(((2 * AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH) + 7) * sizeof(gchar));
  xmlStrprintf(str,
	       ((2 * AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH) + 7),
	       "%lf + I * %lf",
	       (gdouble) value->real,
	       (gdouble) value->imag);
#else
  str = g_strdup_printf("%lf + I * %lf",
			(gdouble) value->real,
			(gdouble) value->imag);
#endif
  
  return(str);
}

/**
 * ags_file_util_get_string:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string
 * @max_length: the maximum length of string
 *
 * Get string from @str.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_get_string(AgsFileUtil *file_util,
			 gchar *str,
			 gint max_length)
{
  gchar *converted_string;
  
  if(file_util == NULL ||
     str == NULL){
    return(NULL);
  }

  converted_string = NULL;

  if(file_util->reverse_converter != NULL){
    GError *error;

    error = NULL;
    converted_string = g_convert_with_iconv(str,
					    max_length,
					    file_util->reverse_converter,
					    NULL,
					    NULL,
					    &error);

    if(error != NULL){
      g_warning("iconv convert failed");
      
      g_error_free(error);
    }
  }else{
    converted_string = g_strndup(str,
				 max_length);
  }
  
  return(converted_string);
}

/**
 * ags_file_util_put_string:
 * @file_util: the #AgsFileUtil-struct
 * @str: the string
 * @length: the length of string
 * 
 * Put @str to string.
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_string(AgsFileUtil *file_util,
			 gchar *str,
			 gint length)
{
  gchar *converted_string;

  if(file_util == NULL ||
     str == NULL){
    return(NULL);
  }

  converted_string = NULL;
  
  if(file_util->converter != NULL){
    GError *error;

    error = NULL;
    converted_string = g_convert_with_iconv(str,
					    length,
					    file_util->converter,
					    NULL,
					    NULL,
					    &error);

    if(error != NULL){
      g_warning("iconv convert failed");
      
      g_error_free(error);
    }
  }else{
    converted_string = g_strndup(str,
				 length);
  }
  
  return(converted_string);
}
