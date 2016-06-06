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

