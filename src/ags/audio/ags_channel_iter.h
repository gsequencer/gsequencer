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

#ifndef __AGS_CHANNEL_ITER_H__
#define __AGS_CHANNEL_ITER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_channel.h>

typedef struct _AgsChannelIter AgsChannelIter;

typedef enum{
  AGS_CHANNEL_ITER_LEVEL_STRICT,
  AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES,
  AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_ROOT,
  AGS_CHANNEL_ITER_DIRECTION_LEAFES_TO_ROOT,
  AGS_CHANNEL_ITER_DIRECTION_ROOT_TO_LEAFES,
}AgsChannelIterFlags;

struct _AgsChannelIter
{
  AgsChannel *current_start;

  AgsChannelIter *parent;
  GList *children;
  AgsChannelIter *current_iter;

  AgsChannel *current;
};

AgsChannelIter* ags_channel_iter_alloc(AgsChannel *start);

void ags_channel_iter_free(AgsChannelIter *iter);

AgsChannel* ags_channel_iter_prev(AgsChannelIter *iter, guint mode);
AgsChannel* ags_channel_iter_next(AgsChannelIter *iter, guint mode);

#endif /*__AGS_CHANNEL_ITER_H__*/

