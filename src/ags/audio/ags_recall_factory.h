/* This file is part of GSequencer.
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
  AGS_RECALL_FACTORY_OUTPUT    = 1,
  AGS_RECALL_FACTORY_INPUT     = 1 << 1,
  AGS_RECALL_FACTORY_REMAP     = 1 << 2,
  AGS_RECALL_FACTORY_ADD       = 1 << 3,
  AGS_RECALL_FACTORY_PLAY      = 1 << 4,
  AGS_RECALL_FACTORY_RECALL    = 1 << 5,
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
