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

#ifndef __AGS_MIDI_DIALOG_H__
#define __AGS_MIDI_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_DIALOG                (ags_midi_dialog_get_type())
#define AGS_MIDI_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_DIALOG, AgsMidiDialog))
#define AGS_MIDI_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_DIALOG, AgsMidiDialogClass))
#define AGS_IS_MIDI_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIDI_DIALOG))
#define AGS_IS_MIDI_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIDI_DIALOG))
#define AGS_MIDI_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_DIALOG, AgsMidiDialogClass))

#define AGS_MIDI_DIALOG_DEFAULT_VERSION "6.10.0"
#define AGS_MIDI_DIALOG_DEFAULT_BUILD_ID "Tue Apr 30 13:11:09 UTC 2024"

typedef struct _AgsMidiDialog AgsMidiDialog;
typedef struct _AgsMidiDialogClass AgsMidiDialogClass;

typedef enum{
  AGS_MIDI_DIALOG_IO_OPTIONS     = 1 <<  1,
  AGS_MIDI_DIALOG_MAPPING        = 1 <<  2,
  AGS_MIDI_DIALOG_DEVICE         = 1 <<  3,
}AgsMidiDialogFlags;

struct _AgsMidiDialog
{
  GtkWindow window;

  guint flags;
  guint connectable_flags;
  
  gchar *version;
  gchar *build_id;
  
  AgsMachine *machine;

  GtkBox *io_options;
  
  GtkSpinButton *midi_group;
  GtkSpinButton *midi_channel;
  
  GtkCheckButton *playback;
  GtkCheckButton *record;

  GtkBox *mapping;
  
  GtkSpinButton *audio_start;
  GtkSpinButton *audio_end;
  
  GtkSpinButton *midi_start;
  GtkSpinButton *midi_end;

  GtkBox *device;
  
  GtkComboBox *midi_device;

  GtkBox *action_area;

  GtkButton *activate_button;
};

struct _AgsMidiDialogClass
{
  GtkWindowClass window;

  void (*response)(AgsMidiDialog *midi_dialog,
		   gint response_id);
};

GType ags_midi_dialog_get_type(void);

void ags_midi_dialog_load_sequencers(AgsMidiDialog *midi_dialog);

void ags_midi_dialog_response(AgsMidiDialog *midi_dialog,
			      gint response_id);

AgsMidiDialog* ags_midi_dialog_new(GtkWindow *transient_for,
				   AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_MIDI_DIALOG_H__*/
