/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags-lib/lib/ags_math.h>

#include <stdlib.h>

gboolean
is_prim(guint x)
{
  guint i;

  if (x == 2){
    return(1);
  }
  
  for (i = 3; i <= sqrt(x); i += 2){
    if (x % i == 0){
      return 0;
    }
  }

  return 1;
}

void
prim_factors(guint x,
	     guint **x_factors, guint *count)
{
  guint *factors;
  guint current_x;
  guint i, n_factors;

  if(x < 2){
    *x_factors = NULL;
    *count = 0;
  }

  current_x = x;

  factors = NULL;
  n_factors = 0;

  for (i = 2; current_x != 0 && current_x - i != 0; ){
    if(!is_prim(i)){
      i++;
      continue;
    }

    if ((current_x % i) == 0){
      current_x /= i;

      n_factors++;
      factors = (guint *) realloc(factors, n_factors * sizeof(guint));
      factors[n_factors - 1] = i;
    }else{
      i++;
    }
  }

  if(current_x != 0){
    n_factors++;
    factors = (guint *) realloc(factors, n_factors * sizeof(guint));
    factors[n_factors - 1] = i;
  }

  *x_factors = factors;
  *count = n_factors;
}

guint
smallest_common_factor(guint a, guint b)
{
  guint result;
  guint *a_prims, *b_prims;
  guint count_a, count_b;
  guint i0, i1;

  result = 1;

  prim_factors(a, &a_prims, &count_a);
  prim_factors(b, &b_prims, &count_b);

  i0 = 0;
  i1 = 0;

  while(i0 < count_a && i1 < count_b){
    if(a_prims[i0] == b_prims[i1]){
      result *= a_prims[i0];
      
      i0++;
      i1++;
    }else if(a_prims[i0] > b_prims[i1]){
      while(i1 < count_b && a_prims[i0] > b_prims[i1]){
	result *= b_prims[i1];

	i1++;
      }
    }else{
      result *= a_prims[i0];
      i0++;
    }
  }

  while(i0 != count_a){
    result *= a_prims[i0];

    i0++;
  }

  while(i1 < count_b){
    result *= b_prims[i0];

    i1++;
  }

  if(result == 1 && (a != 1 || b != 1)){
    result = 0;
  }

  return (result);
}
