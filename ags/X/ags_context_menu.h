/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
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

#ifndef __AGS_CONTEXT_MENU_H__
#define __AGS_CONTEXT_MENU_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_CONTEXT_MENU                (ags_context_menu_get_type ())
#define AGS_CONTEXT_MENU(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONTEXT_MENU, AgsContextMenu))
#define AGS_CONTEXT_MENU_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONTEXT_MENU, AgsContextMenuClass))
#define AGS_IS_CONTEXT_MENU(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONTEXT_MENU))
#define AGS_IS_CONTEXT_MENU_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONTEXT_MENU))
#define AGS_CONTEXT_MENU_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CONTEXT_MENU, AgsContextMenuClass))

#define AGS_MENU_ITEM_FILENAME_KEY "ags-menu-bar-filename-key\0"
#define AGS_MENU_ITEM_EFFECT_KEY "ags-menu-bar-effect-key\0"

typedef struct _AgsContextMenu AgsContextMenu;
typedef struct _AgsContextMenuClass AgsContextMenuClass;

typedef enum{
  AGS_CONTEXT_MENU_CONNECTED        = 1,
};

struct _AgsContextMenu
{
  GtkMenu context_menu;

  guint flags;
  
  GtkMenu *file;

  GtkMenu *edit;
  GtkMenu *add;
  GtkMenu *live;

  GtkMenu *midi;
  
  GtkMenu *help;
};

struct _AgsContextMenuClass
{
  GtkMenuClass context_menu;
};

GType ags_context_menu_get_type(void);

AgsContextMenu* ags_context_menu_new();

#endif /*__AGS_CONTEXT_MENU_H__*/
