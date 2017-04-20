/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_PLAY_DSSI_AUDIO_H__
#define __AGS_PLAY_DSSI_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#include <dssi.h>

#define AGS_TYPE_PLAY_DSSI_AUDIO                (ags_play_dssi_audio_get_type())
#define AGS_PLAY_DSSI_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudio))
#define AGS_PLAY_DSSI_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudio))
#define AGS_IS_PLAY_DSSI_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_DSSI_AUDIO))
#define AGS_IS_PLAY_DSSI_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_DSSI_AUDIO))
#define AGS_PLAY_DSSI_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudioClass))

typedef struct _AgsPlayDssiAudio AgsPlayDssiAudio;
typedef struct _AgsPlayDssiAudioClass AgsPlayDssiAudioClass;

struct _AgsPlayDssiAudio
{
  AgsRecallAudio recall_audio;

  gchar *filename;
  gchar *effect;
  unsigned long index;

  unsigned long bank;
  unsigned long program;
  
  DSSI_Descriptor *plugin_descriptor;

  unsigned long *input_port;
  unsigned long input_lines;

  unsigned long *output_port;
  unsigned long output_lines;
};

struct _AgsPlayDssiAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_play_dssi_audio_get_type();

void ags_play_dssi_audio_load(AgsPlayDssiAudio *play_dssi_audio);

GList* ags_play_dssi_audio_load_ports(AgsPlayDssiAudio *play_dssi_audio);
void ags_play_dssi_audio_load_conversion(AgsPlayDssiAudio *play_dssi_audio,
					 GObject *port,
					 gpointer port_descriptor);

GList* ags_play_dssi_audio_find(GList *recall,
				gchar *filename, gchar *effect);

AgsPlayDssiAudio* ags_play_dssi_audio_new();

#endif /*__AGS_PLAY_DSSI_AUDIO_H__*/
