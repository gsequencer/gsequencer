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

#ifndef __AGS_PRESET_DIALOG_H__
#define __AGS_PRESET_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_preset_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_PRESET_DIALOG                (ags_preset_dialog_get_type())
#define AGS_PRESET_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PRESET_DIALOG, AgsPresetDialog))
#define AGS_PRESET_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PRESET_DIALOG, AgsPresetDialogClass))
#define AGS_IS_PRESET_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PRESET_DIALOG))
#define AGS_IS_PRESET_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PRESET_DIALOG))
#define AGS_PRESET_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PRESET_DIALOG, AgsPresetDialogClass))

#define AGS_PRESET_DIALOG_DEFAULT_VERSION "4.1.0"
#define AGS_PRESET_DIALOG_DEFAULT_BUILD_ID "Tue May 31 07:48:20 AM UTC 2022"

typedef struct _AgsPresetDialog AgsPresetDialog;
typedef struct _AgsPresetDialogClass AgsPresetDialogClass;

struct _AgsPresetDialog
{
  GtkDialog dialog;

  guint flags;
  guint connectable_flags;
  
  gchar *version;
  gchar *build_id;
  
  AgsMachine *machine;

  GtkScrolledWindow *preset_editor_scrolled_window;
  AgsPresetEditor *preset_editor;

  GtkButton *ok;
};

struct _AgsPresetDialogClass
{
  GtkDialogClass dialog;
};

GType ags_preset_dialog_get_type(void);

AgsPresetDialog* ags_preset_dialog_new(gchar *title,
				       GtkWindow *transient_for,
				       AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_PRESET_DIALOG_H__*/
