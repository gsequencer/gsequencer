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

#ifndef __AGS_AUDIO_H__
#define __AGS_AUDIO_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_run_order.h>

#define AGS_TYPE_AUDIO                (ags_audio_get_type ())
#define AGS_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO, AgsAudio))
#define AGS_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO, AgsAudioClass))
#define AGS_IS_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO))
#define AGS_IS_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO))
#define AGS_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO, AgsAudioClass))

typedef struct _AgsAudio AgsAudio;
typedef struct _AgsAudioClass AgsAudioClass;

typedef enum{
  AGS_AUDIO_OUTPUT_HAS_RECYCLING        = 1,
  AGS_AUDIO_INPUT_HAS_RECYCLING         = 1 <<  1,
  AGS_AUDIO_INPUT_TAKES_FILE            = 1 <<  2,
  AGS_AUDIO_HAS_NOTATION                = 1 <<  3,
  AGS_AUDIO_SYNC                        = 1 <<  4, // can be combined with below
  AGS_AUDIO_ASYNC                       = 1 <<  5,
  AGS_AUDIO_RUNNING                     = 1 <<  6,
  AGS_AUDIO_PLAYING                     = 1 <<  7,
  AGS_AUDIO_NOTATION_DEFAULT            = 1 <<  8,
}AgsAudioFlags;

struct _AgsAudio
{
  GObject object;

  guint flags;

  GObject *devout;
  guint level;
  
  guint sequence_length;
  guint audio_channels;
  guint frequence;

  guint output_pads;
  guint output_lines;

  guint input_pads;
  guint input_lines;

  AgsChannel *output;
  AgsChannel *input;

  GList *notation;

  AgsDevoutPlay *devout_play;

  GList *recall_id;
  GList *run_order;

  GList *container;

  GList *recall;
  GList *play;

  GList *recall_remove;
  GList *play_remove;

  GtkWidget *machine;
};

struct _AgsAudioClass
{
  GObjectClass object;

  void (*set_audio_channels)(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old);
  void (*set_pads)(AgsAudio *audio,
		   GType type,
		   guint pads, guint pads_old);
};

GType ags_audio_get_type();

void ags_audio_set_flags(AgsAudio *audio, guint flags);
void ags_audio_unset_flags(AgsAudio *audio, guint flags);

void ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_audio_set_pads(AgsAudio *audio, GType type, guint pads);

void ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length);

void ags_audio_find_group_id_from_child(AgsAudio *audio,
					AgsChannel *input, AgsRecallID *input_recall_id, gboolean input_do_recall,
					AgsRecallID **child_recall_id, gboolean *child_do_recall);

void ags_audio_add_run_order(AgsAudio *audio, AgsRunOrder *run_order);
void ags_audio_remove_run_order(AgsAudio *audio, AgsRunOrder *run_order);

void ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id);
void ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id);
void ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container);
void ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container, gboolean play);
void ags_audio_add_recall(AgsAudio *audio, GObject *recall, gboolean play);
void ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play);

void ags_audio_recall_change_state(AgsAudio *audio, gboolean enable);

void ags_audio_duplicate_recall(AgsAudio *audio,
				gboolean playback, gboolean sequencer, gboolean notation,
				AgsRecycling *first_recycling, AgsRecycling *last_recycling,
				AgsGroupId group_id,
				guint audio_signal_level, gboolean called_by_output);
void ags_audio_init_recall(AgsAudio *audio, gint stage,
			   AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			   AgsGroupId group_id);
void ags_audio_resolve_recall(AgsAudio *audio,
			      AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			      AgsGroupId group_id);

void ags_audio_play(AgsAudio *audio,
		    AgsRecycling *first_recycling, AgsRecycling *last_recycling,
		    AgsGroupId group_id,
		    gint stage, gboolean do_recall);

guint ags_audio_recursive_play_init(AgsAudio *audio,
				    gboolean playback, gboolean sequencer, gboolean notation);

void ags_audio_cancel(AgsAudio *audio,
		      AgsGroupId group_id,
		      AgsRecycling *first_recycling, AgsRecycling *last_recycling,
		      gboolean do_recall);

void ags_audio_open_files(AgsAudio *audio,
			  GSList *filenames,
			  gboolean overwrite_channels,
			  gboolean create_channels);

AgsAudio* ags_audio_new();

#endif /*__AGS_AUDIO_H__*/
