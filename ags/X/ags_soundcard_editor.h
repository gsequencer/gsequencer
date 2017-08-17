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

#ifndef __AGS_SOUNDCARD_EDITOR_H__
#define __AGS_SOUNDCARD_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_SOUNDCARD_EDITOR                (ags_soundcard_editor_get_type())
#define AGS_SOUNDCARD_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditor))
#define AGS_SOUNDCARD_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditorClass))
#define AGS_IS_SOUNDCARD_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOUNDCARD_EDITOR))
#define AGS_IS_SOUNDCARD_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOUNDCARD_EDITOR))
#define AGS_SOUNDCARD_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditorClass))

typedef struct _AgsSoundcardEditor AgsSoundcardEditor;
typedef struct _AgsSoundcardEditorClass AgsSoundcardEditorClass;

typedef enum{
  AGS_SOUNDCARD_EDITOR_CONNECTED    = 1,
}AgsSoundcardEditorFlags;

struct _AgsSoundcardEditor
{
  GtkVBox vbox;

  guint flags;
  
  GObject *soundcard;
  GObject *soundcard_thread;
  
  GtkComboBoxText *backend;  
  GtkComboBoxText *card;
  
  GtkHBox *sink_hbox;
  GtkButton *add_sink;
  GtkButton *remove_sink;
  
  GtkSpinButton *audio_channels;
  GtkSpinButton *samplerate;
  GtkSpinButton *buffer_size;
  GtkComboBoxText *format;

  GtkButton *remove;
};

struct _AgsSoundcardEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_soundcard_editor_get_type(void);

void ags_soundcard_editor_add_sink(AgsSoundcardEditor *soundcard_editor,
				   gchar *device);
void ags_soundcard_editor_remove_sink(AgsSoundcardEditor *soundcard_editor,
				      gchar *device);

void ags_soundcard_editor_add_soundcard(AgsSoundcardEditor *soundcard_editor,
					GObject *soundcard);
void ags_soundcard_editor_remove_soundcard(AgsSoundcardEditor *soundcard_editor,
					   GObject *soundcard);

void ags_soundcard_editor_load_pulse_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_jack_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_alsa_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_oss_card(AgsSoundcardEditor *soundcard_editor);

AgsSoundcardEditor* ags_soundcard_editor_new();

#endif /*__AGS_SOUNDCARD_EDITOR_H__*/
