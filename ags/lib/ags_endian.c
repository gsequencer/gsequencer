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

#include <ags/lib/ags_endian.h>

gfloat
ags_endian_swap_float(gfloat x)
{
  char c;
  union{
    gfloat float_data;
    char char_data[4];
  }data;

  data.float_data = x;

  c = data.char_data[0];  
  data.char_data[0] = data.char_data[3];
  data.char_data[3] = c;

  c = data.char_data[1];
  data.char_data[1] = data.char_data[2];
  data.char_data[2] = c;
  
  return(data.float_data);
}

