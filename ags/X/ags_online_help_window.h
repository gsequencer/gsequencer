/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_ONLINE_HELP_WINDOW_H__
#define __AGS_ONLINE_HELP_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/config.h>

#if defined(AGS_WITH_WEBKIT)
#include <webkit2/webkit2.h>
#endif

#if defined(AGS_WITH_POPPLER)
#include <poppler.h>
#endif

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_ONLINE_HELP_WINDOW                (ags_online_help_window_get_type())
#define AGS_ONLINE_HELP_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ONLINE_HELP_WINDOW, AgsOnlineHelpWindow))
#define AGS_ONLINE_HELP_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ONLINE_HELP_WINDOW, AgsOnlineHelpWindowClass))
#define AGS_IS_ONLINE_HELP_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ONLINE_HELP_WINDOW))
#define AGS_IS_ONLINE_HELP_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ONLINE_HELP_WINDOW))
#define AGS_ONLINE_HELP_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ONLINE_HELP_WINDOW, AgsOnlineHelpWindowClass))

#define AGS_ONLINE_HELP_WINDOW_DEFAULT_PDF_URI "file:///usr/share/doc/gsequencer/user-manual.pdf"
#define AGS_ONLINE_HELP_WINDOW_DEFAULT_IMAGE_DPI (150)

typedef struct _AgsOnlineHelpWindow AgsOnlineHelpWindow;
typedef struct _AgsOnlineHelpWindowClass AgsOnlineHelpWindowClass;

typedef enum{
  AGS_ONLINE_HELP_WINDOW_CONNECTED     = 1,
}AgsOnlineHelpWindowFlags;

struct _AgsOnlineHelpWindow
{
  GtkWindow window;

  guint flags;
  
  GtkButton *home;

  GtkButton *next;
  GtkButton *prev;

  GtkEntry *location;

  gchar *start_filename;
  
#if defined(AGS_WITH_WEBKIT)
  WebKitWebView *web_view;
#else
  gpointer web_view;
#endif
  
  gint max_width;
  gint max_height;

  GtkVScrollbar *pdf_vscrollbar;
  GtkHScrollbar *pdf_hscrollbar;
  
  GtkWidget *pdf_drawing_area;
  
#if defined(AGS_WITH_POPPLER)
  PopplerDocument *pdf_document;
#else
  gpointer pdf_document;
#endif
};

struct _AgsOnlineHelpWindowClass
{
  GtkWindowClass window;
};

GType ags_online_help_window_get_type(void);

AgsOnlineHelpWindow* ags_online_help_window_new();

G_END_DECLS

#endif /*__AGS_ONLINE_HELP_WINDOW_H__*/
