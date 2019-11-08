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

G_BEGIN_DECLS

typedef enum{
  AGS_SYNTH_OSCILLATOR_SIN,
  AGS_SYNTH_OSCILLATOR_SAWTOOTH,
  AGS_SYNTH_OSCILLATOR_TRIANGLE,
  AGS_SYNTH_OSCILLATOR_SQUARE,
  AGS_SYNTH_OSCILLATOR_IMPULSE,
  AGS_SYNTH_OSCILLATOR_LAST,
}AgsSynthOscillatorMode;

G_END_DECLS

#endif /*__AGS_SYNTH_ENUMS_H__*/
