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

#ifndef __AGS_PLAYBACK_WINDOW_H__
#define __AGS_PLAYBACK_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_PLAYBACK_WINDOW                (ags_playback_window_get_type())
#define AGS_PLAYBACK_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYBACK_WINDOW, AgsPlaybackWindow))
#define AGS_PLAYBACK_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAYBACK_WINDOW, AgsPlaybackWindowClass))
#define AGS_IS_PLAYBACK_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAYBACK_WINDOW))
#define AGS_IS_PLAYBACK_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAYBACK_WINDOW))
#define AGS_PLAYBACK_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAYBACK_WINDOW, AgsPlaybackWindowClass))

typedef struct _AgsPlaybackWindow AgsPlaybackWindow;
typedef struct _AgsPlaybackWindowClass AgsPlaybackWindowClass;

typedef enum{
  AGS_PLAYBACK_WINDOW_CONNECTED     = 1,
}AgsPlaybackWindowFlags;

struct _AgsPlaybackWindow
{
  GtkWindow window;

  guint flags;

  GObject *application_context;

  GtkWidget *main_window;
};

struct _AgsPlaybackWindowClass
{
  GtkWindowClass window;
};

GType ags_playback_window_get_type(void);

AgsPlaybackWindow* ags_playback_window_new();

#endif /*__AGS_PLAYBACK_WINDOW_H__*/
