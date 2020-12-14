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

#include <ags/X/ags_composite_view_callbacks.h>

void
ags_composite_view_vscrollbar_callback(GtkRange *scrollbar,
				       AgsCompositeView *composite_view)
{
  if(composite_view->block_vscrollbar){
    return;
  }

  composite_view->block_vscrollbar = TRUE;
  
  //TODO:JK: implement me

  composite_view->block_vscrollbar = FALSE;
}

void
ags_composite_view_hscrollbar_callback(GtkRange *scrollbar,
				       AgsCompositeView *composite_view)
{
  if(composite_view->block_hscrollbar){
    return;
  }

  composite_view->block_hscrollbar = TRUE;
  
  //TODO:JK: implement me

  composite_view->block_hscrollbar = FALSE;
}
