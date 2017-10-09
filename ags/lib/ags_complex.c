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

#include <ags/lib/ags_complex.h>

#include <stdlib.h>

/**
 * SECTION:ags_complex
 * @short_description: Boxed type of complex
 * @title: AgsComplex
 * @section_id:
 * @include: ags/lib/ags_complex.h
 *
 * Boxed type of complex data type.
 */

GType
ags_complex_get_type(void)
{
  static GType ags_type_complex = 0;

  if(!ags_type_complex){
    ags_type_complex =
      g_boxed_type_register_static("AgsComplex",
				   (GBoxedCopyFunc) ags_complex_copy,
				   (GBoxedFreeFunc) ags_complex_free);
  }
  
  return(ags_type_complex);
}

/**
 * ags_complex_alloc:
 *
 * Allocate #AgsComplex
 *
 * Returns: a new #AgsComplex
 *
 * Since: 1.0.0
 */
AgsComplex*
ags_complex_alloc()
{
  AgsComplex *ptr;

  ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  ptr[0][0] = 0.0;
  ptr[0][1] = 0.0;

  return(ptr);
}

/**
 * ags_complex_copy:
 * @ptr: the original #AgsComplex
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsComplex
 *
 * Since: 1.0.0
 */
gpointer
ags_complex_copy(AgsComplex *ptr)
{
  AgsComplex *new_ptr;
  
  new_ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  new_ptr[0][0] = ptr[0][0];
  new_ptr[0][1] = ptr[0][1];

  return(new_ptr);
}

/**
 * ags_complex_free:
 * @ptr: the #AgsComplex
 *
 * Frees the memory of @ptr.
 *
 * Since: 1.0.0
 */
void
ags_complex_free(AgsComplex *ptr)
{
  g_free(ptr);
}

/**
 * ags_complex_get:
 * @ptr: the #AgsComplex
 * 
 * Get complex number.
 *
 * Returns: number as complex data type
 *
 * Since: 1.0.0
 */
complex
ags_complex_get(AgsComplex *ptr)
{
  complex z;

  z = ptr[0][0] + I * ptr[0][1];

  return(z);
}

/**
 * ags_complex_set:
 * @ptr: the #AgsComplex
 * @z: the complex data to set
 * 
 * Set complex number.
 *
 * Since: 1.0.0
 */
void
ags_complex_set(AgsComplex *ptr, complex z)
{
  ptr[0][0] = creal(z);
  ptr[0][1] = cimag(z);
}

