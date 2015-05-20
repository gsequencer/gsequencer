/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_midi_import_wizard.h>
#include <ags/X/ags_midi_import_wizard_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

void ags_midi_import_wizard_class_init(AgsMidiImportWizardClass *midi_import_wizard);
void ags_midi_import_wizard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_import_wizard_init(AgsMidiImportWizard *midi_import_wizard);
void ags_midi_import_wizard_connect(AgsConnectable *connectable);
void ags_midi_import_wizard_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_midi_import_wizard
 * @short_description: pack pad editors.
 * @title: AgsMidiImportWizard
 * @section_id:
 * @include: ags/X/ags_midi_import_wizard.h
 *
 * #AgsMidiImportWizard is a wizard to import midi files and do track mapping..
 */

static guint midi_import_wizard_signals[LAST_SIGNAL];

GType
ags_midi_import_wizard_get_type(void)
{
  static GType ags_type_midi_import_wizard = 0;

  if(!ags_type_midi_import_wizard){
    static const GTypeInfo ags_midi_import_wizard_info = {
      sizeof (AgsMidiImportWizardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_import_wizard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiImportWizard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_import_wizard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_import_wizard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_import_wizard = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsMidiImportWizard\0", &ags_midi_import_wizard_info,
							 0);

    g_type_add_interface_static(ags_type_midi_import_wizard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_midi_import_wizard);
}

void
ags_midi_import_wizard_class_init(AgsMidiImportWizardClass *midi_import_wizard)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) midi_import_wizard;
}

void
ags_midi_import_wizard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_import_wizard_connect;
  connectable->disconnect = ags_midi_import_wizard_disconnect;
}

void
ags_midi_import_wizard_init(AgsMidiImportWizard *midi_import_wizard)
{
}

void
ags_midi_import_wizard_connect(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);
}

void
ags_midi_import_wizard_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

/**
 * ags_midi_import_wizard_new:
 * @parent: the parent #AgsWindow
 *
 * Creates an #AgsMidiImportWizard
 *
 * Returns: a new #AgsMidiImportWizard
 *
 * Since: 0.4.3
 */
AgsMidiImportWizard*
ags_midi_import_wizard_new(GtkWidget *parent)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = (AgsMidiImportWizard *) g_object_new(AGS_TYPE_MIDI_IMPORT_WIZARD,
							    NULL);

  midi_import_wizard->parent = parent;
  
  return(midi_import_wizard);
}
