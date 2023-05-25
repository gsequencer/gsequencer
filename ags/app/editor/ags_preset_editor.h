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

#ifndef __AGS_PRESET_EDITOR_H__
#define __AGS_PRESET_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PRESET_EDITOR                (ags_preset_editor_get_type())
#define AGS_PRESET_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PRESET_EDITOR, AgsPresetEditor))
#define AGS_PRESET_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PRESET_EDITOR, AgsPresetEditorClass))
#define AGS_IS_PRESET_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PRESET_EDITOR))
#define AGS_IS_PRESET_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PRESET_EDITOR))
#define AGS_PRESET_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PRESET_EDITOR, AgsPresetEditorClass))

#define AGS_PRESET_EDITOR_DEFAULT_VERSION "4.1.0"
#define AGS_PRESET_EDITOR_DEFAULT_BUILD_ID "Tue May 31 07:48:20 AM UTC 2022"

#define AGS_PRESET_EDITOR_TREE_VIEW_HEIGHT_REQUEST (600)
#define AGS_PRESET_EDITOR_TREE_VIEW_WIDTH_REQUEST (1000)

typedef struct _AgsPresetEditor AgsPresetEditor;
typedef struct _AgsPresetEditorClass AgsPresetEditorClass;

struct _AgsPresetEditor
{
  GtkBox box;

  guint flags;
  guint connectable_flags;

  gchar *version;
  gchar *build_id;

  GtkEntry *filename;

  GtkButton *load;
  GtkButton *open_preset;
  GtkButton *save_preset;
  GtkButton *apply_preset;

  GtkTreeView *preset_tree_view;
  GtkCellRenderer *value_renderer;
};

struct _AgsPresetEditorClass
{
  GtkBoxClass box;
};

GType ags_preset_editor_get_type(void);

void ags_preset_editor_save_preset(AgsPresetEditor *preset_editor);

void ags_preset_editor_open_preset(AgsPresetEditor *preset_editor,
				   gchar *filename);

void ags_preset_editor_apply_preset(AgsPresetEditor *preset_editor);

AgsPresetEditor* ags_preset_editor_new();

G_END_DECLS

#endif /*__AGS_PRESET_EDITOR_H__*/
