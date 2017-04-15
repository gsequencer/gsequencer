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

#ifndef __AGS_PLAY_LV2_AUDIO_H__
#define __AGS_PLAY_LV2_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>

#include <ags/lib/ags_turtle.h>

#include <ags/audio/ags_recall_audio.h>


#define AGS_TYPE_PLAY_LV2_AUDIO                (ags_play_lv2_audio_get_type())
#define AGS_PLAY_LV2_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2Audio))
#define AGS_PLAY_LV2_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2Audio))
#define AGS_IS_PLAY_LV2_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_LV2_AUDIO))
#define AGS_IS_PLAY_LV2_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_LV2_AUDIO))
#define AGS_PLAY_LV2_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2AudioClass))

typedef struct _AgsPlayLv2Audio AgsPlayLv2Audio;
typedef struct _AgsPlayLv2AudioClass AgsPlayLv2AudioClass;

typedef enum{
  AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT   = 1,
  AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT    = 1 <<  1,
  AGS_PLAY_LV2_AUDIO_HAS_WORKER       = 1 <<  2,
}AgsPlayLv2AudioFLags;

struct _AgsPlayLv2Audio
{
  AgsRecallAudio recall_audio;

  guint flags;
  
  AgsTurtle *turtle;
  
  gchar *filename;
  gchar *effect;
  gchar *uri;
  uint32_t index;

  LV2_Descriptor *plugin_descriptor;

  uint32_t *input_port;
  uint32_t input_lines;

  uint32_t *output_port;
  uint32_t output_lines;

  uint32_t event_port;
  uint32_t atom_port;
};

struct _AgsPlayLv2AudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_play_lv2_audio_get_type();

AgsPlayLv2Audio* ags_play_lv2_audio_new();

#endif /*__AGS_PLAY_LV2_AUDIO_H__*/
