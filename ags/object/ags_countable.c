/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/object/ags_countable.h>

void ags_countable_base_init(AgsCountableInterface *ginterface);

/**
 * SECTION:ags_countable
 * @short_description: a counter interface
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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_countable = 0;

    static const GTypeInfo ags_countable_info = {
      sizeof(AgsCountableInterface),
      (GBaseInitFunc) ags_countable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_countable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsCountable", &ags_countable_info,
						 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_countable);
  }

  return g_define_type_id__static;
}

void
ags_countable_base_init(AgsCountableInterface *ginterface)
{
  /* empty */
}

/**
 * ags_countable_get_sequencer_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of sequencer.
 *
 * Returns: the current position
 *
 * Since: 3.0.0
 */
guint64
ags_countable_get_sequencer_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), G_MAXUINT64);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_sequencer_counter, G_MAXUINT64);
  
  return(countable_interface->get_sequencer_counter(countable));
}

/**
 * ags_countable_get_notation_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of notation.
 *
 * Returns: the current position
 *
 * Since: 3.0.0
 */
guint64
ags_countable_get_notation_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), G_MAXUINT64);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_notation_counter, G_MAXUINT64);
  
  return(countable_interface->get_notation_counter(countable));
}

/**
 * ags_countable_get_wave_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of wave.
 *
 * Returns: the current position
 *
 * Since: 3.0.0
 */
guint64
ags_countable_get_wave_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), G_MAXUINT64);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_wave_counter, G_MAXUINT64);

  return(countable_interface->get_wave_counter(countable));
}

/**
 * ags_countable_get_midi_counter:
 * @countable: an #AgsCountable
 *
 * Retrieve current position of MIDI.
 *
 * Returns: the current position
 *
 * Since: 3.0.0
 */
guint64
ags_countable_get_midi_counter(AgsCountable *countable)
{
  AgsCountableInterface *countable_interface;

  g_return_val_if_fail(AGS_IS_COUNTABLE(countable), G_MAXUINT64);
  countable_interface = AGS_COUNTABLE_GET_INTERFACE(countable);
  g_return_val_if_fail(countable_interface->get_midi_counter, G_MAXUINT64);

  return(countable_interface->get_midi_counter(countable));
}
