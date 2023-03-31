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

#include <ags/app/ags_ui_provider.h>

void ags_ui_provider_class_init(AgsUiProviderInterface *ginterface);

/**
 * SECTION:ags_ui_provider
 * @short_description: provide user interface
 * @title: AgsUiProvider
 * @section_id: AgsUiProvider
 * @include: ags/app/ags_ui_provider.h
 *
 * The #AgsUiProvider provides you the window capabilities.
 */

enum {
  SETUP_COMPLETED,
  CHECK_MESSAGE,
  CLEAN_MESSAGE,
  UPDATE_UI,
  LAST_SIGNAL,
};

static guint ui_provider_signals[LAST_SIGNAL];

GType
ags_ui_provider_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ui_provider = 0;
    
    ags_type_ui_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
							 "AgsUiProvider",
							 sizeof(AgsUiProviderInterface),
							 (GClassInitFunc) ags_ui_provider_class_init,
							 0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ui_provider);
  }

  return g_define_type_id__volatile;
}

void
ags_ui_provider_class_init(AgsUiProviderInterface *ginterface)
{
  /**
   * AgsUiProvider::setup-completed:
   * @ui_provider: the #AgsUiProvider object
   *
   * The ::setup-completed signal is emitted every check message of the UI provider. This notifies
   * about to check for messages from message delivery.
   *
   * Since: 3.0.8
   */
  ui_provider_signals[SETUP_COMPLETED] =
    g_signal_new("setup-completed",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsUiProviderInterface, setup_completed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsUiProvider::check-message:
   * @ui_provider: the #AgsUiProvider object
   *
   * The ::check-message signal is emitted every check message of the UI provider. This notifies
   * about to check for messages from message delivery.
   *
   * Since: 3.0.0
   */
  ui_provider_signals[CHECK_MESSAGE] =
    g_signal_new("check-message",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsUiProviderInterface, check_message),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsUiProvider::clean-message:
   * @ui_provider: the #AgsUiProvider object
   *
   * The ::clean-message signal is emitted every clean message of the UI provider. This notifies
   * about to clean messages from message delivery.
   *
   * Since: 3.0.0
   */
  ui_provider_signals[CLEAN_MESSAGE] =
    g_signal_new("clean-message",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsUiProviderInterface, clean_message),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsUiProvider::update-ui:
   * @ui_provider: the #AgsUiProvider object
   *
   * The ::update-ui signal is emitted every %AGS_UI_PROVIDER_DEFAULT_TIMEOUT
   * interval.
   *
   * Since: 4.2.1
   */
  ui_provider_signals[UPDATE_UI] =
    g_signal_new("update-ui",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsUiProviderInterface, update_ui),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

/**
 * ags_ui_provider_get_show_animation:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get show animation.
 * 
 * Returns: %TRUE if animation does show, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_ui_provider_get_show_animation(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), FALSE);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_show_animation, FALSE);

  return(ui_provider_interface->get_show_animation(ui_provider));
}

/**
 * ags_ui_provider_set_show_animation:
 * @ui_provider: the #AgsUiProvider
 * @show_animation: do show animation
 * 
 * Set show animation.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_show_animation(AgsUiProvider *ui_provider,
				   gboolean show_animation)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_show_animation);

  ui_provider_interface->set_show_animation(ui_provider,
					    show_animation);
}

/**
 * ags_ui_provider_get_gui_ready:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get GUI ready.
 * 
 * Returns: %TRUE if gui is ready, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_ui_provider_get_gui_ready(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), FALSE);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_gui_ready, FALSE);

  return(ui_provider_interface->get_gui_ready(ui_provider));
}

/**
 * ags_ui_provider_set_gui_ready:
 * @ui_provider: the #AgsUiProvider
 * @gui_ready: is GUI ready
 * 
 * Set GUI ready.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_gui_ready(AgsUiProvider *ui_provider,
			      gboolean gui_ready)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_gui_ready);

  ui_provider_interface->set_gui_ready(ui_provider,
				       gui_ready);
}

/**
 * ags_ui_provider_get_file_ready:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get file ready.
 * 
 * Returns: %TRUE if gui is ready, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_ui_provider_get_file_ready(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), FALSE);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_file_ready, FALSE);

  return(ui_provider_interface->get_file_ready(ui_provider));
}

/**
 * ags_ui_provider_set_file_ready:
 * @ui_provider: the #AgsUiProvider
 * @file_ready: is GUI ready
 * 
 * Set file ready.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_file_ready(AgsUiProvider *ui_provider,
			       gboolean file_ready)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_file_ready);

  ui_provider_interface->set_file_ready(ui_provider,
					file_ready);
}

/**
 * ags_ui_provider_get_gui_scale_factor:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get GUI scale factor.
 * 
 * Returns: the GUI scale factor as gdouble
 * 
 * Since: 3.0.0
 */
gdouble
ags_ui_provider_get_gui_scale_factor(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), FALSE);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_gui_scale_factor, FALSE);

  return(ui_provider_interface->get_gui_scale_factor(ui_provider));
}

/**
 * ags_ui_provider_set_gui_scale_factor:
 * @ui_provider: the #AgsUiProvider
 * @gui_scale_factor: GUI scale factor
 * 
 * Set the GUI scale factor as gdouble.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_gui_scale_factor(AgsUiProvider *ui_provider,
				     gdouble gui_scale_factor)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_gui_scale_factor);

  ui_provider_interface->set_gui_scale_factor(ui_provider,
					      gui_scale_factor);
}

/**
 * ags_ui_provider_schedule_task:
 * @ui_provider: the #AgsUiProvider
 * @task: the #AgsTask
 * 
 * Schedule @task to be launched.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_schedule_task(AgsUiProvider *ui_provider,
			      AgsTask *task)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->schedule_task);

  ui_provider_interface->schedule_task(ui_provider,
				       task);
}

/**
 * ags_ui_provider_schedule_task_all:
 * @ui_provider: the #AgsUiProvider
 * @task: the #GList-struct containig #AgsTask
 * 
 * Schedule all @task to be launched.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_schedule_task_all(AgsUiProvider *ui_provider,
				  GList *task)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->schedule_task_all);

  ui_provider_interface->schedule_task_all(ui_provider,
					   task);
}

/**
 * ags_ui_provider_setup_completed:
 * @ui_provider: the #AgsUiProvider
 *
 * Application context setup completed.
 *
 * Since: 3.0.8
 */
void
ags_ui_provider_setup_completed(AgsUiProvider *ui_provider)
{
  g_signal_emit(ui_provider, ui_provider_signals[SETUP_COMPLETED], 0);
}

/**
 * ags_ui_provider_check_message:
 * @ui_provider: the #AgsUiProvider
 *
 * Check messages the UI provider needs to consume.
 *
 * Since: 3.0.0
 */
void
ags_ui_provider_check_message(AgsUiProvider *ui_provider)
{
  g_signal_emit(ui_provider, ui_provider_signals[CHECK_MESSAGE], 0);
}

/**
 * ags_ui_provider_clean_message:
 * @ui_provider: the #AgsUiProvider
 *
 * Clean messages the UI provider needs to consume.
 *
 * Since: 3.0.0
 */
void
ags_ui_provider_clean_message(AgsUiProvider *ui_provider)
{
  g_signal_emit(ui_provider, ui_provider_signals[CLEAN_MESSAGE], 0);
}

/**
 * ags_ui_provider_update_ui:
 * @ui_provider: the #AgsUiProvider
 *
 * Update UI.
 *
 * Since: 4.2.1
 */
void
ags_ui_provider_update_ui(AgsUiProvider *ui_provider)
{
  g_signal_emit(ui_provider, ui_provider_signals[UPDATE_UI], 0);
}

/**
 * ags_ui_provider_get_animation_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_animation_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_animation_window, NULL);

  return(ui_provider_interface->get_animation_window(ui_provider));
}

/**
 * ags_ui_provider_set_animation_window:
 * @ui_provider: the #AgsUiProvider
 * @animation_window: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_animation_window(AgsUiProvider *ui_provider,
				     GtkWidget *animation_window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_animation_window);

  ui_provider_interface->set_animation_window(ui_provider,
					      animation_window);
}

/**
 * ags_ui_provider_get_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get window.
 * 
 * Returns: the #AgsWindow
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_window, NULL);

  return(ui_provider_interface->get_window(ui_provider));
}

/**
 * ags_ui_provider_set_window:
 * @ui_provider: the #AgsUiProvider
 * @window: the #AgsWindow
 * 
 * Set window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_window(AgsUiProvider *ui_provider,
			   GtkWidget *window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_window);

  ui_provider_interface->set_window(ui_provider,
				    window);
}

/**
 * ags_ui_provider_get_export_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get export window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_export_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_export_window, NULL);

  return(ui_provider_interface->get_export_window(ui_provider));
}

/**
 * ags_ui_provider_set_export_window:
 * @ui_provider: the #AgsUiProvider
 * @export_window: the #GtkWidget
 * 
 * Set export window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_export_window(AgsUiProvider *ui_provider,
				  GtkWidget *export_window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_export_window);

  ui_provider_interface->set_export_window(ui_provider,
					   export_window);
}

/**
 * ags_ui_provider_get_online_help_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get online help window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.5.0
 */
GtkWidget*
ags_ui_provider_get_online_help_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_online_help_window, NULL);

  return(ui_provider_interface->get_online_help_window(ui_provider));
}

/**
 * ags_ui_provider_set_online_help_window:
 * @ui_provider: the #AgsUiProvider
 * @online_help_window: the #GtkWidget
 * 
 * Set online help window.
 * 
 * Since: 3.5.0
 */
void
ags_ui_provider_set_online_help_window(AgsUiProvider *ui_provider,
				       GtkWidget *online_help_window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_online_help_window);

  ui_provider_interface->set_online_help_window(ui_provider,
						online_help_window);
}

/**
 * ags_ui_provider_get_preferences:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_preferences(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_preferences, NULL);

  return(ui_provider_interface->get_preferences(ui_provider));
}

/**
 * ags_ui_provider_set_preferences:
 * @ui_provider: the #AgsUiProvider
 * @preferences: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_preferences(AgsUiProvider *ui_provider,
				GtkWidget *preferences)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_preferences);

  ui_provider_interface->set_preferences(ui_provider,
					 preferences);
}

/**
 * ags_ui_provider_get_history_browser:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_history_browser(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_history_browser, NULL);

  return(ui_provider_interface->get_history_browser(ui_provider));
}

/**
 * ags_ui_provider_set_history_browser:
 * @ui_provider: the #AgsUiProvider
 * @history_browser: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_history_browser(AgsUiProvider *ui_provider,
				    GtkWidget *history_browser)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_history_browser);

  ui_provider_interface->set_history_browser(ui_provider,
					     history_browser);
}

/**
 * ags_ui_provider_get_midi_browser:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_midi_browser(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_midi_browser, NULL);

  return(ui_provider_interface->get_midi_browser(ui_provider));
}

/**
 * ags_ui_provider_set_midi_browser:
 * @ui_provider: the #AgsUiProvider
 * @midi_browser: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_midi_browser(AgsUiProvider *ui_provider,
				 GtkWidget *midi_browser)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_midi_browser);

  ui_provider_interface->set_midi_browser(ui_provider,
					  midi_browser);
}

/**
 * ags_ui_provider_et_sample_browser:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_sample_browser(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_sample_browser, NULL);

  return(ui_provider_interface->get_sample_browser(ui_provider));
}

/**
 * ags_ui_provider_set_sample_browser:
 * @ui_provider: the #AgsUiProvider
 * @sample_browser: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_sample_browser(AgsUiProvider *ui_provider,
				   GtkWidget *sample_browser)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_sample_browser);

  ui_provider_interface->set_sample_browser(ui_provider,
					    sample_browser);
}

/**
 * ags_ui_provider_get_midi_import_wizard:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_midi_import_wizard(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_midi_import_wizard, NULL);

  return(ui_provider_interface->get_midi_import_wizard(ui_provider));
}

/**
 * ags_ui_provider_set_midi_import_wizard:
 * @ui_provider: the #AgsUiProvider
 * @midi_import_wizard: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_midi_import_wizard(AgsUiProvider *ui_provider,
				       GtkWidget *midi_import_wizard)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_midi_import_wizard);

  ui_provider_interface->set_midi_import_wizard(ui_provider,
						midi_import_wizard);
}

/**
 * ags_ui_provider_get_midi_export_wizard:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_midi_export_wizard(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_midi_export_wizard, NULL);

  return(ui_provider_interface->get_midi_export_wizard(ui_provider));
}

/**
 * ags_ui_provider_set_midi_export_wizard:
 * @ui_provider: the #AgsUiProvider
 * @midi_export_wizard: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_midi_export_wizard(AgsUiProvider *ui_provider,
				       GtkWidget *midi_export_wizard)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_midi_export_wizard);

  ui_provider_interface->set_midi_export_wizard(ui_provider,
						midi_export_wizard);
}

/**
 * ags_ui_provider_get_machine:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get machine.
 * 
 * Returns: the #GList-struct containing #AgsMachine
 * 
 * Since: 3.0.0
 */
GList*
ags_ui_provider_get_machine(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_machine, NULL);

  return(ui_provider_interface->get_machine(ui_provider));
}

/**
 * ags_ui_provider_set_machine:
 * @ui_provider: the #AgsUiProvider
 * @machine: the #GList-struct containing #AgsMachine
 * 
 * Set machine.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_machine(AgsUiProvider *ui_provider,
			    GList *machine)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_machine);

  ui_provider_interface->set_machine(ui_provider,
				     machine);
}

/**
 * ags_ui_provider_get_composite_editor:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_composite_editor(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_composite_editor, NULL);

  return(ui_provider_interface->get_composite_editor(ui_provider));
}

/**
 * ags_ui_provider_set_composite_editor:
 * @ui_provider: the #AgsUiProvider
 * @composite_editor: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_composite_editor(AgsUiProvider *ui_provider,
				     GtkWidget *composite_editor)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_composite_editor);

  ui_provider_interface->set_composite_editor(ui_provider,
					      composite_editor);
}

/**
 * ags_ui_provider_get_navigation:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get animation window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.0.0
 */
GtkWidget*
ags_ui_provider_get_navigation(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_navigation, NULL);

  return(ui_provider_interface->get_navigation(ui_provider));
}

/**
 * ags_ui_provider_set_navigation:
 * @ui_provider: the #AgsUiProvider
 * @navigation: the #GtkWidget
 * 
 * Set animation window.
 * 
 * Since: 3.0.0
 */
void
ags_ui_provider_set_navigation(AgsUiProvider *ui_provider,
			       GtkWidget *navigation)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_navigation);

  ui_provider_interface->set_navigation(ui_provider,
					navigation);
}

/**
 * ags_ui_provider_get_app:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get application.
 * 
 * Returns: the #GtkApplication
 * 
 * Since: 3.18.0
 */
GtkApplication*
ags_ui_provider_get_app(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_app, NULL);

  return(ui_provider_interface->get_app(ui_provider));
}

/**
 * ags_ui_provider_set_app:
 * @ui_provider: the #AgsUiProvider
 * @app: the #GtkApplication
 * 
 * Set app.
 * 
 * Since: 3.18.0
 */
void
ags_ui_provider_set_app(AgsUiProvider *ui_provider,
			GtkApplication *app)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_app);

  ui_provider_interface->set_app(ui_provider,
				 app);
}

/**
 * ags_ui_provider_get_meta_data_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get meta-data window.
 * 
 * Returns: the #GtkWidget
 * 
 * Since: 3.18.0
 */
GtkWidget*
ags_ui_provider_get_meta_data_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_meta_data_window, NULL);

  return(ui_provider_interface->get_meta_data_window(ui_provider));
}

/**
 * ags_ui_provider_set_meta_data_window:
 * @ui_provider: the #AgsUiProvider
 * @meta_data_window: the #GtkWidget
 * 
 * Set meta-data window.
 * 
 * Since: 3.18.0
 */
void
ags_ui_provider_set_meta_data_window(AgsUiProvider *ui_provider,
				     GtkWidget *meta_data_window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_meta_data_window);

  ui_provider_interface->set_meta_data_window(ui_provider,
					      meta_data_window);
}
