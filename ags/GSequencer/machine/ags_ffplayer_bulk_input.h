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

#ifndef __AGS_FFPLAYER_BULK_INPUT_H__
#define __AGS_FFPLAYER_BULK_INPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_effect_bulk.h>

G_BEGIN_DECLS

#define AGS_TYPE_FFPLAYER_BULK_INPUT                (ags_ffplayer_bulk_input_get_type())
#define AGS_FFPLAYER_BULK_INPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInput))
#define AGS_FFPLAYER_BULK_INPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInputClass))
#define AGS_IS_FFPLAYER_BULK_INPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER_BULK_INPUT))
#define AGS_IS_FFPLAYER_BULK_INPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FFPLAYER_BULK_INPUT))
#define AGS_FFPLAYER_BULK_INPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInputClass))

#define AGS_FFPLAYER_BULK_INPUT_DEFAULT_VERSION "0.4.3"
#define AGS_FFPLAYER_BULK_INPUT_DEFAULT_BUILD_ID "CEST 20-03-2015 08:24"

typedef struct _AgsFFPlayerBulkInput AgsFFPlayerBulkInput;
typedef struct _AgsFFPlayerBulkInputClass AgsFFPlayerBulkInputClass;

typedef enum{
  AGS_FFPLAYER_BULK_INPUT_CONNECTED        = 1,
}AgsFFPlayerBulkInputFlags;

struct _AgsFFPlayerBulkInput
{
  AgsEffectBulk effect_bulk;

  guint mapped_input_pad;
};

struct _AgsFFPlayerBulkInputClass
{
  AgsEffectBulkClass effect_bulk;
};

GType ags_ffplayer_bulk_input_get_type(void);

void ags_ffplayer_bulk_input_input_map_recall(AgsFFPlayerBulkInput *ffplayer_bulk_input,
					      guint audio_channel_start,
					      guint input_pad_start);

AgsFFPlayerBulkInput* ags_ffplayer_bulk_input_new(AgsAudio *audio,
						  GType channel_type);

G_END_DECLS

#endif /*__AGS_FFPLAYER_BULK_INPUT_H__*/
