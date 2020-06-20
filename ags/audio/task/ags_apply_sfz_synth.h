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

#ifndef __AGS_APPLY_SFZ_SYNTH_H__
#define __AGS_APPLY_SFZ_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_sfz_synth_generator.h>

G_BEGIN_DECLS

#define AGS_TYPE_APPLY_SFZ_SYNTH                (ags_apply_sfz_synth_get_type())
#define AGS_APPLY_SFZ_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SFZ_SYNTH, AgsApplySFZSynth))
#define AGS_APPLY_SFZ_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SFZ_SYNTH, AgsApplySFZSynthClass))
#define AGS_IS_APPLY_SFZ_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SFZ_SYNTH))
#define AGS_IS_APPLY_SFZ_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SFZ_SYNTH))
#define AGS_APPLY_SFZ_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SFZ_SYNTH, AgsApplySFZSynthClass))

typedef struct _AgsApplySFZSynth AgsApplySFZSynth;
typedef struct _AgsApplySFZSynthClass AgsApplySFZSynthClass;

struct _AgsApplySFZSynth
{
  AgsTask task;

  AgsSFZSynthGenerator *sfz_synth_generator;

  AgsChannel *start_channel;

  gdouble base_note;
  guint count;

  guint requested_frame_count;
};

struct _AgsApplySFZSynthClass
{
  AgsTaskClass task;
};

GType ags_apply_sfz_synth_get_type();

AgsApplySFZSynth* ags_apply_sfz_synth_new(AgsSFZSynthGenerator *sfz_synth_generator,
					  AgsChannel *start_channel,
					  gdouble base_note, guint count);

G_END_DECLS

#endif /*__AGS_APPLY_SFZ_SYNTH_H__*/
