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

#include <ags/X/ags_server_preferences.h>
#include <ags/X/ags_server_preferences_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_server_preferences_class_init(AgsServerPreferencesClass *server_preferences);
void ags_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_preferences_init(AgsServerPreferences *server_preferences);
void ags_server_preferences_connect(AgsConnectable *connectable);
void ags_server_preferences_disconnect(AgsConnectable *connectable);
static void ags_server_preferences_finalize(GObject *gobject);
void ags_server_preferences_show(GtkWidget *widget);

static gpointer ags_server_preferences_parent_class = NULL;

GType
ags_server_preferences_get_type(void)
{
  static GType ags_type_server_preferences = 0;

  if(!ags_type_server_preferences){
    static const GTypeInfo ags_server_preferences_info = {
      sizeof (AgsServerPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServerPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_server_preferences = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsServerPreferences\0", &ags_server_preferences_info,
							 0);
    
    g_type_add_interface_static(ags_type_server_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_server_preferences);
}

void
ags_server_preferences_class_init(AgsServerPreferencesClass *server_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_server_preferences_parent_class = g_type_class_peek_parent(server_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) server_preferences;

  gobject->finalize = ags_server_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) server_preferences;

  widget->show = ags_server_preferences_show;
}

void
ags_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_server_preferences_connect;
  connectable->disconnect = ags_server_preferences_disconnect;
}

void
ags_server_preferences_init(AgsServerPreferences *server_preferences)
{
}

void
ags_server_preferences_connect(AgsConnectable *connectable)
{
  AgsServerPreferences *server_preferences;

  server_preferences = AGS_SERVER_PREFERENCES(connectable);
}

void
ags_server_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_server_preferences_finalize(GObject *gobject)
{
}

void
ags_server_preferences_show(GtkWidget *widget)
{
  AgsServerPreferences *server_preferences;
  pthread_t thread;

  server_preferences = AGS_SERVER_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_server_preferences_parent_class)->show(widget);
}

AgsServerPreferences*
ags_server_preferences_new()
{
  AgsServerPreferences *server_preferences;

  server_preferences = (AgsServerPreferences *) g_object_new(AGS_TYPE_SERVER_PREFERENCES,
							     NULL);
  
  return(server_preferences);
}
