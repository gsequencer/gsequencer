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

#ifndef __AGS_APPLY_SF2_SYNTH_H__
#define __AGS_APPLY_SF2_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_sf2_synth_generator.h>

G_BEGIN_DECLS

#define AGS_TYPE_APPLY_SF2_SYNTH                (ags_apply_sf2_synth_get_type())
#define AGS_APPLY_SF2_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SF2_SYNTH, AgsApplySF2Synth))
#define AGS_APPLY_SF2_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SF2_SYNTH, AgsApplySF2SynthClass))
#define AGS_IS_APPLY_SF2_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SF2_SYNTH))
#define AGS_IS_APPLY_SF2_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SF2_SYNTH))
#define AGS_APPLY_SF2_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SF2_SYNTH, AgsApplySF2SynthClass))

typedef struct _AgsApplySF2Synth AgsApplySF2Synth;
typedef struct _AgsApplySF2SynthClass AgsApplySF2SynthClass;

struct _AgsApplySF2Synth
{
  AgsTask task;

  AgsSF2SynthGenerator *sf2_synth_generator;

  AgsChannel *start_channel;

  gdouble base_note;
  guint count;

  guint requested_frame_count;
};

struct _AgsApplySF2SynthClass
{
  AgsTaskClass task;
};

GType ags_apply_sf2_synth_get_type();

AgsApplySF2Synth* ags_apply_sf2_synth_new(AgsSF2SynthGenerator *sf2_synth_generator,
					  AgsChannel *start_channel,
					  gdouble base_note, guint count);

G_END_DECLS

#endif /*__AGS_APPLY_SF2_SYNTH_H__*/
