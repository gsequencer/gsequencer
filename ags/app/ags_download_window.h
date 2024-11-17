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

#ifndef __AGS_DOWNLOAD_WINDOW_H__
#define __AGS_DOWNLOAD_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include "ags/config.h"

G_BEGIN_DECLS

#define AGS_TYPE_DOWNLOAD_WINDOW                (ags_download_window_get_type())
#define AGS_DOWNLOAD_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DOWNLOAD_WINDOW, AgsDownloadWindow))
#define AGS_DOWNLOAD_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DOWNLOAD_WINDOW, AgsDownloadWindowClass))
#define AGS_IS_DOWNLOAD_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DOWNLOAD_WINDOW))
#define AGS_IS_DOWNLOAD_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DOWNLOAD_WINDOW))
#define AGS_DOWNLOAD_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DOWNLOAD_WINDOW, AgsDownloadWindowClass))

#define AGS_DOWNLOAD_WINDOW_DEFAULT_SCRIPT_LOCATION AGS_DATA_DIR "/gsequencer/scripts/"
#define AGS_DOWNLOAD_WINDOW_DEFAULT_DOWNLOAD_SCRIPT AGS_DOWNLOAD_WINDOW_DEFAULT_SCRIPT_LOCATION "download_all.sh"

typedef struct _AgsDownloadWindow AgsDownloadWindow;
typedef struct _AgsDownloadWindowClass AgsDownloadWindowClass;

struct _AgsDownloadWindow
{
  GtkWindow window;

  guint flags;
  guint connectable_flags;

  gchar *download_script;

  GtkWidget *terminal;
  
  GtkButton *activate_button;
};

struct _AgsDownloadWindowClass
{
  GtkWindowClass window;
};

GType ags_download_window_get_type(void);

AgsDownloadWindow* ags_download_window_new(GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_DOWNLOAD_WINDOW_H__*/
