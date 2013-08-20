/* AGS - Advanced GTK Sequencer
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

#ifndef __AGS_SERVER_PREFERENCES_H__
#define __AGS_SERVER_PREFERENCES_H__

#include <gtk/gtk.h>

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
};

struct _AgsServerPreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_server_preferences_get_type(void);

AgsServerPreferences* ags_server_preferences_new();

#endif /*__AGS_SERVER_PREFERENCES_H__*/
