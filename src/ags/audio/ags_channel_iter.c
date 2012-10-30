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

#include <ags/audio/ags_channel_iter.h>

AgsChannelIter*
ags_channel_iter_alloc(AgsChannel *start)
{
  AgsChannelIter *iter;

  iter = (AgsChannelIter *) malloc(sizeof(AgsChannelIter));

  iter->flags = 0;

  iter->current_start = start;

  iter->parent = NULL;
  iter->children = NULL;

  iter->current = NULL;

  return(iter);
}

gboolean
ags_channel_iter_prev(AgsChannelIter *iter, guint mode)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_channel_iter_next(AgsChannelIter *iter, guint mode)
{
  //TODO:JK: implement me

  return(FALSE);
}
