/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SHEET_WINDOW_H__
#define __AGS_SHEET_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_sheet_editor.h>

#define AGS_TYPE_SHEET_WINDOW                (ags_sheet_window_get_type())
#define AGS_SHEET_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SHEET_WINDOW, AgsSheetWindow))
#define AGS_SHEET_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SHEET_WINDOW, AgsSheetWindowClass))
#define AGS_IS_SHEET_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SHEET_WINDOW))
#define AGS_IS_SHEET_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SHEET_WINDOW))
#define AGS_SHEET_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SHEET_WINDOW, AgsSheetWindowClass))

typedef struct _AgsSheetWindow AgsSheetWindow;
typedef struct _AgsSheetWindowClass AgsSheetWindowClass;

typedef enum{
  AGS_SHEET_WINDOW_CONNECTED         = 1,
}AgsSheetWindowFlags;

struct _AgsSheetWindow
{
  GtkWindow window;

  guint flags;

  GObject *soundcard;
  
  GtkWidget *parent_window;

  AgsSheetEditor *sheet_editor;
};

struct _AgsSheetWindowClass
{
  GtkWindowClass window;
};

GType ags_sheet_window_get_type(void);

AgsSheetWindow* ags_sheet_window_new(GtkWidget *parent_window);

#endif /*__AGS_SHEET_WINDOW_H__*/
