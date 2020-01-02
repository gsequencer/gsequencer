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

#include <ags/lib/ags_uuid.h>

#include <stdlib.h>
#include <string.h>

#include <uuid/uuid.h>

/**
 * SECTION:ags_uuid
 * @short_description: Boxed type of uuid
 * @title: AgsUUID
 * @section_id:
 * @include: ags/lib/ags_uuid.h
 *
 * Boxed type of uuid_t data type.
 */

GType
ags_uuid_get_type(void)
{
  static GType ags_type_uuid = 0;

  if(!ags_type_uuid){
    ags_type_uuid =
      g_boxed_type_register_static("AgsUUID",
				   (GBoxedCopyFunc) ags_uuid_copy,
				   (GBoxedFreeFunc) ags_uuid_free);
  }
  
  return(ags_type_uuid);
}

/**
 * ags_uuid_alloc:
 * 
 * Allocate #AgsUUID.
 * 
 * Returns: a new #AgsUUID
 * 
 * Since: 2.0.0
 */
AgsUUID*
ags_uuid_alloc()
{
  AgsUUID *ptr;

  ptr = (AgsUUID *) malloc(sizeof(AgsUUID));
  memset(ptr, 0, sizeof(AgsUUID));

  return(ptr);
}

/**
 * ags_uuid_copy:
 * @ptr: the original #AgsUUID
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsUUID
 *
 * Since: 2.0.0
 */
gpointer
ags_uuid_copy(AgsUUID *ptr)
{
  AgsUUID *new_ptr;

  new_ptr = (AgsUUID *) malloc(sizeof(AgsUUID));
  memcpy(new_ptr->data, ptr->data, sizeof(AgsUUID));

  return(ptr);
}

/**
 * ags_uuid_free:
 * @ptr: the #AgsUUID
 *
 * Free the memory of @ptr.
 *
 * Since: 2.0.0
 */
void
ags_uuid_free(AgsUUID *ptr)
{
  g_free(ptr);
}

/**
 * ags_uuid_generate:
 * @ptr: the #AgsUUID
 * 
 * Generate random UUID.
 * 
 * Since: 2.0.0
 */
void
ags_uuid_generate(AgsUUID *ptr)
{
  if(ptr == NULL){
    return;
  }

  uuid_generate_time_safe(ptr->data);
}

/**
 * ags_uuid_compare:
 * @a: an #AgsUUID
 * @b: another #AgsUUID
 * 
 * Compare @a with @b.
 *
 * Returns: 0 on success, otherwise a value less or greater than 0
 * 
 * Since: 2.0.0
 */
gint
ags_uuid_compare(AgsUUID *a,
		 AgsUUID *b)
{
  gint retval;
  
  retval = uuid_compare(a->data,
			b->data);

  return(retval);
}

/**
 * ags_uuid_to_string:
 * @ptr: the #AgsUUID
 * 
 * Get string representation of @ptr.
 * 
 * Returns: the UUID as string if @ptr not %NULL, else %NULL
 * 
 * Since: 2.0.0
 */
gchar*
ags_uuid_to_string(AgsUUID *ptr)
{
  gchar *uuid_str;

  if(ptr == NULL){
    return(NULL);
  }
  
  uuid_str = (gchar *) malloc((AGS_UUID_STRING_DEFAULT_LENGTH + 1) * sizeof(gchar));
  uuid_unparse(ptr->data,
	       uuid_str);

  return(uuid_str);
}

/**
 * ags_uuid_from_string:
 * @str: the UUID as string
 * 
 * Retrieve #AgsUUID by parsing @str.
 * 
 * Returns: the new #AgsUUID on success, otherwise %NULL
 * 
 * Since: 2.0.0
 */
AgsUUID*
ags_uuid_from_string(gchar *str)
{
  AgsUUID *id;
  
  int retval;
  
  if(str == NULL){
    return(NULL);
  }

  id = ags_uuid_alloc();
  retval = uuid_parse(str, id->data);

  if(retval != 0){
    ags_uuid_free(id);

    return(NULL);
  }

  return(id);
}
