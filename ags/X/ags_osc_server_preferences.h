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

#ifndef __AGS_OSC_SERVER_PREFERENCES_H__
#define __AGS_OSC_SERVER_PREFERENCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_SERVER_PREFERENCES                (ags_osc_server_preferences_get_type())
#define AGS_OSC_SERVER_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_SERVER_PREFERENCES, AgsOscServerPreferences))
#define AGS_OSC_SERVER_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_SERVER_PREFERENCES, AgsOscServerPreferencesClass))
#define AGS_IS_OSC_SERVER_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OSC_SERVER_PREFERENCES))
#define AGS_IS_OSC_SERVER_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OSC_SERVER_PREFERENCES))
#define AGS_OSC_SERVER_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OSC_SERVER_PREFERENCES, AgsOscServerPreferencesClass))

typedef struct _AgsOscServerPreferences AgsOscServerPreferences;
typedef struct _AgsOscServerPreferencesClass AgsOscServerPreferencesClass;

typedef enum{
  AGS_OSC_SERVER_PREFERENCES_CONNECTED    = 1,
  AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE = 1 <<  1,
}AgsOscServerPreferencesFlags;

struct _AgsOscServerPreferences
{
  GtkVBox vbox;

  guint flags;
  
  GtkCheckButton *auto_start;
  GtkButton *start;
  GtkButton *stop;

  GtkCheckButton *any_address;
  
  GtkCheckButton *enable_ip4;
  GtkEntry *ip4_address;

  GtkCheckButton *enable_ip6;
  GtkEntry *ip6_address;

  GtkEntry *port;

  GtkSpinButton *monitor_timeout;
};

struct _AgsOscServerPreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_osc_server_preferences_get_type(void);

AgsOscServerPreferences* ags_osc_server_preferences_new();

G_END_DECLS

#endif /*__AGS_OSC_SERVER_PREFERENCES_H__*/
