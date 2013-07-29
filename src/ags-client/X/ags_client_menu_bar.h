/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_CLIENT_MENU_BAR_H__
#define __AGS_CLIENT_MENU_BAR_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_CLIENT_MENU_BAR                (ags_client_menu_bar_get_type())
#define AGS_CLIENT_MENU_BAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLIENT_MENU_BAR, AgsClientMenuBar))
#define AGS_CLIENT_MENU_BAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CLIENT_MENU_BAR, AgsClientMenuBarClass))
#define AGS_IS_CLIENT_MENU_BAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CLIENT_MENU_BAR))
#define AGS_IS_CLIENT_MENU_BAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CLIENT_MENU_BAR))
#define AGS_CLIENT_MENU_BAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CLIENT_MENU_BAR, AgsClientMenuBarClass))

typedef struct _AgsClientMenuBar AgsClientMenuBar;
typedef struct _AgsClientMenuBarClass AgsClientMenuBarClass;

struct _AgsClientMenuBar
{
  GtkMenuBar menu_bar;
};

struct _AgsClientMenuBarClass
{
  GtkMenuBarClass menu_bar;
};

GType ags_client_menu_bar_get_type();

AgsClientMenuBar* ags_client_menu_bar_new();

#endif /*__AGS_CLIENT_MENU_BAR_H__*/
