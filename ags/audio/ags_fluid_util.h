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

#ifndef __AGS_FLUID_UTIL_H__
#define __AGS_FLUID_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS
#define AGS_TYPE_FLUID_UTIL         (ags_fluid_util_get_type())

#define AGS_FLUID_INTERP_BITS        (8)
#define AGS_FLUID_INTERP_BITS_MASK   (0xff000000)
#define AGS_FLUID_INTERP_BITS_SHIFT  (24)

#define AGS_FLUID_FRACT_MAX ((gdouble) 4294967296.0)

#define AGS_FLUID_INTERP_MAX         (2048)
#define AGS_FLUID_SINC_INTERP_ORDER 7     /* 7th order constant */

#define AGS_FLUID_CENTS_HZ_SIZE     (1200)

typedef struct _AgsFluidUtil AgsFluidUtil;

struct _AgsFluidUtil
{
  //empty
};

GType ags_fluid_util_get_type(void);

#define ags_fluid_phase_set(a,b) a=b;
#define ags_fluid_phase_set_int(a, b)    ((a) = ((guint64)(b)) << 32)

#define ags_fluid_phase_set_float(a, b) \
  (a) = (((guint64)(b)) << 32) \
  | (guint32) (((gdouble)(b) - (gint)(b)) * (gdouble)AGS_FLUID_FRACT_MAX)

#define ags_fluid_phase_from_index_fract(index, fract) \
  ((((guint64)(index)) << 32) + (fract))

#define ags_fluid_phase_index(_x) \
  ((guint)((_x) >> 32))
#define ags_fluid_phase_fract(_x) \
  ((guint32)((_x) & 0xFFFFFFFF))

#define ags_fluid_phase_index_round(_x) \
  ((guint)(((_x) + 0x80000000) >> 32))

#define ags_fluid_phase_fract_to_tablerow(_x) \
  ((guint)(ags_fluid_phase_fract(_x) & AGS_FLUID_INTERP_BITS_MASK) >> AGS_FLUID_INTERP_BITS_SHIFT)

#define ags_fluid_phase_double(_x) \
  ((gdouble)(ags_fluid_phase_index(_x)) + ((gdouble)ags_fluid_phase_fract(_x) / AGS_FLUID_FRACT_MAX))

#define ags_fluid_phase_incr(a, b)  a += b

#define ags_fluid_phase_decr(a, b)  a -= b

#define ags_fluid_phase_sub_int(a, b)  ((a) -= (guint64)(b) << 32)

#define ags_fluid_phase_index_plusplus(a)  (((a) += 0x100000000LL)

gdouble ags_fluid_ct2hz_real(gdouble cents);
gdouble ags_fluid_ct2hz(gdouble cents);

G_END_DECLS

#endif /*__AGS_FLUID_UTIL_H__*/
