/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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
  AGS_MIDI_CC_EDITOR_BANK_SELECT, 
  AGS_MIDI_CC_EDITOR_MODULATION_WHEEL,
  AGS_MIDI_CC_EDITOR_BREATH_CONTROLLER,
  AGS_MIDI_CC_EDITOR_FOOT_CONTROLLER,
  AGS_MIDI_CC_EDITOR_PORTAMENTO_TIME,
  AGS_MIDI_CC_EDITOR_DATA_ENTRY,
  AGS_MIDI_CC_EDITOR_CHANNEL_VOLUME,
  AGS_MIDI_CC_EDITOR_BALANCE,
  AGS_MIDI_CC_EDITOR_PAN,
  AGS_MIDI_CC_EDITOR_EXPRESSION_CONTROLLER,
  AGS_MIDI_CC_EDITOR_EFFECT_CONTROL_1,
  AGS_MIDI_CC_EDITOR_EFFECT_CONTROL_2,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_1,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_2,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_3,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_4,
  AGS_MIDI_CC_EDITOR_DAMPER_PEDAL,
  AGS_MIDI_CC_EDITOR_PORTAMENTO,
  AGS_MIDI_CC_EDITOR_SOSTENUTO,
  AGS_MIDI_CC_EDITOR_SOFT_PEDAL,
  AGS_MIDI_CC_EDITOR_LEGATO_FOOTSWITCH,
  AGS_MIDI_CC_EDITOR_HOLD_2,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_1,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_2,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_3,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_4,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_5,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_6,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_7,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_8,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_9,
  AGS_MIDI_CC_EDITOR_SOUND_CONTROLLER_10,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_5,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_6,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_7,
  AGS_MIDI_CC_EDITOR_GENERAL_PURPOSE_CONTROLLER_8,
  AGS_MIDI_CC_EDITOR_PORTAMENTO_CONTROL,  
}AgsMidiCCEditorControl;

struct _AgsMidiCCEditor
{
  GtkBox box;

  guint flags;
  guint connectable_flags;

  guint control;
  
  GtkLabel *label;
  GtkDropDown *port;
  GtkCheckButton *enabled;
};

struct _AgsMidiCCEditorClass
{
  GtkBoxClass box;
};

GType ags_midi_cc_editor_get_type(void);

AgsMidiCCEditor* ags_midi_cc_editor_new();

G_END_DECLS

#endif /*__AGS_MIDI_CC_EDITOR_H__*/
