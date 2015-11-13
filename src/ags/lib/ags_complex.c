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

#include <ags/lib/ags_complex.h>

/**
 * SECTION:ags_complex
 * @short_description: Boxed type of complex
 * @title: AgsComplex
 * @section_id:
 * @include: ags/lib/ags_complex.h
 *
 * GBoxed of complex data type.
 */

GType
ags_complex_get_type(void)
{
  static volatile gsize ags_define_type_id__volatile = 0;

  if(g_once_init_enter(&ags_define_type_id__volatile)){
    GType ags_define_type_id =
      g_boxed_type_register_static(g_intern_static_string ("AgsComplex\0"),
				   (GBoxedCopyFunc) ags_complex_copy,
				   (GBoxedFreeFunc) ags_complex_free);
    
    g_once_init_leave(&ags_define_type_id__volatile, ags_define_type_id);
  }
  
  return(ags_define_type_id__volatile);
}

/**
 * ags_complex_alloc:
 *
 * Allocate #AgsComplex
 *
 * Returns: a new #AgsComplex
 *
 * Since: 0.7.1
 */
AgsComplex*
ags_complex_alloc()
{
  AgsComplex *ptr;

  ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  *ptr[0] = 0;
  *ptr[1] = 0;

  return(ptr);
}

/**
 * ags_complex_copy:
 * @ptr: the original #AgsCopy
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsComplex
 *
 * Since: 0.7.1
 */
gpointer
ags_complex_copy(AgsComplex *ptr)
{
  AgsComplex *new_ptr;
  
  new_ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  *new_ptr[0] = *ptr[0];
  *new_ptr[1] = *ptr[1];

  return(new_ptr);
}

/**
 * ags_complex_free:
 * @ptr: the #AgsComplex
 *
 * Frees the memory of @ptr.
 *
 * Since: 0.7.1
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
 * Since: 0.7.1
 */
complex
ags_complex_get(AgsComplex *ptr)
{
  complex z;

  z = *ptr[0] + I * *ptr[1];

  return(z);
}

/**
 * ags_complex_set:
 * @ptr: the #AgsComplex
 * @z: the complex data to set
 * 
 * Set complex number.
 *
 * Since: 0.7.1
 */
void
ags_complex_set(AgsComplex *ptr, complex z)
{
  *ptr[0] = creal(z);
  *ptr[1] = cimag(z);
}

