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

#include <ags/app/ags_machine.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_composite_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_WINDOW                (ags_window_get_type())
#define AGS_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WINDOW, AgsWindow))
#define AGS_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WINDOW, AgsWindowClass))
#define AGS_IS_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WINDOW))
#define AGS_IS_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WINDOW))
#define AGS_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WINDOW, AgsWindowClass))

typedef struct _AgsWindow AgsWindow;
typedef struct _AgsWindowClass AgsWindowClass;

typedef enum{
  AGS_WINDOW_LOADING          = 1,
  AGS_WINDOW_SAVING           = 1 <<  1,
  AGS_WINDOW_TERMINATING      = 1 <<  2,
}AgsWindowFlags;

struct _AgsWindow
{
  GtkApplicationWindow application_window;

  guint flags;
  guint connectable_flags;
  
  gboolean no_config;
  gboolean shows_menu_bar;
  
  gchar *filename;
  
  char *name;

  GtkPopoverMenuBar *menu_bar;
  
  GtkHeaderBar *header_bar;
  GtkMenuButton *app_button;
  GtkMenuButton *add_button;
  GtkMenuButton *edit_button;
  
  GtkPaned *paned;

  GList *machine;
  
  GtkBox *machine_box;

  GList *machine_counter;

  AgsMachine *selected;

  AgsCompositeEditor *composite_editor;

  AgsNavigation *navigation;
};

struct _AgsWindowClass
{
  GtkApplicationWindowClass application_window;
};

GType ags_window_get_type(void);

GList* ags_window_get_machine(AgsWindow *window);
void ags_window_add_machine(AgsWindow *window,
			    AgsMachine *machine);
void ags_window_remove_machine(AgsWindow *window,
			       AgsMachine *machine);

void ags_window_load_add_menu_ladspa(AgsWindow *window,
				     GMenu *menu);
void ags_window_load_add_menu_dssi(AgsWindow *window,
				   GMenu *menu);
void ags_window_load_add_menu_lv2(AgsWindow *window,
				  GMenu *menu);
void ags_window_load_add_menu_vst3(AgsWindow *window,
				   GMenu *menu);

void ags_window_load_add_menu_live_dssi(AgsWindow *window,
					GMenu *menu);
void ags_window_load_add_menu_live_lv2(AgsWindow *window,
				       GMenu *menu);
void ags_window_load_add_menu_live_vst3(AgsWindow *window,
					GMenu *menu);

void ags_window_show_error(AgsWindow *window,
			   gchar *message);

gboolean ags_window_load_file_timeout(AgsWindow *window);

AgsWindow* ags_window_new();

G_END_DECLS

#endif /*__AGS_WINDOW_H__*/
