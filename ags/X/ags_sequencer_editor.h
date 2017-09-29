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

#ifndef __AGS_SEQUENCER_EDITOR_H__
#define __AGS_SEQUENCER_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_SEQUENCER_EDITOR                (ags_sequencer_editor_get_type())
#define AGS_SEQUENCER_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEQUENCER_EDITOR, AgsSequencerEditor))
#define AGS_SEQUENCER_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SEQUENCER_EDITOR, AgsSequencerEditorClass))
#define AGS_IS_SEQUENCER_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SEQUENCER_EDITOR))
#define AGS_IS_SEQUENCER_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SEQUENCER_EDITOR))
#define AGS_SEQUENCER_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SEQUENCER_EDITOR, AgsSequencerEditorClass))

typedef struct _AgsSequencerEditor AgsSequencerEditor;
typedef struct _AgsSequencerEditorClass AgsSequencerEditorClass;

typedef enum{
  AGS_SEQUENCER_EDITOR_CONNECTED    = 1,
}AgsSequencerEditorFlags;

struct _AgsSequencerEditor
{
  GtkVBox vbox;

  guint flags;
  
  GObject *sequencer;
  GObject *sequencer_thread;
  
  GtkComboBoxText *backend;
  
  GtkComboBoxText *card;
  
  GtkHBox *jack_hbox;
  GtkButton *add_jack;
  GtkButton *remove_jack;

  GtkButton *remove;
};

struct _AgsSequencerEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_sequencer_editor_get_type(void);

void ags_sequencer_editor_add_jack(AgsSequencerEditor *sequencer_editor,
				   gchar *device);
void ags_sequencer_editor_remove_jack(AgsSequencerEditor *sequencer_editor,
				      gchar *device);

void ags_sequencer_editor_add_sequencer(AgsSequencerEditor *sequencer_editor,
					GObject *sequencer);
void ags_sequencer_editor_remove_sequencer(AgsSequencerEditor *sequencer_editor,
					   GObject *sequencer);

void ags_sequencer_editor_load_jack_card(AgsSequencerEditor *sequencer_editor);
void ags_sequencer_editor_load_alsa_card(AgsSequencerEditor *sequencer_editor);
void ags_sequencer_editor_load_oss_card(AgsSequencerEditor *sequencer_editor);

AgsSequencerEditor* ags_sequencer_editor_new();

#endif /*__AGS_SEQUENCER_EDITOR_H__*/
