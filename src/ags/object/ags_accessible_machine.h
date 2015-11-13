/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_ACCESSIBLE_MACHINE_H__
#define __AGS_ACCESSIBLE_MACHINE_H__

#include <glib-object.h>

#define AGS_TYPE_ACCESSIBLE_MACHINE                    (ags_accessible_machine_get_type())
#define AGS_ACCESSIBLE_MACHINE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACCESSIBLE_MACHINE, AgsAccessibleMachine))
#define AGS_ACCESSIBLE_MACHINE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_ACCESSIBLE_MACHINE, AgsAccessibleMachineInterface))
#define AGS_IS_ACCESSIBLE_MACHINE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ACCESSIBLE_MACHINE))
#define AGS_IS_ACCESSIBLE_MACHINE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_ACCESSIBLE_MACHINE))
#define AGS_ACCESSIBLE_MACHINE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_ACCESSIBLE_MACHINE, AgsAccessibleMachineInterface))

typedef void AgsAccessibleMachine;
typedef struct _AgsAccessibleMachineInterface AgsAccessibleMachineInterface;

struct _AgsAccessibleMachineInterface
{
  AgsGenericAccessible generic_accessible;

  gboolean (*move_to_pad)(AgsAccessibleMachine *accessible_machine);
  gboolean (*move_to_line)(AgsAccessibleMachine *accessible_machine);
  gboolean (*move_to_line_member)(AgsAccessibleMachine *accessible_machine);

  gboolean (*move_up)(AgsAccessibleMachine *accessible_machine);
  gboolean (*move_down)(AgsAccessibleMachine *accessible_machine);
  gboolean (*destroy)(AgsAccessibleMachine *accessible_machine);
  gboolean (*rename)(AgsAccessibleMachine *accessible_machine);
  
  gboolean (*popup_context_menu)(AgsAccessibleMachine *accessible_machine);
};

GType ags_accessible_machine_get_type();

gboolean ags_accessible_machine_move_to_pad(AgsAccessibleMachine *accessible_machine);
gboolean ags_accessible_machine_move_to_line(AgsAccessibleMachine *accessible_machine);
gboolean ags_accessible_machine_move_to_line_member(AgsAccessibleMachine *accessible_machine);

gboolean ags_accessible_machine_move_up(AgsAccessibleMachine *accessible_machine);
gboolean ags_accessible_machine_move_down(AgsAccessibleMachine *accessible_machine);
gboolean ags_accessible_machine_destroy(AgsAccessibleMachine *accessible_machine);
gboolean ags_accessible_machine_rename(AgsAccessibleMachine *accessible_machine);
  
gboolean ags_accessible_machine_popup_context_menu(AgsAccessibleMachine *accessible_machine);

#endif /*__AGS_ACCESSIBLE_MACHINE_H__*/
