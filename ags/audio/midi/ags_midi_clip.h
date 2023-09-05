/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_MIDI_CLIP_UTIL_H__
#define __AGS_MIDI_CLIP_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CLIP_UTIL         (ags_midi_clip_util_get_type())

typedef struct _AgsMidiClipUtil AgsMidiClipUtil;

struct _AgsMidiClipUtil
{
  guint major;
  guint minor;
};

GType ags_midi_clip_util_get_type(void);

AgsMidiClipUtil* ags_midi_clip_util_alloc();
void ags_midi_clip_util_free(AgsMidiClipUtil *midi_clip_util);

AgsMidiClipUtil* ags_midi_clip_util_copy(AgsMidiClipUtil *midi_clip_util);

G_END_DECLS

#endif /*__AGS_MIDI_CLIP_UTIL_H__*/
