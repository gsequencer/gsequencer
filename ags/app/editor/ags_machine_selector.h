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

#ifndef __AGS_MACHINE_SELECTOR_H__
#define __AGS_MACHINE_SELECTOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_machine_radio_button.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_SELECTOR                (ags_machine_selector_get_type())
#define AGS_MACHINE_SELECTOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_SELECTOR, AgsMachineSelector))
#define AGS_MACHINE_SELECTOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_SELECTOR, AgsMachineSelectorClass))
#define AGS_IS_MACHINE_SELECTOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_SELECTOR))
#define AGS_IS_MACHINE_SELECTOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_SELECTOR))
#define AGS_MACHINE_SELECTOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MACHINE_SELECTOR, AgsMachineSelectorClass))

typedef struct _AgsMachineSelector AgsMachineSelector;
typedef struct _AgsMachineSelectorClass AgsMachineSelectorClass;

typedef enum{
  AGS_MACHINE_SELECTOR_CONNECTED              = 1,
  AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING   = 1 <<  1,
  AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO       = 1 <<  2,
  AGS_MACHINE_SELECTOR_BLOCK_REVERSE_MAPPING  = 1 <<  3,
}AgsMachineSelectorFlags;

typedef enum{
  AGS_MACHINE_SELECTOR_EDIT_NOTATION    = 1,
  AGS_MACHINE_SELECTOR_EDIT_SHEET       = 1 <<  1,
  AGS_MACHINE_SELECTOR_EDIT_AUTOMATION  = 1 <<  2,
  AGS_MACHINE_SELECTOR_EDIT_WAVE        = 1 <<  3,
}AgsMachineSelectorEdit;

struct _AgsMachineSelector
{
  GtkBox box;

  guint flags;
  guint edit;
  
  GtkLabel *label;

  GtkMenuButton *menu_button;
  GMenu *popup;

  GtkWidget *shift_piano;
  
  GtkWidget *current;

  GtkDialog *machine_selection;
};

struct _AgsMachineSelectorClass
{
  GtkBoxClass box;

  void (*changed)(AgsMachineSelector *machine_selector, AgsMachine *machine);
};

GType ags_machine_selector_get_type(void);

gboolean ags_machine_selector_test_flags(AgsMachineSelector *machine_selector,
					 guint flags);
void ags_machine_selector_set_flags(AgsMachineSelector *machine_selector,
				    guint flags);
void ags_machine_selector_unset_flags(AgsMachineSelector *machine_selector,
				    guint flags);

GList* ags_machine_selector_get_machine_radio_button(AgsMachineSelector *machine_selector);
void ags_machine_selector_add_machine_radio_button(AgsMachineSelector *machine_selector,
						   AgsMachineRadioButton *machine_radio_button);
void ags_machine_selector_remove_machine_radio_button(AgsMachineSelector *machine_selector,
						      AgsMachineRadioButton *machine_radio_button);

void ags_machine_selector_set_edit(AgsMachineSelector *machine_selector, guint edit);

void ags_machine_selector_add_index(AgsMachineSelector *machine_selector);
void ags_machine_selector_remove_index(AgsMachineSelector *machine_selector,
				       guint nth);

void ags_machine_selector_link_index(AgsMachineSelector *machine_selector,
				     AgsMachine *machine);

void ags_machine_selector_changed(AgsMachineSelector *machine_selector, AgsMachine *machine);

GMenu* ags_machine_selector_popup_new(AgsMachineSelector *machine_selector);

AgsMachineSelector* ags_machine_selector_new();

G_END_DECLS

#endif /*__AGS_MACHINE_SELECTOR_H__*/
