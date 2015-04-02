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

#ifndef __AGS_FFPLAYER_BULK_INPUT_H__
#define __AGS_FFPLAYER_BULK_INPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_effect_bulk.h>

#define AGS_TYPE_FFPLAYER_BULK_INPUT                (ags_ffplayer_bulk_input_get_type())
#define AGS_FFPLAYER_BULK_INPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInput))
#define AGS_FFPLAYER_BULK_INPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInputClass))
#define AGS_IS_FFPLAYER_BULK_INPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER_BULK_INPUT))
#define AGS_IS_FFPLAYER_BULK_INPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FFPLAYER_BULK_INPUT))
#define AGS_FFPLAYER_BULK_INPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FFPLAYER_BULK_INPUT, AgsFFPlayerBulkInputClass))

#define AGS_FFPLAYER_BULK_INPUT_DEFAULT_VERSION "0.4.3\0"
#define AGS_FFPLAYER_BULK_INPUT_DEFAULT_BUILD_ID "CEST 20-03-2015 08:24\0"

typedef struct _AgsFFPlayerBulkInput AgsFFPlayerBulkInput;
typedef struct _AgsFFPlayerBulkInputClass AgsFFPlayerBulkInputClass;

typedef enum{
  AGS_FFPLAYER_BULK_INPUT_CONNECTED        = 1,
}AgsFFPlayerBulkInputFlags;

struct _AgsFFPlayerBulkInput
{
  AgsEffectBulk effect_bulk;
};

struct _AgsFFPlayerBulkInputClass
{
  AgsEffectBulkClass effect_bulk;
};

GType ags_ffplayer_bulk_input_get_type(void);

AgsFFPlayerBulkInput* ags_ffplayer_bulk_input_new(AgsAudio *audio,
						  GType channel_type);

#endif /*__AGS_FFPLAYER_BULK_INPUT_H__*/
