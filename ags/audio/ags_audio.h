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

#ifndef __AGS_AUDIO_H__
#define __AGS_AUDIO_H__

#include <glib-object.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_AUDIO                (ags_audio_get_type ())
#define AGS_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO, AgsAudio))
#define AGS_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO, AgsAudioClass))
#define AGS_IS_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO))
#define AGS_IS_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO))
#define AGS_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO, AgsAudioClass))

typedef struct _AgsAudio AgsAudio;
typedef struct _AgsAudioClass AgsAudioClass;

typedef enum{
  AGS_AUDIO_SYNC                        = 1, // can be combined with below
  AGS_AUDIO_ASYNC                       = 1 <<  1,
  AGS_AUDIO_OUTPUT_HAS_RECYCLING        = 1 <<  2,
  AGS_AUDIO_INPUT_HAS_RECYCLING         = 1 <<  3,
  AGS_AUDIO_INPUT_TAKES_FILE            = 1 <<  4,
  AGS_AUDIO_INPUT_TAKES_SYNTH           = 1 <<  5,
  AGS_AUDIO_REVERSE_MAPPING             = 1 <<  6,
  AGS_AUDIO_HAS_NOTATION                = 1 <<  7,
  AGS_AUDIO_NOTATION_DEFAULT            = 1 <<  8,
  AGS_AUDIO_PATTERN_MODE                = 1 <<  9,
  AGS_AUDIO_RUNNING                     = 1 << 10,
  AGS_AUDIO_PLAYING                     = 1 << 11,
}AgsAudioFlags;

struct _AgsAudio
{
  GObject object;

  guint flags;

  GObject *soundcard;
  guint level;

  GObject *sequencer;
  GObject *midi_file;
  
  guint samplerate;
  guint buffer_size;
  guint format;
  guint sequence_length;

  guint audio_channels;

  guint output_pads;
  guint output_lines;

  guint input_pads;
  guint input_lines;

  guint audio_start_mapping;
  guint audio_end_mapping;

  guint midi_start_mapping;
  guint midi_end_mapping;

  GList *audio_connection;
  
  AgsChannel *output;
  AgsChannel *input;

  GObject *playback_domain;
  
  GList *notation;
  GList *automation;
  
  GList *recall_id;
  GList *recycling_context;

  GList *container;
  GList *recall;
  GList *play;

  GList *recall_remove; //TODO:JK: verify deprecation
  GList *play_remove; //TODO:JK: verify deprecation

  GObject *machine;
};

struct _AgsAudioClass
{
  GObjectClass object;

  void (*check_connection)(AgsAudio *audio);
  
  void (*set_audio_channels)(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old);
  void (*set_pads)(AgsAudio *audio,
		   GType type,
		   guint pads, guint pads_old);

  AgsRecallID* (*init_run)(AgsAudio *audio);
  void (*tact)(AgsAudio *audio, AgsRecallID *recall_id);
  void (*done)(AgsAudio *audio, AgsRecallID *recall_id);
};

GType ags_audio_get_type();

void ags_audio_set_soundcard(AgsAudio *audio, GObject *soundcard);

void ags_audio_set_flags(AgsAudio *audio, guint flags);
void ags_audio_unset_flags(AgsAudio *audio, guint flags);

void ags_audio_check_connection(AgsAudio *audio);

void ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_audio_set_pads(AgsAudio *audio, GType type, guint pads);

void ags_audio_set_samplerate(AgsAudio *audio, guint samplerate);
void ags_audio_set_buffer_size(AgsAudio *audio, guint buffer_size);
void ags_audio_set_format(AgsAudio *audio, guint format);
void ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length);

void ags_audio_add_audio_connection(AgsAudio *audio,
				    GObject *audio_connection);
void ags_audio_remove_audio_connection(AgsAudio *audio,
				       GObject *audio_connection);

void ags_audio_add_notation(AgsAudio *audio,
			      GObject *notation);
void ags_audio_remove_notation(AgsAudio *audio,
			       GObject *notation);

void ags_audio_add_automation(AgsAudio *audio,
			      GObject *automation);
void ags_audio_remove_automation(AgsAudio *audio,
				 GObject *automation);

void ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id);
void ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id);

void ags_audio_add_recycling_context(AgsAudio *audio, GObject *recycling_context);
void ags_audio_remove_recycling_context(AgsAudio *audio, GObject *recycling_context);

void ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container);
void ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container);

void ags_audio_add_recall(AgsAudio *audio, GObject *recall, gboolean play);
void ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play);

void ags_audio_recall_change_state(AgsAudio *audio, gboolean enable);

AgsRecallID* ags_audio_init_run(AgsAudio *audio);
void ags_audio_duplicate_recall(AgsAudio *audio,
				AgsRecallID *recall_id);
void ags_audio_init_recall(AgsAudio *audio, gint stage,
			   AgsRecallID *recall_id);
void ags_audio_resolve_recall(AgsAudio *audio,
			      AgsRecallID *recall_id);
gboolean ags_audio_is_playing(AgsAudio *audio);
void ags_audio_play(AgsAudio *audio,
		    AgsRecallID *recall_id,
		    gint stage);
void ags_audio_tact(AgsAudio *audio, AgsRecallID *recall_id);
void ags_audio_done(AgsAudio *audio, AgsRecallID *recall_id);

void ags_audio_cancel(AgsAudio *audio,
		      AgsRecallID *recall_id);
void ags_audio_remove(AgsAudio *audio,
		      AgsRecallID *recall_id);

GList* ags_audio_find_port(AgsAudio *audio);

void ags_audio_open_files(AgsAudio *audio,
			  GSList *filenames,
			  gboolean overwrite_channels,
			  gboolean create_channels);

void ags_audio_recursive_set_property(AgsAudio *audio,
				      GParameter *parameter, gint n_params);

GList* ags_audio_recursive_play_init(AgsAudio *audio,
				     gboolean playback, gboolean sequencer, gboolean notation);

AgsAudio* ags_audio_new(GObject *soundcard);

#endif /*__AGS_AUDIO_H__*/
