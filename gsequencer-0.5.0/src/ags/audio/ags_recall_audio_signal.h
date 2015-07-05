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

#ifndef __AGS_RECALL_AUDIO_SIGNAL_H__
#define __AGS_RECALL_AUDIO_SIGNAL_H__
 
#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>

#define AGS_TYPE_RECALL_AUDIO_SIGNAL                (ags_recall_audio_signal_get_type())
#define AGS_RECALL_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_AUDIO_SIGNAL, AgsRecallAudioSignal))
#define AGS_RECALL_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_AUDIO_SIGNAL, AgsRecallAudioSignalClass))
#define AGS_IS_RECALL_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_AUDIO_SIGNAL))
#define AGS_IS_RECALL_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_AUDIO_SIGNAL))
#define AGS_RECALL_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_AUDIO_SIGNAL, AgsRecallAudioSignalClass))

typedef struct _AgsRecallAudioSignal AgsRecallAudioSignal;
typedef struct _AgsRecallAudioSignalClass AgsRecallAudioSignalClass;

typedef enum{
  AGS_RECALL_AUDIO_SIGNAL_INITIAL_RUN      = 1,
}AgsRecallAudioSignalFlags;

struct _AgsRecallAudioSignal
{
  AgsRecall recall;

  guint flags;

  guint audio_channel;

  AgsAudioSignal *destination;

  AgsAudioSignal *source;
};

struct _AgsRecallAudioSignalClass
{
  AgsRecallClass recall;
};

GType ags_recall_audio_signal_get_type();

AgsRecallAudioSignal* ags_recall_audio_signal_new(AgsAudioSignal *destination,
						  AgsAudioSignal *source,
						  AgsDevout *devout);

#endif /*__AGS_RECALL_AUDIO_SIGNAL_H__*/

