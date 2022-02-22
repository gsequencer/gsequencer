/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_AUTOMATION_WINDOW_H__
#define __AGS_AUTOMATION_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_automation_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_WINDOW                (ags_automation_window_get_type())
#define AGS_AUTOMATION_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindow))
#define AGS_AUTOMATION_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindowClass))
#define AGS_IS_AUTOMATION_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUTOMATION_WINDOW))
#define AGS_IS_AUTOMATION_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUTOMATION_WINDOW))
#define AGS_AUTOMATION_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUTOMATION_WINDOW, AgsAutomationWindowClass))

typedef struct _AgsAutomationWindow AgsAutomationWindow;
typedef struct _AgsAutomationWindowClass AgsAutomationWindowClass;

typedef enum{
  AGS_AUTOMATION_WINDOW_CONNECTED         = 1,
  AGS_AUTOMATION_WINDOW_LIVE_AUTOMATION   = 1 << 1,
}AgsAutomationWindowFlags;

struct _AgsAutomationWindow
{
  GtkWindow window;

  guint flags;

  GtkWidget *parent_window;

  AgsAutomationEditor *automation_editor;
};

struct _AgsAutomationWindowClass
{
  GtkWindowClass window;
};

GType ags_automation_window_get_type(void);

AgsAutomationWindow* ags_automation_window_new(GtkWidget *parent_window);

G_END_DECLS

#endif /*__AGS_AUTOMATION_WINDOW_H__*/
