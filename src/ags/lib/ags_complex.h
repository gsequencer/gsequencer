/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_COMPLEX__
#define __AGS_COMPLEX__

#include <glib.h>
#include <glib-object.h>

#include <complex.h>

#define AGS_TYPE_COMPLEX                (ags_complex_get_type())

typedef long double AgsComplex[2];

GType ags_complex_get_type(void);

AgsComplex* ags_complex_alloc();

gpointer ags_complex_copy(AgsComplex *ptr);
void ags_complex_free(AgsComplex *ptr);

complex ags_complex_get(AgsComplex *ptr);
void ags_complex_set(AgsComplex *ptr, complex z);

#endif /*__AGS_COMPLEX__*/
