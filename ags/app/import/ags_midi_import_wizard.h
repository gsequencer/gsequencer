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

#ifndef __AGS_MIDI_IMPORT_WIZARD_H__
#define __AGS_MIDI_IMPORT_WIZARD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_IMPORT_WIZARD                (ags_midi_import_wizard_get_type())
#define AGS_MIDI_IMPORT_WIZARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_IMPORT_WIZARD, AgsMidiImportWizard))
#define AGS_MIDI_IMPORT_WIZARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_IMPORT_WIZARD, AgsMidiImportWizardClass))
#define AGS_IS_MIDI_IMPORT_WIZARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_IMPORT_WIZARD))
#define AGS_IS_MIDI_IMPORT_WIZARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_IMPORT_WIZARD))
#define AGS_MIDI_IMPORT_WIZARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_IMPORT_WIZARD, AgsMidiImportWizardClass))

typedef struct _AgsMidiImportWizard AgsMidiImportWizard;
typedef struct _AgsMidiImportWizardClass AgsMidiImportWizardClass;

typedef enum{
  AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER       = 1,
  AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION   = 1 << 1,
}AgsMidiImportWizardFlags;

struct _AgsMidiImportWizard
{
  GtkWindow window;

  guint flags;
  guint connectable_flags;
  
  GtkWidget *file_widget;
  GtkWidget *track_collection;

  GtkButton *prev_button;
  GtkButton *next_button;

  GtkButton *activate_button;
};

struct _AgsMidiImportWizardClass
{
  GtkWindowClass window;

  void (*response)(AgsMidiImportWizard *midi_import,
		   gint response_id);
};

GType ags_midi_import_wizard_get_type(void);

gboolean ags_midi_import_wizard_test_flags(AgsMidiImportWizard *midi_import_wizard,
					   guint flags);
void ags_midi_import_wizard_set_flags(AgsMidiImportWizard *midi_import_wizard,
				      guint flags);
void ags_midi_import_wizard_unset_flags(AgsMidiImportWizard *midi_import_wizard,
					guint flags);

void ags_midi_import_wizard_response(AgsMidiImportWizard *midi_import_wizard,
				     gint response_id);

AgsMidiImportWizard* ags_midi_import_wizard_new();

G_END_DECLS

#endif /*__AGS_MIDI_IMPORT_WIZARD_H__*/
