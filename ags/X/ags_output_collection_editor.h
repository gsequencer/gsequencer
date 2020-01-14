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

#ifndef __AGS_OUTPUT_COLLECTION_EDITOR_H__
#define __AGS_OUTPUT_COLLECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_OUTPUT_COLLECTION_EDITOR                (ags_output_collection_editor_get_type())
#define AGS_OUTPUT_COLLECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OUTPUT_COLLECTION_EDITOR, AgsOutputCollectionEditor))
#define AGS_OUTPUT_COLLECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OUTPUT_COLLECTION_EDITOR, AgsOutputCollectionEditorClass))
#define AGS_IS_OUTPUT_COLLECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OUTPUT_COLLECTION_EDITOR))
#define AGS_IS_OUTPUT_COLLECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OUTPUT_COLLECTION_EDITOR))
#define AGS_OUTPUT_COLLECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OUTPUT_COLLECTION_EDITOR, AgsOutputCollectionEditorClass))

typedef struct _AgsOutputCollectionEditor AgsOutputCollectionEditor;
typedef struct _AgsOutputCollectionEditorClass AgsOutputCollectionEditorClass;

typedef enum{
  AGS_OUTPUT_COLLECTION_EDITOR_CONNECTED    = 1,
}AgsOutputCollectionEditorFlags;

struct _AgsOutputCollectionEditor
{
  GtkTable table;

  guint flags;
  
  GType channel_type;

  GtkSpinButton *first_line;
  GtkSpinButton *count;
  
  GtkComboBoxText *soundcard;
  GtkSpinButton *audio_channel;
};

struct _AgsOutputCollectionEditorClass
{
  GtkTableClass table;
};

GType ags_output_collection_editor_get_type();

void ags_output_collection_editor_check(AgsOutputCollectionEditor *output_collection_editor);

AgsOutputCollectionEditor* ags_output_collection_editor_new(GType channel_type);

G_END_DECLS

#endif /*__AGS_OUTPUT_COLLECTION_EDITOR_H__*/
