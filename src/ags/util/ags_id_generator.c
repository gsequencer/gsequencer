/* This file is part of GSequencer.
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
#include <ags/util/ags_id_generator.h>

#include <stdlib.h>

#include <uuid/uuid.h>

/**
 * SECTION:ags_id_generator
 * @short_description: UUID util functions
 * @title: AgsIdGenerator
 * @section_id:
 * @include: ags/util/ags_id_generator.h
 *
 * Functions related to UUID.
 */

#define AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH 36

/**
 * ags_id_generator_create_uuid:
 *
 * Creates a new string containing UUID. 
 *
 * Returns: the string
 *
 * Since: 0.4
 */
gchar*
ags_id_generator_create_uuid()
{
  uuid_t out;
  gchar *uuid_str;

  /* make sure uuid is really unique */
  while(uuid_generate_time_safe(out) != 0);

  /* create a string representation */
  uuid_str = (gchar *) malloc((AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH + 1) * sizeof(gchar));
  uuid_unparse(out, uuid_str);
  uuid_str[AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH] = '\0';

  return(uuid_str);
}
