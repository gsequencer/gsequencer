/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SERVER_PREFERENCES_H__
#define __AGS_SERVER_PREFERENCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_SERVER_PREFERENCES                (ags_server_preferences_get_type())
#define AGS_SERVER_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER_PREFERENCES, AgsServerPreferences))
#define AGS_SERVER_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SERVER_PREFERENCES, AgsServerPreferencesClass))
#define AGS_IS_SERVER_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SERVER_PREFERENCES))
#define AGS_IS_SERVER_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SERVER_PREFERENCES))
#define AGS_SERVER_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SERVER_PREFERENCES, AgsServerPreferencesClass))

typedef struct _AgsServerPreferences AgsServerPreferences;
typedef struct _AgsServerPreferencesClass AgsServerPreferencesClass;

struct _AgsServerPreferences
{
  GtkVBox vbox;

  GtkCheckButton *start;

  GtkEntry *address;
  GtkEntry *port;

  GtkEntry *username;
  GtkEntry *password;
};

struct _AgsServerPreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_server_preferences_get_type(void);

AgsServerPreferences* ags_server_preferences_new();

G_END_DECLS

#endif /*__AGS_SERVER_PREFERENCES_H__*/
