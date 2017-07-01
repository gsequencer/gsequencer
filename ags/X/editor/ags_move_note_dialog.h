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

#ifndef __AGS_MOVE_NOTE_DIALOG_H__
#define __AGS_MOVE_NOTE_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_MOVE_NOTE_DIALOG                (ags_move_note_dialog_get_type())
#define AGS_MOVE_NOTE_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MOVE_NOTE_DIALOG, AgsMoveNoteDialog))
#define AGS_MOVE_NOTE_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MOVE_NOTE_DIALOG, AgsMoveNoteDialogClass))
#define AGS_IS_MOVE_NOTE_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MOVE_NOTE_DIALOG))
#define AGS_IS_MOVE_NOTE_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MOVE_NOTE_DIALOG))
#define AGS_MOVE_NOTE_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MOVE_NOTE_DIALOG, AgsMoveNoteDialogClass))

#define AGS_MOVE_NOTE_DIALOG_MAX_X (16 * 19200)
#define AGS_MOVE_NOTE_DIALOG_MAX_Y (1024)

typedef struct _AgsMoveNoteDialog AgsMoveNoteDialog;
typedef struct _AgsMoveNoteDialogClass AgsMoveNoteDialogClass;

typedef enum{
  AGS_MOVE_NOTE_DIALOG_CONNECTED   = 1,
}AgsMoveNoteDialogFlags;

struct _AgsMoveNoteDialog
{
  GtkDialog dialog;

  guint flags;

  GObject *application_context;
  
  GtkWidget *main_window;

  GtkRadioButton *relative;
  GtkRadioButton *absolute;

  GtkSpinButton *move_x;
  GtkSpinButton *move_y;
};

struct _AgsMoveNoteDialogClass
{
  GtkDialogClass dialog;
};

GType ags_move_note_dialog_get_type(void);

AgsMoveNoteDialog* ags_move_note_dialog_new(GtkWidget *main_window);

#endif /*__AGS_MOVE_NOTE_DIALOG_H__*/
