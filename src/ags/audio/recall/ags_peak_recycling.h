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

#ifndef __AGS_PEAK_RECYCLING_H__
#define __AGS_PEAK_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_recycling.h>

#define AGS_TYPE_PEAK_RECYCLING                (ags_peak_recycling_get_type())
#define AGS_PEAK_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PEAK_RECYCLING, AgsPeakRecycling))
#define AGS_PEAK_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PEAK_RECYCLING, AgsPeakRecyclingClass))
#define AGS_IS_PEAK_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PEAK_RECYCLING))
#define AGS_IS_PEAK_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PEAK_RECYCLING))
#define AGS_PEAK_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PEAK_RECYCLING, AgsPeakRecyclingClass))

typedef struct _AgsPeakRecycling AgsPeakRecycling;
typedef struct _AgsPeakRecyclingClass AgsPeakRecyclingClass;

struct _AgsPeakRecycling
{
  AgsRecallRecycling recall_recycling;
};

struct _AgsPeakRecyclingClass
{
  AgsRecallRecyclingClass recall_recycling;
};

GType ags_peak_recycling_get_type();

AgsPeakRecycling* ags_peak_recycling_new(AgsRecycling *recycling);

#endif /*__AGS_PEAK_RECYCLING_H__*/
