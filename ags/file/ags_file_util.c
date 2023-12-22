/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

  ptr = (AgsFileUtil *) malloc(sizeof(AgsFileUtil));
  
  ptr[0].app_encoding = g_strdup(app_encoding);
  ptr[0].encoding = g_strdup(encoding);

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
  
  new_ptr = (AgsFileUtil *) malloc(sizeof(AgsFileUtil));
  
  new_ptr->app_encoding = g_strdup(ptr->app_encoding);
  new_ptr->encoding = g_strdup(ptr->encoding);

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
  g_free(ptr->app_encoding);
  g_free(ptr->encoding);
  
  g_free(ptr);
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
  
  if(file_util == NULL){
    return(0);
  }

  value = (gint) g_ascii_strtoll(str,
				 NULL,
				 10);
  
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

  str = g_strdup_printf("%d",
			value);
  
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
  
  if(file_util == NULL){
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

  str = g_strdup_printf("%u",
			value);
  
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
  
  if(file_util == NULL){
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
  
  str = g_strdup_printf("%llu",
			value);
  
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
  
  if(file_util == NULL){
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
  
  str = g_strdup_printf("%lld",
			value);
  
  return(str);
}

/**
 * ags_file_util_get_float:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gfloat
ags_file_util_get_float(AgsFileUtil *file_util,
			gchar *str)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_put_float:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * 
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_float(AgsFileUtil *file_util,
			gfloat value)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_get_double:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gdouble
ags_file_util_get_double(AgsFileUtil *file_util,
			 gchar *str)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_put_double:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * 
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_double(AgsFileUtil *file_util,
			 gdouble value)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_get_complex:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
AgsComplex*
ags_file_util_get_complex(AgsFileUtil *file_util,
			  gchar *str)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_put_complex:
 * @file_util: the #AgsFileUtil-struct
 * @value: the value
 * 
 * 
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_complex(AgsFileUtil *file_util,
			  AgsComplex *value)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_get_string:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
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
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_put_string:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
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
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_get_base64:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_get_base64(AgsFileUtil *file_util,
			 gchar *str,
			 gsize *data_length)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_put_base64:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 6.3.0
 */
gchar*
ags_file_util_put_base64(AgsFileUtil *file_util,
			 gchar *data,
			 gsize data_length)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_printf:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gint
ags_file_util_printf(AgsFileUtil *file_util,
		     gchar *format,
		     ...)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_sscanf:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gint
ags_file_util_sscanf(AgsFileUtil *file_util,
		     gchar *str,
		     gchar *format,
		     ...)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_vprintf:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gint
ags_file_util_vprintf(AgsFileUtil *file_util,
		      gchar *format,
		      va_list args)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}

/**
 * ags_file_util_vsscanf:
 * @file_util: the #AgsFileUtil-struct
 * 
 * 
 *
 * Since: 6.3.0
 */
gint
ags_file_util_vsscanf(AgsFileUtil *file_util,
		      gchar *str,
		      gchar *format,
		      va_list args)
{
  if(file_util == NULL){
    return();
  }
  
  return();
}
