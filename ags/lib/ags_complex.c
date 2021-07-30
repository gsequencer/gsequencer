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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_complex = 0;

    ags_type_complex =
      g_boxed_type_register_static("AgsComplex",
				   (GBoxedCopyFunc) ags_complex_copy,
				   (GBoxedFreeFunc) ags_complex_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_complex);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_complex_alloc:
 *
 * Allocate #AgsComplex-struct
 *
 * Returns: a new #AgsComplex-struct
 *
 * Since: 3.0.0
 */
AgsComplex*
ags_complex_alloc()
{
  AgsComplex *ptr;

  ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  ptr[0].real = 0.0;
  ptr[0].imag = 0.0;

  return(ptr);
}

/**
 * ags_complex_copy:
 * @ptr: the original #AgsComplex-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsComplex-struct
 *
 * Since: 3.0.0
 */
gpointer
ags_complex_copy(AgsComplex *ptr)
{
  AgsComplex *new_ptr;
  
  new_ptr = (AgsComplex *) malloc(sizeof(AgsComplex));
  
  new_ptr->real = ptr->real;
  new_ptr->imag = ptr->imag;

  return(new_ptr);
}

/**
 * ags_complex_free:
 * @ptr: the #AgsComplex-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
double complex
ags_complex_get(AgsComplex *ptr)
{
  double _Complex z;

  z = ptr->real + I * ptr->imag;

  return(z);
}

/**
 * ags_complex_set:
 * @ptr: the #AgsComplex-struct
 * @z: the complex data to set
 * 
 * Set complex number.
 *
 * Since: 3.0.0
 */
void
ags_complex_set(AgsComplex *ptr, double _Complex z)
{
  ptr->real = creal(z);
  ptr->imag = cimag(z);
}

/**
 * ags_complex_get:
 * @ptr: the #AgsComplex-struct
 * @real: (out): the real part
 * @imag: (out): the imaginary part
 * 
 * Get complex number.
 *
 * Returns: number as complex data type
 *
 * Since: 3.7.11
 */
void
ags_complex_get_term(AgsComplex *ptr,
		     gdouble *real,
		     gdouble *imag)
{
  if(real != NULL){
    real[0] = ptr->real;
  }

  if(imag != NULL){
    imag[0] = ptr->imag;
  }
}

/**
 * ags_complex_set:
 * @ptr: the #AgsComplex-struct
 * @real: the real part
 * @imag: the imaginary part
 * 
 * Set complex number.
 *
 * Since: 3.7.11
 */
void ags_complex_set_term(AgsComplex *ptr,
			  gdouble real,
			  gdouble imag)
{
  ptr->real = real;
  ptr->imag = imag;
}
