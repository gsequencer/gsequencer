/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
  
  while((iter = index(iter, '\'')) != NULL){
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
    
    while((iter = index(iter, '\'')) != NULL){
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

    memcpy(&(retval[offset]), prev, (index(str, '\0') - prev) * sizeof(char));
  }else{
    retval = g_strdup(str);
  }

  return(retval);
}

guint
ags_strv_length(gchar **str_array)
{
  //TODO:JK: implement me

  return(0);
}

gboolean
ags_strv_contains(gchar **str_array,
		  gchar *str)
{
  //TODO:JK: implement me

  return(FALSE);
}
