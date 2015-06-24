/* This file is part of GSequencer.
 * 
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_MENU_H__
#define __AGS_MENU_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/widget/ags_table.h>

#define AGS_TYPE_MENU                (ags_menu_get_type())
#define AGS_MENU(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENU, AgsMenu))
#define AGS_MENU_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENU, AgsMenuClass))
#define AGS_IS_MENU(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MENU))
#define AGS_IS_MENU_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MENU))
#define AGS_MENU_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MENU, AgsMenuClass))

typedef struct _AgsMenu AgsMenu;
typedef struct _AgsMenuClass AgsMenuClass;

struct _AgsMenu
{
  GtkWindow window;

  AgsTable *table;
};

struct _AgsMenuClass
{
  GtkWindowClass window;
};

GType ags_menu_get_type(void);

AgsMenu* ags_menu_new();

#endif /*__AGS_MENU_H__*/
