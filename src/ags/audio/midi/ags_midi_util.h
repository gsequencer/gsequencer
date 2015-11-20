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

#ifndef __AGS_MIDI_UTIL_H__
#define __AGS_MIDI_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

long ags_midi_util_envelope_to_velocity(AgsComplex *attack,
					AgsComplex *decay,
					AgsComplex *sustain,
					AgsComplex *release,
					AgsComplex *ratio,
					guint samplerate,
					guint start_frame, guint end_frame);
long ags_midi_util_envelope_to_pressure(AgsComplex *attack,
					AgsComplex *decay,
					AgsComplex *sustain,
					AgsComplex *release,
					AgsComplex *ratio,
					guint samplerate,
					guint start_frame, guint end_frame);

guint ags_midi_util_delta_time_to_offset(long delta_time,
					 gdouble bpm, gdouble delay_factor,
					 gdouble *delay, guint *attack);

#endif /*__AGS_MIDI_UTIL_H__*/
