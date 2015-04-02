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

#ifndef __AGS_MENU_ITEM_H__
#define __AGS_MENU_ITEM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/widget/ags_table.h>

#define AGS_TYPE_MENU_ITEM                (ags_menu_get_type())
#define AGS_MENU_ITEM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENU_ITEM, AgsMenuItem))
#define AGS_MENU_ITEM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENU_ITEM, AgsMenuItemClass))
#define AGS_IS_MENU_ITEM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MENU_ITEM))
#define AGS_IS_MENU_ITEM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MENU_ITEM))
#define AGS_MENU_ITEM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MENU_ITEM, AgsMenuItemClass))

typedef struct _AgsMenuItem AgsMenuItem;
typedef struct _AgsMenuItemClass AgsMenuItemClass;

struct _AgsMenuItem
{
  AgsTable table;
};

struct _AgsMenuItemClass
{
  AgsTableClass table;
};

GType ags_menu_item_get_type(void);

AgsMenuItem* ags_menu_item_new();

#endif /*__AGS_MENU_ITEM_H__*/
