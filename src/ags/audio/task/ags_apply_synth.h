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

#ifndef __AGS_APPLY_SYNTH_H__
#define __AGS_APPLY_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_APPLY_SYNTH                (ags_apply_synth_get_type())
#define AGS_APPLY_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SYNTH, AgsApplySynth))
#define AGS_APPLY_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SYNTH, AgsApplySynthClass))
#define AGS_IS_APPLY_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SYNTH))
#define AGS_IS_APPLY_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SYNTH))
#define AGS_APPLY_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SYNTH, AgsApplySynthClass))

typedef struct _AgsApplySynth AgsApplySynth;
typedef struct _AgsApplySynthClass AgsApplySynthClass;

typedef enum{
  AGS_APPLY_SYNTH_INVALID,
  AGS_APPLY_SYNTH_SIN,
  AGS_APPLY_SYNTH_SAW,
  AGS_APPLY_SYNTH_SQUARE,
  AGS_APPLY_SYNTH_TRIANGLE,
}AgsApplySynthWave;

struct _AgsApplySynth
{
  AgsTask task;

  AgsChannel *start_channel;
  guint count;

  guint wave;

  guint attack;
  guint frame_count;
  guint frequency;
  guint phase;
  guint start;

  gdouble volume;

  guint loop_start;
  guint loop_end;
};

struct _AgsApplySynthClass
{
  AgsTaskClass task;
};

GType ags_apply_synth_get_type();

AgsApplySynth* ags_apply_synth_new(AgsChannel *start_channel, guint count,
				   guint wave,
				   guint attack, guint frame_count,
				   guint frequency, guint phase, guint start,
				   gdouble volume,
				   guint loop_start, guint loop_end);

#endif /*__AGS_APPLY_SYNTH_H__*/
