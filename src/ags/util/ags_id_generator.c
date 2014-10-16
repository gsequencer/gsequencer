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
 * Creates an new string containing UUID. 
 *
 * Returns: a new string
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
