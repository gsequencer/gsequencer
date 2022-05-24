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

#ifndef __AGS_CROP_NOTE_DIALOG_H__
#define __AGS_CROP_NOTE_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_CROP_NOTE_DIALOG                (ags_crop_note_dialog_get_type())
#define AGS_CROP_NOTE_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CROP_NOTE_DIALOG, AgsCropNoteDialog))
#define AGS_CROP_NOTE_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CROP_NOTE_DIALOG, AgsCropNoteDialogClass))
#define AGS_IS_CROP_NOTE_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CROP_NOTE_DIALOG))
#define AGS_IS_CROP_NOTE_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CROP_NOTE_DIALOG))
#define AGS_CROP_NOTE_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CROP_NOTE_DIALOG, AgsCropNoteDialogClass))

#define AGS_CROP_NOTE_DIALOG_MAX_WIDTH (128 * 16)

typedef struct _AgsCropNoteDialog AgsCropNoteDialog;
typedef struct _AgsCropNoteDialogClass AgsCropNoteDialogClass;

struct _AgsCropNoteDialog
{
  GtkDialog dialog;

  guint connectable_flags;

  GtkCheckButton *absolute;
  
  GtkCheckButton *in_place;
  GtkCheckButton *do_resize;

  GtkSpinButton *crop_note;
  GtkSpinButton *padding_note;
};

struct _AgsCropNoteDialogClass
{
  GtkDialogClass dialog;
};

GType ags_crop_note_dialog_get_type(void);

AgsCropNoteDialog* ags_crop_note_dialog_new();

G_END_DECLS

#endif /*__AGS_CROP_NOTE_DIALOG_H__*/
