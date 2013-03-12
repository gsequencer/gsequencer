/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/widget/ags_menu.h>

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

    ags_type_menu = g_type_register_static(GTK_TYPE_WINDOW,
					   "AgsMenu", &ags_menu_info,
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
  GValue window_type_value = {0,};

  g_value_init(&window_type_value, G_TYPE_ENUM);
  g_value_set_enum(&window_type_value, GTK_WINDOW_POPUP);
  g_object_set_property(G_OBJECT(menu),
			"type",
			&window_type_value);
  g_value_unset(&window_type_value);

  menu->table = ags_table_new(0, 0, FALSE);
}

AgsMenu*
ags_menu_new()
{
  AgsMenu *menu;

  menu = (AgsMenu *) g_object_new(AGS_TYPE_MENU, NULL);
  
  return(menu);
}
