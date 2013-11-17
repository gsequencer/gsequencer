/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_RECALL_FACTORY_H__
#define __AGS_RECALL_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_container.h>

#define AGS_TYPE_RECALL_FACTORY                (ags_recall_factory_get_type())
#define AGS_RECALL_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_FACTORY, AgsRecallFactory))
#define AGS_RECALL_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_FACTORY, AgsRecallFactoryClass))
#define AGS_IS_RECALL_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_FACTORY))
#define AGS_IS_RECALL_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_FACTORY))
#define AGS_RECALL_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_FACTORY, AgsRecallFactoryClass))

typedef struct _AgsRecallFactory AgsRecallFactory;
typedef struct _AgsRecallFactoryClass AgsRecallFactoryClass;

typedef enum{
  AGS_RECALL_FACTORY_OUTPUT,
  AGS_RECALL_FACTORY_INPUT,
  AGS_RECALL_FACTORY_REMAP,
  AGS_RECALL_FACTORY_ADD,
  AGS_RECALL_FACTORY_PLAY,
  AGS_RECALL_FACTORY_RECALL
}AgsRecallFactoryCreateFlags;

struct _AgsRecallFactory
{
  GObject object;
};

struct _AgsRecallFactoryClass
{
  GObjectClass object;
};

GType ags_recall_factory_get_type();

GList* ags_recall_factory_create(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 guint create_flags, guint recall_flags);

void ags_recall_factory_remove(AgsAudio *audio,
			       AgsRecallContainer *recall_container);

/*  */
AgsRecallFactory* ags_recall_factory_get_instance();
AgsRecallFactory* ags_recall_factory_new();

#endif /*__AGS_RECALL_FACTORY_H__*/
