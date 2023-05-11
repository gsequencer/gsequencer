/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_RAMP_MARKER_DIALOG_H__
#define __AGS_RAMP_MARKER_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_RAMP_MARKER_DIALOG                (ags_ramp_marker_dialog_get_type())
#define AGS_RAMP_MARKER_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RAMP_MARKER_DIALOG, AgsRampMarkerDialog))
#define AGS_RAMP_MARKER_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RAMP_MARKER_DIALOG, AgsRampMarkerDialogClass))
#define AGS_IS_RAMP_MARKER_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RAMP_MARKER_DIALOG))
#define AGS_IS_RAMP_MARKER_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RAMP_MARKER_DIALOG))
#define AGS_RAMP_MARKER_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_RAMP_MARKER_DIALOG, AgsRampMarkerDialogClass))

#define AGS_RAMP_MARKER_MAX_BEATS (16 * 16 * 1200)
#define AGS_RAMP_MARKER_MAX_STEPS (128)
#define AGS_RAMP_MARKER_DEFAULT_WIDTH (64)

typedef struct _AgsRampMarkerDialog AgsRampMarkerDialog;
typedef struct _AgsRampMarkerDialogClass AgsRampMarkerDialogClass;

struct _AgsRampMarkerDialog
{
  GtkDialog dialog;

  guint connectable_flags;
  
  GtkComboBoxText *control_name;

  GtkSpinButton *ramp_x0;
  GtkSpinButton *ramp_y0;

  GtkSpinButton *ramp_x1;
  GtkSpinButton *ramp_y1;

  GtkSpinButton *ramp_step_count;
};

struct _AgsRampMarkerDialogClass
{
  GtkDialogClass dialog;
};

GType ags_ramp_marker_dialog_get_type(void);

AgsRampMarkerDialog* ags_ramp_marker_dialog_new();

G_END_DECLS

#endif /*__AGS_RAMP_MARKER_DIALOG_H__*/
