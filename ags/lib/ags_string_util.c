/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

/**
 * SECTION:ags_string_util
 * @short_description: string util
 * @title: AgsStringUtil
 * @section_id:
 * @include: ags/lib/ags_string_util.h
 *
 * Common string utility functions.
 */

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

