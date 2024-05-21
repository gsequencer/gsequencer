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

#ifndef __AGS_MACHINE_RADIO_BUTTON_H__
#define __AGS_MACHINE_RADIO_BUTTON_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_RADIO_BUTTON                (ags_machine_radio_button_get_type())
#define AGS_MACHINE_RADIO_BUTTON(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_RADIO_BUTTON, AgsMachineRadioButton))
#define AGS_MACHINE_RADIO_BUTTON_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_RADIO_BUTTON, AgsMachineRadioButtonClass))
#define AGS_IS_MACHINE_RADIO_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_RADIO_BUTTON))
#define AGS_IS_MACHINE_RADIO_BUTTON_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_RADIO_BUTTON))
#define AGS_MACHINE_RADIO_BUTTON_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MACHINE_RADIO_BUTTON, AgsMachineRadioButtonClass))

typedef struct _AgsMachineRadioButton AgsMachineRadioButton;
typedef struct _AgsMachineRadioButtonClass AgsMachineRadioButtonClass;

struct _AgsMachineRadioButton
{
  GtkCheckButton check_button;

  AgsConnectableFlags connectable_flags;
  
  AgsMachine *machine;
};

struct _AgsMachineRadioButtonClass
{
  GtkCheckButtonClass check_button;
};

GType ags_machine_radio_button_get_type(void);

AgsMachineRadioButton* ags_machine_radio_button_new();

G_END_DECLS

#endif /*__AGS_MACHINE_RADIO_BUTTON_H__*/
