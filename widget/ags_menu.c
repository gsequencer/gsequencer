#include "ags_menu.h"

GType ags_menu_get_type(void);
void ags_menu_class_init(AgsMenuClass *menu);
void ags_menu_init(AgsMenu *menu);

GType
ags_menu_get_type(void)
{
  static GType ags_type_menu = 0;

  if(!ags_type_menu){
    static const GTypeInfo ags_menu_info = {
      sizeof(AgsMenuClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_menu_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMenu),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_menu_init,
    };

    ags_type_menu = g_type_register_static(AGS_TYPE_TABLE,
					   "AgsMenu\0", &ags_menu_info,
					   0);
  }

  return(ags_type_menu);
}

void
ags_menu_class_init(AgsMenuClass *menu)
{
}

void
ags_menu_init(AgsMenu *menu)
{
}

AgsMenu*
ags_menu_new()
{
  AgsMenu *menu;

  menu = (AgsMenu *) g_object_new(AGS_TYPE_MENU, NULL);
  
  return(menu);
}
