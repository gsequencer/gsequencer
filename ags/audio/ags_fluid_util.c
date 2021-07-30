/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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
 *
 * FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include <ags/audio/ags_fluid_util.h>

#include <math.h>

gpointer ags_fluid_util_copy(gpointer ptr);
void ags_fluid_util_free(gpointer ptr);

void ags_fluid_conversion_config();

gboolean ct2hz_tab_initialized = FALSE;

gdouble ct2hz_tab[AGS_FLUID_CENTS_HZ_SIZE];

GMutex ct2hz_tab_mutex;

/**
 * SECTION:ags_fluid_util
 * @short_description: fluid util
 * @title: AgsFluidUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_util.h
 *
 * Fluid utility functions.
 */

GType
ags_fluid_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fluid_util = 0;

    ags_type_fluid_util =
      g_boxed_type_register_static("AgsFluidUtil",
				   (GBoxedCopyFunc) ags_fluid_util_copy,
				   (GBoxedFreeFunc) ags_fluid_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fluid_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_fluid_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsFluidUtil));
 
  return(retval);
}

void
ags_fluid_util_free(gpointer ptr)
{
  g_free(ptr);
}

/*
 * void fluid_synth_init
 *
 * Does all the initialization for this module.
 */
void
ags_fluid_conversion_config()
{
  gint i;

  g_mutex_lock(&ct2hz_tab_mutex);

  if(ct2hz_tab_initialized){
    g_mutex_unlock(&ct2hz_tab_mutex);

    return;
  }

  for(i = 0; i < AGS_FLUID_CENTS_HZ_SIZE; i++){
    // 6,875 is just a factor that we already multiply into the lookup table to save
    // that multiplication in fluid_ct2hz_real()
    // 6.875 Hz because 440Hz / 2^6
    ct2hz_tab[i] = 6.875 * powl(2.0, (double) i / 1200.0);
  }

  ct2hz_tab_initialized = TRUE;
  
  g_mutex_unlock(&ct2hz_tab_mutex);
}

/*
 * Converts absolute cents to Hertz
 * 
 * As per sfspec section 9.3:
 * 
 * ABSOLUTE CENTS - An absolute logarithmic measure of frequency based on a
 * reference of MIDI key number scaled by 100.
 * A cent is 1/1200 of an octave [which is the twelve hundredth root of two],
 * and value 6900 is 440 Hz (A-440).
 * 
 * Implemented below basically is the following:
 *   440 * 2^((cents-6900)/1200)
 * = 440 * 2^((int)((cents-6900)/1200)) * 2^(((int)cents-6900)%1200))
 * = 2^((int)((cents-6900)/1200)) * (440 * 2^(((int)cents-6900)%1200)))
 *                                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *                           This second factor is stored in the lookup table.
 *
 * The first factor can be implemented with a fast shift when the exponent
 * is always an int. This is the case when using 440/2^6 Hz rather than 440Hz
 * reference.
 */
gdouble
ags_fluid_ct2hz_real(gdouble cents)
{
  ags_fluid_conversion_config();
  
  if(cents < 0){
    return((gdouble) 1.0);
  }else{
    guint mult, fac, rem;
    guint icents = (guint) cents;
    
    icents += 300u;

    // don't use stdlib div() here, it turned out have poor performance
    fac = icents / 1200u;
    rem = icents % 1200u;

    // Think of "mult" as the factor that we multiply (440/2^6)Hz with,
    // or in other words mult is the "first factor" of the above
    // functions comment.
    //
    // Assuming sizeof(uint)==4 this will give us a maximum range of
    // 32 * 1200cents - 300cents == 38100 cents == 29,527,900,160 Hz
    // which is much more than ever needed. For bigger values, just
    // safely wrap around (the & is just a replacement for the quick
    // modulo operation % 32).
    mult = 1u << (fac & (sizeof(mult)*8u - 1u));

    // don't use ldexp() either (poor performance)
    return(mult * ct2hz_tab[rem]);
  }
}

/*
 * fluid_ct2hz
 */
gdouble
ags_fluid_ct2hz(gdouble cents)
{
  /* Filter fc limit: SF2.01 page 48 # 8 */
  if(cents >= 13500){
    cents = 13500;             /* 20 kHz */
  }else if(cents < 1500){
    cents = 1500;              /* 20 Hz */
  }

  return(ags_fluid_ct2hz_real(cents));
}
