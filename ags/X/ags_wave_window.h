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

#ifndef __AGS_WAVE_WINDOW_H__
#define __AGS_WAVE_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_wave_editor.h>

#define AGS_TYPE_WAVE_WINDOW                (ags_wave_window_get_type())
#define AGS_WAVE_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_WINDOW, AgsWaveWindow))
#define AGS_WAVE_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_WINDOW, AgsWaveWindowClass))
#define AGS_IS_WAVE_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WAVE_WINDOW))
#define AGS_IS_WAVE_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WAVE_WINDOW))
#define AGS_WAVE_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WAVE_WINDOW, AgsWaveWindowClass))

typedef struct _AgsWaveWindow AgsWaveWindow;
typedef struct _AgsWaveWindowClass AgsWaveWindowClass;

typedef enum{
  AGS_WAVE_WINDOW_CONNECTED         = 1,
}AgsWaveWindowFlags;

struct _AgsWaveWindow
{
  GtkWindow window;

  guint flags;

  GObject *soundcard;
  
  GtkWidget *parent_window;

  AgsWaveEditor *wave_editor;
};

struct _AgsWaveWindowClass
{
  GtkWindowClass window;
};

GType ags_wave_window_get_type(void);

AgsWaveWindow* ags_wave_window_new(GtkWidget *parent_window);

#endif /*__AGS_WAVE_WINDOW_H__*/
