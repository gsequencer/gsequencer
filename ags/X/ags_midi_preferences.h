/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#define AGS_TYPE_MIDI_PREFERENCES                (ags_midi_preferences_get_type())
#define AGS_MIDI_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferences))
#define AGS_MIDI_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferencesClass))
#define AGS_IS_MIDI_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIDI_PREFERENCES))
#define AGS_IS_MIDI_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIDI_PREFERENCES))
#define AGS_MIDI_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_PREFERENCES, AgsMidiPreferencesClass))

typedef struct _AgsMidiPreferences AgsMidiPreferences;
typedef struct _AgsMidiPreferencesClass AgsMidiPreferencesClass;

typedef enum{
  AGS_MIDI_PREFERENCES_CONNECTED    = 1,
}AgsMidiPreferencesFlags;

struct _AgsMidiPreferences
{
  GtkVBox vbox;

  guint flags;
  
  GtkVBox *sequencer_editor;
  GtkButton *add;
};

struct _AgsMidiPreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_midi_preferences_get_type(void);

AgsMidiPreferences* ags_midi_preferences_new();

#endif /*__AGS_MIDI_PREFERENCES_H__*/
