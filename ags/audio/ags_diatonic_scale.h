/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_DIATONIC_SCALE_H__
#define __AGS_DIATONIC_SCALE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_DIATONIC_SCALE         (ags_diatonic_scale_get_type())

typedef struct _AgsDiatonicScale AgsDiatonicScale;

struct _AgsDiatonicScale
{
  //empty
};

GType ags_diatonic_scale_get_type(void);

guint ags_diatonic_scale_note_to_midi_key(gchar *note,
					  glong *key);
guint ags_diatonic_scale_midi_key_to_note(glong key,
					  gchar **note);

G_END_DECLS

#endif /*__AGS_DIATONIC_SCALE_H__*/
