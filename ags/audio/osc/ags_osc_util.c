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

#include <ags/audio/osc/ags_osc_util.h>

#include <ags/libags.h>

#include <string.h>

/**
 * ags_osc_util_type_tag_string_count_type:
 * @type_tag: the type tag string
 * 
 * Count types in @type_tag.
 * 
 * Returns: the count of types specified
 * 
 * Since: 2.1.0
 */
guint
ags_osc_util_type_tag_string_count_type(gchar *type_tag)
{
  gsize count;

  if(type_tag == NULL ||
     type_tag[0] != ','){
    return(0);
  }
  
  count = strlen(type_tag);

  if(count <= 0){
    return(0);
  }

  count -= 1;
  
  return(count);
}
