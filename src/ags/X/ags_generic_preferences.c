/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/ags_generic_preferences.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences);
void ags_generic_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_preferences_init(AgsGenericPreferences *generic_preferences);
void ags_generic_preferences_connect(AgsConnectable *connectable);
void ags_generic_preferences_disconnect(AgsConnectable *connectable);
static void ags_generic_preferences_finalize(GObject *gobject);
void ags_generic_preferences_show(GtkWidget *widget);

void ags_generic_preferences_reset(AgsGenericPreferences *generic_preferences);
void* ags_generic_preferences_refresh(void *ptr);

/**
 * SECTION:ags_generic_preferences
 * @short_description: A composite widget to do generic related preferences
 * @title: AgsGenericPreferences
 * @section_id: 
 * @include: ags/X/ags_generic_preferences.h
 *
 * #AgsGenericPreferences enables you to make generic related preferences.
 */

static gpointer ags_generic_preferences_parent_class = NULL;

GType
ags_generic_preferences_get_type(void)
{
  static GType ags_type_generic_preferences = 0;

  if(!ags_type_generic_preferences){
    static const GTypeInfo ags_generic_preferences_info = {
      sizeof (AgsGenericPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_generic_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_generic_preferences = g_type_register_static(GTK_TYPE_VBOX,
							  "AgsGenericPreferences\0", &ags_generic_preferences_info,
							  0);
    
    g_type_add_interface_static(ags_type_generic_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_generic_preferences);
}

void
ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_generic_preferences_parent_class = g_type_class_peek_parent(generic_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) generic_preferences;

  gobject->finalize = ags_generic_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) generic_preferences;

  widget->show = ags_generic_preferences_show;
}

void
ags_generic_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_generic_preferences_connect;
  connectable->disconnect = ags_generic_preferences_disconnect;
}

void
ags_generic_preferences_init(AgsGenericPreferences *generic_preferences)
{
  generic_preferences->autosave_thread = (GtkCheckButton *) gtk_check_button_new_with_label("autosave thread\0");
  gtk_box_pack_start(GTK_BOX(generic_preferences),
		     GTK_WIDGET(generic_preferences->autosave_thread),
		     FALSE, FALSE,
		     0);
}

void
ags_generic_preferences_connect(AgsConnectable *connectable)
{
  AgsGenericPreferences *generic_preferences;

  generic_preferences = AGS_GENERIC_PREFERENCES(connectable);
}

void
ags_generic_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_generic_preferences_finalize(GObject *gobject)
{
}

void
ags_generic_preferences_show(GtkWidget *widget)
{
  AgsGenericPreferences *generic_preferences;
  pthread_t thread;

  generic_preferences = AGS_GENERIC_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_generic_preferences_parent_class)->show(widget);
}

/**
 * ags_generic_preferences_new:
 *
 * Creates an #AgsGenericPreferences
 *
 * Returns: a new #AgsGenericPreferences
 *
 * Since: 0.4
 */
AgsGenericPreferences*
ags_generic_preferences_new()
{
  AgsGenericPreferences *generic_preferences;

  generic_preferences = (AgsGenericPreferences *) g_object_new(AGS_TYPE_GENERIC_PREFERENCES,
							       NULL);
  
  return(generic_preferences);
}
