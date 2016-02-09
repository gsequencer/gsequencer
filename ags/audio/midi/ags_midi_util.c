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
 * ags_midi_util_envelope_to_velocity:
 * @attack:
 * @decay:
 * @sustain:
 * @release:
 * @ratio:
 * @samplerate:
 * @start_frame:
 * @end_frame:
 *
 *
 *
 * Returns:
 *
 * Since: 0.7.2
 */
long
ags_midi_util_envelope_to_velocity(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  long velocity;

  velocity = 127;

  //TODO:JK: implement me
  
  return(velocity);
}

/**
 * ags_midi_util_velocity_to_envelope:
 * @delta_time:
 * @is_release:
 * @attack:
 * @decay:
 * @sustain:
 * @release:
 * @ratio:
 * @samplerate
 * @start_frame:
 * @end_frame:
 *
 *
 *
 * Since: 0.7.2
 */
void
ags_midi_util_velocity_to_envelope(long delta_time,
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
 * @attack:
 * @decay:
 * @sustain:
 * @release:
 * @ratio:
 * @samplerate:
 * @start_frame:
 * @end_frame:
 *
 *
 *
 * Returns:
 *
 * Since: 0.7.2
 */
long
ags_midi_util_envelope_to_pressure(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  long pressure;

  pressure = 127;

  //TODO:JK: implement me
  
  return(pressure);
}

/**
 * ags_midi_util_pressure_to_envelope:
 * @delta_time:
 * @is_sustain:
 * @attack:
 * @decay:
 * @sustain:
 * @release:
 * @ratio:
 * @samplerate:
 * @start_frame:
 * @end_frame:
 *
 *
 *
 * Since: 0.7.2
 */
void
ags_midi_util_pressure_to_envelope(long delta_time,
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
 * @delta_time:
 * @bpm:
 * @delay_factor:
 * @delay:
 * @attack:
 *
 *
 *
 * Since: 0.7.2
 */
guint
ags_midi_util_delta_time_to_offset(long delta_time,
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
 * @x:
 * @bpm:
 * @delay_factor:
 *
 *
 *
 * Since: 0.7.2
 */
long
ags_midi_util_offset_to_delta_time(guint x,
				   gdouble bpm, gdouble delay_factor)
{
  //TODO:JK: implement me
}
