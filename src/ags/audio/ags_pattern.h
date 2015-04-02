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

#ifndef __AGS_PATTERN_H__
#define __AGS_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_PATTERN                (ags_pattern_get_type())
#define AGS_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PATTERN, AgsPattern))
#define AGS_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PATTERN, AgsPatternClass))
#define AGS_IS_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN))
#define AGS_IS_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN))
#define AGS_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PATTERN, AgsPatternClass))

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

AgsPattern* ags_pattern_get_by_timestamp(GList *list, GObject *timestamp);

void ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint lenght);

gboolean ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit);
void ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit);

AgsPattern* ags_pattern_new();

#endif /*__AGS_PATTERN_H__*/
