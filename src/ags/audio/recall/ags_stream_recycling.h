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

#ifndef __AGS_STREAM_RECYCLING_H__
#define __AGS_STREAM_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_recycling.h>

#define AGS_TYPE_STREAM_RECYCLING                (ags_stream_recycling_get_type())
#define AGS_STREAM_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecycling))
#define AGS_STREAM_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecyclingClass))
#define AGS_IS_STREAM_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_RECYCLING))
#define AGS_IS_STREAM_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_RECYCLING))
#define AGS_STREAM_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecyclingClass))

typedef struct _AgsStreamRecycling AgsStreamRecycling;
typedef struct _AgsStreamRecyclingClass AgsStreamRecyclingClass;

struct _AgsStreamRecycling
{
  AgsRecallRecycling recall_recycling;
};

struct _AgsStreamRecyclingClass
{
  AgsRecallRecyclingClass recall_recycling;
};

GType ags_stream_recycling_get_type();

AgsStreamRecycling* ags_stream_recycling_new(AgsRecycling *recycling);

#endif /*__AGS_STREAM_RECYCLING_H__*/
