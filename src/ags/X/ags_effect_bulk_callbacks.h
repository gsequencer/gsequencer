/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_EFFECT_BULK_CALLBACKS_H__
#define __AGS_EFFECT_BULK_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

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

void ags_effect_bulk_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer);

#endif /*__AGS_EFFECT_BULK_CALLBACKS_H__*/
