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

#ifndef __AGS_RECORD_MIDI_AUDIO_H__
#define __AGS_RECORD_MIDI_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_RECORD_MIDI_AUDIO                (ags_record_midi_audio_get_type())
#define AGS_RECORD_MIDI_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECORD_MIDI_AUDIO, AgsRecordMidiAudio))
#define AGS_RECORD_MIDI_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECORD_MIDI_AUDIO, AgsRecordMidiAudio))
#define AGS_IS_RECORD_MIDI_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECORD_MIDI_AUDIO))
#define AGS_IS_RECORD_MIDI_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECORD_MIDI_AUDIO))
#define AGS_RECORD_MIDI_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECORD_MIDI_AUDIO, AgsRecordMidiAudioClass))

typedef struct _AgsRecordMidiAudio AgsRecordMidiAudio;
typedef struct _AgsRecordMidiAudioClass AgsRecordMidiAudioClass;

struct _AgsRecordMidiAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *playback;

  AgsPort *record;
  AgsPort *filename;

  AgsPort *division;
  AgsPort *tempo;
  AgsPort *bpm;
};

struct _AgsRecordMidiAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_record_midi_audio_get_type();

AgsRecordMidiAudio* ags_record_midi_audio_new();

#endif /*__AGS_RECORD_MIDI_AUDIO_H__*/
