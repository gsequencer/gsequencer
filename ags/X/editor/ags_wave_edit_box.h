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

#ifndef __AGS_WAVE_EDIT_BOX_H__
#define __AGS_WAVE_EDIT_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_WAVE_EDIT_BOX                (ags_wave_edit_box_get_type())
#define AGS_WAVE_EDIT_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBox))
#define AGS_WAVE_EDIT_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBoxClass))
#define AGS_IS_WAVE_EDIT_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDIT_BOX))
#define AGS_IS_WAVE_EDIT_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDIT_BOX))
#define AGS_WAVE_EDIT_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBoxClass))

#define AGS_WAVE_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT (128)

typedef struct _AgsWaveEditBox AgsWaveEditBox;
typedef struct _AgsWaveEditBoxClass AgsWaveEditBoxClass;

typedef enum{
  AGS_WAVE_EDIT_BOX_FIXED_EDIT_SIZE  = 1,
}AgsWaveEditBoxFlags;

struct _AgsWaveEditBox
{
  GtkBox box;

  guint flags;

  guint fixed_edit_height;
};

struct _AgsWaveEditBoxClass
{
  GtkBoxClass box;
};

GType ags_wave_edit_box_get_type(void);

AgsWaveEditBox* ags_wave_edit_box_new();

#endif /*__AGS_WAVE_EDIT_BOX_H__*/
