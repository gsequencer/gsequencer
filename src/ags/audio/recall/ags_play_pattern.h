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

#ifndef __AGS_PLAY_PATTERN_H__
#define __AGS_PLAY_PATTERN_H__

#include <glib-object.h>

#include <ags/audio/ags_recall.h>

//#include <ags/audio/recall/ags_delay.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_pattern.h>

#define AGS_TYPE_PLAY_PATTERN             (ags_play_pattern_get_type())
#define AGS_PLAY_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PLAY_PATTERN, AgsPlayPattern))
#define AGS_PLAY_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAY_PATTERN, AgsPlayPattern))

typedef struct _AgsPlayPattern AgsPlayPattern;
typedef struct _AgsPlayPatternClass AgsPlayPatternClass;

struct _AgsPlayPattern{
  AgsRecall recall;

  //  AgsDelay *delay;

  AgsChannel *channel;
  AgsPattern *pattern;

  guint i;
  guint j;
  guint bit;
  guint length;

  gboolean loop;
};

struct _AgsPlayPatternClass{
  AgsRecallClass recall;
};

GType ags_play_pattern_get_type();

void ags_play_pattern_connect(AgsPlayPattern *play_pattern);

//AgsPlayPattern* ags_play_pattern_new(AgsChannel *channel, AgsDelay *delay);

#endif /*__AGS_PLAY_PATTERN_H__*/
