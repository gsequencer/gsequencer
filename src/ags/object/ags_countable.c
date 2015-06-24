/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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

#include <ags/object/ags_countable.h>

#include <stdio.h>

void ags_countable_base_init(AgsCountableInterface *interface);

/**
 * SECTION:ags_countable
 * @short_description: unique access to dialogs
 * @title: AgsCountable
 * @section_id:
 * @include: ags/object/ags_countable.h
 *
 * The #AgsCountable interface gives you a unique access to counters. Generally we
 * distinguish between notation and sequencer counters. Those may have their own
 * relative positions, especially the sequencers may loop.
 */

GType
ags_countable_get_type()
{
  static GType ags_type_countable = 0;

  if(!ags_type_countable){
    static const GTypeInfo ags_countable_info = {
      sizeof(AgsCountableInterface),
      (GBaseInitFunc) ags_countable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_countable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsCountable\0", &ags_countable_info,
						 0);
  }

  return(ags_type_countable);
}

void
ags_countable_base_init(AgsCountableInterface *interface)
{
  /* empty */
}

/**
 * ags_countable_get_notation_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of notation.
 *
 * Returns: the current position
 *
 * Since: 0.4
 */
guint
ags_countable_get_notation_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), -1);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_notation_counter, -1);
  countable_interface->get_notation_counter(countable);
}

/**
 * ags_countable_get_sequencer_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of sequencer.
 *
 * Returns: the current position
 *
 * Since: 0.4
 */
guint
ags_countable_get_sequencer_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), -1);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_sequencer_counter, -1);
  countable_interface->get_sequencer_counter(countable);
}
