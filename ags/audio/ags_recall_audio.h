/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_RECALL_AUDIO_H__
#define __AGS_RECALL_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_AUDIO                (ags_recall_audio_get_type())
#define AGS_RECALL_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_AUDIO, AgsRecallAudio))
#define AGS_RECALL_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_AUDIO, AgsRecallAudioClass))
#define AGS_IS_RECALL_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_AUDIO))
#define AGS_IS_RECALL_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_AUDIO))
#define AGS_RECALL_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_AUDIO, AgsRecallAudioClass))

typedef struct _AgsRecallAudio AgsRecallAudio;
typedef struct _AgsRecallAudioClass AgsRecallAudioClass;

struct _AgsRecallAudio
{
  AgsRecall recall;

  guint flags;

  AgsAudio *audio;
};

struct _AgsRecallAudioClass
{
  AgsRecallClass recall;
};

GType ags_recall_audio_get_type();

AgsAudio* ags_recall_audio_get_audio(AgsRecallAudio *recall_audio);
void ags_recall_audio_set_audio(AgsRecallAudio *recall_audio,
				AgsAudio *audio);

AgsRecallAudio* ags_recall_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_RECALL_AUDIO_H__*/
