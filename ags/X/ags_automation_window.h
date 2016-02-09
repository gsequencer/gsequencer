/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_AUTOMATION_WINDOW_H__
#define __AGS_AUTOMATION_WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_automation_editor.h>

#define AGS_TYPE_AUTOMATION_WINDOW                (ags_automation_window_get_type())
#define AGS_AUTOMATION_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindow))
#define AGS_AUTOMATION_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindowClass))
#define AGS_IS_AUTOMATION_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUTOMATION_WINDOW))
#define AGS_IS_AUTOMATION_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUTOMATION_WINDOW))
#define AGS_AUTOMATION_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindowClass))

typedef struct _AgsAutomationWindow AgsAutomationWindow;
typedef struct _AgsAutomationWindowClass AgsAutomationWindowClass;

typedef enum{
  AGS_AUTOMATION_WINDOW_LIVE_AUTOMATION   = 1,
}AgsAutomationWindowFlags;

struct _AgsAutomationWindow
{
  GtkWindow window;

  GtkWidget *parent_window;

  AgsAutomationEditor *automation_editor;
};

struct _AgsAutomationWindowClass
{
  GtkWindowClass window;
};

GType ags_automation_window_get_type(void);

AgsAutomationWindow* ags_automation_window_new(GtkWidget *parent_window);

#endif /*__AGS_AUTOMATION_WINDOW_H__*/
