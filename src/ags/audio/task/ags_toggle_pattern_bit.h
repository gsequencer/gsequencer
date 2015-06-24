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

#ifndef __AGS_TOGGLE_PATTERN_BIT_H__
#define __AGS_TOGGLE_PATTERN_BIT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_pattern.h>

#define AGS_TYPE_TOGGLE_PATTERN_BIT                (ags_toggle_pattern_bit_get_type())
#define AGS_TOGGLE_PATTERN_BIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TOGGLE_PATTERN_BIT, AgsTogglePatternBit))
#define AGS_TOGGLE_PATTERN_BIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TOGGLE_PATTERN_BIT, AgsTogglePatternBitClass))
#define AGS_IS_TOGGLE_PATTERN_BIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TOGGLE_PATTERN_BIT))
#define AGS_IS_TOGGLE_PATTERN_BIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TOGGLE_PATTERN_BIT))
#define AGS_TOGGLE_PATTERN_BIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TOGGLE_PATTERN_BIT, AgsTogglePatternBitClass))

typedef struct _AgsTogglePatternBit AgsTogglePatternBit;
typedef struct _AgsTogglePatternBitClass AgsTogglePatternBitClass;

struct _AgsTogglePatternBit
{
  AgsTask task;
  
  AgsPattern *pattern;
  guint line;

  guint index_i;
  guint index_j;
  guint bit;
};

struct _AgsTogglePatternBitClass
{
  AgsTaskClass task;

  void (*refresh_gui)(AgsTogglePatternBit *toggle_pattern_bit);
};

GType ags_toggle_pattern_bit_get_type();

void ags_toggle_pattern_bit_refresh_gui(AgsTogglePatternBit *toggle_pattern_bit);

AgsTogglePatternBit* ags_toggle_pattern_bit_new(AgsPattern *pattern,
						guint line,
						guint index_i, guint index_j,
						guint bit);

#endif /*__AGS_TOGGLE_PATTERN_BIT_H__*/
