/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_MIDI_EXPORT_WIZARD_H__
#define __AGS_MIDI_EXPORT_WIZARD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/midi/ags_midi_builder.h>

#define AGS_TYPE_MIDI_EXPORT_WIZARD                (ags_midi_export_wizard_get_type())
#define AGS_MIDI_EXPORT_WIZARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_EXPORT_WIZARD, AgsMidiExportWizard))
#define AGS_MIDI_EXPORT_WIZARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_EXPORT_WIZARD, AgsMidiExportWizardClass))
#define AGS_IS_MIDI_EXPORT_WIZARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_EXPORT_WIZARD))
#define AGS_IS_MIDI_EXPORT_WIZARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_EXPORT_WIZARD))
#define AGS_MIDI_EXPORT_WIZARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIDI_EXPORT_WIZARD, AgsMidiExportWizardClass))

#define AGS_MIDI_EXPORT_WIZARD_DEFAULT_FILENAME "out.mid\0"

#define AGS_MIDI_EXPORT_WIZARD_DEFAULT_TIMES (30)
#define AGS_MIDI_EXPORT_WIZARD_DEFAULT_CLICKS (4)
#define AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION (0xe250)

#define AGS_MIDI_EXPORT_WIZARD_DEFAULT_PULSE_UNIT (16.0 * AGS_SOUNDCARD_DEFAULT_BPM / 60.0 * 1.0 / (AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION >> 8) / (0xff & AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION) * 1000000.0)

typedef struct _AgsMidiExportWizard AgsMidiExportWizard;
typedef struct _AgsMidiExportWizardClass AgsMidiExportWizardClass;

typedef enum{
  AGS_MIDI_EXPORT_WIZARD_CONNECTED                 = 1,
  AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION   = 1 << 1,
  AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER         = 1 << 2,
}AgsMidiExportWizardFlags;

struct _AgsMidiExportWizard
{
  GtkDialog dialog;

  guint flags;

  GObject *application_context;

  GtkWidget *main_window;

  AgsMidiBuilder *midi_builder;
  guint pulse_unit;
  
  GtkWidget *machine_collection;
  GtkWidget *file_chooser;
};

struct _AgsMidiExportWizardClass
{
  GtkDialogClass dialog;
};

GType ags_midi_export_wizard_get_type(void);

AgsMidiExportWizard* ags_midi_export_wizard_new();

#endif /*__AGS_MIDI_EXPORT_WIZARD_H__*/
