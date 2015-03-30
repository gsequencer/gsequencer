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

#ifndef __AGS_EFFECT_BULK_H__
#define __AGS_EFFECT_BULK_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_EFFECT_BULK                (ags_effect_bulk_get_type())
#define AGS_EFFECT_BULK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_BULK, AgsEffectBulk))
#define AGS_EFFECT_BULK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_BULK, AgsEffectBulkClass))
#define AGS_IS_EFFECT_BULK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_BULK))
#define AGS_IS_EFFECT_BULK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_BULK))
#define AGS_EFFECT_BULK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_BULK, AgsEffectBulkClass))

#define AGS_EFFECT_BULK_DEFAULT_VERSION "0.4.3\0"
#define AGS_EFFECT_BULK_DEFAULT_BUILD_ID "CEST 20-03-2015 08:24\0"

typedef struct _AgsEffectBulk AgsEffectBulk;
typedef struct _AgsEffectBulkClass AgsEffectBulkClass;

typedef enum{
  AGS_EFFECT_BULK_CONNECTED        = 1,
}AgsEffectBulkFlags;

struct _AgsEffectBulk
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  GType *channel_type;
  AgsAudio *audio;

  GtkTable *table;
};

struct _AgsEffectBulkClass
{
  GtkVBoxClass vbox;

  GList* (*add_effect)(AgsEffectBulk *effect_bulk,
		       gchar *filename,
		       gchar *effect);
  void (*remove_effect)(AgsEffectBulk *effect_bulk,
			guint nth);
};

GType ags_effect_bulk_get_type(void);

GList* ags_effect_bulk_add_effect(AgsEffectBulk *effect_bulk,
				  gchar *filename,
				  gchar *effect);
void ags_effect_bulk_remove_effect(AgsEffectBulk *effect_bulk,
				   guint nth);

AgsEffectBulk* ags_effect_bulk_new(AgsAudio *audio,
				   GType channel_type);

#endif /*__AGS_EFFECT_BULK_H__*/
