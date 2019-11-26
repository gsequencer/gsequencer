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

#ifndef __AGS_ANIMATION_WINDOW_H__
#define __AGS_ANIMATION_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_ANIMATION_WINDOW                (ags_animation_window_get_type())
#define AGS_ANIMATION_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ANIMATION_WINDOW, AgsAnimationWindow))
#define AGS_ANIMATION_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ANIMATION_WINDOW, AgsAnimationWindowClass))
#define AGS_IS_ANIMATION_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ANIMATION_WINDOW))
#define AGS_IS_ANIMATION_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ANIMATION_WINDOW))
#define AGS_ANIMATION_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ANIMATION_WINDOW, AgsAnimationWindowClass))

typedef struct _AgsAnimationWindow AgsAnimationWindow;
typedef struct _AgsAnimationWindowClass AgsAnimationWindowClass;

struct _AgsAnimationWindow
{
  GtkWindow window;

  guint flags;

  guint message_count;

  gchar *filename;

  guint image_size;
  unsigned char *bg_data;
  unsigned char *cache_data;
};

struct _AgsAnimationWindowClass
{
  GtkWindowClass window;
};

GType ags_animation_window_get_type(void);

gboolean ags_animation_window_progress_timeout(AgsAnimationWindow *animation_window);

AgsAnimationWindow* ags_animation_window_new();

#endif /*__AGS_ANIMATION_WINDOW_H__*/
