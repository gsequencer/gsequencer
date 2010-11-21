#include <ags/object/ags_tactable.h>

void ags_tactable_base_init(AgsTactableInterface *interface);

GType
ags_tactable_get_type()
{
  static GType ags_type_tactable = 0;

  if(!ags_type_tactable){
    static const GTypeInfo ags_tactable_info = {
      sizeof(AgsTactableInterface),
      (GBaseInitFunc) ags_tactable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_tactable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsTactable\0", &ags_tactable_info,
					       0);
  }

  return(ags_type_tactable);
}

void
ags_tactable_base_init(AgsTactableInterface *interface)
{
  /* empty */
}

void
ags_tactable_change_bpm(AgsTactable *tactable, double bpm, double old_bpm)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_bpm);
  tactable_interface->change_bpm(tactable, bpm, old_bpm);
}
