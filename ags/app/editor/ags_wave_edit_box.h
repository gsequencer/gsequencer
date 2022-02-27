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

#ifndef __AGS_WAVE_EDIT_BOX_H__
#define __AGS_WAVE_EDIT_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_EDIT_BOX                (ags_wave_edit_box_get_type())
#define AGS_WAVE_EDIT_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBox))
#define AGS_WAVE_EDIT_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBoxClass))
#define AGS_IS_WAVE_EDIT_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDIT_BOX))
#define AGS_IS_WAVE_EDIT_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDIT_BOX))
#define AGS_WAVE_EDIT_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDIT_BOX, AgsWaveEditBoxClass))

#define AGS_WAVE_EDIT_BOX_DEFAULT_WAVE_HEIGHT (128)
#define AGS_WAVE_EDIT_BOX_DEFAULT_SPACING (8)

typedef struct _AgsWaveEditBox AgsWaveEditBox;
typedef struct _AgsWaveEditBoxClass AgsWaveEditBoxClass;

struct _AgsWaveEditBox
{
  GtkBox box;

  /* private */
  guint wave_edit_count;
  
  GList *wave_edit;
};

struct _AgsWaveEditBoxClass
{
  GtkBoxClass box;

  void (*child_width_request)(AgsWaveEditBox *wave_edit_box,
			      GtkWidget *wave_edit,
			      gint width_request);
  void (*child_height_request)(AgsWaveEditBox *wave_edit_box,
			       GtkWidget *wave_edit,
			       gint height_request);
};

GType ags_wave_edit_box_get_type(void);

guint ags_wave_edit_box_get_wave_edit_count(AgsWaveEditBox *wave_edit_box);

void ags_wave_edit_box_add(AgsWaveEditBox *wave_edit_box,
			   GtkWidget *wave_edit);
void ags_wave_edit_box_remove(AgsWaveEditBox *wave_edit_box,
			      guint position);

void ags_wave_edit_box_child_width_request(AgsWaveEditBox *wave_edit_box,
					   GtkWidget *wave_edit,
					   gint width_request);
void ags_wave_edit_box_child_height_request(AgsWaveEditBox *wave_edit_box,
					    GtkWidget *wave_edit,
					    gint height_request);

GList* ags_wave_edit_box_get_wave_edit(AgsWaveEditBox *wave_edit_box);

AgsWaveEditBox* ags_wave_edit_box_new(GtkOrientation orientation);

G_END_DECLS

#endif /*__AGS_WAVE_EDIT_BOX_H__*/
