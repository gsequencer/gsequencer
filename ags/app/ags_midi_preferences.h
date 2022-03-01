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

#ifndef __AGS_MIDI_PREFERENCES_H__
#define __AGS_MIDI_PREFERENCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_sequencer_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_PREFERENCES                (ags_midi_preferences_get_type())
#define AGS_MIDI_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferences))
#define AGS_MIDI_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferencesClass))
#define AGS_IS_MIDI_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIDI_PREFERENCES))
#define AGS_IS_MIDI_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIDI_PREFERENCES))
#define AGS_MIDI_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferencesClass))

typedef struct _AgsMidiPreferences AgsMidiPreferences;
typedef struct _AgsMidiPreferencesClass AgsMidiPreferencesClass;

struct _AgsMidiPreferences
{
  GtkBox box;

  guint connectable_flags;

  GList *sequencer_editor;
  
  GtkBox *sequencer_editor_box;

  GtkButton *add;
};

struct _AgsMidiPreferencesClass
{
  GtkBoxClass box;
};

GType ags_midi_preferences_get_type(void);

GList* ags_midi_preferences_get_sequencer_editor(AgsMidiPreferences *midi_preferences);
void ags_midi_preferences_add_sequencer_editor(AgsMidiPreferences *midi_preferences,
					       AgsSequencerEditor *sequencer_editor);
void ags_midi_preferences_remove_sequencer_editor(AgsMidiPreferences *midi_preferences,
						  AgsSequencerEditor *sequencer_editor);

AgsMidiPreferences* ags_midi_preferences_new();

G_END_DECLS

#endif /*__AGS_MIDI_PREFERENCES_H__*/
