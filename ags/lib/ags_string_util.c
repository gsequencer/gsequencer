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

#include <ags/lib/ags_string_util.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

gpointer ags_string_util_copy(gpointer ptr);
void ags_string_util_free(gpointer ptr);

/**
 * SECTION:ags_string_util
 * @short_description: string util
 * @title: AgsStringUtil
 * @section_id:
 * @include: ags/lib/ags_string_util.h
 *
 * Common string utility functions.
 */

GType
ags_string_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_string_util = 0;

    ags_type_string_util =
      g_boxed_type_register_static("AgsStringUtil",
				   (GBoxedCopyFunc) ags_string_util_copy,
				   (GBoxedFreeFunc) ags_string_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_string_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_string_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsStringUtil));
 
  return(retval);
}

void
ags_string_util_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_string_util_escape_single_quote:
 * @str: the string to escape
 * 
 * Escape all occurence of single quotes.
 * 
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 3.0.0
 */
gchar*
ags_string_util_escape_single_quote(gchar *str)
{
  gchar *retval;
  gchar *iter, *prev;
  guint count;
  guint n_chars;
  guint offset;

  if(str == NULL){
    return(NULL);
  }
  
  iter = str;
  count = 0;
  
  while((iter = strchr(iter, '\'')) != NULL){
    count++;
    iter++;
  }

  if(count > 0){
    n_chars = strlen(str);
    
    retval = (gchar *) malloc((n_chars + (5 * count) + 1) * sizeof(gchar));
    retval[n_chars + (5 * count)] = '\0';

    iter = str;
    prev = str;
    offset = 0;
    
    while((iter = strchr(iter, '\'')) != NULL){
      memcpy(&(retval[offset]), prev, (iter - prev) * sizeof(char));      
      retval[offset + (iter - prev)] = '&';
      retval[offset + (iter - prev + 1)] = 'a';
      retval[offset + (iter - prev + 2)] = 'p';
      retval[offset + (iter - prev + 3)] = 'o';
      retval[offset + (iter - prev + 4)] = 's';
      retval[offset + (iter - prev + 5)] = ';';
      
      offset += (iter - prev + 6);
      iter++;
      prev = iter;
    }

    memcpy(&(retval[offset]), prev, (strchr(str, '\0') - prev) * sizeof(char));
  }else{
    retval = g_strdup(str);
  }

  return(retval);
}

/**
 * ags_string_util_to_mcoded7:
 * @str: the string to escape
 * 
 * String to mcoded7 encoding.
 * 
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 5.5.0
 */
gchar*
ags_string_util_to_mcoded7(gchar *str)
{
  gchar *mcoded7_str;
  
  guint length;
  guint i, i_stop;
  guint j, k;

  if(str == NULL){
    return(NULL);
  }
  
  length = strlen(str);

  i_stop = 8 * ceil(length / 8);

  mcoded7_str = (gchar *) malloc(i_stop * sizeof(gchar));
  memset(mcoded7_str, 0, i_stop * sizeof(gchar));
  
  for(i = 0, j = 0; i < i_stop;){
    for(k = 0; k < 8 && j + k < length; k++){
      mcoded7_str[i] = 0x80 & (str[j]) >> (k + 1);
    }
    
    mcoded7_str[i + 1] = 0x7f & (str[j]);

    if(j + 1 < length){
      mcoded7_str[i + 2] = 0x7f & (str[j + 1]);
    }else{
      mcoded7_str[i + 2] = 0x0;
    }
    
    if(j + 1 < length){
      mcoded7_str[i + 3] = 0x7f & (str[j + 2]);
    }else{
      mcoded7_str[i + 3] = 0x0;
    }
    
    if(j + 1 < length){
      mcoded7_str[i + 4] = 0x7f & (str[j + 3]);
    }else{
      mcoded7_str[i + 4] = 0x0;
    }
    
    if(j + 1 < length){
      mcoded7_str[i + 5] = 0x7f & (str[j + 4]);
    }else{
      mcoded7_str[i + 5] = 0x0;
    }
    
    if(j + 1 < length){
      mcoded7_str[i + 6] = 0x7f & (str[j + 5]);
    }else{
      mcoded7_str[i + 6] = 0x0;
    }
    
    if(j + 1 < length){
      mcoded7_str[i + 7] = 0x7f & (str[j + 6]);
    }else{
      mcoded7_str[i + 7] = 0x0;
    }    
    
    i += 8;
    j += 7;
  }
  
  return(mcoded7_str);
}

/**
 * ags_string_util_from_mcoded7:
 * @mcoded7_str: the string to escape
 * 
 * String from mcoded7 encoding.
 * 
 * Returns: (transfer full): the newly allocated string
 * 
 * Since: 5.5.0
 */
gchar*
ags_string_util_from_mcoded7(gchar *mcoded7_str)
{
  gchar *str;
  
  guint length;
  guint i, i_stop;
  guint j, k;

  if(str == NULL){
    return(NULL);
  }
  
  length = strlen(mcoded7_str);
  
  if(length % 8 != 0){
    g_critical("invalid mcoded7 string");

    return(NULL);
  }
  
  i_stop = length;

  str = (gchar *) malloc((i_stop * 8) * sizeof(gchar));
  memset(str, 0, (i_stop * 8) * sizeof(gchar));

  for(i = 0, j = 0; i < i_stop;){
    k = 1;
    
    str[i] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 1] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 2] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 3] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 4] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 5] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    str[i + 6] = ((0x80 >> k) & (mcoded7_str[j])) | (mcoded7_str[j + 1]);
    k++;
    
    i += 7;
    j += 8;
  }
  
  return(str);
}

/**
 * ags_strv_length:
 * @str_array: (element-type utf8) (array zero-terminated=1) (transfer none): the string vector
 * 
 * Count the number of non-%NULL entries in the array.
 * 
 * Returns: the length of the vector
 * 
 * Since: 3.0.0
 */
guint
ags_strv_length(gchar **str_array)
{
  guint i;
  
  if(str_array == NULL){
    return(0);
  }

  for(i = 0; *str_array != NULL; i++, str_array++);

  return(i);
}

/**
 * ags_strv_contains:
 * @str_array: (element-type utf8) (array zero-terminated=1) (transfer none): the string vector
 * @str: the string to match
 * 
 * Check occurence of @str within @str_array
 * 
 * Returns: %TRUE if found, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_strv_contains(gchar **str_array,
		  gchar *str)
{
  guint i;
  
  if(str_array == NULL){
    return(FALSE);
  }

  for(i = 0; str_array[0] != NULL; i++, str_array++){
    if(str_array[0] == str ||
       !g_strcmp0(str_array[0],
		  str)){
      return(TRUE);
    }
  }

  return(FALSE);
}

/**
 * ags_strv_index:
 * @str_array: (element-type utf8) (array zero-terminated=1) (transfer none): the string vector
 * @str: the string to match
 * 
 * Check by comparing @str against @str_array items.
 * 
 * Returns: the first matching position or -1, if not found
 * 
 * Since: 3.0.0
 */
gint
ags_strv_index(gchar **str_array,
	       gchar *str)
{
  gint i;
  
  if(str_array == NULL){
    return(-1);
  }

  for(i = 0; str_array[0] != NULL; i++, str_array++){
    if(!g_strcmp0(str_array[0], str)){
      return(i);
    }
  }

  return(-1);
}

/**
 * ags_strv_equal:
 * @strv1: the string vector
 * @strv2: an other string vector
 * 
 * Check @strv1 and @strv2 to be equal. The arrays don't have to be
 * sorted.
 * 
 * Returns: %TRUE on success, otherwise %FALSE 
 * 
 * Since: 3.2.1
 */
gboolean
ags_strv_equal(gchar **strv1,
	       gchar **strv2)
{
  gchar **iter;

  gboolean success;
  
  if(strv1 == NULL &&
     strv2 == NULL){
    return(TRUE);
  }

  if((strv1 == NULL &&
      strv2 != NULL) ||
     (strv1 != NULL &&
      strv2 == NULL)){
    return(FALSE);
  }

  if(g_strv_length(strv1) != g_strv_length(strv2)){
    return(FALSE);
  }

  success = TRUE;
  
  for(iter = strv2; success && iter[0] != NULL; iter++){
    if(!ags_strv_contains(strv1, iter[0])){
      success = FALSE;
    }
  }

  return(success);
}
