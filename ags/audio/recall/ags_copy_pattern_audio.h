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

#ifndef __AGS_COPY_PATTERN_AUDIO_H__
#define __AGS_COPY_PATTERN_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_port.h>

G_BEGIN_DECLS

#define AGS_TYPE_COPY_PATTERN_AUDIO                (ags_copy_pattern_audio_get_type())
#define AGS_COPY_PATTERN_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudio))
#define AGS_COPY_PATTERN_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudio))
#define AGS_IS_COPY_PATTERN_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_AUDIO))
#define AGS_IS_COPY_PATTERN_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_AUDIO))
#define AGS_COPY_PATTERN_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudioClass))

#define AGS_COPY_PATTERN_AUDIO_MAX_BANK_INDEX_0 (256.0)
#define AGS_COPY_PATTERN_AUDIO_MAX_BANK_INDEX_1 (256.0)

typedef struct _AgsCopyPatternAudio AgsCopyPatternAudio;
typedef struct _AgsCopyPatternAudioClass AgsCopyPatternAudioClass;

struct _AgsCopyPatternAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *bank_index_0;
  AgsPort *bank_index_1;
};

struct _AgsCopyPatternAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_copy_pattern_audio_get_type();

AgsCopyPatternAudio* ags_copy_pattern_audio_new(AgsAudio *audio,
						guint bank_index_0,
						guint bank_index_1);

G_END_DECLS

#endif /*__AGS_COPY_PATTERN_AUDIO_H__*/
