/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_INDICATOR_H__
#define __AGS_INDICATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_INDICATOR                (ags_indicator_get_type())
#define AGS_INDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INDICATOR, AgsIndicator))
#define AGS_INDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INDICATOR, AgsIndicatorClass))
#define AGS_IS_INDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INDICATOR))
#define AGS_IS_INDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INDICATOR))
#define AGS_INDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INDICATOR, AgsIndicatorClass))

typedef struct _AgsIndicator AgsIndicator;
typedef struct _AgsIndicatorClass AgsIndicatorClass;

struct _AgsIndicator
{
  GtkWidget widget;

  GtkAdjustment *adjustment;
};

struct _AgsIndicatorClass
{
  GtkWidgetClass widget;
};

GType ags_indicator_get_type(void);

AgsIndicator* ags_indicator_new();

#endif /*__AGS_INDICATOR_H__*/
