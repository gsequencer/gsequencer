/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_UI_PROVIDER_H__
#define __AGS_UI_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_UI_PROVIDER                    (ags_ui_provider_get_type())
#define AGS_UI_PROVIDER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_UI_PROVIDER, AgsUiProvider))
#define AGS_UI_PROVIDER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_UI_PROVIDER, AgsUiProviderInterface))
#define AGS_IS_UI_PROVIDER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_UI_PROVIDER))
#define AGS_IS_UI_PROVIDER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_UI_PROVIDER))
#define AGS_UI_PROVIDER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_UI_PROVIDER, AgsUiProviderInterface))

#define AGS_UI_PROVIDER_DEFAULT_TIMEOUT (1.0 / 25.0)
#define AGS_UI_PROVIDER_UPDATE_UI_TIMEOUT (1.0 / 4.0)
#define AGS_UI_PROVIDER_DEFAULT_PADDING (6)
#define AGS_UI_PROVIDER_DEFAULT_SPACING (6)
#define AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING (6)
#define AGS_UI_PROVIDER_DEFAULT_ROW_SPACING (6)
#define AGS_UI_PROVIDER_DEFAULT_MARGIN_END (6)

#define AGS_UI_PROVIDER_DEFAULT_WINDOW_WIDTH (1024)
#define AGS_UI_PROVIDER_DEFAULT_WINDOW_HEIGHT (768)

#define AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH (1024)
#define AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT (768)

#define AGS_UI_PROVIDER_DEFAULT_QUIT_DIALOG_WIDTH (800)
#define AGS_UI_PROVIDER_DEFAULT_QUIT_DIALOG_HEIGHT (400)

typedef struct _AgsUiProvider AgsUiProvider;
typedef struct _AgsUiProviderInterface AgsUiProviderInterface;

struct _AgsUiProviderInterface
{
  GTypeInterface ginterface;
    
  gboolean (*get_show_animation)(AgsUiProvider *ui_provider);
  void (*set_show_animation)(AgsUiProvider *ui_provider,
			     gboolean show_animation);

  gboolean (*get_gui_ready)(AgsUiProvider *ui_provider);
  void (*set_gui_ready)(AgsUiProvider *ui_provider,
			gboolean gui_ready);  

  gboolean (*get_file_ready)(AgsUiProvider *ui_provider);
  void (*set_file_ready)(AgsUiProvider *ui_provider,
			gboolean file_ready);

  gdouble (*get_gui_scale_factor)(AgsUiProvider *ui_provider);
  void (*set_gui_scale_factor)(AgsUiProvider *ui_provider,
			       gdouble gui_scale_factor);

  void (*schedule_task)(AgsUiProvider *ui_provider,
			AgsTask *task);
  void (*schedule_task_all)(AgsUiProvider *ui_provider,
			    GList *task);

  void (*setup_completed)(AgsUiProvider *ui_provider);

  void (*check_message)(AgsUiProvider *ui_provider);
  void (*clean_message)(AgsUiProvider *ui_provider);

  void (*update_ui)(AgsUiProvider *ui_provider);
   
  GtkWidget* (*get_animation_window)(AgsUiProvider *ui_provider);
  void (*set_animation_window)(AgsUiProvider *ui_provider,
			       GtkWidget *animation_window);

  GtkWidget* (*get_window)(AgsUiProvider *ui_provider);
  void (*set_window)(AgsUiProvider *ui_provider,
		     GtkWidget *window);
  
  GtkWidget* (*get_export_window)(AgsUiProvider *ui_provider);
  void (*set_export_window)(AgsUiProvider *ui_provider,
			    GtkWidget *export_window);
  
  GtkWidget* (*get_online_help_window)(AgsUiProvider *ui_provider);
  void (*set_online_help_window)(AgsUiProvider *ui_provider,
				 GtkWidget *online_help_window);
  
  GtkWidget* (*get_preferences)(AgsUiProvider *ui_provider);
  void (*set_preferences)(AgsUiProvider *ui_provider,
			  GtkWidget *preferences);
  
  GtkWidget* (*get_history_browser)(AgsUiProvider *ui_provider);
  void (*set_history_browser)(AgsUiProvider *ui_provider,
			      GtkWidget *history_browser);

  GtkWidget* (*get_midi_browser)(AgsUiProvider *ui_provider);
  void (*set_midi_browser)(AgsUiProvider *ui_provider,
			   GtkWidget *midi_browser);

  GtkWidget* (*get_sample_browser)(AgsUiProvider *ui_provider);
  void (*set_sample_browser)(AgsUiProvider *ui_provider,
			     GtkWidget *sample_browser);
  
  GtkWidget* (*get_midi_import_wizard)(AgsUiProvider *ui_provider);
  void (*set_midi_import_wizard)(AgsUiProvider *ui_provider,
				 GtkWidget *midi_import_wizard);

  GtkWidget* (*get_midi_export_wizard)(AgsUiProvider *ui_provider);
  void (*set_midi_export_wizard)(AgsUiProvider *ui_provider,
				 GtkWidget *midi_export_wizard);
  
  GList* (*get_machine)(AgsUiProvider *ui_provider);
  void (*set_machine)(AgsUiProvider *ui_provider,
		      GList *machine);

  GtkWidget* (*get_composite_editor)(AgsUiProvider *ui_provider);
  void (*set_composite_editor)(AgsUiProvider *ui_provider,
			       GtkWidget *composite_editor);  

  GtkWidget* (*get_navigation)(AgsUiProvider *ui_provider);
  void (*set_navigation)(AgsUiProvider *ui_provider,
			 GtkWidget *navigation);

  GtkApplication* (*get_app)(AgsUiProvider *ui_provider);
  void (*set_app)(AgsUiProvider *ui_provider,
		  GtkApplication *app);

  GtkWidget* (*get_meta_data_window)(AgsUiProvider *ui_provider);
  void (*set_meta_data_window)(AgsUiProvider *ui_provider,
			       GtkWidget *meta_data_window);

  GList* (*get_visible_window)(AgsUiProvider *ui_provider);
  void (*set_visible_window)(AgsUiProvider *ui_provider,
			     GList *visible_window);
};

GType ags_ui_provider_get_type();

gboolean ags_ui_provider_get_show_animation(AgsUiProvider *ui_provider);
void ags_ui_provider_set_show_animation(AgsUiProvider *ui_provider,
					gboolean show_animation);

gboolean ags_ui_provider_get_gui_ready(AgsUiProvider *ui_provider);
void ags_ui_provider_set_gui_ready(AgsUiProvider *ui_provider,
				   gboolean gui_ready);

gboolean ags_ui_provider_get_file_ready(AgsUiProvider *ui_provider);
void ags_ui_provider_set_file_ready(AgsUiProvider *ui_provider,
				    gboolean file_ready);

gdouble ags_ui_provider_get_gui_scale_factor(AgsUiProvider *ui_provider);
void ags_ui_provider_set_gui_scale_factor(AgsUiProvider *ui_provider,
					  gdouble gui_scale_factor);

void ags_ui_provider_schedule_task(AgsUiProvider *ui_provider,
				   AgsTask *task);
void ags_ui_provider_schedule_task_all(AgsUiProvider *ui_provider,
				       GList *task);

void ags_ui_provider_setup_completed(AgsUiProvider *ui_provider);

void ags_ui_provider_check_message(AgsUiProvider *ui_provider);
void ags_ui_provider_clean_message(AgsUiProvider *ui_provider);

void ags_ui_provider_update_ui(AgsUiProvider *ui_provider);

GtkWidget* ags_ui_provider_get_animation_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_animation_window(AgsUiProvider *ui_provider,
					  GtkWidget *animation_window);

GtkWidget* ags_ui_provider_get_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_window(AgsUiProvider *ui_provider,
				GtkWidget *window);
  
GtkWidget* ags_ui_provider_get_export_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_export_window(AgsUiProvider *ui_provider,
				       GtkWidget *export_window);
  
GtkWidget* ags_ui_provider_get_online_help_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_online_help_window(AgsUiProvider *ui_provider,
					    GtkWidget *online_help_window);

GtkWidget* ags_ui_provider_get_preferences(AgsUiProvider *ui_provider);
void ags_ui_provider_set_preferences(AgsUiProvider *ui_provider,
				     GtkWidget *preferences);
  
GtkWidget* ags_ui_provider_get_history_browser(AgsUiProvider *ui_provider);
void ags_ui_provider_set_history_browser(AgsUiProvider *ui_provider,
					 GtkWidget *history_browser);

GtkWidget* ags_ui_provider_get_midi_browser(AgsUiProvider *ui_provider);
void ags_ui_provider_set_midi_browser(AgsUiProvider *ui_provider,
				      GtkWidget *midi_browser);

GtkWidget* ags_ui_provider_get_sample_browser(AgsUiProvider *ui_provider);
void ags_ui_provider_set_sample_browser(AgsUiProvider *ui_provider,
					GtkWidget *sample_browser);
  
GtkWidget* ags_ui_provider_get_midi_import_wizard(AgsUiProvider *ui_provider);
void ags_ui_provider_set_midi_import_wizard(AgsUiProvider *ui_provider,
					    GtkWidget *midi_import_wizard);

GtkWidget* ags_ui_provider_get_midi_export_wizard(AgsUiProvider *ui_provider);
void ags_ui_provider_set_midi_export_wizard(AgsUiProvider *ui_provider,
					    GtkWidget *midi_export_wizard);

GList* ags_ui_provider_get_machine(AgsUiProvider *ui_provider);
void ags_ui_provider_set_machine(AgsUiProvider *ui_provider,
				 GList *machine);

GtkWidget* ags_ui_provider_get_composite_editor(AgsUiProvider *ui_provider);
void ags_ui_provider_set_composite_editor(AgsUiProvider *ui_provider,
					  GtkWidget *composite_editor);  

GtkWidget* ags_ui_provider_get_navigation(AgsUiProvider *ui_provider);
void ags_ui_provider_set_navigation(AgsUiProvider *ui_provider,
				    GtkWidget *navigation);

GtkApplication* ags_ui_provider_get_app(AgsUiProvider *ui_provider);
void ags_ui_provider_set_app(AgsUiProvider *ui_provider,
			     GtkApplication *app);

GtkWidget* ags_ui_provider_get_meta_data_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_meta_data_window(AgsUiProvider *ui_provider,
					  GtkWidget *meta_data_window);

GList* ags_ui_provider_get_visible_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_visible_window(AgsUiProvider *ui_provider,
					GList *visible_window);

G_END_DECLS

#endif /*__AGS_UI_PROVIDER_H__*/
