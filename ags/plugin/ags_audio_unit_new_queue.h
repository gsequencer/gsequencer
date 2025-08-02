/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_NEW_QUEUE_H__
#define __AGS_AUDIO_UNIT_NEW_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-vst.h>

#include <ags/plugin/ags_base_plugin.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_NEW_QUEUE                (ags_audio_unit_new_queue_get_type())
#define AGS_AUDIO_UNIT_NEW_QUEUE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE, AgsAudioUnitNewQueue))
#define AGS_AUDIO_UNIT_NEW_QUEUE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE, AgsAudioUnitNewQueueClass))
#define AGS_IS_AUDIO_UNIT_NEW_QUEUE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE))
#define AGS_IS_AUDIO_UNIT_NEW_QUEUE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE))
#define AGS_AUDIO_UNIT_NEW_QUEUE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE, AgsAudioUnitNewQueueClass))

typedef struct _AgsAudioUnitNewQueue AgsAudioUnitNewQueue;

struct _AgsAudioUnitNewQueue
{
  gboolean in_use;
  
  gint64 creation_timestamp;

  gchar *plugin_name;
  
  gpointer audio_unit;
};

GType ags_audio_unit_new_queue_get_type(void);

AgsAudioUnitNewQueue* ags_audio_unit_new_queue_alloc();
void ags_audio_unit_new_queue_alloc(AgsAudioUnitNewQueue *new_queue);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_NEW_QUEUE_H__*/
