/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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
  static gsize g_enum_type_id__static;

  if(g_once_init_enter(&g_enum_type_id__static)){
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

    g_once_init_leave(&g_enum_type_id__static, g_enum_type_id);
  }
  
  return(g_enum_type_id__static);
}

GType
ags_synth_key_mode_get_type()
{
  static gsize g_enum_type_id__static;

  if(g_once_init_enter(&g_enum_type_id__static)){
    static const GEnumValue values[] = {
      { AGS_SYNTH_KEY_1_1, "AGS_SYNTH_KEY_1_1", "synth-key-1-1" },
      { AGS_SYNTH_KEY_2_2, "AGS_SYNTH_KEY_2_2", "synth-key-2-2" },
      { AGS_SYNTH_KEY_4_4, "AGS_SYNTH_KEY_4_4", "synth-key-4-4" },
      { AGS_SYNTH_KEY_8_8, "AGS_SYNTH_KEY_8_8", "synth-key-8-8" },
      { AGS_SYNTH_KEY_16_16, "AGS_SYNTH_KEY_16_16", "synth-key-16-16" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSynthKeyMode"), values);

    g_once_init_leave(&g_enum_type_id__static, g_enum_type_id);
  }
  
  return(g_enum_type_id__static);
}

GType
ags_pitch_type_mode_get_type()
{
  static gsize g_enum_type_id__static;

  if(g_once_init_enter(&g_enum_type_id__static)){
    static const GEnumValue values[] = {
      { AGS_PITCH_TYPE_FLUID_INTERPOLATE_NONE, "AGS_PITCH_TYPE_FLUID_INTERPOLATE_NONE", "pitch-type-fluid-interpolate-none" },
      { AGS_PITCH_TYPE_FLUID_INTERPOLATE_LINEAR, "AGS_PITCH_TYPE_FLUID_INTERPOLATE_LINEAR", "pitch-type-fluid-interpolate-linear" },
      { AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER, "AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER", "pitch-type-fluid-interpolate-4th-order" },
      { AGS_PITCH_TYPE_FLUID_INTERPOLATE_7TH_ORDER, "AGS_PITCH_TYPE_FLUID_INTERPOLATE_7TH_ORDER", "pitch-type-fluid-interpolate-7th-order" },
      { AGS_PITCH_TYPE_INTERPOLATE_2X_ALIAS, "AGS_PITCH_TYPE_INTERPOLATE_2X_ALIAS", "pitch-type-interpolate-2x-alias" },
      { AGS_PITCH_TYPE_INTERPOLATE_4X_ALIAS, "AGS_PITCH_TYPE_INTERPOLATE_4X_ALIAS", "pitch-type-interpolate-4x-alias" },
      { AGS_PITCH_TYPE_INTERPOLATE_16X_ALIAS, "AGS_PITCH_TYPE_INTERPOLATE_16X_ALIAS", "pitch-type-interpolate-16x-alias" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsPitchTypeMode"), values);

    g_once_init_leave(&g_enum_type_id__static, g_enum_type_id);
  }
  
  return(g_enum_type_id__static);
}
