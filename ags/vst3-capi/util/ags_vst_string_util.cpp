/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/vst3-capi/util/ags_vst_string_util.h>

extern "C" {

  gunichar2*
  ags_vst_string_util_to_unicode16(gchar *source_str)
  {
    gunichar2 *destination_str;

    gsize destination_length;

    destination_str = NULL;
    destination_length = -1;

    ags_vst_string_util_to_unicode16_extended(str,
					      &destination_str, &destination_length);

    return(destination_str);
  }

  void ags_vst_string_util_to_unicode16_extended(gchar *source_str,
						 gunichar2 **destination_str, gsize *destination_length)
  {
    gunichar2 *str;

    glong items_written;
    
    GError *error;
    
    if(source_str == NULL){
      if(destination_str != NULL){
	destination_str[0] = NULL;
      }
      
      if(destination_length != NULL){
	destination_length[0] = 0;
      }
      
      return;
    }

    error = NULL;
    
    if(destination_length != NULL){
      str = g_utf8_to_utf16(source_str,
			    destination_length[0],
			    NULL,
			    &items_written,
			    &error);
    }else{
      str = g_utf8_to_utf16(source_str,
			    -1,
			    NULL,
			    &items_written,
			    &error);
    }

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
    
    if(destination_str != NULL){
      if(destination_str[0] != NULL){
	gsize length;
	
	memcpy(destination_str[0], str, length * sizeof(gunichar2));
	destination_str[length] = 0x0;
      }else{
	destination_str[0] = str;
      }
    }else{
      g_free(str);
    }

    if(destination_length != NULL){
      destination_length[0] = items_written;
    }
  }

  gchar*
  ags_vst_string_util_from_unicode16(gunichar2 *source_str)
  {
    gchar *str;

    GError *error;

    error = NULL;

    str = g_utf16_to_utf8(source_str,
			  -1,
			  NULL,
			  NULL,
			  &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
    
    return(str);
  }

}
