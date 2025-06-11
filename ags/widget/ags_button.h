/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_BUTTON_H__
#define __AGS_BUTTON_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_BUTTON                (ags_button_get_type())
#define AGS_BUTTON(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUTTON, AgsButton))
#define AGS_BUTTON_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BUTTON, AgsButtonClass))
#define AGS_IS_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_BUTTON))
#define AGS_IS_BUTTON_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_BUTTON))
#define AGS_BUTTON_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_BUTTON, AgsButtonClass))

#define AGS_BUTTON_DEFAULT_BORDER_WIDTH (1.25)

#define AGS_BUTTON_SMALL_FONT_SIZE (8)
#define AGS_BUTTON_NORMAL_FONT_SIZE (12)
#define AGS_BUTTON_LARGE_FONT_SIZE (14)

#define AGS_BUTTON_DEFAULT_FONT_SIZE (12)

typedef struct _AgsButton AgsButton;
typedef struct _AgsButtonClass AgsButtonClass;

typedef enum{
  AGS_BUTTON_SHOW_LABEL      = 1,
  AGS_BUTTON_SHOW_ICON       = 1 <<  1,
}AgsButtonFlags;

typedef enum{
  AGS_BUTTON_SIZE_INHERIT,
  AGS_BUTTON_SIZE_SMALL,
  AGS_BUTTON_SIZE_NORMAL,
  AGS_BUTTON_SIZE_LARGE,
}AgsButtonSize;

struct _AgsButton
{
  GtkWidget widget;

  AgsButtonFlags flags;
  AgsButtonSize button_size;

  guint font_size;

  gchar *font_name;

  gchar *label;

  gchar *icon_name;
};

struct _AgsButtonClass
{
  GtkWidgetClass widget;

  void (*clicked)(AgsButton *button);
};

GType ags_button_get_type(void);
GType ags_button_flags_get_type(void);
GType ags_button_size_get_type(void);

/* instantiate */
AgsButton* ags_button_new();

G_END_DECLS

#endif /*__AGS_BUTTON_H__*/
