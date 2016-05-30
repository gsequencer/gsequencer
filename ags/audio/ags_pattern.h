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

#ifndef __AGS_PATTERN_H__
#define __AGS_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_timestamp_thread.h>
#include <ags/thread/ags_timestamp.h>

#define AGS_TYPE_PATTERN                (ags_pattern_get_type())
#define AGS_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PATTERN, AgsPattern))
#define AGS_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PATTERN, AgsPatternClass))
#define AGS_IS_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN))
#define AGS_IS_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN))
#define AGS_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PATTERN, AgsPatternClass))

#define AGS_PATTERN_DEFAULT_BPM (120.0)

#define AGS_PATTERN_TICS_PER_BEAT (1.0)
#define AGS_PATTERN_MINIMUM_NOTE_LENGTH (1.0 / 16.0)
#define AGS_PATTERN_MAXIMUM_NOTE_LENGTH (16.0)

#define AGS_PATTERN_DEFAULT_LENGTH (65535.0 / AGS_PATTERN_TICS_PER_BEAT - AGS_PATTERN_MAXIMUM_NOTE_LENGTH)
#define AGS_PATTERN_DEFAULT_JIFFIE (60.0 / AGS_PATTERN_DEFAULT_BPM / AGS_PATTERN_TICS_PER_BEAT)
#define AGS_PATTERN_DEFAULT_DURATION (AGS_PATTERN_DEFAULT_LENGTH * AGS_PATTERN_DEFAULT_JIFFIE * AGS_MICROSECONDS_PER_SECOND)

typedef struct _AgsPattern AgsPattern;
typedef struct _AgsPatternClass AgsPatternClass;

struct _AgsPattern
{
  GObject object;

  GObject *timestamp;

  guint dim[3];
  guint ***pattern;

  GObject *port;

  guint i;
  guint j;
  guint bit;
};

struct _AgsPatternClass
{
  GObjectClass object;
};

GType ags_pattern_get_type();

GList* ags_pattern_find_near_timestamp(GList *pattern, GObject *timestamp);

void ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint length);

gboolean ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit);
void ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit);

AgsPattern* ags_pattern_new();

#endif /*__AGS_PATTERN_H__*/
