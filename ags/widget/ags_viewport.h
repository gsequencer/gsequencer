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

#ifndef __AGS_VIEWPORT_H__
#define __AGS_VIEWPORT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_VIEWPORT                (ags_viewport_get_type())
#define AGS_VIEWPORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VIEWPORT, AgsViewport))
#define AGS_VIEWPORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VIEWPORT, AgsViewportClass))
#define AGS_IS_VIEWPORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_VIEWPORT))
#define AGS_IS_VIEWPORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_VIEWPORT))
#define AGS_VIEWPORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_VIEWPORT, AgsViewportClass))

#define AGS_VIEWPORT_MINIMAL_WIDTH (1)
#define AGS_VIEWPORT_MINIMAL_HEIGHT (1)

typedef struct _AgsViewport AgsViewport;
typedef struct _AgsViewportClass AgsViewportClass;

struct _AgsViewport
{
  GtkBin bin;

  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  gint remaining_width;
  gint remaining_height;
};

struct _AgsViewportClass
{
  GtkBinClass bin;
};

GType ags_viewport_get_type(void);

/* instantiate */
AgsViewport* ags_viewport_new();

G_END_DECLS

#endif /*__AGS_VIEWPORT_H__*/
