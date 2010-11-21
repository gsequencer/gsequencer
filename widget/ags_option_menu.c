#include <ags/widget/ags_option_menu.h>

void ags_option_menu_class_init(AgsOptionMenuClass *option_menu);
void ags_option_menu_init(AgsOptionMenu *option_menu);

void ags_option_menu_changed(AgsOptionMenu *option_menu);

GType
ags_option_menu_get_type(void)
{
  static GType ags_type_option_menu = 0;

  if(!ags_type_option_menu){
    static const GTypeInfo ags_option_menu_info = {
      sizeof(AgsOptionMenuClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_option_menu_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOptionMenu),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_option_menu_init,
    };

    ags_type_option_menu = g_type_register_static(GTK_TYPE_HBOX,
						  "AgsOptionMenu\0", &ags_option_menu_info,
						  0);
  }

  return(ags_type_option_menu);
}

void
ags_option_menu_class_init(AgsOptionMenuClass *option_menu)
{
  option_menu->changed = NULL;
  option_menu->focus_changed = NULL;
}

void
ags_option_menu_init(AgsOptionMenu *option_menu)
{


  option_menu->menu = NULL;
  option_menu->menu_item = NULL;
  option_menu->focus = NULL;
}

AgsOptionMenu*
ags_option_menu_new()
{
  AgsOptionMenu *option_menu;

  option_menu = (AgsOptionMenu *) g_object_new(AGS_TYPE_OPTION_MENU, NULL);
  
  return(option_menu);
}
