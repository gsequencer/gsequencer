/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_VINDICATOR_H__
#define __AGS_VINDICATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_indicator.h"

#define AGS_TYPE_VINDICATOR                (ags_vindicator_get_type())
#define AGS_VINDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INDICATOR, AgsVIndicator))
#define AGS_VINDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INDICATOR, AgsVIndicatorClass))
#define AGS_IS_VINDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INDICATOR))
#define AGS_IS_VINDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INDICATOR))
#define AGS_VINDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INDICATOR, AgsVIndicatorClass))

typedef struct _AgsVIndicator AgsVIndicator;
typedef struct _AgsVIndicatorClass AgsVIndicatorClass;

struct _AgsVIndicator
{
  AgsIndicator indicator;
};

struct _AgsVIndicatorClass
{
  GtkWidgetClass indicator
};

GType ags_vindicator_get_type(void);

AgsVIndicator* ags_vindicator_new();

#endif /*__AGS_VINDICATOR_H__*/


