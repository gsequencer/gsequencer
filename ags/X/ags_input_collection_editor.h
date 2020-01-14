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

#ifndef __AGS_INPUT_COLLECTION_EDITOR_H__
#define __AGS_INPUT_COLLECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_INPUT_COLLECTION_EDITOR                (ags_input_collection_editor_get_type())
#define AGS_INPUT_COLLECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INPUT_COLLECTION_EDITOR, AgsInputCollectionEditor))
#define AGS_INPUT_COLLECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INPUT_COLLECTION_EDITOR, AgsInputCollectionEditorClass))
#define AGS_IS_INPUT_COLLECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_INPUT_COLLECTION_EDITOR))
#define AGS_IS_INPUT_COLLECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_INPUT_COLLECTION_EDITOR))
#define AGS_INPUT_COLLECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INPUT_COLLECTION_EDITOR, AgsInputCollectionEditorClass))

typedef struct _AgsInputCollectionEditor AgsInputCollectionEditor;
typedef struct _AgsInputCollectionEditorClass AgsInputCollectionEditorClass;

typedef enum{
  AGS_INPUT_COLLECTION_EDITOR_CONNECTED    = 1,
}AgsInputCollectionEditorFlags;

struct _AgsInputCollectionEditor
{
  GtkTable table;

  guint flags;
  
  GType channel_type;

  GtkSpinButton *first_line;
  GtkSpinButton *count;
  
  GtkComboBoxText *soundcard;
  GtkSpinButton *audio_channel;
};

struct _AgsInputCollectionEditorClass
{
  GtkTableClass table;
};

GType ags_input_collection_editor_get_type();

void ags_input_collection_editor_check(AgsInputCollectionEditor *input_collection_editor);

AgsInputCollectionEditor* ags_input_collection_editor_new(GType channel_type);

G_END_DECLS

#endif /*__AGS_INPUT_COLLECTION_EDITOR_H__*/
