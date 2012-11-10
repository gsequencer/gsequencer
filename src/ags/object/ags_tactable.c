/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/object/ags_tactable.h>

#include <ags/object/ags_marshal.h>

void ags_tactable_class_init(AgsTactableInterface *interface);

GType
ags_tactable_get_type()
{
  static GType tactable_type = 0;

  if(!tactable_type){
    tactable_type = g_type_register_static_simple (G_TYPE_INTERFACE,
						   "AgsTactable\0",
						   sizeof (AgsTactableInterface),
						   (GClassInitFunc) ags_tactable_class_init,
						   0, NULL, 0);
  }
  
  return tactable_type;
}

void
ags_tactable_class_init(AgsTactableInterface *interface)
{
  g_signal_new("change_sequencer_duration\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsTactableInterface, change_sequencer_duration),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);

  g_signal_new("change_notation_duration\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsTactableInterface, change_notation_duration),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);

  g_signal_new("change_tact\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsTactableInterface, change_tact),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);

  g_signal_new("change_bpm\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsTactableInterface, change_bpm),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);
}

void
ags_tactable_change_sequencer_duration(AgsTactable *tactable, double duration)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_sequencer_duration);
  tactable_interface->change_sequencer_duration(tactable, duration);
}

void
ags_tactable_change_notation_duration(AgsTactable *tactable, double duration)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_notation_duration);
  tactable_interface->change_notation_duration(tactable, duration);
}

void
ags_tactable_change_tact(AgsTactable *tactable, double tact)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_tact);
  tactable_interface->change_tact(tactable, tact);
}

void
ags_tactable_change_bpm(AgsTactable *tactable, double bpm)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_bpm);
  tactable_interface->change_bpm(tactable, bpm);
}
