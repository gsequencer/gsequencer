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

#include <ags/audio/midi/ags_midi_util.h>

/**
 * SECTION:ags_midi_util
 * @short_description: MIDI util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_util.h
 *
 * Utility functions for MIDI.
 */

/**
 * ags_midi_util_envelope_to_velocity:
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Envelope to velocity.
 *
 * Returns: the velocity
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_envelope_to_velocity(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  glong velocity;

  velocity = 127;

  //TODO:JK: implement me
  
  return(velocity);
}

/**
 * ags_midi_util_velocity_to_envelope:
 * @delta_time: delta time
 * @is_release: is release
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Velocity to envelope.
 *
 * Since: 0.7.2
 */
void
ags_midi_util_velocity_to_envelope(glong delta_time,
				   gboolean is_release,
				   AgsComplex **attack,
				   AgsComplex **decay,
				   AgsComplex **sustain,
				   AgsComplex **release,
				   AgsComplex **ratio,
				   guint *samplerate,
				   guint *start_frame, guint *end_frame)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_util_envelope_to_pressure:
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Envelope to pressure.
 *
 * Returns: the pressure
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_envelope_to_pressure(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  glong pressure;

  pressure = 127;

  //TODO:JK: implement me
  
  return(pressure);
}

/**
 * ags_midi_util_pressure_to_envelope:
 * @delta_time: delta time
 * @is_sustain: is sustain
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Pressure to envelope.
 *
 * Since: 0.7.2
 */
void
ags_midi_util_pressure_to_envelope(glong delta_time,
				   gboolean is_sustain,
				   AgsComplex **attack,
				   AgsComplex **decay,
				   AgsComplex **sustain,
				   AgsComplex **release,
				   AgsComplex **ratio,
				   guint *samplerate,
				   guint *start_frame, guint *end_frame)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_util_delta_time_to_offset:
 * @delta_time: delta time
 * @bpm: bpm
 * @delay_factor: delay factor
 * @delay: delay
 * @attack: attack
 *
 * Delta time to offset
 *
 * Returns: the offset
 *
 * Since: 0.7.2
 */
guint
ags_midi_util_delta_time_to_offset(glong delta_time,
				   gdouble bpm, gdouble delay_factor,
				   gdouble *delay, guint *attack)
{
  guint offset;
  
  offset = 0;

  //TODO:JK: implement me

  return(offset);
}

/**
 * ags_midi_util_offset_to_delta_time:
 * @x: offset
 * @bpm: bpm
 * @delay_factor: delay factor
 *
 * Offset to delta time
 *
 * Returns: the delta time
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_offset_to_delta_time(guint x,
				   gdouble bpm, gdouble delay_factor)
{
  //TODO:JK: implement me
}
