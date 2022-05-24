/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_notation.h>

#include <math.h>

G_BEGIN_DECLS

#define AGS_TYPE_CHANNEL                (ags_channel_get_type())
#define AGS_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL, AgsChannel))
#define AGS_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL, AgsChannelClass))
#define AGS_IS_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANNEL))
#define AGS_IS_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL))
#define AGS_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL, AgsChannelClass))

#define AGS_CHANNEL_GET_OBJ_MUTEX(obj) (&(((AgsChannel *) obj)->obj_mutex))
#define AGS_CHANNEL_GET_PLAY_MUTEX(obj) (&(((AgsChannel *) obj)->play_mutex))
#define AGS_CHANNEL_GET_RECALL_MUTEX(obj) (&(((AgsChannel *) obj)->recall_mutex))

#define AGS_CHANNEL_MINIMUM_OCTAVE (0)
#define AGS_CHANNEL_MAXIMUM_OCTAVE (10)
#define AGS_CHANNEL_DEFAULT_OCTAVE (AGS_CHANNEL_MINIMUM_OCTAVE)

#define AGS_CHANNEL_MINIMUM_OCTAVE_SEMITONE (0)
#define AGS_CHANNEL_MAXIMUM_OCTAVE_SEMITONE (12)
#define AGS_CHANNEL_DEFAULT_OCTAVE_SEMITONE (AGS_CHANNEL_MINIMUM_OCTAVE_SEMITONE)

#define AGS_CHANNEL_MINIMUM_SEMITONE (0)
#define AGS_CHANNEL_MAXIMUM_SEMITONE (128)
#define AGS_CHANNEL_DEFAULT_SEMITONE (AGS_CHANNEL_MINIMUM_SEMITONE)

#define AGS_CHANNEL_MINIMUM_NOTE_FREQUENCY (440.0 * exp((-69.0 / 12.0) * log(2.0)))
#define AGS_CHANNEL_MAXIMUM_NOTE_FREQUENCY (440.0 * exp((58.0 / 12.0) * log(2.0)))
#define AGS_CHANNEL_DEFAULT_NOTE_FREQUENCY (AGS_CHANNEL_MINIMUM_NOTE_FREQUENCY)

#define AGS_CHANNEL_MINIMUM_MIDI_NOTE (0)
#define AGS_CHANNEL_MAXIMUM_MIDI_NOTE (127)
#define AGS_CHANNEL_DEFAULT_MIDI_NOTE (AGS_CHANNEL_MINIMUM_MIDI_NOTE)

typedef struct _AgsChannel AgsChannel;
typedef struct _AgsChannelClass AgsChannelClass;

/**
 * AgsChannelFlags:
 * @AGS_CHANNEL_BYPASS: don't apply any data
 *
 * Enum values to control the behavior or indicate internal state of #AgsChannel by
 * enable/disable as flags.
 */
typedef enum{
  AGS_CHANNEL_BYPASS             = 1,
}AgsChannelFlags;

#define AGS_CHANNEL_ERROR (ags_channel_error_quark())

typedef enum{
  AGS_CHANNEL_ERROR_LOOP_IN_LINK,
}AgsChannelError;

struct _AgsChannel
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  guint ability_flags;
  guint behaviour_flags;
  guint staging_flags[AGS_SOUND_SCOPE_LAST];

  gboolean staging_completed[AGS_SOUND_SCOPE_LAST];
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;
  
  GObject *audio;

  GObject *output_soundcard;
  gint output_soundcard_channel;
  
  GObject *input_soundcard;
  gint input_soundcard_channel;

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

  GList *recall_id;
  GList *recycling_context;

  GList *recall_container;

  GRecMutex play_mutex;

  GList *play;

  GRecMutex recall_mutex;

  GList *recall;

  gpointer line_widget;
  gpointer file_data;
};

struct _AgsChannelClass
{
  GObjectClass gobject;

  void (*recycling_changed)(AgsChannel *channel,
			    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			    AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

  void (*duplicate_recall)(AgsChannel *channel,
			   AgsRecallID *recall_id);
  void (*resolve_recall)(AgsChannel *channel,
			 AgsRecallID *recall_id);

  void (*init_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id, guint staging_flags);
  void (*play_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id, guint staging_flags);

  void (*done_recall)(AgsChannel *channel,
		      AgsRecallID *recall_id);
  void (*cancel_recall)(AgsChannel *channel,
			AgsRecallID *recall_id);

  void (*cleanup_recall)(AgsChannel *channel,
			 AgsRecallID *recall_id);

  GList* (*start)(AgsChannel *channel,
		  gint sound_scope);
  void (*stop)(AgsChannel *channel,
	       GList *recall_id, gint sound_scope);

  GList* (*check_scope)(AgsChannel *channel, gint sound_scope);
  
  void (*recursive_run_stage)(AgsChannel *channel,
			      gint sound_scope, guint staging_flags);
};

GType ags_channel_get_type();
GType ags_channel_flags_get_type();

GQuark ags_channel_error_quark();

GRecMutex* ags_channel_get_obj_mutex(AgsChannel *channel);
GRecMutex* ags_channel_get_play_mutex(AgsChannel *channel);
GRecMutex* ags_channel_get_recall_mutex(AgsChannel *channel);

gboolean ags_channel_test_flags(AgsChannel *channel, guint flags);
void ags_channel_set_flags(AgsChannel *channel, guint flags);
void ags_channel_unset_flags(AgsChannel *channel, guint flags);

gboolean ags_channel_test_ability_flags(AgsChannel *channel, guint ability_flags);
void ags_channel_set_ability_flags(AgsChannel *channel, guint ability_flags);
void ags_channel_unset_ability_flags(AgsChannel *channel, guint ability_flags);

gboolean ags_channel_test_behaviour_flags(AgsChannel *channel, guint behaviour_flags);
void ags_channel_set_behaviour_flags(AgsChannel *channel, guint behaviour_flags);
void ags_channel_unset_behaviour_flags(AgsChannel *channel, guint behaviour_flags);

gboolean ags_channel_test_staging_flags(AgsChannel *channel, gint sound_scope,
					guint staging_flags);
void ags_channel_set_staging_flags(AgsChannel *channel, gint sound_scope,
				   guint staging_flags);
void ags_channel_unset_staging_flags(AgsChannel *channel, gint sound_scope,
				     guint staging_flags);

gboolean ags_channel_test_staging_completed(AgsChannel *channel, gint sound_scope);
void ags_channel_set_staging_completed(AgsChannel *channel, gint sound_scope);
void ags_channel_unset_staging_completed(AgsChannel *channel, gint sound_scope);

/* parent */
GObject* ags_channel_get_audio(AgsChannel *channel);
void ags_channel_set_audio(AgsChannel *channel, GObject *audio);

/* channels */
AgsChannel* ags_channel_next(AgsChannel *channel);
AgsChannel* ags_channel_prev(AgsChannel *channel);

AgsChannel* ags_channel_next_pad(AgsChannel *channel);
AgsChannel* ags_channel_prev_pad(AgsChannel *channel);

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

AgsChannel* ags_channel_get_link(AgsChannel *channel);

void ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
			  GError **error);
void ags_channel_reset_recycling(AgsChannel *channel,
				 AgsRecycling *first_recycling, AgsRecycling *last_recycling);

void ags_channel_recycling_changed(AgsChannel *channel,
				   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
				   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
				   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

/* soundcard */
GObject* ags_channel_get_output_soundcard(AgsChannel *channel);
void ags_channel_set_output_soundcard(AgsChannel *channel, GObject *output_soundcard);

gint ags_channel_get_output_soundcard_channel(AgsChannel *channel);
void ags_channel_set_output_soundcard_channel(AgsChannel *channel, gint output_soundcard_channel);

GObject* ags_channel_get_input_soundcard(AgsChannel *channel);
void ags_channel_set_input_soundcard(AgsChannel *channel, GObject *input_soundcard);

gint ags_channel_get_input_soundcard_channel(AgsChannel *channel);
void ags_channel_set_input_soundcard_channel(AgsChannel *channel, gint input_soundcard_channel);

/* presets */
guint ags_channel_get_samplerate(AgsChannel *channel);
void ags_channel_set_samplerate(AgsChannel *channel, guint samplerate);

guint ags_channel_get_buffer_size(AgsChannel *channel);
void ags_channel_set_buffer_size(AgsChannel *channel, guint buffer_size);

guint ags_channel_get_format(AgsChannel *channel);
void ags_channel_set_format(AgsChannel *channel, guint format);

/* alignment */
guint ags_channel_get_pad(AgsChannel *channel);
void ags_channel_set_pad(AgsChannel *channel, guint pad);

guint ags_channel_get_audio_channel(AgsChannel *channel);
void ags_channel_set_audio_channel(AgsChannel *channel, guint audio_channel);

guint ags_channel_get_line(AgsChannel *channel);
void ags_channel_set_line(AgsChannel *channel, guint line);

/* key */
gint ags_channel_get_octave(AgsChannel *channel);
void ags_channel_set_octave(AgsChannel *channel, gint octave);

guint ags_channel_get_key(AgsChannel *channel);
void ags_channel_set_key(AgsChannel *channel, guint key);

gint ags_channel_get_absolute_key(AgsChannel *channel);
void ags_channel_set_absolute_key(AgsChannel *channel, gint absolute_key);

/* children */
GList* ags_channel_get_pattern(AgsChannel *channel);
void ags_channel_set_pattern(AgsChannel *channel, GList *pattern);

void ags_channel_add_pattern(AgsChannel *channel, GObject *pattern);
void ags_channel_remove_pattern(AgsChannel *channel, GObject *pattern);

/* recall related */
GObject* ags_channel_get_playback(AgsChannel *channel);
void ags_channel_set_playback(AgsChannel *channel, GObject *playback);

GList* ags_channel_get_recall_id(AgsChannel *channel);
void ags_channel_set_recall_id(AgsChannel *channel, GList *recall_id);

void ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id);
void ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id);

GList* ags_channel_get_recall_container(AgsChannel *channel);
void ags_channel_set_recall_container(AgsChannel *channel, GList *recall_container);

void ags_channel_add_recall_container(AgsChannel *channel, GObject *recall_container);
void ags_channel_remove_recall_container(AgsChannel *channel, GObject *recall_container);

GList* ags_channel_get_play(AgsChannel *channel);
void ags_channel_set_play(AgsChannel *channel, GList *play);

GList* ags_channel_get_recall(AgsChannel *channel);
void ags_channel_set_recall(AgsChannel *channel, GList *recall);

void ags_channel_add_recall(AgsChannel *channel, GObject *recall,
			    gboolean play_context);
void ags_channel_insert_recall(AgsChannel *channel, GObject *recall,
			       gboolean play_context,
			       gint position);
void ags_channel_remove_recall(AgsChannel *channel, GObject *recall,
			       gboolean play_context);

/* stages */
void ags_channel_duplicate_recall(AgsChannel *channel,
				  AgsRecallID *recall_id);
void ags_channel_resolve_recall(AgsChannel *channel,
				AgsRecallID *recall_id);

void ags_channel_init_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, guint staging_flags);
void ags_channel_play_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, guint staging_flags);

void ags_channel_done_recall(AgsChannel *channel,
			     AgsRecallID *recall_id);
void ags_channel_cancel_recall(AgsChannel *channel,
			       AgsRecallID *recall_id);

void ags_channel_cleanup_recall(AgsChannel *channel,
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
					gint n_params,
					gchar **parameter_name, GValue *value);

void ags_channel_recursive_run_stage(AgsChannel *channel,
				     gint sound_scope, guint staging_flags);

/* instantiate */
AgsChannel* ags_channel_new(GObject *audio);

G_END_DECLS

#endif /*__AGS_CHANNEL_H__*/
