#include <ags/object/ags_effect.h>

#include <stdio.h>

void ags_effect_base_init(AgsEffectInterface *interface);

GType
ags_effect_get_type()
{
  static GType ags_type_effect = 0;

  if(!ags_type_effect){
    static const GTypeInfo ags_effect_info = {
      sizeof(AgsEffectInterface),
      (GBaseInitFunc) ags_effect_base_init,
      NULL, /* base_finalize */
    };

    ags_type_effect = g_type_register_static(G_TYPE_INTERFACE,
					     "AgsEffect\0", &ags_effect_info,
					     0);
  }

  return(ags_type_effect);
}

void
ags_effect_base_init(AgsEffectInterface *interface)
{
  /* empty */
}
