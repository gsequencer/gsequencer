#include "ags_parameter.h"

#include <stdlib.h>


void
ags_parameter_grow(guint n_params, GParameter *src,
		   guint *new_n_params, GParameter **dst,
		   const gchar *first_property_name, ...)
{
  GParameter *new_parameter;
  gchar *property_name;
  GValue value;
  guint i, count;
  va_list ap;

  /* count the new properties */
  va_start(ap, first_property_name);

  for(i = 0; ; i++){
    property_name = va_arg(ap, gchar *);
    
    if(property_name == NULL){
      break;
    }
  }
    
  va_end(ap);
  count = i;

  /* create new parameter array */
  new_parameter = (GParameter *) malloc((i + n_params) * sizeof(GParameter));

  /* add the existing parameters */
  for(i = 0; i < n_params; i++){
    new_parameter[i].name = src[i];
    g_value_copy(src[i]->value,
		 new_parameter[i].value);
  }

  /* add the new parameters */
  count += n_params;
  va_start(ap, first_property_name);

  for(i = n_params; i < count; i++){
    property_name = va_arg(ap, gchar *);
    value = va_arg(ap, GValue);

    new_parameter[i].name = property;
    g_value_copy(value,
		 new_parameter[i].value);
  }
    
  va_end(ap);

  *new_n_params = count;
  *dst = new_parameter;
}
