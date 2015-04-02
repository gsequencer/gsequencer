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

#ifndef __AGS_DELAY_AUDIO_H__
#define __AGS_DELAY_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_DELAY_AUDIO                (ags_delay_audio_get_type())
#define AGS_DELAY_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_DELAY_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_IS_DELAY_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DELAY_AUDIO))
#define AGS_IS_DELAY_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DELAY_AUDIO))
#define AGS_DELAY_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudioClass))

typedef struct _AgsDelayAudio AgsDelayAudio;
typedef struct _AgsDelayAudioClass AgsDelayAudioClass;

struct _AgsDelayAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *bpm;
  AgsPort *tact;

  AgsPort *notation_delay;
  AgsPort *sequencer_delay;

  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
};

struct _AgsDelayAudioClass
{
  AgsRecallAudioClass recall_audio;

  void (*notation_duration_changed)(AgsDelayAudio *delay_audio);
  void (*sequencer_duration_changed)(AgsDelayAudio *delay_audio);
};

GType ags_delay_audio_get_type();

void ags_delay_audio_notation_duration_changed(AgsDelayAudio *delay_audio);
void ags_delay_audio_sequencer_duration_changed(AgsDelayAudio *delay_audio);

AgsDelayAudio* ags_delay_audio_new();

#endif /*__AGS_DELAY_AUDIO_H__*/
