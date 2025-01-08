/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_FX_STAR_SYNTH_RECYCLING_H__
#define __AGS_FX_STAR_SYNTH_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>

#include <ags/audio/fx/ags_fx_notation_recycling.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_STAR_SYNTH_RECYCLING                (ags_fx_star_synth_recycling_get_type())
#define AGS_FX_STAR_SYNTH_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_STAR_SYNTH_RECYCLING, AgsFxStarSynthRecycling))
#define AGS_FX_STAR_SYNTH_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_STAR_SYNTH_RECYCLING, AgsFxStarSynthRecyclingClass))
#define AGS_IS_FX_STAR_SYNTH_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_STAR_SYNTH_RECYCLING))
#define AGS_IS_FX_STAR_SYNTH_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_STAR_SYNTH_RECYCLING))
#define AGS_FX_STAR_SYNTH_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_STAR_SYNTH_RECYCLING, AgsFxStarSynthRecyclingClass))

typedef struct _AgsFxStarSynthRecycling AgsFxStarSynthRecycling;
typedef struct _AgsFxStarSynthRecyclingClass AgsFxStarSynthRecyclingClass;

struct _AgsFxStarSynthRecycling
{
  AgsFxNotationRecycling fx_notation_recycling;
};

struct _AgsFxStarSynthRecyclingClass
{
  AgsFxNotationRecyclingClass fx_notation_recycling;
};

GType ags_fx_star_synth_recycling_get_type();

/*  */
AgsFxStarSynthRecycling* ags_fx_star_synth_recycling_new(AgsRecycling *recycling);

G_END_DECLS

#endif /*__AGS_FX_STAR_SYNTH_RECYCLING_H__*/
