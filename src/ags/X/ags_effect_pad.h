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

#ifndef __AGS_EFFECT_PAD_H__
#define __AGS_EFFECT_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_EFFECT_PAD                (ags_effect_pad_get_type())
#define AGS_EFFECT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_PAD, AgsEffectPad))
#define AGS_EFFECT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_PAD, AgsEffectPadClass))
#define AGS_IS_EFFECT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_PAD))
#define AGS_IS_EFFECT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_PAD))
#define AGS_EFFECT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_PAD, AgsEffectPadClass))

#define AGS_EFFECT_PAD_DEFAULT_VERSION "0.4.3\0"
#define AGS_EFFECT_PAD_DEFAULT_BUILD_ID "CEST 20-03-2015 08:24\0"

#define AGS_EFFECT_PAD_COLUMNS_COUNT (2)

typedef struct _AgsEffectPad AgsEffectPad;
typedef struct _AgsEffectPadClass AgsEffectPadClass;

typedef enum{
  AGS_EFFECT_PAD_CONNECTED           = 1,
  AGS_EFFECT_PAD_SHOW_GROUPING       = 1 << 1,
  AGS_EFFECT_PAD_GROUP_ALL           = 1 << 2,
  AGS_EFFECT_PAD_GROUP_LINE          = 1 << 3,
  AGS_EFFECT_PAD_MAPPED_RECALL       = 1 << 4,
  AGS_EFFECT_PAD_PREMAPPED_RECALL    = 1 << 5,
}AgsEffectPadFlags;

struct _AgsEffectPad
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsChannel *channel;
  
  guint cols;
  GtkTable *table;
};

struct _AgsEffectPadClass
{
  GtkVBoxClass vbox;

  void (*resize_lines)(AgsEffectPad *effect_pad, GType line_type,
		       guint audio_channels, guint audio_channels_old);
};

GType ags_effect_pad_get_type(void);

void ags_effect_pad_resize_lines(AgsEffectPad *effect_pad, GType line_type,
				 guint audio_channels, guint audio_channels_old);

AgsEffectPad* ags_effect_pad_new(AgsChannel *channel);

#endif /*__AGS_EFFECT_PAD_H__*/
