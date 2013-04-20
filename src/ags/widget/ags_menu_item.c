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

#include <ags/widget/ags_menu_item.h>

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
