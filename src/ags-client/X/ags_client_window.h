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

#ifndef __AGS_SCRIPT_EDITOR_H__
#define __AGS_CLIENT_WINDOW_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags-client/X/ags_client_menu_bar.h>
#include <ags-client/X/ags_client_toolbar.h>
#include <ags-client/X/ags_script_editor.h>
#include <ags-client/X/ags_client_log.h>

#define AGS_TYPE_CLIENT_WINDOW                (ags_client_window_get_type())
#define AGS_CLIENT_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLIENT_WINDOW, AgsClientWindow))
#define AGS_CLIENT_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CLIENT_WINDOW, AgsClientWindowClass))
#define AGS_IS_CLIENT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CLIENT_WINDOW))
#define AGS_IS_CLIENT_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CLIENT_WINDOW))
#define AGS_CLIENT_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CLIENT_WINDOW, AgsClientWindowClass))

typedef struct _AgsClientWindow AgsClientWindow;
typedef struct _AgsClientWindowClass AgsClientWindowClass;

struct _AgsClientWindow
{
  GtkWindow window;

  AgsClientMenuBar *menu_bar;
  AgsScriptEditor *script_editor;
  AgsClientToolbar *toolbar;
  AgsClientLog *log;
};

struct _AgsClientWindowClass
{
  GtkWindowClass window;
};

GType ags_client_window_get_type();

AgsClientWindow* ags_client_window_new();

#endif /*__AGS_CLIENT_WINDOW_H__*/
