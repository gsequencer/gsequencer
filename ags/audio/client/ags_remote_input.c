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

#include <ags/audio/client/ags_remote_input.h>

#include <ags/object/ags_connectable.h>

void ags_remote_input_class_init(AgsRemoteInputClass *remote_input_class);
void ags_remote_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remote_input_init(AgsRemoteInput *remote_input);

GType
ags_remote_input_get_type (void)
{
  static GType ags_type_remote_input = 0;

  if(!ags_type_remote_input){
    static const GTypeInfo ags_remote_input_info = {
      sizeof (AgsRemoteInputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remote_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoteInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remote_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remote_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remote_input = g_type_register_static(AGS_TYPE_REMOTE_CHANNEL,
						   "AgsRemoteInput\0",
						   &ags_remote_input_info, 0);

    g_type_add_interface_static(ags_type_remote_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_remote_input);
}

void
ags_remote_input_class_init(AgsRemoteInputClass *remote_input_class)
{
}

void
ags_remote_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
}

void
ags_remote_input_init(AgsRemoteInput *remote_input)
{
}
