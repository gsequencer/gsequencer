#include <ags/widget/gs_menu_item.h>

void ags_menu_item_class_init(AgsMenuItemClass *menu_item);
void ags_menu_item_init(AgsMenuItem *menu_item);

GType
ags_menu_item_get_type(void)
{
  static GType ags_type_menu_item = 0;

  if(!ags_type_menu_item){
    static const GTypeInfo ags_menu_item_info = {
      sizeof(AgsMenuItemClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_menu_item_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMenuItem),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_menu_item_init,
    };

    ags_type_menu_item = g_type_register_static(AGS_TYPE_TABLE,
					   "AgsMenuItem\0", &ags_menu_item_info,
					   0);
  }

  return(ags_type_menu_item);
}

void
ags_menu_item_class_init(AgsMenuItemClass *menu_item)
{
}

void
ags_menu_item_init(AgsMenuItem *menu_item)
{
}

AgsMenuItem*
ags_menu_item_new()
{
  AgsMenuItem *menu_item;

  menu_item = (AgsMenuItem *) g_object_new(AGS_TYPE_MENU_ITEM, NULL);
  
  return(menu_item);
}
