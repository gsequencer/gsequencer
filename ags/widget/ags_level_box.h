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

#ifndef __AGS_LEVEL_BOX_H__
#define __AGS_LEVEL_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_LEVEL_BOX                (ags_level_box_get_type())
#define AGS_LEVEL_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LEVEL_BOX, AgsLevelBox))
#define AGS_LEVEL_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LEVEL_BOX, AgsLevelBoxClass))
#define AGS_IS_LEVEL_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LEVEL_BOX))
#define AGS_IS_LEVEL_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LEVEL_BOX))
#define AGS_LEVEL_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_LEVEL_BOX, AgsLevelBoxClass))

#define AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH (60)
#define AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT (128)

typedef struct _AgsLevelBox AgsLevelBox;
typedef struct _AgsLevelBoxClass AgsLevelBoxClass;

typedef enum{
  AGS_LEVEL_BOX_FIXED_LEVEL_SIZE  = 1,
}AgsLevelBoxFlags;

struct _AgsLevelBox
{
  GtkBox box;

  guint flags;

  guint fixed_level_width;
  guint fixed_level_height;
};

struct _AgsLevelBoxClass
{
  GtkBoxClass box;
};

GType ags_level_box_get_type(void);

AgsLevelBox* ags_level_box_new();

#endif /*__AGS_LEVEL_BOX_H__*/
