/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOUNDCARD_EDITOR                (ags_soundcard_editor_get_type())
#define AGS_SOUNDCARD_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditor))
#define AGS_SOUNDCARD_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditorClass))
#define AGS_IS_SOUNDCARD_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOUNDCARD_EDITOR))
#define AGS_IS_SOUNDCARD_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOUNDCARD_EDITOR))
#define AGS_SOUNDCARD_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SOUNDCARD_EDITOR, AgsSoundcardEditorClass))

typedef struct _AgsSoundcardEditor AgsSoundcardEditor;
typedef struct _AgsSoundcardEditorClass AgsSoundcardEditorClass;

typedef enum{
  AGS_SOUNDCARD_EDITOR_CONNECTED     = 1,
  AGS_SOUNDCARD_EDITOR_BLOCK_RESET   = 1 <<  1,
  AGS_SOUNDCARD_EDITOR_BLOCK_BACKEND = 1 <<  2,
  AGS_SOUNDCARD_EDITOR_BLOCK_CARD    = 1 <<  3,
  AGS_SOUNDCARD_EDITOR_BLOCK_ADD     = 1 <<  4,
  AGS_SOUNDCARD_EDITOR_BLOCK_LOAD    = 1 <<  5,
}AgsSoundcardEditorFlags;

struct _AgsSoundcardEditor
{
  GtkBox box;

  guint flags;
  
  GObject *soundcard;
  GObject *soundcard_thread;
  
  GtkComboBoxText *backend;  
  GtkComboBoxText *card;
  
  GtkBox *port_hbox;
  GtkButton *add_port;
  GtkButton *remove_port;

  GtkComboBoxText *capability;
  
  GtkSpinButton *audio_channels;
  GtkSpinButton *samplerate;
  GtkSpinButton *buffer_size;
  GtkComboBoxText *format;

  GtkCheckButton *use_cache;
  GtkSpinButton *cache_buffer_size;

  GtkLabel *wasapi_share_mode_label;
  GtkComboBoxText *wasapi_share_mode;

  GtkLabel *wasapi_buffer_size_label;
  GtkSpinButton *wasapi_buffer_size;
  
  GtkButton *remove;
};

struct _AgsSoundcardEditorClass
{
  GtkBoxClass box;
};

GType ags_soundcard_editor_get_type(void);

void ags_soundcard_editor_add_port(AgsSoundcardEditor *soundcard_editor,
				   gchar *device);
void ags_soundcard_editor_remove_port(AgsSoundcardEditor *soundcard_editor,
				      gchar *device);

void ags_soundcard_editor_add_soundcard(AgsSoundcardEditor *soundcard_editor,
					GObject *soundcard);
void ags_soundcard_editor_remove_soundcard(AgsSoundcardEditor *soundcard_editor,
					   GObject *soundcard);

void ags_soundcard_editor_load_core_audio_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_pulse_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_jack_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_wasapi_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_alsa_card(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_load_oss_card(AgsSoundcardEditor *soundcard_editor);

void ags_soundcard_editor_show_wasapi_control(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_hide_wasapi_control(AgsSoundcardEditor *soundcard_editor);

AgsSoundcardEditor* ags_soundcard_editor_new();

G_END_DECLS

#endif /*__AGS_SOUNDCARD_EDITOR_H__*/
