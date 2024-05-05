/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Daniel Maksymow, Joël Krähemann
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

#ifndef AGS_META_DATA_WINDOW_H
#define AGS_META_DATA_WINDOW_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_META_DATA_WINDOW                (ags_meta_data_window_get_type())
#define AGS_META_DATA_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_META_DATA_WINDOW, AgsMetaDataWindow))
#define AGS_META_DATA_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_META_DATA_WINDOW, AgsMetaDataWindowClass))
#define AGS_IS_META_DATA_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_META_DATA_WINDOW))
#define AGS_IS_META_DATA_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_META_DATA_WINDOW))
#define AGS_META_DATA_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_META_DATA_WINDOW, AgsMetaDataWindowClass))

typedef struct _AgsMetaDataWindow AgsMetaDataWindow;
typedef struct _AgsMetaDataWindowClass AgsMetaDataWindowClass;
  
struct _AgsMetaDataWindow
{
  GtkWindow window;

  GtkEntry *author;
  GtkEntry *title;
  GtkEntry *album;
  GtkEntry *release_date;
  GtkEntry *copyright;
  GtkEntry *license;
  GtkTextView *comment;
};

struct _AgsMetaDataWindowClass
{
  GtkWindowClass window;
};

GType ags_meta_data_window_get_type();

AgsMetaDataWindow* ags_meta_data_window_new();

G_END_DECLS

#endif
