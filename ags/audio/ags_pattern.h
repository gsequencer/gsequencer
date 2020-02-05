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

#ifndef __AGS_PATTERN_H__
#define __AGS_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_PATTERN                (ags_pattern_get_type())
#define AGS_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PATTERN, AgsPattern))
#define AGS_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PATTERN, AgsPatternClass))
#define AGS_IS_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN))
#define AGS_IS_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN))
#define AGS_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PATTERN, AgsPatternClass))

#define AGS_PATTERN_GET_OBJ_MUTEX(obj) (&(((AgsPattern *) obj)->obj_mutex))

#define AGS_PATTERN_DEFAULT_BPM (120.0)

#define AGS_PATTERN_TICS_PER_BEAT (1.0)
#define AGS_PATTERN_MINIMUM_NOTE_LENGTH (1.0 / 16.0)
#define AGS_PATTERN_MAXIMUM_NOTE_LENGTH (16.0)

#define AGS_PATTERN_DEFAULT_LENGTH (65535.0 / AGS_PATTERN_TICS_PER_BEAT - AGS_PATTERN_MAXIMUM_NOTE_LENGTH)
#define AGS_PATTERN_DEFAULT_JIFFIE (60.0 / AGS_PATTERN_DEFAULT_BPM / AGS_PATTERN_TICS_PER_BEAT)
#define AGS_PATTERN_DEFAULT_DURATION (AGS_PATTERN_DEFAULT_LENGTH * AGS_PATTERN_DEFAULT_JIFFIE * AGS_USEC_PER_SEC)
#define AGS_PATTERN_DEFAULT_OFFSET (64)

typedef struct _AgsPattern AgsPattern;
typedef struct _AgsPatternClass AgsPatternClass;

/**
 * AgsPatternFlags:
 * @AGS_PATTERN_ADDED_TO_REGISTRY: the pattern was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_PATTERN_CONNECTED: indicates the port was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsPattern by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PATTERN_ADDED_TO_REGISTRY     = 1,
  AGS_PATTERN_CONNECTED             = 1 <<  1,
}AgsPatternFlags;

struct _AgsPattern
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  GObject *channel;

  AgsTimestamp *timestamp;

  guint dim[3];
  guint ***pattern;

  GObject *port;

  guint i;
  guint j;
  guint bit;
};

struct _AgsPatternClass
{
  GObjectClass gobject;
};

GType ags_pattern_get_type();

GRecMutex* ags_pattern_get_obj_mutex(AgsPattern *pattern);

gboolean ags_pattern_test_flags(AgsPattern *pattern, guint flags);
void ags_pattern_set_flags(AgsPattern *pattern, guint flags);
void ags_pattern_unset_flags(AgsPattern *pattern, guint flags);

GList* ags_pattern_find_near_timestamp(GList *pattern, AgsTimestamp *timestamp);

GObject* ags_pattern_get_channel(AgsPattern *pattern);
void ags_pattern_set_channel(AgsPattern *pattern,
			     GObject *channel);

AgsTimestamp* ags_pattern_get_timestamp(AgsPattern *pattern);
void ags_pattern_set_timestamp(AgsPattern *pattern,
			       AgsTimestamp *timestamp);

void ags_pattern_get_dim(AgsPattern *pattern, guint *dim0, guint *dim1, guint *length);
void ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint length);

GObject* ags_pattern_get_port(AgsPattern *pattern);
void ags_pattern_set_port(AgsPattern *pattern,
			  GObject *port);

gboolean ags_pattern_is_empty(AgsPattern *pattern, guint i, guint j);

gboolean ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit);
void ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit);

AgsPattern* ags_pattern_new();

G_END_DECLS

#endif /*__AGS_PATTERN_H__*/
