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

#ifndef __AGS_ALSA_HDA_CONTROL_UTIL_H__
#define __AGS_ALSA_HDA_CONTROL_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_END_DECLS

void ags_alsa_hda_control_util_hda_send_verb(gchar *hwdep,
					     gint nid, gint verb,
					     GValue *parameter,
					     GError **error);

G_END_DECLS

#endif /*__AGS_ALSA_HDA_CONTROL_UTIL_H__*/
