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

#include <ags/X/ags_performance_preferences.h>
#include <ags/X/ags_performance_preferences_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences);
void ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_performance_preferences_init(AgsPerformancePreferences *performance_preferences);
void ags_performance_preferences_connect(AgsConnectable *connectable);
void ags_performance_preferences_disconnect(AgsConnectable *connectable);
static void ags_performance_preferences_finalize(GObject *gobject);
void ags_performance_preferences_show(GtkWidget *widget);

void ags_performance_preferences_reset(AgsPerformancePreferences *performance_preferences);
void* ags_performance_preferences_refresh(void *ptr);

static gpointer ags_performance_preferences_parent_class = NULL;

GType
ags_performance_preferences_get_type(void)
{
  static GType ags_type_performance_preferences = 0;

  if(!ags_type_performance_preferences){
    static const GTypeInfo ags_performance_preferences_info = {
      sizeof (AgsPerformancePreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_performance_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPerformancePreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_performance_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_performance_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_performance_preferences = g_type_register_static(GTK_TYPE_VBOX,
							      "AgsPerformancePreferences\0", &ags_performance_preferences_info,
							      0);
    
    g_type_add_interface_static(ags_type_performance_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_performance_preferences);
}

void
ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_performance_preferences_parent_class = g_type_class_peek_parent(performance_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) performance_preferences;

  gobject->finalize = ags_performance_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) performance_preferences;

  widget->show = ags_performance_preferences_show;
}

void
ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_performance_preferences_connect;
  connectable->disconnect = ags_performance_preferences_disconnect;
}

void
ags_performance_preferences_init(AgsPerformancePreferences *performance_preferences)
{
  performance_preferences->super_threaded = (GtkCheckButton *) gtk_check_button_new_with_label("Super Threaded\0");
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(performance_preferences->super_threaded),
		     FALSE, FALSE,
		     0);
}

void
ags_performance_preferences_connect(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;

  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);
}

void
ags_performance_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_performance_preferences_finalize(GObject *gobject)
{
}

void
ags_performance_preferences_show(GtkWidget *widget)
{
  AgsPerformancePreferences *performance_preferences;
  pthread_t thread;

  performance_preferences = AGS_PERFORMANCE_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_performance_preferences_parent_class)->show(widget);
}

AgsPerformancePreferences*
ags_performance_preferences_new()
{
  AgsPerformancePreferences *performance_preferences;

  performance_preferences = (AgsPerformancePreferences *) g_object_new(AGS_TYPE_PERFORMANCE_PREFERENCES,
								       NULL);
  
  return(performance_preferences);
}

