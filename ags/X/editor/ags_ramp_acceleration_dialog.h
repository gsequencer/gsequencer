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

#ifndef __AGS_RAMP_ACCELERATION_DIALOG_H__
#define __AGS_RAMP_ACCELERATION_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_RAMP_ACCELERATION_DIALOG                (ags_ramp_acceleration_dialog_get_type())
#define AGS_RAMP_ACCELERATION_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RAMP_ACCELERATION_DIALOG, AgsRampAccelerationDialog))
#define AGS_RAMP_ACCELERATION_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RAMP_ACCELERATION_DIALOG, AgsRampAccelerationDialogClass))
#define AGS_IS_RAMP_ACCELERATION_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RAMP_ACCELERATION_DIALOG))
#define AGS_IS_RAMP_ACCELERATION_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RAMP_ACCELERATION_DIALOG))
#define AGS_RAMP_ACCELERATION_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_RAMP_ACCELERATION_DIALOG, AgsRampAccelerationDialogClass))

#define AGS_RAMP_ACCELERATION(ptr) ((AgsRampAcceleration *)(ptr))

#define AGS_RAMP_ACCELERATION_MAX_BEATS (16 * 16 * 1200)
#define AGS_RAMP_ACCELERATION_MAX_STEPS (128)
#define AGS_RAMP_ACCELERATION_DEFAULT_WIDTH (64)

typedef struct _AgsRampAccelerationDialog AgsRampAccelerationDialog;
typedef struct _AgsRampAccelerationDialogClass AgsRampAccelerationDialogClass;
typedef struct _AgsRampAutomation AgsRampAutomation;

typedef enum{
  AGS_RAMP_ACCELERATION_DIALOG_CONNECTED   = 1,
}AgsRampAccelerationDialogFlags;

struct _AgsRampAccelerationDialog
{
  GtkDialog dialog;

  guint flags;

  GObject *application_context;
  
  GtkWidget *main_window;

  GtkComboBoxText *port;

  GtkSpinButton *ramp_x0;
  GtkSpinButton *ramp_y0;

  GtkSpinButton *ramp_x1;
  GtkSpinButton *ramp_y1;

  GtkSpinButton *ramp_step_count;
};

struct _AgsRampAccelerationDialogClass
{
  GtkDialogClass dialog;
};

GType ags_ramp_acceleration_dialog_get_type(void);

AgsRampAccelerationDialog* ags_ramp_acceleration_dialog_new(GtkWidget *main_window);

#endif /*__AGS_RAMP_ACCELERATION_DIALOG_H__*/
