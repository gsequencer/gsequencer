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

#include <ags/X/ags_preferences.h>
#include <ags/X/ags_preferences_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_applicable.h>

void ags_preferences_class_init(AgsPreferencesClass *preferences);
void ags_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_preferences_init(AgsPreferences *preferences);
void ags_preferences_connect(AgsConnectable *connectable);
void ags_preferences_disconnect(AgsConnectable *connectable);
void ags_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_preferences_apply(AgsApplicable *applicable);
void ags_preferences_reset(AgsApplicable *applicable);
static void ags_preferences_finalize(GObject *gobject);
void ags_preferences_show(GtkWidget *widget);

static gpointer ags_preferences_parent_class = NULL;

GType
ags_preferences_get_type(void)
{
  static GType ags_type_preferences = 0;

  if(!ags_type_preferences){
    static const GTypeInfo ags_preferences_info = {
      sizeof (AgsPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_preferences = g_type_register_static(GTK_TYPE_DIALOG,
						  "AgsPreferences\0", &ags_preferences_info,
						  0);
    
    g_type_add_interface_static(ags_type_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_preferences);
}

void
ags_preferences_class_init(AgsPreferencesClass *preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_preferences_parent_class = g_type_class_peek_parent(preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) preferences;

  //  gobject->finalize = ags_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) preferences;
}

void
ags_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_preferences_connect;
  connectable->disconnect = ags_preferences_disconnect;
}

void
ags_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_preferences_set_update;
  applicable->apply = ags_preferences_apply;
  applicable->reset = ags_preferences_reset;
}

void
ags_preferences_init(AgsPreferences *preferences)
{
  GtkNotebook *notebook;
  GtkButton *button;

  preferences->flags = 0;

  preferences->window = NULL;

  gtk_window_set_title(GTK_WINDOW(preferences),
		       g_strdup("preferences\0"));
  gtk_window_set_deletable(GTK_WINDOW(preferences),
			   TRUE);

  notebook = (GtkNotebook *) gtk_notebook_new();
  g_object_set(G_OBJECT(notebook),
	       "tab-pos\0", GTK_POS_LEFT,
	       NULL);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(preferences)->vbox),
		    GTK_WIDGET(notebook));

  preferences->audio_preferences = ags_audio_preferences_new();
  gtk_notebook_append_page(notebook,
			   GTK_WIDGET(preferences->audio_preferences),
			   gtk_label_new("audio\0"));

  preferences->performance_preferences = ags_performance_preferences_new();
  gtk_notebook_append_page(notebook,
			   GTK_WIDGET(preferences->performance_preferences),
			   gtk_label_new("performance\0"));

  preferences->server_preferences = ags_server_preferences_new();
  gtk_notebook_append_page(notebook,
			   GTK_WIDGET(preferences->server_preferences),
			   gtk_label_new("server\0"));

  gtk_dialog_add_action_widget(GTK_DIALOG(preferences),
			       gtk_button_new_from_stock(GTK_STOCK_APPLY),
			       GTK_RESPONSE_APPLY);

  gtk_dialog_add_action_widget(GTK_DIALOG(preferences),
			       gtk_button_new_from_stock(GTK_STOCK_CANCEL),
			       GTK_RESPONSE_CANCEL);

  gtk_dialog_add_action_widget(GTK_DIALOG(preferences),
			       gtk_button_new_from_stock(GTK_STOCK_OK),
			       GTK_RESPONSE_OK);
}

void
ags_preferences_connect(AgsConnectable *connectable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(connectable);

  ags_connectable_connect(preferences->audio_preferences);
  ags_connectable_connect(preferences->performance_preferences);
  ags_connectable_connect(preferences->server_preferences);

  g_signal_connect_after(G_OBJECT(preferences), "response\0",
			 G_CALLBACK(ags_preferences_response_callback), NULL);
}

void
ags_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(preferences->audio_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->performance_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->server_preferences), update);
}

void
ags_preferences_apply(AgsApplicable *applicable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(applicable);

  ags_applicable_apply(AGS_APPLICABLE(preferences->audio_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->performance_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->server_preferences));
}

void
ags_preferences_reset(AgsApplicable *applicable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(applicable);

  ags_applicable_reset(AGS_APPLICABLE(preferences->audio_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->performance_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->server_preferences));
}

static void
ags_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_preferences_show(GtkWidget *widget)
{
}

AgsPreferences*
ags_preferences_new()
{
  AgsPreferences *preferences;

  preferences = (AgsPreferences *) g_object_new(AGS_TYPE_PREFERENCES,
						NULL);
  
  return(preferences);
}
