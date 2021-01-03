/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

/**
 * SECTION:ags_synth_enums
 * @short_description: synth enumerations
 * @title: AgsSynthEnums
 * @section_id:
 * @include: ags/audio/ags_synth_enums.h
 *
 * Enumerations related to synth.
 */

GType
ags_synth_oscillator_mode_get_type()
{
  static volatile gsize g_enum_type_id__volatile;

  if(g_once_init_enter (&g_enum_type_id__volatile)){
    static const GEnumValue values[] = {
      { AGS_SYNTH_OSCILLATOR_SIN, "AGS_SYNTH_OSCILLATOR_SIN", "synth-oscillator-sin" },
      { AGS_SYNTH_OSCILLATOR_SAWTOOTH, "AGS_SYNTH_OSCILLATOR_SAWTOOTH", "synth-oscillator-sawtooth" },
      { AGS_SYNTH_OSCILLATOR_TRIANGLE, "AGS_SYNTH_OSCILLATOR_TRIANGLE", "synth-oscillator-triangle" },
      { AGS_SYNTH_OSCILLATOR_SQUARE, "AGS_SYNTH_OSCILLATOR_SQUARE", "synth-oscillator-square" },
      { AGS_SYNTH_OSCILLATOR_IMPULSE, "AGS_SYNTH_OSCILLATOR_IMPULSE", "synth-oscillator-impulse" },
      { AGS_SYNTH_OSCILLATOR_LAST, "AGS_SYNTH_OSCILLATOR_LAST", "synth-oscillator-last" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSynthOscillatorMode"), values);

    g_once_init_leave (&g_enum_type_id__volatile, g_enum_type_id);
  }
  
  return g_enum_type_id__volatile;
}
