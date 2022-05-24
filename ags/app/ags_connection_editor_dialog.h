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

#ifndef __AGS_CONNECTION_EDITOR_DIALOG_H__
#define __AGS_CONNECTION_EDITOR_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_connection_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONNECTION_EDITOR_DIALOG                (ags_connection_editor_dialog_get_type())
#define AGS_CONNECTION_EDITOR_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_EDITOR_DIALOG, AgsConnectionEditorDialog))
#define AGS_CONNECTION_EDITOR_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_EDITOR_DIALOG, AgsConnectionEditorDialogClass))
#define AGS_IS_CONNECTION_EDITOR_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_EDITOR_DIALOG))
#define AGS_IS_CONNECTION_EDITOR_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_EDITOR_DIALOG))
#define AGS_CONNECTION_EDITOR_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONNECTION_EDITOR_DIALOG, AgsConnectionEditorDialogClass))

typedef struct _AgsConnectionEditorDialog AgsConnectionEditorDialog;
typedef struct _AgsConnectionEditorDialogClass AgsConnectionEditorDialogClass;

struct _AgsConnectionEditorDialog
{
  GtkDialog dialog;

  AgsConnectionEditor *connection_editor;
};

struct _AgsConnectionEditorDialogClass
{
  GtkDialogClass dialog;
};

GType ags_connection_editor_dialog_get_type(void);

AgsConnectionEditorDialog* ags_connection_editor_dialog_new(gchar *title,
							    GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_CONNECTION_EDITOR_DIALOG_H__*/
