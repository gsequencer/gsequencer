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

#ifndef __AGS_MUTE_AUDIO_RUN_H__
#define __AGS_MUTE_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_run.h>

#define AGS_TYPE_MUTE_AUDIO_RUN                (ags_mute_audio_run_get_type())
#define AGS_MUTE_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MUTE_AUDIO_RUN, AgsMuteAudioRun))
#define AGS_MUTE_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MUTE_AUDIO_RUN, AgsMuteAudioRun))
#define AGS_IS_MUTE_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MUTE_AUDIO_RUN))
#define AGS_IS_MUTE_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MUTE_AUDIO_RUN))
#define AGS_MUTE_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MUTE_AUDIO_RUN, AgsMuteAudioRunClass))

typedef struct _AgsMuteAudioRun AgsMuteAudioRun;
typedef struct _AgsMuteAudioRunClass AgsMuteAudioRunClass;

struct _AgsMuteAudioRun
{
  AgsRecallAudioRun recall_audio_run;
};

struct _AgsMuteAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_mute_audio_run_get_type();

AgsMuteAudioRun* ags_mute_audio_run_new();

#endif /*__AGS_MUTE_AUDIO_RUN_H__*/
