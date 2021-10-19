/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_SCROLLED_WINDOW_H__
#define __AGS_SCROLLED_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_viewport.h>

G_BEGIN_DECLS

#define AGS_TYPE_SCROLLED_WINDOW                (ags_scrolled_window_get_type())
#define AGS_SCROLLED_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCROLLED_WINDOW, AgsScrolledWindow))
#define AGS_SCROLLED_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCROLLED_WINDOW, AgsScrolledWindowClass))
#define AGS_IS_SCROLLED_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SCROLLED_WINDOW))
#define AGS_IS_SCROLLED_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SCROLLED_WINDOW))
#define AGS_SCROLLED_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SCROLLED_WINDOW, AgsScrolledWindowClass))

typedef struct _AgsScrolledWindow AgsScrolledWindow;
typedef struct _AgsScrolledWindowClass AgsScrolledWindowClass;

struct _AgsScrolledWindow
{
  GtkGrid grid;

  AgsViewport *viewport;
  
  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;
  
  gboolean show_hscrollbar;
  gboolean show_vscrollbar;
};

struct _AgsScrolledWindowClass
{
  GtkGridClass grid;
};

GType ags_scrolled_window_get_type(void);

/* instantiate */
AgsScrolledWindow* ags_scrolled_window_new();

G_END_DECLS

#endif /*__AGS_SCROLLED_WINDOW_H__*/
