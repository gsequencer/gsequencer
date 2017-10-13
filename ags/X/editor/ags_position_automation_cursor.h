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

#ifndef __AGS_POSITION_AUTOMATION_CURSOR_DIALOG_H__
#define __AGS_POSITION_AUTOMATION_CURSOR_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG                (ags_position_automation_cursor_dialog_get_type())
#define AGS_POSITION_AUTOMATION_CURSOR_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG, AgsPositionAutomationCursor))
#define AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG, AgsPositionAutomationCursorClass))
#define AGS_IS_POSITION_AUTOMATION_CURSOR_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG))
#define AGS_IS_POSITION_AUTOMATION_CURSOR_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG))
#define AGS_POSITION_AUTOMATION_CURSOR_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG, AgsPositionAutomationCursorClass))

#define AGS_POSITION_AUTOMATION_CURSOR_MAX_BEATS (16 * 1200)

typedef struct _AgsPositionAutomationCursor AgsPositionAutomationCursor;
typedef struct _AgsPositionAutomationCursorClass AgsPositionAutomationCursorClass;

typedef enum{
  AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CONNECTED   = 1,
}AgsPositionAutomationCursorFlags;

struct _AgsPositionAutomationCursor
{
  GtkDialog dialog;

  guint flags;

  GObject *application_context;
  
  GtkWidget *main_window;

  GtkCheckButton *set_focus;

  GtkSpinButton *position_x;
  GtkComboBoxText *position_automation_area;
};

struct _AgsPositionAutomationCursorClass
{
  GtkDialogClass dialog;
};

GType ags_position_automation_cursor_dialog_get_type(void);

AgsPositionAutomationCursor* ags_position_automation_cursor_dialog_new(GtkWidget *main_window);

#endif /*__AGS_POSITION_AUTOMATION_CURSOR_DIALOG_H__*/
