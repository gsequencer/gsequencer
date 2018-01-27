/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_CAPTURE_SOUND_AUDIO_RUN_H__
#define __AGS_CAPTURE_SOUND_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN                (ags_capture_sound_audio_run_get_type())
#define AGS_CAPTURE_SOUND_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN, AgsCaptureSoundAudioRun))
#define AGS_CAPTURE_SOUND_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN, AgsCaptureSoundAudioRun))
#define AGS_IS_CAPTURE_SOUND_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN))
#define AGS_IS_CAPTURE_SOUND_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN))
#define AGS_CAPTURE_SOUND_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN, AgsCaptureSoundAudioRunClass))

typedef struct _AgsCaptureSoundAudioRun AgsCaptureSoundAudioRun;
typedef struct _AgsCaptureSoundAudioRunClass AgsCaptureSoundAudioRunClass;

struct _AgsCaptureSoundAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  void *file_buffer;

  GObject *audio_file;
};

struct _AgsCaptureSoundAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_capture_sound_audio_run_get_type();

AgsCaptureSoundAudioRun* ags_capture_sound_audio_run_new();

#endif /*__AGS_CAPTURE_SOUND_AUDIO_RUN_H__*/
