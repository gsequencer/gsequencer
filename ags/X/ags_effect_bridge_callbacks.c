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

#include <ags/X/ags_effect_bridge_callbacks.h>

void
ags_effect_bridge_set_audio_channels_callback(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old,
					      AgsEffectBridge *effect_bridge)
{
  ags_effect_bridge_resize_audio_channels(effect_bridge,
					  audio_channels, audio_channels_old);
}

void
ags_effect_bridge_set_pads_callback(AgsAudio *audio,
				    GType channel_type,
				    guint pads, guint pads_old,
				    AgsEffectBridge *effect_bridge)
{  
  ags_effect_bridge_resize_pads(effect_bridge,
				channel_type,
				pads, pads_old);
}

