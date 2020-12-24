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

#ifndef __AGS_INPUT_EDITOR_H__
#define __AGS_INPUT_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_INPUT_EDITOR                (ags_input_editor_get_type())
#define AGS_INPUT_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INPUT_EDITOR, AgsInputEditor))
#define AGS_INPUT_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INPUT_EDITOR, AgsInputEditorClass))
#define AGS_IS_INPUT_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_INPUT_EDITOR))
#define AGS_IS_INPUT_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_INPUT_EDITOR))
#define AGS_INPUT_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INPUT_EDITOR, AgsInputEditorClass))

#define AGS_INPUT_EDITOR_DEFAULT_VERSION "2.0.0"
#define AGS_INPUT_EDITOR_DEFAULT_BUILD_ID "Sun Aug 26 18:34:53 UTC 2018"

typedef struct _AgsInputEditor AgsInputEditor;
typedef struct _AgsInputEditorClass AgsInputEditorClass;

typedef enum{
  AGS_INPUT_EDITOR_CONNECTED    = 1,
}AgsInputEditorFlags;

struct _AgsInputEditor
{
  GtkBox box;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  GtkComboBox *soundcard;
  GtkSpinButton *audio_channel;
};

struct _AgsInputEditorClass
{
  GtkBoxClass box;
};

GType ags_input_editor_get_type(void);

void ags_input_editor_check(AgsInputEditor *input_editor);

AgsInputEditor* ags_input_editor_new();

G_END_DECLS

#endif /*__AGS_INPUT_EDITOR_H__*/
