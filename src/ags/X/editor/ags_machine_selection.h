/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_MACHINE_SELECTION_H__
#define __AGS_MACHINE_SELECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_window.h>

#define AGS_TYPE_MACHINE_SELECTION                (ags_machine_selection_get_type())
#define AGS_MACHINE_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_SELECTION, AgsMachineSelection))
#define AGS_MACHINE_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_SELECTION, AgsMachineSelectionClass))
#define AGS_IS_MACHINE_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_SELECTION))
#define AGS_IS_MACHINE_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_SELECTION))
#define AGS_MACHINE_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MACHINE_SELECTION, AgsMachineSelectionClass))

typedef struct _AgsMachineSelection AgsMachineSelection;
typedef struct _AgsMachineSelectionClass AgsMachineSelectionClass;

struct _AgsMachineSelection
{
  GtkDialog dialog;

  AgsWindow *window;
  GList *machine;
};

struct _AgsMachineSelectionClass
{
  GtkDialogClass dialog;
};

GType ags_machine_selection_get_type(void);

void ags_machine_selection_load_default(AgsMachineSelection *machine_selection);
AgsMachine* ags_machine_selection_run(AgsMachineSelection *machine_selection);

AgsMachineSelection* ags_machine_selection_new(AgsWindow *window);

#endif /*__AGS_MACHINE_SELECTION_H__*/
