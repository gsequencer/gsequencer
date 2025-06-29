/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_MIDI_CC_EDITOR_H__
#define __AGS_MIDI_CC_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CC_EDITOR                (ags_midi_cc_editor_get_type())
#define AGS_MIDI_CC_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_MIDI_CC_EDITOR, AgsMidiCCEditor))
#define AGS_MIDI_CC_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIDI_CC_EDITOR, AgsMidiCCEditorClass))
#define AGS_IS_MIDI_CC_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_CC_EDITOR))
#define AGS_IS_MIDI_CC_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_CC_EDITOR))
#define AGS_MIDI_CC_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_CC_EDITOR, AgsMidiCCEditorClass))

typedef struct _AgsMidiCCEditor AgsMidiCCEditor;
typedef struct _AgsMidiCCEditorClass AgsMidiCCEditorClass;

typedef enum{
  AGS_MIDI_CC_EDITOR_SHOW_MIDI_1_0     = 1,
  AGS_MIDI_CC_EDITOR_SHOW_MIDI_2_0     = 1 <<  1,
}AgsMidiCCEditorFlags;

struct _AgsMidiCCEditor
{
  GtkBox box;

  AgsMidiCCEditorFlags flags;
  AgsConnectableFlags connectable_flags;

  GtkWidget *parent_midi_cc_dialog;
  
  gchar **port;

  /* MIDI 1 */
  GtkBox *midi1_box;

  AgsRecallMidi1ControlChange midi1_control;

  GtkLabel *midi1_control_label;

  GtkDropDown *midi1_port_drop_down;

  GtkSpinButton *midi1_channel;
  
  /* MIDI 2 */
  GtkBox *midi2_box;
  
  AgsRecallMidi2ControlChange midi2_control;
  
  GtkLabel *midi2_control_label;

  GtkDropDown *midi2_port_drop_down;

  GtkSpinButton *midi2_group;

  GtkSpinButton *midi2_channel;

  GtkSpinButton *midi2_note;
};

struct _AgsMidiCCEditorClass
{
  GtkBoxClass box;
};

GType ags_midi_cc_editor_get_type(void);

gboolean ags_midi_cc_editor_test_flags(AgsMidiCCEditor *midi_cc_editor,
				       AgsMidiCCEditorFlags flags);
void ags_midi_cc_editor_set_flags(AgsMidiCCEditor *midi_cc_editor,
				  AgsMidiCCEditorFlags flags);
void ags_midi_cc_editor_unset_flags(AgsMidiCCEditor *midi_cc_editor,
				    AgsMidiCCEditorFlags flags);

void ags_midi_cc_editor_midi1_load_port(AgsMidiCCEditor *midi_cc_editor);
void ags_midi_cc_editor_midi2_load_port(AgsMidiCCEditor *midi_cc_editor);

GList* ags_midi_cc_editor_find_midi1_control(GList *midi_cc_editor,
					     gchar *midi1_control);
GList* ags_midi_cc_editor_find_midi2_control(GList *midi_cc_editor,
					     gchar *midi2_control);

/* instantiate */
AgsMidiCCEditor* ags_midi_cc_editor_new();

G_END_DECLS

#endif /*__AGS_MIDI_CC_EDITOR_H__*/
