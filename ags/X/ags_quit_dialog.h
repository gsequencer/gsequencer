/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_QUIT_DIALOG_H__
#define __AGS_QUIT_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_QUIT_DIALOG                (ags_quit_dialog_get_type())
#define AGS_QUIT_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_QUIT_DIALOG, AgsQuitDialog))
#define AGS_QUIT_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_QUIT_DIALOG, AgsQuitDialogClass))
#define AGS_IS_QUIT_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_QUIT_DIALOG))
#define AGS_IS_QUIT_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_QUIT_DIALOG))
#define AGS_QUIT_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_QUIT_DIALOG, AgsQuitDialogClass))

typedef struct _AgsQuitDialog AgsQuitDialog;
typedef struct _AgsQuitDialogClass AgsQuitDialogClass;

typedef enum{
  AGS_QUIT_DIALOG_CONNECTED     = 1,
}AgsQuitDialogFlags;

typedef enum{
  AGS_QUIT_DIALOG_QUESTION_SAVE_FILE,
  AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE,
}AgsQuitDialogQuestion;

struct _AgsQuitDialog
{
  GtkDialog dialog;

  guint flags;

  GtkCheckButton *accept_all;

  guint current_question;
  GtkLabel *question;

  guint nth_wave_export_machine;
  GList *wave_export_machine;
  
  GtkButton *yes;
  GtkButton *no;

  GtkButton *cancel;

  GtkDialog *confirm;
};

struct _AgsQuitDialogClass
{
  GtkDialogClass dialog;
};

GType ags_quit_dialog_get_type(void);

void ags_quit_dialog_fast_export(AgsQuitDialog *quit_dialog,
				 AgsMachine *machine);

AgsQuitDialog* ags_quit_dialog_new();

G_END_DECLS

#endif /*__AGS_QUIT_DIALOG_H__*/
