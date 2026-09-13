/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/ags_abyss_synth_util.h>

#include <ags/libags-audio-globals.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_envelope_util.h>
#include <ags/audio/ags_lfo_synth_util.h>
#include <ags/audio/ags_noise_util.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_abyss_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsAbyssSynthUtil
 * @section_id:
 * @include: ags/audio/ags_abyss_synth_util.h
 *
 * Utility functions to compute abyss synths.
 */

GType
ags_abyss_synth_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_abyss_synth_util = 0;

    ags_type_abyss_synth_util =
      g_boxed_type_register_static("AgsAbyssSynthUtil",
				   (GBoxedCopyFunc) ags_abyss_synth_util_copy,
				   (GBoxedFreeFunc) ags_abyss_synth_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_abyss_synth_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_abyss_synth_util_alloc:
 *
 * Allocate #AgsAbyssSynthUtil-struct
 *
 * Returns: a new #AgsAbyssSynthUtil-struct
 *
 * Since: 9.4.0
 */
AgsAbyssSynthUtil*
ags_abyss_synth_util_alloc()
{
  AgsAbyssSynthUtil *ptr;

  ptr = (AgsAbyssSynthUtil *) g_new(AgsAbyssSynthUtil,
					1);

  ptr[0] = AGS_ABYSS_SYNTH_UTIL_INITIALIZER;
  
  return(ptr);
}

/**
 * ags_abyss_synth_util_copy:
 * @ptr: the original #AgsAbyssSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAbyssSynthUtil-struct
 *
 * Since: 9.4.0
 */
gpointer
ags_abyss_synth_util_copy(AgsAbyssSynthUtil *ptr)
{
  AgsAbyssSynthUtil *new_ptr;
  
  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsAbyssSynthUtil *) g_new(AgsAbyssSynthUtil,
					    1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;
  
  return(new_ptr);
}

/**
 * ags_abyss_synth_util_free:
 * @ptr: the #AgsAbyssSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_free(AgsAbyssSynthUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
 
  g_free(ptr);
}
