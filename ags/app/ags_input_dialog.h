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

#ifndef __AGS_INPUT_DIALOG_H__
#define __AGS_INPUT_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_INPUT_DIALOG                (ags_input_dialog_get_type())
#define AGS_INPUT_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INPUT_DIALOG, AgsInputDialog))
#define AGS_INPUT_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INPUT_DIALOG, AgsInputDialogClass))
#define AGS_IS_INPUT_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INPUT_DIALOG))
#define AGS_IS_INPUT_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INPUT_DIALOG))
#define AGS_INPUT_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INPUT_DIALOG, AgsInputDialogClass))

typedef struct _AgsInputDialog AgsInputDialog;
typedef struct _AgsInputDialogClass AgsInputDialogClass;

typedef enum{
  AGS_INPUT_DIALOG_SHOW_STRING_INPUT        = 1,
  AGS_INPUT_DIALOG_SHOW_SPIN_BUTTON_INPUT   = 1 <<  1,
}AgsInputDialogFlags;

struct _AgsInputDialog
{
  GtkDialog dialog;

  guint flags;

  GtkEntry *string_input;

  GtkLabel *spin_button_label;  
  GtkSpinButton *spin_button_input;
};

struct _AgsInputDialogClass
{
  GtkDialogClass dialog;
};

GType ags_input_dialog_get_type(void);

gboolean ags_input_dialog_test_flags(AgsInputDialog *input_dialog,
				     guint flags);
void ags_input_dialog_set_flags(AgsInputDialog *input_dialog,
				guint flags);
void ags_input_dialog_unset_flags(AgsInputDialog *input_dialog,
				  guint flags);

AgsInputDialog* ags_input_dialog_new(gchar *title,
				     GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_INPUT_DIALOG_H__*/
