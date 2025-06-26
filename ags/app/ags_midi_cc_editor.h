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

struct _AgsMidiCCEditor
{
  GtkBox box;

  guint flags;
  AgsConnectableFlags connectable_flags;

  GtkWidget *parent_midi_cc_dialog;
  
  AgsRecallMidi2ControlChange control;
  GtkLabel *control_label;

  gchar **port;
  GtkDropDown *port_drop_down;

  GtkSpinButton *midi_group;

  GtkSpinButton *midi_channel;

  GtkSpinButton *midi_note;
};

struct _AgsMidiCCEditorClass
{
  GtkBoxClass box;
};

GType ags_midi_cc_editor_get_type(void);

void ags_midi_cc_editor_load_port(AgsMidiCCEditor *midi_cc_editor);

GList* ags_midi_cc_editor_find_control(GList *midi_cc_editor,
				       gchar *control);

/* instantiate */
AgsMidiCCEditor* ags_midi_cc_editor_new();

G_END_DECLS

#endif /*__AGS_MIDI_CC_EDITOR_H__*/
