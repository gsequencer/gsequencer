/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_SCROLLED_WAVE_EDIT_BOX_H__
#define __AGS_SCROLLED_WAVE_EDIT_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_wave_edit_box.h>

G_BEGIN_DECLS

#define AGS_TYPE_SCROLLED_WAVE_EDIT_BOX                (ags_scrolled_wave_edit_box_get_type())
#define AGS_SCROLLED_WAVE_EDIT_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCROLLED_WAVE_EDIT_BOX, AgsScrolledWaveEditBox))
#define AGS_SCROLLED_WAVE_EDIT_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCROLLED_WAVE_EDIT_BOX, AgsScrolledWaveEditBoxClass))
#define AGS_IS_SCROLLED_WAVE_EDIT_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCROLLED_WAVE_EDIT_BOX))
#define AGS_IS_SCROLLED_WAVE_EDIT_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCROLLED_WAVE_EDIT_BOX))
#define AGS_SCROLLED_WAVE_EDIT_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCROLLED_WAVE_EDIT_BOX, AgsScrolledWaveEditBoxClass))

typedef struct _AgsScrolledWaveEditBox AgsScrolledWaveEditBox;
typedef struct _AgsScrolledWaveEditBoxClass AgsScrolledWaveEditBoxClass;

struct _AgsScrolledWaveEditBox
{
  GtkGrid grid;

  GtkScrolledWindow *scrolled_window;

  GtkBox *wave_edit_box;
};

struct _AgsScrolledWaveEditBoxClass
{
  GtkGridClass grid;
};

GType ags_scrolled_wave_edit_box_get_type(void);

GtkScrolledWindow* ags_scrolled_wave_edit_box_get_scrolled_window(AgsScrolledWaveEditBox *scrolled_wave_edit_box);

AgsWaveEditBox* ags_scrolled_wave_edit_box_get_wave_edit_box(AgsScrolledWaveEditBox *scrolled_wave_edit_box);

AgsScrolledWaveEditBox* ags_scrolled_wave_edit_box_new();

G_END_DECLS

#endif /*__AGS_SCROLLED_WAVE_EDIT_BOX_H__*/
