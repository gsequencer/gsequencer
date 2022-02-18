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

#ifndef __AGS_WINDOW_H__
#define __AGS_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_context_menu.h>
#include <ags/app/ags_menu_bar.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_notation_editor.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_automation_window.h>
#include <ags/app/ags_wave_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_composite_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_WINDOW                (ags_window_get_type())
#define AGS_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WINDOW, AgsWindow))
#define AGS_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WINDOW, AgsWindowClass))
#define AGS_IS_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WINDOW))
#define AGS_IS_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WINDOW))
#define AGS_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WINDOW, AgsWindowClass))

#define AGS_MACHINE_COUNTER(ptr) ((AgsMachineCounter *)(ptr))

typedef struct _AgsWindow AgsWindow;
typedef struct _AgsWindowClass AgsWindowClass;
typedef struct _AgsMachineCounter AgsMachineCounter;

typedef enum{
  AGS_WINDOW_CONNECTED    = 1,
  AGS_WINDOW_READY        = 1 << 1,
  AGS_WINDOW_LOADING      = 1 << 2,
  AGS_WINDOW_SAVING       = 1 << 3,
  AGS_WINDOW_TERMINATING  = 1 << 4,
}AgsWindowFlags;

struct _AgsWindow
{
  GtkApplicationWindow application_window;

  guint flags;

  gboolean no_config;
  
  gchar *filename;
  
  char *name;

  GtkHeaderBar *header_bar;
  GtkMenuButton *app_button;
  GtkMenuButton *add_button;
  
  AgsMenuBar *menu_bar;
  AgsContextMenu *context_menu;
  
  GtkPaned *paned;

  GtkBox *machines;
  GList *machine_counter;
  AgsMachine *selected;

  AgsNotationEditor *notation_editor;
  AgsNavigation *navigation;

  GList *dialog;
  
  AgsAutomationWindow *automation_window;
  AgsWaveWindow *wave_window;

  AgsExportWindow *export_window;

  GtkWidget *midi_import_wizard;
  GtkWidget *midi_export_wizard;
  GtkWidget *midi_file_chooser;
  
  AgsPreferences *preferences;

  AgsCompositeEditor *composite_editor;
};

struct _AgsWindowClass
{
  GtkApplicationWindowClass application_window;
};

struct _AgsMachineCounter
{
  gchar *version;
  gchar *build_id;

  GType machine_type;
  gchar *filename;
  gchar *effect;
  
  guint counter;
};

GType ags_window_get_type(void);

GList* ags_window_standard_machine_counter_alloc();
AgsMachineCounter* ags_window_find_machine_counter(AgsWindow *window,
						   GType machine_type);

void ags_window_increment_machine_counter(AgsWindow *window,
					  GType machine_type);
void ags_window_decrement_machine_counter(AgsWindow *window,
					  GType machine_type);

AgsMachineCounter* ags_machine_counter_alloc(gchar *version, gchar *build_id,
					     GType machine_type, guint initial_value);

void ags_window_show_error(AgsWindow *window,
			   gchar *message);

gboolean ags_window_load_file_timeout(AgsWindow *window);

AgsWindow* ags_window_new();

G_END_DECLS

#endif /*__AGS_WINDOW_H__*/
