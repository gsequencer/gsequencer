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

#ifndef __AGS_MIDI_CC_DIALOG_H__
#define __AGS_MIDI_CC_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CC_DIALOG                (ags_midi_cc_dialog_get_type())
#define AGS_MIDI_CC_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_CC_DIALOG, AgsMidiCCDialog))
#define AGS_MIDI_CC_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_CC_DIALOG, AgsMidiCCDialogClass))
#define AGS_IS_MIDI_CC_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MIDI_CC_DIALOG))
#define AGS_IS_MIDI_CC_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIDI_CC_DIALOG))
#define AGS_MIDI_CC_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_CC_DIALOG, AgsMidiCCDialogClass))

#define AGS_MIDI_CC_DIALOG_DEFAULT_VERSION "8.0.0"
#define AGS_MIDI_CC_DIALOG_DEFAULT_BUILD_ID "Wed Jun 25 21:35:26 UTC 2025"

typedef struct _AgsMidiCCDialog AgsMidiCCDialog;
typedef struct _AgsMidiCCDialogClass AgsMidiCCDialogClass;

struct _AgsMidiCCDialog
{
  GtkWindow window;

  guint flags;
  AgsConnectableFlags connectable_flags;
  
  gchar *version;
  gchar *build_id;
  
  AgsMachine *machine;

  GtkBox *midi_cc_editor;

  GtkBox *action_area;

  GtkButton *add_button;
  GtkButton *activate_button;
};

struct _AgsMidiCCDialogClass
{
  GtkWindowClass window;

  void (*response)(AgsMidiCCDialog *midi_cc_dialog,
		   gint response_id);
};

GType ags_midi_cc_dialog_get_type(void);

void ags_midi_cc_dialog_response(AgsMidiCCDialog *midi_cc_dialog,
				 gint response_id);

AgsMidiCCDialog* ags_midi_cc_dialog_new(GtkWindow *transient_for,
					AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_MIDI_CC_DIALOG_H__*/
