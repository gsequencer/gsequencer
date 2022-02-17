/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SELECT_ACCELERATION_DIALOG_H__
#define __AGS_SELECT_ACCELERATION_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_SELECT_ACCELERATION_DIALOG                (ags_select_acceleration_dialog_get_type())
#define AGS_SELECT_ACCELERATION_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SELECT_ACCELERATION_DIALOG, AgsSelectAccelerationDialog))
#define AGS_SELECT_ACCELERATION_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SELECT_ACCELERATION_DIALOG, AgsSelectAccelerationDialogClass))
#define AGS_IS_SELECT_ACCELERATION_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SELECT_ACCELERATION_DIALOG))
#define AGS_IS_SELECT_ACCELERATION_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SELECT_ACCELERATION_DIALOG))
#define AGS_SELECT_ACCELERATION_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SELECT_ACCELERATION_DIALOG, AgsSelectAccelerationDialogClass))

#define AGS_SELECT_ACCELERATION_MAX_BEATS (16 * 1200)
#define AGS_SELECT_ACCELERATION_DEFAULT_WIDTH (64)

typedef struct _AgsSelectAccelerationDialog AgsSelectAccelerationDialog;
typedef struct _AgsSelectAccelerationDialogClass AgsSelectAccelerationDialogClass;

typedef enum{
  AGS_SELECT_ACCELERATION_DIALOG_CONNECTED   = 1,
}AgsSelectAccelerationDialogFlags;

struct _AgsSelectAccelerationDialog
{
  GtkDialog dialog;

  guint flags;
  
  GtkWidget *main_window;

  GtkCheckButton *copy_selection;

  GtkVBox *port;
  GtkButton *add;

  GtkSpinButton *select_x0;

  GtkSpinButton *select_x1;
};

struct _AgsSelectAccelerationDialogClass
{
  GtkDialogClass dialog;
};

GType ags_select_acceleration_dialog_get_type(void);

AgsSelectAccelerationDialog* ags_select_acceleration_dialog_new(GtkWidget *main_window);

G_END_DECLS

#endif /*__AGS_SELECT_ACCELERATION_DIALOG_H__*/
