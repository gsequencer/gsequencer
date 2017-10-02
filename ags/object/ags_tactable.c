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

#include <ags/object/ags_tactable.h>

#include <ags/object/ags_marshal.h>

void ags_tactable_class_init(AgsTactableInterface *interface);

/**
 * SECTION:ags_tactable
 * @short_description: Unique tempo set or attributes access
 * @title: AgsTactable
 * @section_id: AgsTactable
 * @include: ags/object/ags_tactable.h
 *
 * The #AgsTactable interface gives you a unique access to modify tempo.
 */

enum {
  CHANGE_SEQUENCER_DURATION,
  CHANGE_NOTATION_DURATION,
  CHANGE_TACT,
  CHANGE_BPM,
  LAST_SIGNAL,
};

static guint tactable_signals[LAST_SIGNAL];

GType
ags_tactable_get_type()
{
  static GType tactable_type = 0;

  if(!tactable_type){
    tactable_type = g_type_register_static_simple(G_TYPE_INTERFACE,
						  "AgsTactable",
						  sizeof (AgsTactableInterface),
						  (GClassInitFunc) ags_tactable_class_init,
						  0, NULL, 0);
  }
  
  return(tactable_type);
}

void
ags_tactable_class_init(AgsTactableInterface *interface)
{
  /**
   * AgsTactable::change-sequencer-duration:
   * @tactable: the #AgsTactable
   * @duration: new duration
   *
   * The ::change-sequencer-duration signal notifies about changed duration
   * of sequencer.
   *
   * Since: 1.0.0
   */
  tactable_signals[CHANGE_SEQUENCER_DURATION] = 
    g_signal_new("change-sequencer-duration",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_sequencer_duration),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);

  /**
   * AgsTactable::change-notation-duration:
   * @tactable: the #AgsTactable
   * @duration: new duration
   *
   * The ::change-notation-duration signal notifies about changed duration
   * of notation.
   *
   * Since: 1.0.0
   */
  tactable_signals[CHANGE_NOTATION_DURATION] = 
    g_signal_new("change-notation-duration",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_notation_duration),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);

  /**
   * AgsTactable::change-tact
   * @tactable: the #AgsTactable
   * @tact: new tact
   *
   * The ::change-tact signal notifies about changed tact.
   *
   * Since: 1.0.0
   */
  tactable_signals[CHANGE_TACT] = 
    g_signal_new("change-tact",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_tact),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__DOUBLE_DOUBLE,
		 G_TYPE_NONE, 2,
		 G_TYPE_DOUBLE, G_TYPE_DOUBLE);

  /**
   * AgsTactable::change-bpm:
   * @tactable: the #AgsTactable
   * @duration: new duration
   *
   * The ::change-bpm signal notifies about changed bpm.
   *
   * Since: 1.0.0
   */
  tactable_signals[CHANGE_BPM] = 
    g_signal_new("change-bpm",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_bpm),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__DOUBLE_DOUBLE,
		 G_TYPE_NONE, 2,
		 G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

/**
 * ags_tactable_get_sequencer_duration:
 * @tactable: an #AgsTactable
 *
 * Get sequencer duration.
 *
 * Returns: the sequencer duration
 *
 * Since: 1.0.0
 */
gdouble
ags_tactable_get_sequencer_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), -1.0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_sequencer_duration, -1.0);
  
  return(tactable_interface->get_sequencer_duration(tactable));
}

/**
 * ags_tactable_get_notation_duration:
 * @tactable: an #AgsTactable
 *
 * Get notation duration.
 *
 * Returns: the notation duration
 *
 * Since: 1.0.0
 */
gdouble
ags_tactable_get_notation_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), -1.0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_notation_duration, -1.0);
  
  return(tactable_interface->get_notation_duration(tactable));
}

/**
 * ags_tactable_get_tact:
 * @tactable: an #AgsTactable
 *
 * Get tact.
 *
 * Returns: the tact
 *
 * Since: 1.0.0
 */
gdouble
ags_tactable_get_tact(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), -1.0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_tact, -1.0);
  
  return(tactable_interface->get_tact(tactable));
}

/**
 * ags_tactable_get_bpm:
 * @tactable: an #AgsTactable
 *
 * Get bpm.
 *
 * Returns: the bpm
 *
 * Since: 1.0.0
 */
gdouble
ags_tactable_get_bpm(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), -1.0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_bpm, -1.0);
  
  return(tactable_interface->get_bpm(tactable));
}

/**
 * ags_tactable_change_sequencer_duration:
 * @tactable: an #AgsTactable
 * @duration: the duration
 *
 * Sequencer duration changed.
 * 
 * Since: 1.0.0
 */
void
ags_tactable_change_sequencer_duration(AgsTactable *tactable, double duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_SEQUENCER_DURATION],
		0,
		duration);
}

/**
 * ags_tactable_change_notation_duration:
 * @tactable: an #AgsTactable
 * @duration: the duration
 *
 * Notation duration changed.
 * 
 * Since: 1.0.0
 */
void
ags_tactable_change_notation_duration(AgsTactable *tactable, double duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_NOTATION_DURATION],
		0,
		duration);
}

/**
 * ags_tactable_change_tact:
 * @tactable: an #AgsTactable
 * @new_tact: the new tact
 * @old_tact: the old tact
 *
 * Tact changed.
 * 
 * Since: 1.0.0
 */
void
ags_tactable_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_TACT],
		0,
		new_tact,
		old_tact);
}

/**
 * ags_tactable_change_bpm:
 * @tactable: an #AgsTactable
 * @new_bpm: the new bpm
 * @old_bpm: the old bpm
 *
 * Bpm changed.
 * 
 * Since: 1.0.0
 */
void
ags_tactable_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_BPM],
		0,
		new_bpm,
		old_bpm);
}
