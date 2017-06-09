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

#include <ags/audio/client/ags_remote_output.h>

#include <ags/object/ags_connectable.h>

void ags_remote_output_class_init(AgsRemoteOutputClass *remote_output_class);
void ags_remote_output_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remote_output_init(AgsRemoteOutput *remote_output);

GType
ags_remote_output_get_type (void)
{
  static GType ags_type_remote_output = 0;

  if(!ags_type_remote_output){
    static const GTypeInfo ags_remote_output_info = {
      sizeof (AgsRemoteOutputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remote_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoteOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remote_output_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remote_output_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remote_output = g_type_register_static(AGS_TYPE_REMOTE_CHANNEL,
						    "AgsRemoteOutput",
						    &ags_remote_output_info, 0);

    g_type_add_interface_static(ags_type_remote_output,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_remote_output);
}

void
ags_remote_output_class_init(AgsRemoteOutputClass *remote_output_class)
{
}

void
ags_remote_output_connectable_interface_init(AgsConnectableInterface *connectable)
{
}

void
ags_remote_output_init(AgsRemoteOutput *remote_output)
{
}
