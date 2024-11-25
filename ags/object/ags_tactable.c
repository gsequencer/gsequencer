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

#include <ags/object/ags_tactable.h>

#include <ags/object/ags_marshal.h>

void ags_tactable_class_init(AgsTactableInterface *ginterface);

/**
 * SECTION:ags_tactable
 * @short_description: unique tempo set or attributes access
 * @title: AgsTactable
 * @section_id: AgsTactable
 * @include: ags/object/ags_tactable.h
 *
 * The #AgsTactable interface gives you a unique access to modify tempo.
 */

enum {
  CHANGE_SEQUENCER_DURATION,
  CHANGE_NOTATION_DURATION,
  CHANGE_WAVE_DURATION,
  CHANGE_MIDI_DURATION,
  CHANGE_TACT,
  CHANGE_BPM,
  LAST_SIGNAL,
};

static guint tactable_signals[LAST_SIGNAL];

GType
ags_tactable_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_tactable = 0;

    ags_type_tactable = g_type_register_static_simple(G_TYPE_INTERFACE,
						      "AgsTactable",
						      sizeof (AgsTactableInterface),
						      (GClassInitFunc) ags_tactable_class_init,
						      0, NULL, 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_tactable);
  }

  return g_define_type_id__static;
}

void
ags_tactable_class_init(AgsTactableInterface *ginterface)
{
  /**
   * AgsTactable::change-sequencer-duration:
   * @tactable: the #AgsTactable object
   * @sequencer_duration: new duration
   *
   * The ::change-sequencer-duration signal notifies about changed duration
   * of sequencer.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_SEQUENCER_DURATION] = 
    g_signal_new("change-sequencer-duration",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_sequencer_duration),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT64,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT64);

  /**
   * AgsTactable::change-notation-duration:
   * @tactable: the #AgsTactable object
   * @notation_duration: new duration
   *
   * The ::change-notation-duration signal notifies about changed duration
   * of notation.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_NOTATION_DURATION] = 
    g_signal_new("change-notation-duration",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_notation_duration),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT64,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT64);

  /**
   * AgsTactable::change-wave-duration:
   * @tactable: the #AgsTactable object
   * @wave_duration: new duration
   *
   * The ::change-wave-duration signal notifies about changed duration
   * of wave.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_WAVE_DURATION] = 
    g_signal_new("change-wave-duration",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_wave_duration),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT64,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT64);

  /**
   * AgsTactable::change-midi-duration:
   * @tactable: the #AgsTactable object
   * @midi_duration: new duration
   *
   * The ::change-midi-duration signal notifies about changed duration
   * of midi.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_MIDI_DURATION] = 
    g_signal_new("change-midi-duration",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_midi_duration),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT64,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT64);

  /**
   * AgsTactable::change-tact
   * @tactable: the #AgsTactable object
   * @new_tact: new tact
   * @old_tact: old tact
   *
   * The ::change-tact signal notifies about changed tact.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_TACT] = 
    g_signal_new("change-tact",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_tact),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__DOUBLE_DOUBLE,
		 G_TYPE_NONE, 2,
		 G_TYPE_DOUBLE, G_TYPE_DOUBLE);

  /**
   * AgsTactable::change-bpm:
   * @tactable: the #AgsTactable object
   * @new_bpm: new BPM
   * @old_bpm: old BPM
   *
   * The ::change-bpm signal notifies about changed bpm.
   *
   * Since: 3.0.0
   */
  tactable_signals[CHANGE_BPM] = 
    g_signal_new("change-bpm",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTactableInterface, change_bpm),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__DOUBLE_DOUBLE,
		 G_TYPE_NONE, 2,
		 G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

/**
 * ags_tactable_get_sequencer_duration:
 * @tactable: the #AgsTactable
 *
 * Get sequencer duration.
 *
 * Returns: the sequencer duration
 *
 * Since: 3.0.0
 */
guint64
ags_tactable_get_sequencer_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), 0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_sequencer_duration, 0);
  
  return(tactable_interface->get_sequencer_duration(tactable));
}

/**
 * ags_tactable_get_notation_duration:
 * @tactable: the #AgsTactable
 *
 * Get notation duration.
 *
 * Returns: the notation duration
 *
 * Since: 3.0.0
 */
guint64
ags_tactable_get_notation_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), 0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_notation_duration, 0);
  
  return(tactable_interface->get_notation_duration(tactable));
}

/**
 * ags_tactable_get_wave_duration:
 * @tactable: the #AgsTactable
 *
 * Get wave duration.
 *
 * Returns: the wave duration
 *
 * Since: 3.0.0
 */
guint64
ags_tactable_get_wave_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), 0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_wave_duration, 0);
  
  return(tactable_interface->get_wave_duration(tactable));
}

/**
 * ags_tactable_get_midi_duration:
 * @tactable: the #AgsTactable
 *
 * Get midi duration.
 *
 * Returns: the midi duration
 *
 * Since: 3.0.0
 */
guint64
ags_tactable_get_midi_duration(AgsTactable *tactable)
{
  AgsTactableInterface *tactable_interface;

  g_return_val_if_fail(AGS_IS_TACTABLE(tactable), 0);
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_val_if_fail(tactable_interface->get_midi_duration, 0);
  
  return(tactable_interface->get_midi_duration(tactable));
}

/**
 * ags_tactable_get_bpm:
 * @tactable: the #AgsTactable
 *
 * Get bpm.
 *
 * Returns: the bpm
 *
 * Since: 3.0.0
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
 * ags_tactable_get_tact:
 * @tactable: the #AgsTactable
 *
 * Get tact.
 *
 * Returns: the tact
 *
 * Since: 3.0.0
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
 * ags_tactable_change_sequencer_duration:
 * @tactable: the #AgsTactable
 * @sequencer_duration: the duration
 *
 * Change sequencer duration.
 * 
 * Since: 3.0.0
 */
void
ags_tactable_change_sequencer_duration(AgsTactable *tactable, guint64 sequencer_duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_SEQUENCER_DURATION],
		0,
		sequencer_duration);
}

/**
 * ags_tactable_change_notation_duration:
 * @tactable: the #AgsTactable
 * @notation_duration: the duration
 *
 * Change notation duration.
 * 
 * Since: 3.0.0
 */
void
ags_tactable_change_notation_duration(AgsTactable *tactable, guint64 notation_duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_NOTATION_DURATION],
		0,
		notation_duration);
}

/**
 * ags_tactable_change_wave_duration:
 * @tactable: the #AgsTactable
 * @wave_duration: the duration
 *
 * Change wave duration.
 * 
 * Since: 3.0.0
 */
void
ags_tactable_change_wave_duration(AgsTactable *tactable, guint64 wave_duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_WAVE_DURATION],
		0,
		wave_duration);
}

/**
 * ags_tactable_change_midi_duration:
 * @tactable: the #AgsTactable
 * @midi_duration: the duration
 *
 * Change midi duration.
 * 
 * Since: 3.0.0
 */
void
ags_tactable_change_midi_duration(AgsTactable *tactable, guint64 midi_duration)
{
  g_signal_emit(tactable,
		tactable_signals[CHANGE_MIDI_DURATION],
		0,
		midi_duration);
}

/**
 * ags_tactable_change_tact:
 * @tactable: the #AgsTactable
 * @new_tact: the new tact
 * @old_tact: the old tact
 *
 * Change tact.
 * 
 * Since: 3.0.0
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
 * @tactable: the #AgsTactable
 * @new_bpm: the new bpm
 * @old_bpm: the old bpm
 *
 * Change bpm.
 * 
 * Since: 3.0.0
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
