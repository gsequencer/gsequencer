#include "ags_applicable.h"

#include <stdio.h>

void ags_applicable_base_init(AgsApplicableInterface *interface);

GType
ags_applicable_get_type()
{
  static GType ags_type_applicable = 0;

  if(!ags_type_applicable){
    static const GTypeInfo ags_applicable_info = {
      sizeof(AgsApplicableInterface),
      (GBaseInitFunc) ags_applicable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_applicable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsApplicable\0", &ags_applicable_info,
						 0);
  }

  return(ags_type_applicable);
}

void
ags_applicable_base_init(AgsApplicableInterface *interface)
{
  /* empty */
}

void
ags_applicable_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->set_update);
  applicable_interface->set_update(applicable, update);
}

void
ags_applicable_apply(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->apply);
  applicable_interface->apply(applicable);
}

void
ags_applicable_reset(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->reset);
  applicable_interface->reset(applicable);
}
