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

#ifndef __AGS_AUDIO_PREFERENCES_H__
#define __AGS_AUDIO_PREFERENCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_PREFERENCES                (ags_audio_preferences_get_type())
#define AGS_AUDIO_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_PREFERENCES, AgsAudioPreferences))
#define AGS_AUDIO_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_PREFERENCES, AgsAudioPreferencesClass))
#define AGS_IS_AUDIO_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_PREFERENCES))
#define AGS_IS_AUDIO_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_PREFERENCES))
#define AGS_AUDIO_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_PREFERENCES, AgsAudioPreferencesClass))

typedef struct _AgsAudioPreferences AgsAudioPreferences;
typedef struct _AgsAudioPreferencesClass AgsAudioPreferencesClass;

typedef enum{
  AGS_AUDIO_PREFERENCES_CONNECTED    = 1,
}AgsAudioPreferencesFlags;

struct _AgsAudioPreferences
{
  GtkBox box;

  guint flags;
  
  GtkBox *soundcard_editor;
  GtkButton *add;
  
  GtkComboBoxText *jack_driver;
  GtkButton *start_jack;
  GtkButton *stop_jack;
};

struct _AgsAudioPreferencesClass
{
  GtkBoxClass box;
};

GType ags_audio_preferences_get_type(void);

AgsAudioPreferences* ags_audio_preferences_new();

G_END_DECLS

#endif /*__AGS_AUDIO_PREFERENCES_H__*/
