#include <ags/util/ags_id_generator.h>

#include <uuid/uuid.h>

#define AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH 16

gchar*
ags_id_generator_create_uuid()
{
  uuid_t out;
  gchar *uuid_str;

  /* make sure uuid is really unique */
  while(uuid_generate_time_safe(out) != 0);

  /* create a string representation */
  uuid_str = (gchar *) malloc((AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH + 1) * sizeof(gchar));
  uuid_str[AGS_ID_GENERATOR_DEFAULT_UUID_LENGTH] = '\0';
  uuid_unparse(out, uuid_str);

  return(uuid_str);
}
