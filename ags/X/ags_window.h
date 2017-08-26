/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_context_menu.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_export_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_history_browser.h>

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
  GtkWindow window;

  guint flags;

  char *name;

  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  GObject *soundcard;

  AgsMenuBar *menu_bar;
  AgsContextMenu *context_menu;
  
  GtkVPaned *paned;

  GtkVBox *machines;
  GList *machine_counter;
  AgsMachine *selected;

  AgsEditor *editor;
  AgsNavigation *navigation;

  GList *dialog;
  
  AgsAutomationWindow *automation_window;

  AgsExportWindow *export_window;

  GtkWidget *midi_import_wizard;
  GtkWidget *midi_export_wizard;
  GtkWidget *midi_file_chooser;
  
  AgsPreferences *preferences;
  AgsHistoryBrowser *history_browser;
};

struct _AgsWindowClass
{
  GtkWindowClass window;
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

AgsWindow* ags_window_new(GObject *application_context);

#endif /*__AGS_WINDOW_H__*/
