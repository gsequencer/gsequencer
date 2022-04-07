/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_preferences.h>
#include <ags/app/ags_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/file/ags_simple_file.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ags/i18n.h>

void ags_preferences_class_init(AgsPreferencesClass *preferences);
void ags_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_preferences_init(AgsPreferences *preferences);
void ags_preferences_dispose(GObject *gobject);
void ags_preferences_finalize(GObject *gobject);

void ags_preferences_connect(AgsConnectable *connectable);
void ags_preferences_disconnect(AgsConnectable *connectable);

void ags_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_preferences_apply(AgsApplicable *applicable);
void ags_preferences_reset(AgsApplicable *applicable);

void ags_preferences_show(GtkWidget *widget);

/**
 * SECTION:ags_preferences
 * @short_description: A dialog to do preferences
 * @title: AgsPluginPreferences
 * @section_id: 
 * @include: ags/app/ags_plugin_preferences.h
 *
 * #AgsDialogPreferences enables you to make preferences.
 */

static gpointer ags_preferences_parent_class = NULL;

GType
ags_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_preferences = 0;

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
						  "AgsPreferences", &ags_preferences_info,
						  0);
    
    g_type_add_interface_static(ags_type_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_preferences_class_init(AgsPreferencesClass *preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_preferences_parent_class = g_type_class_peek_parent(preferences);

  /* GtkGobjectClass */
  gobject = (GObjectClass *) preferences;

  gobject->dispose = ags_preferences_dispose;
  gobject->finalize = ags_preferences_finalize;
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) preferences;

  widget->show = ags_preferences_show;
}

void
ags_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
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
  gchar *str;

  preferences->flags = 0;
  preferences->connectable_flags = 0;

  gtk_window_set_title(GTK_WINDOW(preferences),
		       i18n("Preferences"));

  gtk_window_set_deletable(GTK_WINDOW(preferences),
			   TRUE);

  preferences->notebook = (GtkNotebook *) gtk_notebook_new();
  g_object_set(G_OBJECT(preferences->notebook),
	       "tab-pos", GTK_POS_LEFT,
	       NULL);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area(GTK_DIALOG(preferences)),
		 (GtkWidget *) preferences->notebook);

  preferences->generic_preferences = ags_generic_preferences_new();
  gtk_widget_set_hexpand((GtkWidget *) preferences->generic_preferences,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) preferences->generic_preferences,
			 TRUE);
  gtk_notebook_append_page(preferences->notebook,
			   GTK_WIDGET(preferences->generic_preferences),
			   gtk_label_new(i18n("generic")));

  preferences->audio_preferences = ags_audio_preferences_new();
  gtk_widget_set_hexpand((GtkWidget *) preferences->audio_preferences,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) preferences->audio_preferences,
			 TRUE);
  gtk_notebook_append_page(preferences->notebook,
			   GTK_WIDGET(preferences->audio_preferences),
			   gtk_label_new(i18n("audio")));

  preferences->midi_preferences = ags_midi_preferences_new();
  gtk_widget_set_hexpand((GtkWidget *) preferences->midi_preferences,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) preferences->midi_preferences,
			 TRUE);
  gtk_notebook_append_page(preferences->notebook,
			   GTK_WIDGET(preferences->midi_preferences),
			   gtk_label_new(i18n("midi")));

  preferences->performance_preferences = ags_performance_preferences_new();
  gtk_widget_set_hexpand((GtkWidget *) preferences->performance_preferences,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) preferences->performance_preferences,
			 TRUE);
  gtk_notebook_append_page(preferences->notebook,
			   GTK_WIDGET(preferences->performance_preferences),
			   gtk_label_new(i18n("performance")));

  str = ags_config_get_value(ags_config_get_instance(),
			     AGS_CONFIG_GENERIC,
			     "disable-feature");
  
  preferences->osc_server_preferences = ags_osc_server_preferences_new();
  gtk_widget_set_hexpand((GtkWidget *) preferences->osc_server_preferences,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) preferences->osc_server_preferences,
			 TRUE);
  gtk_notebook_append_page(preferences->notebook,
			   GTK_WIDGET(preferences->osc_server_preferences),
			   gtk_label_new(i18n("OSC server")));

  if(str == NULL ||
     !g_ascii_strncasecmp(str,
			  "experimental",
			  13)){
    preferences->server_preferences = NULL;
  }else{
    preferences->server_preferences = ags_server_preferences_new();
    gtk_notebook_append_page(preferences->notebook,
			     GTK_WIDGET(preferences->server_preferences),
			     gtk_label_new(i18n("server")));
  }

  gtk_window_set_default_size(preferences,
			      800, 600);
  
  gtk_dialog_add_buttons((GtkDialog *) preferences,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"), GTK_RESPONSE_REJECT,
			 NULL);
}

void
ags_preferences_dispose(GObject *gobject)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  ags_ui_provider_set_preferences(AGS_UI_PROVIDER(application_context),
				  NULL);

  /* call parent */
  G_OBJECT_CLASS(ags_preferences_parent_class)->dispose(gobject);
}

void
ags_preferences_finalize(GObject *gobject)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  ags_ui_provider_set_preferences(AGS_UI_PROVIDER(application_context),
				  NULL);

  /* call parent */
  G_OBJECT_CLASS(ags_preferences_parent_class)->finalize(gobject);
}

void
ags_preferences_connect(AgsConnectable *connectable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preferences->connectable_flags)) != 0){
    return;
  }

  preferences->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  ags_connectable_connect(AGS_CONNECTABLE(preferences->generic_preferences));
  ags_connectable_connect(AGS_CONNECTABLE(preferences->audio_preferences));
  ags_connectable_connect(AGS_CONNECTABLE(preferences->midi_preferences));
  ags_connectable_connect(AGS_CONNECTABLE(preferences->performance_preferences));
  ags_connectable_connect(AGS_CONNECTABLE(preferences->osc_server_preferences));

  if(preferences->server_preferences != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(preferences->server_preferences));
  }
  
  g_signal_connect_after(G_OBJECT(preferences), "response",
			 G_CALLBACK(ags_preferences_response_callback), NULL);

  g_signal_connect_after(G_OBJECT(preferences->notebook), "switch-page",
			 G_CALLBACK(ags_preferences_notebook_switch_page_callback), preferences);
}

void
ags_preferences_disconnect(AgsConnectable *connectable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preferences->connectable_flags)) == 0){
    return;
  }

  preferences->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(preferences->generic_preferences));
  ags_connectable_disconnect(AGS_CONNECTABLE(preferences->audio_preferences));
  ags_connectable_disconnect(AGS_CONNECTABLE(preferences->midi_preferences));
  ags_connectable_disconnect(AGS_CONNECTABLE(preferences->performance_preferences));
  ags_connectable_disconnect(AGS_CONNECTABLE(preferences->osc_server_preferences));

  if(preferences->server_preferences != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(preferences->server_preferences));
  }

  g_object_disconnect(G_OBJECT(preferences),
		      "any_signal::response",
		      G_CALLBACK(ags_preferences_response_callback),
		      NULL,
		      NULL);

  g_object_disconnect(G_OBJECT(preferences->notebook),
		      "any_signal::switch-page",
		      G_CALLBACK(ags_preferences_notebook_switch_page_callback),
		      preferences,
		      NULL);
}

void
ags_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(preferences->generic_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->audio_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->midi_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->performance_preferences), update);
  ags_applicable_set_update(AGS_APPLICABLE(preferences->osc_server_preferences), update);

  if(preferences->server_preferences != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(preferences->server_preferences), update);
  }
}

void
ags_preferences_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  GtkDialog *dialog;

  AgsApplySoundConfig *apply_sound_config;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;

  preferences = AGS_PREFERENCES(applicable);
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  config = ags_config_get_instance();

  ags_config_clear(config);

  ags_applicable_apply(AGS_APPLICABLE(preferences->generic_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->audio_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->midi_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->performance_preferences));
  ags_applicable_apply(AGS_APPLICABLE(preferences->osc_server_preferences));

  if(preferences->server_preferences != NULL){
    ags_applicable_apply(AGS_APPLICABLE(preferences->server_preferences));
  }
  
  ags_config_save(config);

  apply_sound_config = ags_apply_sound_config_new(NULL);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) apply_sound_config);

  /* notify user about safe GSequencer */
  dialog = (GtkDialog *) gtk_message_dialog_new((GtkWindow *) window,
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_INFO,
						GTK_BUTTONS_OK,
						"After finished your modifications you should safe your file");

  gtk_window_set_deletable((GtkWindow *) dialog,
			   TRUE);
  
  gtk_widget_show((GtkWidget *) dialog);
}

void
ags_preferences_reset(AgsApplicable *applicable)
{
  AgsPreferences *preferences;

  preferences = AGS_PREFERENCES(applicable);

  ags_applicable_reset(AGS_APPLICABLE(preferences->generic_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->audio_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->midi_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->performance_preferences));
  ags_applicable_reset(AGS_APPLICABLE(preferences->osc_server_preferences));

  if(preferences->server_preferences != NULL){
    ags_applicable_reset(AGS_APPLICABLE(preferences->server_preferences));
  }
}

void
ags_preferences_show(GtkWidget *widget)
{
  AgsPreferences *preferences;

  preferences = (AgsPreferences *) widget;
  
  GTK_WIDGET_CLASS(ags_preferences_parent_class)->show(widget);

  gtk_widget_hide((GtkWidget *) preferences->audio_preferences->add);

  gtk_widget_hide((GtkWidget *) preferences->midi_preferences->add);
}

/**
 * ags_preferences_new:
 *
 * Create a new instance of #AgsPreferences
 *
 * Returns: the new #AgsPreferences
 *
 * Since: 3.0.0
 */
AgsPreferences*
ags_preferences_new()
{
  AgsPreferences *preferences;

  preferences = (AgsPreferences *) g_object_new(AGS_TYPE_PREFERENCES,
						NULL);
  
  return(preferences);
}
