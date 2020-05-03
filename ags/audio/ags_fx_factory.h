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

#ifndef __AGS_FX_FACTORY_H__
#define __AGS_FX_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>

G_BEGIN_DECLS

/**
 * AgsFxFactoryCreateFlags:
 * @AGS_FX_FACTORY_OUTPUT: output related
 * @AGS_FX_FACTORY_INPUT: input related
 * @AGS_FX_FACTORY_REMAP: remap
 * @AGS_FX_FACTORY_ADD: do add
 * @AGS_FX_FACTORY_PLAY: bounded to play
 * @AGS_FX_FACTORY_RECALL: bounded to recall
 * @AGS_FX_FACTORY_BULK: operates on bulk mode
 * @AGS_FX_FACTORY_LIVE: is live version
 * 
 * Enum values controlling instantiating the recall implementation.
 */
typedef enum{
  AGS_FX_FACTORY_OUTPUT    = 1,
  AGS_FX_FACTORY_INPUT     = 1 << 1,
  AGS_FX_FACTORY_REMAP     = 1 << 2,
  AGS_FX_FACTORY_ADD       = 1 << 3,
  AGS_FX_FACTORY_PLAY      = 1 << 4,
  AGS_FX_FACTORY_RECALL    = 1 << 5,
  AGS_FX_FACTORY_BULK      = 1 << 6,
  AGS_FX_FACTORY_LIVE      = 1 << 7,
}AgsFxFactoryCreateFlags;

GList* ags_fx_factory_create(AgsAudio *audio,
			     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect,
			     guint start_audio_channel, guint stop_audio_channel,
			     guint start_pad, guint stop_pad,
			     gint position,
			     guint create_flags, guint recall_flags);

G_END_DECLS

#endif /*__AGS_FX_FACTORY_H__*/
