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

#ifndef __AGS_EFFECT_LINE_CALLBACKS_H__
#define __AGS_EFFECT_LINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_effect_line.h>

void ags_effect_line_remove_recall_callback(AgsRecall *recall, AgsEffectLine *effect_line);

void ags_effect_line_add_effect_callback(AgsChannel *channel,
					 gchar *filename,
					 gchar *effect,
					 AgsEffectLine *effect_line);
void ags_effect_line_remove_effect_callback(AgsChannel *channel,
					    guint nth,
					    AgsEffectLine *effect_line);

void ags_effect_line_output_port_run_post_callback(AgsRecall *recall,
						   AgsEffectLine *effect_line);

#endif /*__AGS_EFFECT_LINE_CALLBACKS_H__*/
