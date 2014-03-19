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
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_server_preferences_connect;
  connectable->disconnect = ags_server_preferences_disconnect;
}

void
ags_server_preferences_init(AgsServerPreferences *server_preferences)
{
  GtkTable *table;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(2, 5, FALSE);
  gtk_box_pack_start(GTK_BOX(server_preferences),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  server_preferences->start = (GtkCheckButton *) gtk_check_button_new_with_label("start server\0");
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->start),
		   0, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "address\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->address = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->address),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* port */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "port\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->port = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->port),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* username */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "username\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->username = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->username),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* password */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "password\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->password = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_visibility(server_preferences->password, FALSE);
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->password),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);
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
