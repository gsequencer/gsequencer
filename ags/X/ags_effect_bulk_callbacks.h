/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015,2017 Joël Krähemann
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

#ifndef __AGS_EFFECT_BULK_CALLBACKS_H__
#define __AGS_EFFECT_BULK_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/audio/ags_recall.h>

#include <ags/X/ags_effect_bulk.h>

void ags_effect_bulk_add_callback(GtkWidget *button,
				  AgsEffectBulk *effect_bulk);
void ags_effect_bulk_remove_callback(GtkWidget *button,
				     AgsEffectBulk *effect_bulk);

void ags_effect_bulk_plugin_browser_response_callback(GtkDialog *dialog,
						      gint response,
						      AgsEffectBulk *effect_bulk);

void ags_effect_bulk_set_audio_channels_callback(AgsAudio *audio,
						 guint audio_channels,
						 guint audio_channels_old,
						 AgsEffectBulk *effect_bulk);
void ags_effect_bulk_set_pads_callback(AgsAudio *audio,
				       GType channel_type,
				       guint pads,
				       guint pads_old,
				       AgsEffectBulk *effect_bulk);

void ags_effect_bulk_output_port_run_post_callback(AgsRecall *recall,
						   AgsEffectBulk *effect_bulk);

#endif /*__AGS_EFFECT_BULK_CALLBACKS_H__*/
