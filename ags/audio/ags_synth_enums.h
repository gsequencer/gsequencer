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

#ifndef __AGS_SYNTH_ENUMS_H__
#define __AGS_SYNTH_ENUMS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_SYNTH_OSCILLATOR_MODE_GET_TYPE               (ags_sound_scope_get_type())

G_BEGIN_DECLS

/**
 * AgsSynthOscillatorMode:
 * @AGS_SYNTH_OSCILLATOR_SIN: sinus oscillator
 * @AGS_SYNTH_OSCILLATOR_SAWTOOTH: sawtooth oscillator
 * @AGS_SYNTH_OSCILLATOR_TRIANGLE: triangle oscillator
 * @AGS_SYNTH_OSCILLATOR_SQUARE: square oscillator
 * @AGS_SYNTH_OSCILLATOR_IMPULSE: impulse oscillator
 * @AGS_SYNTH_OSCILLATOR_LAST: last mode
 * 
 * Enum values to specify oscillator mode.
 */
typedef enum{
  AGS_SYNTH_OSCILLATOR_SIN,
  AGS_SYNTH_OSCILLATOR_SAWTOOTH,
  AGS_SYNTH_OSCILLATOR_TRIANGLE,
  AGS_SYNTH_OSCILLATOR_SQUARE,
  AGS_SYNTH_OSCILLATOR_IMPULSE,
  AGS_SYNTH_OSCILLATOR_LAST,
}AgsSynthOscillatorMode;

GType ags_synth_oscillator_mode_get_type();

G_END_DECLS

#endif /*__AGS_SYNTH_ENUMS_H__*/
