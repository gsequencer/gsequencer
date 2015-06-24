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

#ifndef __AGS_CARTESIAN_H__
#define __AGS_CARTESIAN_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_CARTESIAN                (ags_cartesian_get_type())
#define AGS_CARTESIAN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CARTESIAN, AgsCartesian))
#define AGS_CARTESIAN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CARTESIAN, AgsCartesianClass))
#define AGS_IS_CARTESIAN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CARTESIAN))
#define AGS_IS_CARTESIAN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CARTESIAN))
#define AGS_CARTESIAN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CARTESIAN, AgsCartesianClass))

typedef struct _AgsCartesian AgsCartesian;
typedef struct _AgsCartesianClass AgsCartesianClass;

struct _AgsCartesian
{
  GtkWidget widget;
};

struct _AgsCartesianClass
{
  GtkWidgetClass widget;
};

GType ags_cartesian_get_type(void);

AgsCartesian* ags_cartesian_new();

#endif /*__AGS_CARTESIAN_H__*/

