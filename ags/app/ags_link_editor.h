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

#ifndef __AGS_LINK_EDITOR_H__
#define __AGS_LINK_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_pcm_file_chooser_dialog.h>

G_BEGIN_DECLS

#define AGS_TYPE_LINK_EDITOR                (ags_link_editor_get_type())
#define AGS_LINK_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LINK_EDITOR, AgsLinkEditor))
#define AGS_LINK_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LINK_EDITOR, AgsLinkEditorClass))
#define AGS_IS_LINK_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINK_EDITOR))
#define AGS_IS_LINK_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINK_EDITOR))
#define AGS_LINK_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINK_EDITOR, AgsLinkEditorClass))

typedef struct _AgsLinkEditor AgsLinkEditor;
typedef struct _AgsLinkEditorClass AgsLinkEditorClass;

typedef enum{
  AGS_LINK_EDITOR_CONNECTED               = 1,
  AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE  = 1 <<  1,
  AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER      = 1 <<  2,
}AgsLinkEditorFlags;

struct _AgsLinkEditor
{
  GtkBox box;

  guint flags;

  GtkComboBox *combo;
  GtkSpinButton *spin_button;

  AgsAudioFile *audio_file;

  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;
};

struct _AgsLinkEditorClass
{
  GtkBoxClass box;
};

GType ags_link_editor_get_type(void);

AgsLinkEditor* ags_link_editor_new();

G_END_DECLS

#endif /*__AGS_LINK_EDITOR_H__*/
