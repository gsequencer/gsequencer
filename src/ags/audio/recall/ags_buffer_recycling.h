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

#ifndef __AGS_BUFFER_RECYCLING_H__
#define __AGS_BUFFER_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_recycling.h>

#define AGS_TYPE_BUFFER_RECYCLING                (ags_buffer_recycling_get_type())
#define AGS_BUFFER_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUFFER_RECYCLING, AgsBufferRecycling))
#define AGS_BUFFER_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BUFFER_RECYCLING, AgsBufferRecyclingClass))
#define AGS_IS_BUFFER_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BUFFER_RECYCLING))
#define AGS_IS_BUFFER_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BUFFER_RECYCLING))
#define AGS_BUFFER_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BUFFER_RECYCLING, AgsBufferRecyclingClass))

typedef struct _AgsBufferRecycling AgsBufferRecycling;
typedef struct _AgsBufferRecyclingClass AgsBufferRecyclingClass;

struct _AgsBufferRecycling
{
  AgsRecallRecycling recall_recycling;
};

struct _AgsBufferRecyclingClass
{
  AgsRecallRecyclingClass recall_recycling;
};

GType ags_buffer_recycling_get_type();

AgsBufferRecycling* ags_buffer_recycling_new(AgsRecycling *recycling);

#endif /*__AGS_BUFFER_RECYCLING_H__*/
