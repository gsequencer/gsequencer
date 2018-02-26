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

#ifndef __AGS_CHANNEL_H__
#define __AGS_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_notation.h>

#include <stdarg.h>

#define AGS_TYPE_CHANNEL                (ags_channel_get_type())
#define AGS_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL, AgsChannel))
#define AGS_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL, AgsChannelClass))
#define AGS_IS_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANNEL))
#define AGS_IS_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL))
#define AGS_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL, AgsChannelClass))

#define AGS_CHANNEL_MINIMUM_OCTAVE (0)
#define AGS_CHANNEL_MAXIMUM_OCTAVE (10)

#define AGS_CHANNEL_OCTAVE_SEMITONE_STEPS (12)

#define AGS_CHANNEL_MINIMUM_SEMITONE (0)
#define AGS_CHANNEL_MAXIMUM_SEMITONE (128)

#define AGS_CHANNEL_MINIMUM_NOTE_FREQUENCY (27.5)
#define AGS_CHANNEL_MAXIMUM_NOTE_FREQUENCY (21120.0)

#define AGS_CHANNEL_MAXIMUM_MIDI_NOTE (127)

typedef struct _AgsChannel AgsChannel;
typedef struct _AgsChannelClass AgsChannelClass;

/**
 * AgsChannelFlags:
 * @AGS_CHANNEL_ADDED_TO_REGISTRY: the channel was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_CHANNEL_CONNECTED: indicates the channel was connected by calling #AgsConnectable::connect()
 * @AGS_CHANNEL_BYPASS: don't apply any data
 *
 * Enum values to control the behavior or indicate internal state of #AgsChannel by
 * enable/disable as flags.
 */
typedef enum{
  AGS_CHANNEL_ADDED_TO_REGISTRY  = 1,
  AGS_CHANNEL_CONNECTED          = 1 <<  1,
  AGS_CHANNEL_BYPASS             = 1 <<  2,
}AgsChannelFlags;

#define AGS_CHANNEL_ERROR (ags_channel_error_quark())

typedef enum{
  AGS_CHANNEL_ERROR_LOOP_IN_LINK,
}AgsChannelError;

struct _AgsChannel
{
  GObject object;

  guint flags;
  guint ability_flags;
  guint behaviour_flags;
  guint staging_flags[AGS_SOUND_SCOPE_LAST];

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *audio;

  GObject *output_soundcard;
  guint output_soundcard_channel;
  
  GObject *input_soundcard;
  guint input_soundcard_channel;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint pad;
  guint audio_channel;
  guint line;

  gint octave;
  guint key;
  gint absolute_key;
  
  gdouble note_frequency;
  gchar *note_key;

  guint midi_note;
  
  AgsChannel *prev;
  AgsChannel *prev_pad;
  AgsChannel *next;
  AgsChannel *next_pad;

  AgsChannel *link;

  AgsRecycling *first_recycling;
  AgsRecycling *last_recycling;
  
  GObject *playback;

  GList *pattern;

  GList *remote_channel;

  GList *recall_id;
  GList *recycling_context;

  GList *recall_container;

  pthread_mutexattr_t *play_mutexattr;
  pthread_mutex_t *play_mutex;

  GList *play;

  pthread_mutexattr_t *recall_mutexattr;
  pthread_mutex_t *recall_mutex;

  GList *recall;

  gpointer line_widget;
  gpointer file_data;
};

struct _AgsChannelClass
{
  GObjectClass object;

  void (*recycling_changed)(AgsChannel *channel,
			    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			    AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

  GList* (*add_effect)(AgsChannel *channel,
		       gchar *filename,
		       gchar *effect);
  void (*remove_effect)(AgsChannel *channel,
			guint nth);

  void (*duplicate_recall)(AgsChannel *channel,
			   AgsRecallID *recall_id);
  void (*resolve_recall)(AgsChannel *channel,
			 AgsRecallID *recall_id);

  void (*init_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id, guint staging_flags);
  void (*play_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id, guint staging_flags);

  void (*cancel_recall)(AgsChannel *channel,
			AgsRecallID *recall_id);
  void (*done_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id);
  
  GList* (*start)(AgsChannel *channel,
		  gint sound_scope);
  void (*stop)(AgsChannel *channel,
	       GList *recall_id, gint sound_scope);

  GList* (*check_scope)(AgsChannel *channel, gint sound_scope);
  
  GList* (*recursive_reset_stage)(AgsChannel *channel,
				  gint sound_scope, guint staging_flags);
};

GType ags_channel_get_type();

GQuark ags_channel_error_quark();

pthread_mutex_t* ags_channel_get_class_mutex();

void ags_channel_set_flags(AgsChannel *channel, guint flags);
void ags_channel_unset_flags(AgsChannel *channel, guint flags);

void ags_channel_set_ability_flags(AgsChannel *channel, guint ability_flags);
void ags_channel_unset_ability_flags(AgsChannel *channel, guint ability_flags);

void ags_channel_set_behaviour_flags(AgsChannel *channel, guint behaviour_flags);
void ags_channel_unset_behaviour_flags(AgsChannel *channel, guint behaviour_flags);

void ags_channel_set_staging_flags(AgsChannel *channel, gint sound_scope,
				   guint staging_flags);
void ags_channel_unset_staging_flags(AgsChannel *channel, gint sound_scope,
				     guint staging_flags);

/* channels */
AgsChannel* ags_channel_first(AgsChannel *channel);
AgsChannel* ags_channel_last(AgsChannel *channel);
AgsChannel* ags_channel_nth(AgsChannel *channel, guint nth);

AgsChannel* ags_channel_pad_first(AgsChannel *channel);
AgsChannel* ags_channel_pad_last(AgsChannel *channel);
AgsChannel* ags_channel_pad_nth(AgsChannel *channel, guint nth);

AgsChannel* ags_channel_first_with_recycling(AgsChannel *channel);
AgsChannel* ags_channel_last_with_recycling(AgsChannel *channel);
AgsChannel* ags_channel_prev_with_recycling(AgsChannel *channel);
AgsChannel* ags_channel_next_with_recycling(AgsChannel *channel);

void ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
			  GError **error);
void ags_channel_set_recycling(AgsChannel *channel,
			       AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			       gboolean update, gboolean destroy_old);

void ags_channel_recycling_changed(AgsChannel *channel,
				   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
				   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
				   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

/* soundcard */
void ags_channel_set_output_soundcard(AgsChannel *channel, GObject *output_soundcard);
void ags_channel_set_input_soundcard(AgsChannel *channel, GObject *input_soundcard);

/* presets */
void ags_channel_set_samplerate(AgsChannel *channel, guint samplerate);
void ags_channel_set_buffer_size(AgsChannel *channel, guint buffer_size);
void ags_channel_set_format(AgsChannel *channel, guint format);

/* children */
void ags_channel_add_pattern(AgsChannel *channel, GObject *pattern);
void ags_channel_remove_pattern(AgsChannel *channel, GObject *pattern);

/* recall related */
void ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id);
void ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id);

void ags_channel_add_recall_container(AgsChannel *channel, GObject *recall_container);
void ags_channel_remove_recall_container(AgsChannel *channel, GObject *recall_container);

void ags_channel_add_recall(AgsChannel *channel, GObject *recall, gboolean play);
void ags_channel_remove_recall(AgsChannel *channel, GObject *recall, gboolean play);

/* add/remove effect */
GList* ags_channel_add_effect(AgsChannel *channel,
			      char *filename,
			      gchar *effect);
void ags_channel_remove_effect(AgsChannel *channel,
			       guint nth);

/* stages */
void ags_channel_duplicate_recall(AgsChannel *channel,
				  AgsRecallID *recall_id);
void ags_channel_resolve_recall(AgsChannel *channel,
				AgsRecallID *recall_id);

void ags_channel_init_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, gint staging_flags);
void ags_channel_play_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, gint staging_flags);

void ags_channel_done_recall(AgsChannel *channel,
			     AgsRecallID *recall_id);
void ags_channel_cancel_recall(AgsChannel *channel,
			       AgsRecallID *recall_id);

/* control */
GList* ags_channel_start(AgsChannel *channel,
			 gint sound_scope);

void ags_channel_stop(AgsChannel *channel,
		      GList *recall_id, gint sound_scope);

/* query */
GList* ags_channel_check_scope(AgsChannel *channel, gint sound_scope);

GList* ags_channel_collect_all_channel_ports(AgsChannel *channel);

GList* ags_channel_collect_all_channel_ports_by_specifier_and_context(AgsChannel *channel,
								      gchar *specifier,
								      gboolean play_context);

/* recursive functions */
AgsChannel* ags_channel_get_level(AgsChannel *channel);

void ags_channel_recursive_set_property(AgsChannel *channel,
					GParameter *parameter, gint n_params);

void ags_channel_recursive_reset_recycling_context(AgsChannel *channel,
						   AgsRecyclingContext *old_recycling_context,
						   AgsRecyclingContext *recycling_context);

void ags_channel_recursive_reset_recall_id(AgsChannel *channel,
					   AgsRecallID *valid_recall_id, gboolean valid_set_up, gboolean valid_set_down,
					   AgsRecallID *invalid_recall_id, gboolean invalid_set_up, gboolean invalid_set_down);

void ags_channel_recursive_init(AgsChannel *channel,
				AgsRecallID *recall_id,
				gint stage, gint init_stage,
				gboolean init_up, gboolean init_down);

void ags_channel_recursive_run(AgsChannel *channel,
			       AgsRecallID *recall_id,
			       gint run_stage,
			       gboolean run_up, gboolean run_down,
			       gboolean current_level_only);

void ags_channel_recursive_cancel(AgsChannel *channel,
				  AgsRecallID *recall_id,
				  gboolean cancel_up, gboolean cancel_down);

void ags_channel_recursive_play_threaded(AgsChannel *channel,
					 AgsRecallID *recall_id, gint stage);
void ags_channel_recursive_play(AgsChannel *channel,
				AgsRecallID *recall_id, gint stage);

GList* ags_channel_recursive_reset_stage(AgsChannel *channel,
					 gint sound_scope, guint staging_flags);

/* instantiate */
AgsChannel* ags_channel_new(GObject *audio);

#endif /*__AGS_CHANNEL_H__*/
