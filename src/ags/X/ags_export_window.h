/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_EXPORT_WINDOW_H__
#define __AGS_EXPORT_WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_EXPORT_WINDOW                (ags_export_window_get_type())
#define AGS_EXPORT_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_WINDOW, AgsExportWindow))
#define AGS_EXPORT_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_WINDOW, AgsExportWindowClass))
#define AGS_IS_EXPORT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_WINDOW))
#define AGS_IS_EXPORT_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_WINDOW))
#define AGS_EXPORT_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_WINDOW, AgsExportWindowClass))

typedef struct _AgsExportWindow AgsExportWindow;
typedef struct _AgsExportWindowClass AgsExportWindowClass;

typedef enum{
  AGS_EXPORT_WINDOW_LIVE_EXPORT   = 1,
}AgsExportWindowFlags;

struct _AgsExportWindow
{
  GtkWindow window;

  guint flags;

  GObject *ags_main;
  AgsDevout *devout;

  GtkCheckButton *live_export;

  GtkComboBoxText *mode;
  GtkSpinButton *tact;
  GtkLabel *duration;
  GtkComboBoxText *output_format;

  GtkToggleButton *export;
};

struct _AgsExportWindowClass
{
  GtkWindowClass window;
};

GType ags_export_window_get_type(void);

AgsExportWindow* ags_export_window_new();

#endif /*__AGS_EXPORT_WINDOW_H__*/
