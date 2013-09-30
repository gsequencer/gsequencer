/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_ADD_AUDIO_SIGNAL_H__
#define __AGS_ADD_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_ADD_AUDIO_SIGNAL                (ags_add_audio_signal_get_type())
#define AGS_ADD_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_AUDIO_SIGNAL, AgsAddAudioSignal))
#define AGS_ADD_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_AUDIO_SIGNAL, AgsAddAudioSignalClass))
#define AGS_IS_ADD_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_AUDIO_SIGNAL))
#define AGS_IS_ADD_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_AUDIO_SIGNAL))
#define AGS_ADD_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_AUDIO_SIGNAL, AgsAddAudioSignalClass))

typedef struct _AgsAddAudioSignal AgsAddAudioSignal;
typedef struct _AgsAddAudioSignalClass AgsAddAudioSignalClass;

struct _AgsAddAudioSignal
{
  AgsTask task;

  AgsRecycling *recycling;
  AgsDevout *devout;
  AgsGroupId group_id;
  guint audio_signal_flags;
};

struct _AgsAddAudioSignalClass
{
  AgsTaskClass task;
};

GType ags_add_audio_signal_get_type();

AgsAddAudioSignal* ags_add_audio_signal_new(AgsRecycling *recycling,
					    AgsDevout *devout,
					    AgsGroupId group_id,
					    guint audio_signal_flags);

#endif /*__AGS_ADD_AUDIO_SIGNAL_H__*/
