/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_PAD_EDITOR_H__
#define __AGS_PAD_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PAD_EDITOR                (ags_pad_editor_get_type())
#define AGS_PAD_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD_EDITOR, AgsPadEditor))
#define AGS_PAD_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))
#define AGS_IS_PAD_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PAD_EDITOR))
#define AGS_IS_PAD_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PAD_EDITOR))
#define AGS_PAD_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))

#define AGS_PAD_EDITOR_DEFAULT_VERSION "0.4.2"
#define AGS_PAD_EDITOR_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36"

typedef struct _AgsPadEditor AgsPadEditor;
typedef struct _AgsPadEditorClass AgsPadEditorClass;

typedef enum{
  AGS_PAD_EDITOR_CONNECTED   = 1,
}AgsPadEditorFlags;

struct _AgsPadEditor
{
  GtkBox box;

  guint flags;
  
  gchar *version;
  gchar *build_id;
  
  AgsChannel *pad;

  guint editor_type_count;
  GType *editor_type;
  
  GtkExpander *line_editor_expander;
  GtkBox *line_editor;
};

struct _AgsPadEditorClass
{
  GtkBoxClass box;
};

GType ags_pad_editor_get_type(void);

void ags_pad_editor_set_channel(AgsPadEditor *pad_editor, AgsChannel *channel);

AgsPadEditor* ags_pad_editor_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_PAD_EDITOR_H__*/
