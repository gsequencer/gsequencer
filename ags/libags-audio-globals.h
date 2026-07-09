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

#ifndef __LIBAGS_AUDIO_GLOBALS_H__
#define __LIBAGS_AUDIO_GLOBALS_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

/* util */
#include <ags/audio/ags_fluid_util.h>

G_BEGIN_DECLS

/* ags_fluid_util.c */
extern void ags_fluid_conversion_config();

extern gboolean ags_fluid_ct2hz_tab_initialize;

extern gdouble ags_fluid_ct2hz_tab[AGS_FLUID_CENTS_HZ_SIZE];

extern GMutex ags_fluid_ct2hz_tab_mutex;
G_END_DECLS

/* 4th order */
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>

G_BEGIN_DECLS

/* ags_fluid_interpolate_4th_order_util.c */
extern void ags_fluid_interpolate_4th_order_util_config();

extern gboolean ags_fluid_interp_coeff_4th_order_initialized;

extern gdouble ags_fluid_interp_coeff_4th_order[AGS_FLUID_INTERP_MAX][4];

extern GMutex ags_fluid_interp_coeff_4th_order_mutex;

G_END_DECLS

/* 7th order */
#include <ags/audio/ags_fluid_interpolate_7th_order_util.h>

G_BEGIN_DECLS

/* ags_fluid_interpolate_7th_order_util.c  */
extern void ags_fluid_interpolate_7th_order_util_config();

extern gboolean ags_fluid_interp_coeff_7th_order_initialized;

extern gdouble ags_fluid_interp_coeff_7th_order[AGS_FLUID_INTERP_MAX][AGS_FLUID_SINC_INTERP_ORDER];

extern GMutex ags_fluid_interp_coeff_7th_order_mutex;

G_END_DECLS

#endif /*__LIBAGS_AUDIO_GLOBALS_H__*/
