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

#ifndef __AGS_MACHINE_SELECTION_H__
#define __AGS_MACHINE_SELECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_window.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_SELECTION                (ags_machine_selection_get_type())
#define AGS_MACHINE_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_SELECTION, AgsMachineSelection))
#define AGS_MACHINE_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_SELECTION, AgsMachineSelectionClass))
#define AGS_IS_MACHINE_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_SELECTION))
#define AGS_IS_MACHINE_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_SELECTION))
#define AGS_MACHINE_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MACHINE_SELECTION, AgsMachineSelectionClass))

#define AGS_MACHINE_SELECTION_INDEX "ags-machine-selection-index"

typedef struct _AgsMachineSelection AgsMachineSelection;
typedef struct _AgsMachineSelectionClass AgsMachineSelectionClass;

typedef enum{
  AGS_MACHINE_SELECTION_EDIT_NOTATION    = 1,
  AGS_MACHINE_SELECTION_EDIT_SHEET       = 1 <<  1,
  AGS_MACHINE_SELECTION_EDIT_AUTOMATION  = 1 <<  2,
  AGS_MACHINE_SELECTION_EDIT_WAVE        = 1 <<  3,
}AgsMachineSelectionEdit;

struct _AgsMachineSelection
{
  GtkDialog dialog;

  guint flags;
  guint connectable_flags;
  guint edit;
  
  GList *radio_button;
};

struct _AgsMachineSelectionClass
{
  GtkDialogClass dialog;
};

GType ags_machine_selection_get_type(void);

void ags_machine_selection_set_edit(AgsMachineSelection *machine_selection, guint edit);

GList* ags_machine_selection_get_radio_button(AgsMachineSelection *machine_selection);
void ags_machine_selection_add_radio_button(AgsMachineSelection *machine_selection,
					    GtkCheckButton *radio_button);
void ags_machine_selection_remove_radio_button(AgsMachineSelection *machine_selection,
					       GtkCheckButton *radio_button);

void ags_machine_selection_load_defaults(AgsMachineSelection *machine_selection);

AgsMachineSelection* ags_machine_selection_new(AgsWindow *window);

G_END_DECLS

#endif /*__AGS_MACHINE_SELECTION_H__*/
