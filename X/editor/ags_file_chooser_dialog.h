/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2012 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_FILE_CHOOSER_DIALOG_H__
#define __AGS_FILE_CHOOSER_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_FILE_CHOOSER_DIALOG                (ags_file_chooser_dialog_get_type ())
#define AGS_FILE_CHOOSER_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_CHOOSER_DIALOG, AgsFileChooserDialog))
#define AGS_FILE_CHOOSER_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_CHOOSER_DIALOG, AgsFileChooserDialogClass))
#define AGS_IS_FILE_CHOOSER_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FILE_CHOOSER_DIALOG))
#define AGS_IS_FILE_CHOOSER_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FILE_CHOOSER_DIALOG))
#define AGS_FILE_CHOOSER_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FILE_CHOOSER_DIALOG, AgsFileChooserDialogClass))

typedef struct _AgsFileChooserDialog AgsFileChooserDialog;
typedef struct _AgsFileChooserDialogClass AgsFileChooserDialogClass;

struct _AgsFileChooserDialog
{
  GtkFileChooserDialog file_chooser_dialog;

  GtkComboBoxText *preset;
  GtkComboBoxText *instrument;
  GtkComboBoxText *sample;
};

struct _AgsFileChooserDialogClass
{
  GtkFileChooserDialogClass file_chooser_dialog;
};

GType ags_file_chooser_dialog_get_type();

AgsFileChooserDialog* ags_file_chooser_dialog_new();

#endif /*__AGS_FILE_CHOOSER_DIALOG_H__*/
