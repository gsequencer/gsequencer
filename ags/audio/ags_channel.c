/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_generic_recall_recycling.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/task/ags_cancel_channel.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/recall/ags_play_channel_run.h>

#include <ags/audio/fx/ags_fx_playback_channel_processor.h>

#include <stdlib.h>
#include <stdio.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

#include <ags/i18n.h>

/**
 * SECTION:ags_channel
 * @short_description: Acts as entry point to the audio tree.
 * @title: AgsChannel
 * @section_id:
 * @include: ags/audio/ags_channel.h
 *
 * #AgsChannel is the entry point to the entire audio tree and its nested
 * recycling tree.
 *
 * Every channel has its own #AgsRecallID. As modifying link a new #AgsRecyclingContext
 * is indicated, since it acts as a kind of recall id tree context.
 */

void ags_channel_class_init(AgsChannelClass *channel_class);
void ags_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_init(AgsChannel *channel);
void ags_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_channel_dispose(GObject *gobject);
void ags_channel_finalize(GObject *gobject);

AgsUUID* ags_channel_get_uuid(AgsConnectable *connectable);
gboolean ags_channel_has_resource(AgsConnectable *connectable);
gboolean ags_channel_is_ready(AgsConnectable *connectable);
void ags_channel_add_to_registry(AgsConnectable *connectable);
void ags_channel_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_channel_list_resource(AgsConnectable *connectable);
xmlNode* ags_channel_xml_compose(AgsConnectable *connectable);
void ags_channel_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_channel_is_connected(AgsConnectable *connectable);
void ags_channel_connect(AgsConnectable *connectable);
void ags_channel_disconnect(AgsConnectable *connectable);

void ags_channel_real_set_output_soundcard(AgsChannel *channel, GObject *output_soundcard);

void ags_channel_real_set_input_soundcard(AgsChannel *channel, GObject *input_soundcard);

void ags_channel_real_set_samplerate(AgsChannel *channel, guint samplerate);

void ags_channel_real_set_buffer_size(AgsChannel *channel, guint buffer_size);

void ags_channel_real_set_format(AgsChannel *channel, guint format);

gboolean ags_channel_reset_recycling_recursive_input(AgsChannel *input,
						     AgsAudio **found_next, AgsAudio **found_prev,
						     AgsChannel **next_channel, AgsChannel **prev_channel,
						     AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
						     guint *complete_level_first, guint *complete_level_last,
						     gboolean *find_next, gboolean *find_prev,
						     gboolean *replace_first, gboolean *replace_last);
void ags_channel_reset_recycling_recursive_output(AgsChannel *output,
						  AgsAudio **found_next, AgsAudio **found_prev,
						  AgsChannel **next_channel, AgsChannel **prev_channel,
						  AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
						  guint *complete_level_first, guint *complete_level_last,
						  gboolean *find_next, gboolean *find_prev,
						  gboolean *replace_first, gboolean *replace_last);
void ags_channel_reset_recycling_recursive(AgsChannel *input,
					   AgsAudio **found_next, AgsAudio **found_prev,
					   AgsChannel **next_channel, AgsChannel **prev_channel,
					   AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
					   guint *complete_level_first, guint *complete_level_last,
					   gboolean *find_next, gboolean *find_prev,
					   gboolean *replace_first, gboolean *replace_last);

void ags_channel_reset_recycling_reset_recycling_context_up(AgsChannel *current);
void ags_channel_reset_recycling_reset_recycling_context_down(AgsChannel *current_output,
							      AgsRecyclingContext *new_recycling_context, AgsRecyclingContext *old_recycling_context);
void ags_channel_reset_recycling_reset_recycling_context_down_input(AgsChannel *current_output,
								    AgsRecyclingContext *new_recycling_context, AgsRecyclingContext *old_recycling_context);
  
void ags_channel_reset_recycling_emit_changed_input(AgsChannel *start_channel, AgsChannel *input,
						    AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
						    AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
						    AgsRecycling *first_recycling, AgsRecycling *last_recycling);
void ags_channel_reset_recycling_emit_changed_output(AgsChannel *start_channel, AgsChannel *output,
						     AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
						     AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
						     AgsRecycling *first_recycling, AgsRecycling *last_recycling);
void ags_channel_reset_recycling_emit_changed(AgsChannel *start_channel, AgsChannel *input,
					      AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
					      AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
					      AgsRecycling *first_recycling, AgsRecycling *last_recycling);

void ags_channel_real_recycling_changed(AgsChannel *channel,
					AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					AgsRecycling *new_start_region, AgsRecycling *new_end_region,
					AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

GList* ags_channel_add_ladspa_effect(AgsChannel *channel,
				     gchar *filename,
				     gchar *effect);
GList* ags_channel_add_dssi_effect(AgsChannel *channel,
				   gchar *filename,
				   gchar *effect);
GList* ags_channel_add_lv2_effect(AgsChannel *channel,
				  gchar *filename,
				  gchar *effect);
GList* ags_channel_real_add_effect(AgsChannel *channel,
				   gchar *filename,
				   gchar *effect);
void ags_channel_real_remove_effect(AgsChannel *channel,
				    guint nth);

void ags_channel_real_duplicate_recall(AgsChannel *channel,
				       AgsRecallID *recall_id);
void ags_channel_real_resolve_recall(AgsChannel *channel,
				     AgsRecallID *recall_id);
void ags_channel_real_init_recall(AgsChannel *channel,
				  AgsRecallID *recall_id, guint staging_flags);
void ags_channel_real_play_recall(AgsChannel *channel,
				  AgsRecallID *recall_id, guint staging_flags);
void ags_channel_real_done_recall(AgsChannel *channel,
				  AgsRecallID *recall_id);
void ags_channel_real_cancel_recall(AgsChannel *channel,
				    AgsRecallID *recall_id);

void ags_channel_real_cleanup_recall(AgsChannel *channel,
				     AgsRecallID *recall_id);

void ags_channel_recall_done_callback(AgsRecall *recall,
				      AgsChannel *channel);

GList* ags_channel_real_start(AgsChannel *channel,
			      gint sound_scope);
void ags_channel_real_stop(AgsChannel *channel,
			   GList *recall_id, gint sound_scope);

GList* ags_channel_real_check_scope(AgsChannel *channel, gint sound_scope);

void ags_channel_recursive_set_property_setv(AgsChannel *channel,
					     gint n_params,
					     gchar **parameter_name, GValue *value);
void ags_channel_recursive_set_property_down(AgsChannel *channel,
					     gint n_params,
					     gchar **parameter_name, GValue *value);
void ags_channel_recursive_set_property_down_input(AgsChannel *channel,
						   gint n_params,
						   gchar **parameter_name, GValue *value);

void ags_channel_recursive_setup_run_stage_up(AgsChannel *channel,
					      AgsRecyclingContext *recycling_context,
					      gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_setup_run_stage_down(AgsChannel *channel,
						AgsRecyclingContext *recycling_context,
						gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_setup_run_stage_down_input(AgsChannel *channel,
						      AgsRecyclingContext *recycling_context,
						      gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_prepare_run_stage_up(AgsChannel *channel,
						AgsRecyclingContext *recycling_context,
						gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_prepare_run_stage_down(AgsChannel *channel,
						  AgsRecyclingContext *recycling_context,
						  gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_prepare_run_stage_down_input(AgsChannel *channel,
							AgsRecyclingContext *recycling_context,
							gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_do_run_stage_up(AgsChannel *channel,
					   AgsRecyclingContext *recycling_context,
					   gint sound_scope, guint staging_flags);
void ags_channel_recursive_do_run_stage_down(AgsChannel *channel,
					     AgsRecyclingContext *recycling_context,
					     gint sound_scope, guint staging_flags);
void ags_channel_recursive_do_run_stage_down_input(AgsChannel *channel,
						   AgsRecyclingContext *recycling_context,
						   gint sound_scope, guint staging_flags);
void ags_channel_recursive_cleanup_run_stage_up(AgsChannel *channel,
						AgsRecyclingContext *recycling_context,
						gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_cleanup_run_stage_down(AgsChannel *channel,
						  AgsRecyclingContext *recycling_context,
						  gint sound_scope, guint local_staging_flags);
void ags_channel_recursive_cleanup_run_stage_down_input(AgsChannel *channel,
							AgsRecyclingContext *recycling_context,
							gint sound_scope, guint local_staging_flags);

void ags_channel_real_recursive_run_stage(AgsChannel *channel,
					  gint sound_scope, guint staging_flags);

enum{
  ADD_EFFECT,
  REMOVE_EFFECT,
  RECYCLING_CHANGED,
  DUPLICATE_RECALL,
  RESOLVE_RECALL,
  INIT_RECALL,
  PLAY_RECALL,
  DONE_RECALL,
  CANCEL_RECALL,
  CLEANUP_RECALL,
  START,
  STOP,
  CHECK_SCOPE,
  RECURSIVE_RUN_STAGE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_OUTPUT_SOUNDCARD,
  PROP_OUTPUT_SOUNDCARD_CHANNEL,
  PROP_INPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_PAD,
  PROP_AUDIO_CHANNEL,
  PROP_LINE,
  PROP_OCTAVE,
  PROP_KEY,
  PROP_ABSOLUTE_KEY,
  PROP_NOTE_FREQUENCY,
  PROP_NOTE_KEY,
  PROP_MIDI_NOTE,
  PROP_PREV,
  PROP_PREV_PAD,
  PROP_NEXT,
  PROP_NEXT_PAD,
  PROP_LINK,
  PROP_FIRST_RECYCLING,
  PROP_LAST_RECYCLING,
  PROP_PLAYBACK,
  PROP_PATTERN,
  PROP_RECALL_ID,
  PROP_RECYCLING_CONTEXT,
  PROP_RECALL_CONTAINER,
  PROP_PLAY,
  PROP_RECALL,
};

enum{
  AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID      = 1,
  AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE          = 1 <<  1,
  AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE            = 1 <<  2,
}AgsChannelRecursivePrepareStagingFlags;

enum{
  AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL      = 1,
  AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL      = 1 <<  1,
  AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE                      = 1 <<  2,
}AgsChannelRecursiveCleanupStagingFlags;

static gpointer ags_channel_parent_class = NULL;
static guint channel_signals[LAST_SIGNAL];

GType
ags_channel_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_channel = 0;

    static const GTypeInfo ags_channel_info = {
      sizeof (AgsChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel = g_type_register_static(G_TYPE_OBJECT,
					      "AgsChannel",
					      &ags_channel_info, 0);

    g_type_add_interface_static(ags_type_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_channel_class_init(AgsChannelClass *channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_channel_parent_class = g_type_class_peek_parent(channel);

  /* GObjectClass */
  gobject = (GObjectClass *) channel;

  gobject->set_property = ags_channel_set_property;
  gobject->get_property = ags_channel_get_property;

  gobject->dispose = ags_channel_dispose;
  gobject->finalize = ags_channel_finalize;

  /* properties */
  /**
   * AgsChannel:audio:
   *
   * The assigned #AgsAudio aligning channels.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned with"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsChannel:output-soundcard:
   *
   * The assigned output #AgsSoundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("output-soundcard",
				   i18n_pspec("assigned output soundcard"),
				   i18n_pspec("The output soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsChannel:output-soundcard-channel:
   *
   * The output soundcard channel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("output-soundcard-channel",
				 i18n_pspec("output soundcard channel"),
				 i18n_pspec("The output soundcard channel"),
				 -1,
				 G_MAXINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsChannel:input-soundcard:
   *
   * The assigned input #AgsSoundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("input-soundcard",
				   i18n_pspec("assigned input soundcard"),
				   i18n_pspec("The input soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsChannel:input-soundcard-channel:
   *
   * The input soundcard channel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("input-soundcard-channel",
				 i18n_pspec("input soundcard channel"),
				 i18n_pspec("The input soundcard channel"),
				 -1,
				 G_MAXINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD_CHANNEL,
				  param_spec);
  
  /**
   * AgsChannel:samplerate:
   *
   * The samplerate.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate"),
				  i18n_pspec("The samplerate"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsChannel:buffer-size:
   *
   * The buffer size.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size"),
				  i18n_pspec("The buffer size"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsChannel:format:
   *
   * The format.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format"),
				  i18n_pspec("The format"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);
  
  /**
   * AgsChannel:pad:
   *
   * The nth pad.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("pad",
				  i18n_pspec("nth pad"),
				  i18n_pspec("The nth pad"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);

  /**
   * AgsChannel:audio-channel:
   *
   * The nth audio channel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("nth audio channel"),
				  i18n_pspec("The nth audio channel"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsChannel:line:
   *
   * The nth line.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("line",
				  i18n_pspec("nth line"),
				  i18n_pspec("The nth line"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);

  /**
   * AgsChannel:octave:
   *
   * The nth octave.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("octave",
				 i18n_pspec("nth octave"),
				 i18n_pspec("The nth octave"),
				 AGS_CHANNEL_MINIMUM_OCTAVE,
				 AGS_CHANNEL_MAXIMUM_OCTAVE,
				 AGS_CHANNEL_DEFAULT_OCTAVE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OCTAVE,
				  param_spec);

  /**
   * AgsChannel:key:
   *
   * The nth key.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("key",
				  i18n_pspec("nth key"),
				  i18n_pspec("The nth key"),
				  AGS_CHANNEL_MINIMUM_OCTAVE_SEMITONE,
				  AGS_CHANNEL_MAXIMUM_OCTAVE_SEMITONE,
				  AGS_CHANNEL_DEFAULT_OCTAVE_SEMITONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_KEY,
				  param_spec);

  /**
   * AgsChannel:absolute-key:
   *
   * The nth absolute key.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("absolute-key",
				 i18n_pspec("nth absolute key"),
				 i18n_pspec("The nth absolute key"),
				 AGS_CHANNEL_MINIMUM_SEMITONE,
				 AGS_CHANNEL_MAXIMUM_SEMITONE,
				 AGS_CHANNEL_DEFAULT_SEMITONE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ABSOLUTE_KEY,
				  param_spec);

  /**
   * AgsChannel:note-frequency:
   *
   * The note frequency.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_double("note-frequency",
				    i18n_pspec("note frequency"),
				    i18n_pspec("The note frequency"),
				    AGS_CHANNEL_MINIMUM_NOTE_FREQUENCY,
				    AGS_CHANNEL_MAXIMUM_NOTE_FREQUENCY,
				    AGS_CHANNEL_DEFAULT_NOTE_FREQUENCY,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE_FREQUENCY,
				  param_spec);
  
  /**
   * AgsChannel:note-key:
   *
   * The assigned note key representing this channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("note-key",
				   i18n_pspec("assigned note key"),
				   i18n_pspec("The note key it is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE_KEY,
				  param_spec);

  /**
   * AgsChannel:midi-note:
   *
   * The nth midi note.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("midi-note",
				  i18n_pspec("nth midi note"),
				  i18n_pspec("The nth midi note"),
				  AGS_CHANNEL_MINIMUM_MIDI_NOTE,
				  AGS_CHANNEL_MAXIMUM_MIDI_NOTE,
				  AGS_CHANNEL_DEFAULT_MIDI_NOTE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_NOTE,
				  param_spec);

  /**
   * AgsChannel:prev:
   *
   * The assigned prev #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("prev",
				   "assigned prev",
				   "The prev it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_PREV,
				  param_spec);

  /**
   * AgsChannel:prev-pad:
   *
   * The assigned prev pad #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("prev-pad",
				   "assigned prev pad",
				   "The prev pad it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_PREV_PAD,
				  param_spec);

  /**
   * AgsChannel:next:
   *
   * The assigned next #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("next",
				   "assigned next",
				   "The next it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT,
				  param_spec);

  /**
   * AgsChannel:next-pad:
   *
   * The assigned next pad #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("next-pad",
				   "assigned next pad",
				   "The next it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT_PAD,
				  param_spec);
  
  /**
   * AgsChannel:link:
   *
   * The assigned link as #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("link",
				   i18n_pspec("assigned link"),
				   i18n_pspec("The link it is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINK,
				  param_spec);

  /**
   * AgsChannel:first-recycling:
   *
   * The containing #AgsRecycling it takes it #AgsAudioSignal from.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("first-recycling",
				   i18n_pspec("containing first recycling"),
				   i18n_pspec("The first recycling it contains"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_RECYCLING,
				  param_spec);

  /**
   * AgsChannel:last-recycling:
   *
   * The containing #AgsRecycling it takes it #AgsAudioSignal from.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("last-recycling",
				   i18n_pspec("containing last recycling"),
				   i18n_pspec("The last recycling it contains"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_LAST_RECYCLING,
				  param_spec);
  
  /**
   * AgsChannel:playback:
   *
   * The assigned #AgsPlayback.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("assigned playback"),
				   i18n_pspec("The playback it is assigned with"),
				   AGS_TYPE_PLAYBACK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);

  /**
   * AgsChannel:pattern: (type GList(AgsPatter)) (transfer full)
   *
   * The containing #AgsPattern.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("pattern",
				    i18n_pspec("containing pattern"),
				    i18n_pspec("The pattern it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);

  /**
   * AgsChannel:recall-id: (type GList(AgsRecallID)) (transfer full)
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall-id",
				    i18n_pspec("assigned recall id"),
				    i18n_pspec("The recall id it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsChannel:recycling-context: (type GList(AgsRecyclingContext)) (transfer full)
   *
   * The containing #AgsRecyclingContext.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recycling-context",
				    i18n_pspec("containing recycling-context"),
				    i18n_pspec("The recycling context it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING_CONTEXT,
				  param_spec);

  /**
   * AgsChannel:recall-container: (type GList(AgsRecallContainer)) (transfer full)
   *
   * The containing #AgsRecallContainer.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall-container",
				    i18n_pspec("containing recall-container"),
				    i18n_pspec("The recall container it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsChannel:recall: (type GList(AgsRecall)) (transfer full)
   *
   * The containing #AgsRecall in recall-context.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall",
				    i18n_pspec("containing recall"),
				    i18n_pspec("The recall it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);

  /**
   * AgsChannel:play: (type GList(AgsRecall)) (transfer full)
   *
   * The containing #AgsRecall in play-context.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("play",
				    i18n_pspec("containing play"),
				    i18n_pspec("The play it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /* AgsChannelClass */
  channel->recycling_changed = NULL;

  channel->add_effect = ags_channel_real_add_effect;
  channel->remove_effect = ags_channel_real_remove_effect;

  channel->duplicate_recall = ags_channel_real_duplicate_recall;
  channel->resolve_recall = ags_channel_real_resolve_recall;
  
  channel->init_recall = ags_channel_real_init_recall;
  channel->play_recall = ags_channel_real_play_recall;

  channel->done_recall = ags_channel_real_done_recall;
  channel->cancel_recall = ags_channel_real_cancel_recall;

  channel->cleanup_recall = ags_channel_real_cleanup_recall;

  channel->start = ags_channel_real_start;
  channel->stop = ags_channel_real_stop;

  channel->check_scope = ags_channel_real_check_scope;
  channel->recursive_run_stage = ags_channel_real_recursive_run_stage;
  
  /* signals */
  /**
   * AgsChannel::recycling-changed:
   * @channel: the #AgsChannel recycling changed
   * @old_start_region: first #AgsRecycling
   * @old_end_region: last #AgsRecycling
   * @new_start_region: new first #AgsRecycling
   * @new_end_region: new last #AgsRecycling
   * @old_start_changed_region: modified link #AgsRecycling start
   * @old_end_changed_region: modified link #AgsRecyclig end
   * @new_start_changed_region: replacing link #AgsRecycling start
   * @new_end_changed_region: replacing link #AgsRecycling end
   *
   * The ::recycling-changed signal is invoked to notify modified recycling tree.
   * 
   * Since: 3.0.0
   */
  channel_signals[RECYCLING_CHANGED] =
    g_signal_new("recycling-changed",
		 G_TYPE_FROM_CLASS (channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsChannelClass, recycling_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT,
		 G_TYPE_NONE, 8,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT);

  /**
   * AgsChannel::add-effect:
   * @channel: the #AgsChannel to modify
   * @filename: the effect's filename
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   * 
   * Since: 3.0.0
   */
  channel_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, add_effect),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__STRING_STRING,
		 G_TYPE_POINTER, 2,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsChannel::remove-effect:
   * @channel: the #AgsChannel to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   * 
   * Since: 3.0.0
   */
  channel_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsChannel::duplicate-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::duplicate-recall signal is invoked during playback initialization.
   * 
   * Since: 3.0.0
   */
  channel_signals[DUPLICATE_RECALL] =
    g_signal_new("duplicate-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, duplicate_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsChannel::resolve-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::resolve-recall signal is invoked during playback initialization.
   * 
   * Since: 3.0.0
   */
  channel_signals[RESOLVE_RECALL] =
    g_signal_new("resolve-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, resolve_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsChannel::init-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   * @staging_flags: the staging flags
   *
   * The ::init-recall signal is invoked during playback initialization.
   * 
   * Since: 3.0.0
   */
  channel_signals[INIT_RECALL] =
    g_signal_new("init-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, init_recall),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_UINT);

  /**
   * AgsChannel::play-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   * @staging_flags: the staging flags
   *
   * The ::play-recall signal is invoked during playback run.
   * 
   * Since: 3.0.0
   */
  channel_signals[PLAY_RECALL] =
    g_signal_new("play-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, play_recall),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_UINT);
  
  /**
   * AgsChannel::done-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::done-recall signal is invoked during termination of playback.
   * 
   * Since: 3.0.0
   */
  channel_signals[DONE_RECALL] =
    g_signal_new("done-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, done_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsChannel::cancel-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::cancel-recall signal is invoked during termination of playback.
   * 
   * Since: 3.0.0
   */
  channel_signals[CANCEL_RECALL] =
    g_signal_new("cancel-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, cancel_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsChannel::cleanup-recall:
   * @channel: the #AgsChannel
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::cleanup-recall signal is invoked during termination of playback.
   * 
   * Since: 3.0.0
   */
  channel_signals[CLEANUP_RECALL] =
    g_signal_new("cleanup-recall",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, cleanup_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsChannel::start:
   * @channel: the #AgsChannel
   * @sound_scope: the sound scope
   *
   * The ::start signal is invoked as playback starts.
   * 
   * Returns: (type GLib.List) (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID
   * 
   * Since: 3.0.0
   */
  channel_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, start),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__INT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_INT);

  /**
   * AgsChannel::stop:
   * @channel: the #AgsChannel
   * @recall_id: the #GList-struct containing #AgsRecallID
   * @sound_scope: the sound scope
   *
   * The ::stop signal is invoked as playback stops.
   * 
   * Since: 3.0.0
   */
  channel_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, stop),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_POINTER,
		 G_TYPE_INT);

  /**
   * AgsChannel::check-scope:
   * @channel: the #AgsChannel
   * @sound_scope: the sound scope
   *
   * The ::check-scope signal gives you control of checking scope.
   * 
   * Returns: the #GList-struct containing #AgsRecallID
   * 
   * Since: 3.0.0
   */
  channel_signals[CHECK_SCOPE] =
    g_signal_new("check-scope",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, check_scope),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__INT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_INT);

  /**
   * AgsChannel::recursive-run-stage:
   * @channel: the #AgsChannel
   * @sound_scope: the sound scope
   * @staging_flags: the staging flags
   *
   * The ::recursive-run-stage signal gives you control of checking scope.
   * 
   * Since: 3.0.0
   */
  channel_signals[RECURSIVE_RUN_STAGE] =
    g_signal_new("recursive-run-stage",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, recursive_run_stage),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__INT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_INT,
		 G_TYPE_UINT);
}

void
ags_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_channel_get_uuid;
  connectable->has_resource = ags_channel_has_resource;

  connectable->is_ready = ags_channel_is_ready;
  connectable->add_to_registry = ags_channel_add_to_registry;
  connectable->remove_from_registry = ags_channel_remove_from_registry;

  connectable->list_resource = ags_channel_list_resource;
  connectable->xml_compose = ags_channel_xml_compose;
  connectable->xml_parse = ags_channel_xml_parse;

  connectable->is_connected = ags_channel_is_connected;  
  connectable->connect = ags_channel_connect;
  connectable->disconnect = ags_channel_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

GQuark
ags_channel_error_quark()
{
  return(g_quark_from_static_string("ags-channel-error-quark"));
}

void
ags_channel_init(AgsChannel *channel)
{
  AgsConfig *config;

  gchar *str;

  channel->flags = 0;
  channel->ability_flags = 0;
  channel->behaviour_flags = 0;
  memset(channel->staging_flags, 0, AGS_SOUND_SCOPE_LAST * sizeof(guint));

  memset(channel->staging_completed, FALSE, AGS_SOUND_SCOPE_LAST * sizeof(gboolean));

  /* channel mutex */
  g_rec_mutex_init(&(channel->obj_mutex));

  /* uuid */
  channel->uuid = ags_uuid_alloc();
  ags_uuid_generate(channel->uuid);

  /* config */
  config = ags_config_get_instance();

  /* base init */
  channel->audio = NULL;

  channel->output_soundcard = NULL;
  channel->output_soundcard_channel = -1;

  channel->input_soundcard = NULL;
  channel->input_soundcard_channel = 0;

  /* presets */
  channel->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  channel->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  channel->format = ags_soundcard_helper_config_get_format(config);

  /* allocation info */
  channel->pad = 0;
  channel->audio_channel = 0;
  channel->line = 0;

  channel->octave = AGS_CHANNEL_DEFAULT_OCTAVE;
  channel->key = AGS_CHANNEL_DEFAULT_OCTAVE_SEMITONE;
  channel->absolute_key = AGS_CHANNEL_DEFAULT_SEMITONE;
  
  channel->note_frequency = AGS_CHANNEL_DEFAULT_NOTE_FREQUENCY;
  channel->note_key = NULL;

  channel->midi_note = AGS_CHANNEL_DEFAULT_MIDI_NOTE;

  /* inter-connected channels */
  channel->prev = NULL;
  channel->prev_pad = NULL;
  channel->next = NULL;
  channel->next_pad = NULL;

  /* link and recycling */
  channel->link = NULL;
  
  channel->first_recycling = NULL;
  channel->last_recycling = NULL;

  /* playback */
  channel->playback = (GObject *) ags_playback_new((GObject *) channel);
  g_object_ref(channel->playback);

  /* pattern */
  channel->pattern = NULL;

  /* recall id and recycling context */
  channel->recall_id = NULL;
  channel->recycling_context = NULL;

  /* recall container */
  channel->recall_container = NULL;

  /* play */
  g_rec_mutex_init(&(channel->play_mutex));

  channel->play = NULL;

  /* recall */
  g_rec_mutex_init(&(channel->recall_mutex));

  channel->recall = NULL;

  /* data */
  channel->line_widget = NULL;
  channel->file_data = NULL;
}

void
ags_channel_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;
  
  GRecMutex *channel_mutex;

  channel = AGS_CHANNEL(gobject);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = (AgsAudio *) g_value_get_object(value);

    g_rec_mutex_lock(channel_mutex);

    if((AgsAudio *) channel->audio == audio){
      g_rec_mutex_unlock(channel_mutex);
	
      return;
    }

    if(channel->audio != NULL){
      g_object_unref(G_OBJECT(channel->audio));
    }

    if(audio != NULL){
      g_object_ref(G_OBJECT(audio));
    }

    channel->audio = (GObject *) audio;

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    GObject *output_soundcard;

    output_soundcard = (GObject *) g_value_get_object(value);

    ags_channel_real_set_output_soundcard(channel,
					  output_soundcard);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->output_soundcard_channel = g_value_get_int(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    GObject *input_soundcard;

    input_soundcard = (GObject *) g_value_get_object(value);

    ags_channel_real_set_input_soundcard(channel,
					 input_soundcard);
  }
  break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->input_soundcard_channel = g_value_get_int(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    ags_channel_real_set_samplerate(channel,
				    samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    ags_channel_real_set_buffer_size(channel,
				     buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    ags_channel_real_set_format(channel,
				format);
  }
  break;
  case PROP_PAD:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->pad = g_value_get_uint(value);
      
    g_rec_mutex_unlock(channel_mutex);	
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->audio_channel = g_value_get_uint(value);
      
    g_rec_mutex_unlock(channel_mutex);	
  }
  break;
  case PROP_LINE:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->line = g_value_get_uint(value);
      
    g_rec_mutex_unlock(channel_mutex);	
  }
  break;    
  case PROP_OCTAVE:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->octave = g_value_get_int(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_KEY:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->key = g_value_get_uint(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_ABSOLUTE_KEY:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->absolute_key = g_value_get_int(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NOTE_FREQUENCY:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->note_frequency = g_value_get_double(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NOTE_KEY:
  {
    gchar *note_key;

    note_key = g_value_get_string(value);

    g_rec_mutex_lock(channel_mutex);

    if(channel->note_key == note_key){
      g_rec_mutex_unlock(channel_mutex);
	
      return;
    }

    if(channel->note_key != NULL){
      g_free(channel->note_key);
    }

    channel->note_key = g_strdup(note_key);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_MIDI_NOTE:
  {
    g_rec_mutex_lock(channel_mutex);

    channel->midi_note = g_value_get_uint(value);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_LINK:
  {
    AgsChannel *link;

    link = (AgsChannel *) g_value_get_object(value);

    g_rec_mutex_lock(channel_mutex);
      
    if(channel->link == link){
      g_rec_mutex_unlock(channel_mutex);
	
      return;
    }

    g_rec_mutex_unlock(channel_mutex);
      
    ags_channel_set_link(channel,
			 link,
			 NULL);
  }
  break;
  case PROP_PLAYBACK:
  {
    AgsPlayback *playback;

    playback = (AgsPlayback *) g_value_get_object(value);

    g_rec_mutex_lock(channel_mutex);
      
    if(channel->playback == (GObject *) playback){
      g_rec_mutex_unlock(channel_mutex);

      return;
    }

    if(channel->playback != NULL){
      g_object_unref(channel->playback);
    }

    if(playback != NULL){
      g_object_ref(playback);
    }

    channel->playback = (GObject *) playback;

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PATTERN:
  {
    AgsPattern *pattern;

    pattern = (AgsPattern *) g_value_get_pointer(value);

    ags_channel_add_pattern(channel,
			    (GObject *) pattern);
  }
  break;
  case PROP_RECALL_ID:
  {
    AgsRecallID *recall_id;

    recall_id = (AgsRecallID *) g_value_get_pointer(value);

    ags_channel_add_recall_id(channel,
			      recall_id);
  }
  break;
  case PROP_RECYCLING_CONTEXT:
  {
    AgsRecyclingContext *recycling_context;

    recycling_context = (AgsRecyclingContext *) g_value_get_pointer(value);

    g_rec_mutex_lock(channel_mutex);

    if(recycling_context == NULL ||
       g_list_find(channel->recycling_context, recycling_context) != NULL){
      g_rec_mutex_unlock(channel_mutex);

      return;
    }

    channel->recycling_context = g_list_prepend(channel->recycling_context,
						recycling_context);
    g_object_ref(recycling_context);
      
    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_RECALL_CONTAINER:
  {
    AgsRecallContainer *recall_container;

    recall_container = (AgsRecallContainer *) g_value_get_pointer(value);

    ags_channel_add_recall_container(channel,
				     (GObject *) recall_container);
  }
  break;
  case PROP_PLAY:
  {
    AgsRecall *play;

    play = (AgsRecall *) g_value_get_pointer(value);

    ags_channel_add_recall(channel,
			   (GObject *) play,
			   TRUE);
  }
  break;
  case PROP_RECALL:
  {
    AgsRecall *recall;
      
    /*  */
    recall = (AgsRecall *) g_value_get_pointer(value);

    ags_channel_add_recall(channel,
			   (GObject *) recall,
			   FALSE);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;

  GRecMutex *channel_mutex;

  channel = AGS_CHANNEL(gobject);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->audio);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->output_soundcard);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_int(value, channel->output_soundcard_channel);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value,
		       channel->input_soundcard);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_int(value, channel->input_soundcard_channel);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->samplerate);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->buffer_size);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->format);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PAD:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->pad);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->audio_channel);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_LINE:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->line);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_ABSOLUTE_KEY:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_int(value, channel->absolute_key);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NOTE_FREQUENCY:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_double(value, channel->note_frequency);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NOTE_KEY:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_string(value, channel->note_key);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_MIDI_NOTE:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_uint(value, channel->midi_note);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PREV:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->prev);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PREV_PAD:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->prev_pad);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NEXT:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->next);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_NEXT_PAD:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->next_pad);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_LINK:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->link);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_FIRST_RECYCLING:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->first_recycling);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_LAST_RECYCLING:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->last_recycling);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PLAYBACK:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_object(value, channel->playback);

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PATTERN:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_pointer(value, g_list_copy_deep(channel->pattern,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_RECALL_ID:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_pointer(value, g_list_copy_deep(channel->recall_id,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_RECYCLING_CONTEXT:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_pointer(value, g_list_copy_deep(channel->recycling_context,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_RECALL_CONTAINER:
  {
    g_rec_mutex_lock(channel_mutex);

    g_value_set_pointer(value, g_list_copy_deep(channel->recall_container,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(channel_mutex);
  }
  break;
  case PROP_PLAY:
  {
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /*  */
    g_rec_mutex_lock(play_mutex);
      
    g_value_set_pointer(value,
			g_list_copy_deep(channel->play,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(play_mutex);
  }
  break;
  case PROP_RECALL:
  {      
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /*  */
    g_rec_mutex_lock(recall_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(channel->recall,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_dispose(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;
  AgsRecycling *end_region;

  GList *list, *list_next;
  
  gboolean dispose_recycling;
  
  channel = AGS_CHANNEL(gobject);

  ags_connectable_disconnect(AGS_CONNECTABLE(channel));

  /* audio */
  dispose_recycling = FALSE;

  if(channel->audio != NULL){
    if(((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 &&
	AGS_IS_INPUT(channel) &&
	channel->link == NULL) ||
       ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 &&
	AGS_IS_OUTPUT(channel))){
      dispose_recycling = TRUE;
    }
    
    g_object_unref(channel->audio);

    channel->audio = NULL;
  }

  /* soundcard */
  if(channel->output_soundcard != NULL){
    g_object_unref(channel->output_soundcard);

    channel->output_soundcard = NULL;
  }

  if(channel->input_soundcard != NULL){
    g_object_unref(channel->input_soundcard);

    channel->input_soundcard = NULL;
  }

  /* recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    end_region = channel->last_recycling->next;
    
    while(recycling != end_region){
      recycling_next = recycling->next;

      if(dispose_recycling){
	g_object_run_dispose((GObject *) recycling);
	g_object_unref((GObject *) recycling);
      }
      
      recycling = recycling_next;
    }

    channel->first_recycling = NULL;
    channel->last_recycling = NULL;
  }
  
  /* playback */
  if(channel->playback != NULL){
    AgsPlayback *playback;

    playback = (AgsPlayback *) channel->playback;

    //TODO:JK: stop threads
    
    g_object_set(playback,
		 "channel", NULL,
		 NULL);
    //    g_object_run_dispose(playback);
    g_object_unref(playback);
  
    channel->playback = NULL;
  }

  /* recall id */
  if(channel->recall_id != NULL){
    list = channel->recall_id;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->recall_id,
		     g_object_unref);
  
    channel->recall_id = NULL;
  }
  
  /* recall container */
  if(channel->recall_container != NULL){
    list = channel->recall_container;

    while(list != NULL){
      AgsRecall *recall_audio;
      
      GList *recall_audio_run;
      
      list_next = list->next;

      g_object_get(list->data,
		   "recall-audio", &recall_audio,
		   "recall-audio-run", &recall_audio_run,
		   NULL);
      
      if(recall_audio == NULL &&
	 recall_audio_run == NULL){
	g_object_run_dispose((GObject *) list->data);
      }

      if(recall_audio != NULL){
	g_object_unref(recall_audio);
      }
      
      g_list_free_full(recall_audio_run,
		       g_object_unref);
      
      list = list_next;
    }

    g_list_free_full(channel->recall_container,
		     g_object_unref);

    channel->recall_container = NULL;
  }
  
  /* recall */
  if(channel->recall != NULL){
    g_rec_mutex_lock(AGS_CHANNEL_GET_RECALL_MUTEX(channel));

    list = channel->recall;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose((GObject *) list->data);

      list = list_next;
    }

    g_list_free_full(channel->recall,
		     g_object_unref);
  
    channel->recall = NULL;

    g_rec_mutex_unlock(AGS_CHANNEL_GET_RECALL_MUTEX(channel));
  }
  
  /* play */
  if(channel->play != NULL){
    g_rec_mutex_lock(AGS_CHANNEL_GET_PLAY_MUTEX(channel));

    list = channel->play;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose((GObject *) list->data);

      list = list_next;
    }

    g_list_free_full(channel->play,
		     g_object_unref);

    channel->play = NULL;

    g_rec_mutex_unlock(AGS_CHANNEL_GET_PLAY_MUTEX(channel));
  }
  
  /* pattern */
  if(channel->pattern != NULL){
    list = channel->pattern;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose((GObject *) list->data);

      list = list_next;
    }
    
    g_list_free_full(channel->pattern,
		     g_object_unref);

    channel->pattern = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_channel_parent_class)->dispose(gobject);
}

void
ags_channel_finalize(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;

  channel = AGS_CHANNEL(gobject);

  /* audio */
  if(channel->audio != NULL){
    g_object_unref(channel->audio);
  }

  /* soundcard */
  if(channel->output_soundcard != NULL){
    g_object_unref(channel->output_soundcard);
  }

  if(channel->input_soundcard != NULL){
    g_object_unref(channel->input_soundcard);
  }

  /* recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    while(recycling != channel->last_recycling->next){
      recycling_next = recycling->next;

      g_object_unref((GObject *) recycling);

      recycling = recycling_next;
    }
  }

  /* key string */
  if(channel->note_key != NULL){
    free(channel->note_key);
  }

  /* playback */
  if(channel->playback != NULL){
    g_object_unref(channel->playback);
  }

  /* recall id */
  g_list_free_full(channel->recall_id,
		   g_object_unref);

  /* recall */
  g_list_free_full(channel->recall_container,
		   g_object_unref);
  
  g_list_free_full(channel->recall,
		   g_object_unref);
  g_list_free_full(channel->play,
		   g_object_unref);
  
  /* pattern */
  g_list_free_full(channel->pattern,
		   g_object_unref);
  
  /* call parent class */
  G_OBJECT_CLASS(ags_channel_parent_class)->finalize(gobject);
}

AgsUUID*
ags_channel_get_uuid(AgsConnectable *connectable)
{
  AgsChannel *channel;
  
  AgsUUID *ptr;

  GRecMutex *channel_mutex;

  channel = AGS_CHANNEL(connectable);

  /* get channel signal mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get UUID */
  g_rec_mutex_lock(channel_mutex);

  ptr = channel->uuid;

  g_rec_mutex_unlock(channel_mutex);
  
  return(ptr);
}

gboolean
ags_channel_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_channel_is_ready(AgsConnectable *connectable)
{
  AgsChannel *channel;
  
  gboolean is_ready;

  channel = AGS_CHANNEL(connectable);

  /* check is added */
  is_ready = ags_channel_test_flags(channel, AGS_CHANNEL_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_channel_add_to_registry(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *start_list, *list;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  channel = AGS_CHANNEL(connectable);

  ags_channel_set_flags(channel, AGS_CHANNEL_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) channel);
    ags_registry_add_entry(registry,
			   entry);
  }

  /* get some fields */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  /* add recycling */
  if(first_recycling != NULL){
    recycling = first_recycling;
    g_object_ref(recycling);
    
    /* get end recycling */
    end_recycling = ags_recycling_next(last_recycling);

    while(recycling != end_recycling){
      /* add */
      ags_connectable_add_to_registry(AGS_CONNECTABLE(recycling));

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    g_object_unref(first_recycling);
    g_object_unref(last_recycling);

    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }
  }
  
  /* add play */
  g_object_get(channel,
	       "play", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* add recall */
  g_object_get(channel,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_channel_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_channel_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_channel_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_channel_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_channel_is_connected(AgsConnectable *connectable)
{
  AgsChannel *channel;
  
  gboolean is_connected;

  channel = AGS_CHANNEL(connectable);

  /* check is connected */
  is_connected = ags_channel_test_flags(channel, AGS_CHANNEL_CONNECTED);
  
  return(is_connected);
}

void
ags_channel_connect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsChannel *link;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;

  GList *start_list, *list;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  channel = AGS_CHANNEL(connectable);

  ags_channel_set_flags(channel, AGS_CHANNEL_CONNECTED);

#ifdef AGS_DEBUG
  g_message("connecting channel");
#endif

  /* get some fields */
  g_object_get(channel,
	       "link", &link,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);
  
  /* connect recycling */
  if(first_recycling != NULL &&
     (AGS_IS_OUTPUT(channel) ||
      (AGS_IS_INPUT(channel) && link == NULL))){
    recycling = first_recycling;
    g_object_ref(recycling);
    
    /* get end recycling */
    end_recycling = ags_recycling_next(last_recycling);

    while(recycling != end_recycling){
      /* connect */
      ags_connectable_connect(AGS_CONNECTABLE(recycling));

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }
  }

  if(link != NULL){
    g_object_unref(link);
  }

  if(first_recycling != NULL){
    g_object_unref(first_recycling);
  }
  
  if(last_recycling != NULL){
    g_object_unref(last_recycling);
  }

  //NOTE:JK: playback connected by playback domain
  
  /* connect pattern */
  g_object_get(channel,
	       "pattern", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
    
  /* connect recall container */
  g_object_get(channel,
	       "recall-container", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* connect recall - play context */
  g_object_get(channel,
	       "play", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* connect recall - recall context */
  g_object_get(channel,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_channel_disconnect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsChannel *link;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;

  GList *start_list, *list;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  channel = AGS_CHANNEL(connectable);
  
  ags_channel_unset_flags(channel, AGS_CHANNEL_CONNECTED);
  
#ifdef AGS_DEBUG
  g_message("disconnecting channel");
#endif
  
  /* get some fields */
  g_object_get(channel,
	       "link", &link,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);
  
  /* disconnect recycling */
  if(first_recycling != NULL &&
     (AGS_IS_OUTPUT(channel) ||
      (AGS_IS_INPUT(channel) && link == NULL))){
    recycling = first_recycling;
    g_object_ref(recycling);
    
    /* get end recycling */
    end_recycling = ags_recycling_next(last_recycling);

    while(recycling != end_recycling){
      /* disconnect */
      ags_connectable_disconnect(AGS_CONNECTABLE(recycling));

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }
  }

  if(link != NULL){
    g_object_unref(link);
  }

  if(first_recycling != NULL){
    g_object_unref(first_recycling);
  }
  
  if(last_recycling != NULL){
    g_object_unref(last_recycling);
  }
    
  /* disconnect pattern */
  g_object_get(channel,
	       "pattern", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
    
  /* disconnect recall container */
  g_object_get(channel,
	       "recall-container", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* disconnect recall - play context */
  g_object_get(channel,
	       "play", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* disconnect recall - recall context */
  g_object_get(channel,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_channel_get_obj_mutex:
 * @channel: the #AgsChannel
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @channel
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_channel_get_obj_mutex(AgsChannel *channel)
{
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  return(AGS_CHANNEL_GET_OBJ_MUTEX(channel));
}

/**
 * ags_channel_get_play_mutex:
 * @channel: the #AgsChannel
 * 
 * Get play mutex.
 * 
 * Returns: the #GRecMutex to lock @channel's play property
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_channel_get_play_mutex(AgsChannel *channel)
{
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  return(AGS_CHANNEL_GET_PLAY_MUTEX(channel));
}

/**
 * ags_channel_get_recall_mutex:
 * @channel: the #AgsChannel
 * 
 * Get recall mutex.
 * 
 * Returns: the #GRecMutex to lock @channel's recall property
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_channel_get_recall_mutex(AgsChannel *channel)
{
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  return(AGS_CHANNEL_GET_RECALL_MUTEX(channel));
}

/**
 * ags_channel_test_flags:
 * @channel: the #AgsChannel
 * @flags: the flags
 *
 * Test @flags to be set on @channel.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_channel_test_flags(AgsChannel *channel, guint flags)
{
  gboolean retval;  
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(FALSE);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test */
  g_rec_mutex_lock(channel_mutex);

  retval = (flags & (channel->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(channel_mutex);

  return(retval);
}

/**
 * ags_channel_set_flags:
 * @channel: the #AgsChannel
 * @flags: see #AgsChannelFlags-enum
 *
 * Enable a feature of @channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_flags(AgsChannel *channel, guint flags)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(channel_mutex);

  channel->flags |= flags;
  
  g_rec_mutex_unlock(channel_mutex);
}
    
/**
 * ags_channel_unset_flags:
 * @channel: the #AgsChannel
 * @flags: see #AgsChannelFlags-enum
 *
 * Disable a feature of @channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_unset_flags(AgsChannel *channel, guint flags)
{  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(channel_mutex);

  channel->flags &= (~flags);
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_test_ability_flags:
 * @channel: the #AgsChannel
 * @ability_flags: the ability flags
 *
 * Test @ability_flags to be set on @channel.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_channel_test_ability_flags(AgsChannel *channel, guint ability_flags)
{
  gboolean retval;  
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(FALSE);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test */
  g_rec_mutex_lock(channel_mutex);

  retval = (ability_flags & (channel->ability_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(channel_mutex);

  return(retval);
}

/**
 * ags_channel_set_ability_flags:
 * @channel: the #AgsChannel
 * @ability_flags: see enum AgsSoundAbilityFlags
 *
 * Enable an ability of AgsChannel.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_ability_flags(AgsChannel *channel, guint ability_flags)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  
  GObject *output_soundcard;
  
  guint samplerate, buffer_size;
  guint channel_ability_flags;
  gboolean super_threaded_channel;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get channel fields */
  g_rec_mutex_lock(channel_mutex);
  
  channel_ability_flags = channel->ability_flags;
  
  samplerate = channel->samplerate;
  buffer_size = channel->buffer_size;

  g_rec_mutex_unlock(channel_mutex);

  g_object_get(channel,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "playback", &playback,
	       NULL);
  
  /* get playback fields */
  super_threaded_channel = ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL);

  g_object_get(playback,
	       "playback-domain", &playback_domain,
	       NULL);
  
  if(super_threaded_channel){
    /* playback ability */
    if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_PLAYBACK & (channel_ability_flags)) == 0){
      AgsAudioThread *audio_thread;
      AgsChannelThread *channel_thread;

      channel_thread = ags_channel_thread_new(output_soundcard,
					      (GObject *) channel);
      ags_channel_thread_set_sound_scope(channel_thread,
					 AGS_SOUND_SCOPE_PLAYBACK);

      g_object_set(channel_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_set_channel_thread(playback,
				      (AgsThread *) channel_thread,
				      AGS_SOUND_SCOPE_PLAYBACK);

      /* set thread child */
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     AGS_SOUND_SCOPE_PLAYBACK);

      ags_thread_add_child_extended((AgsThread *) audio_thread,
				    (AgsThread *) channel_thread,
				    TRUE, TRUE);
    }

    /* sequencer ability */
    if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_SEQUENCER & (channel_ability_flags)) == 0){
      AgsAudioThread *audio_thread;
      AgsChannelThread *channel_thread;

      channel_thread = ags_channel_thread_new(output_soundcard,
					      (GObject *) channel);
      ags_channel_thread_set_sound_scope(channel_thread,
					 AGS_SOUND_SCOPE_SEQUENCER);

      g_object_set(channel_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_set_channel_thread(playback,
				      (AgsThread *) channel_thread,
				      AGS_SOUND_SCOPE_SEQUENCER);

      /* set thread child */
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     AGS_SOUND_SCOPE_SEQUENCER);

      ags_thread_add_child_extended((AgsThread *) audio_thread,
				    (AgsThread *) channel_thread,
				    TRUE, TRUE);
    }

    /* notation ability */
    if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_NOTATION & (channel_ability_flags)) == 0){
      AgsAudioThread *audio_thread;
      AgsChannelThread *channel_thread;
      
      channel_thread = ags_channel_thread_new(output_soundcard,
					      (GObject *) channel);
      ags_channel_thread_set_sound_scope(channel_thread,
					 AGS_SOUND_SCOPE_NOTATION);

      g_object_set(channel_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_set_channel_thread(playback,
				      (AgsThread *) channel_thread,
				      AGS_SOUND_SCOPE_NOTATION);    

      /* set thread child */
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     AGS_SOUND_SCOPE_NOTATION);      
      ags_thread_add_child_extended((AgsThread *) audio_thread,
				    (AgsThread *) channel_thread,
				    TRUE, TRUE);
    }

    /* wave ability */
    if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_WAVE & (channel_ability_flags)) == 0){
      AgsAudioThread *audio_thread;
      AgsChannelThread *channel_thread;

      channel_thread = ags_channel_thread_new(output_soundcard,
					      (GObject *) channel);
      ags_channel_thread_set_sound_scope(channel_thread,
					 AGS_SOUND_SCOPE_WAVE);

      g_object_set(channel_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_set_channel_thread(playback,
				      (AgsThread *) channel_thread,
				      AGS_SOUND_SCOPE_WAVE);
      
      /* set thread child */
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     AGS_SOUND_SCOPE_WAVE);

      ags_thread_add_child_extended((AgsThread *) audio_thread,
				    (AgsThread *) channel_thread,
				    TRUE, TRUE);
    }

    /* midi ability */
    if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_MIDI & (channel_ability_flags)) == 0){
      AgsAudioThread *audio_thread;
      AgsChannelThread *channel_thread;

      channel_thread = ags_channel_thread_new(output_soundcard,
					      (GObject *) channel);
      ags_channel_thread_set_sound_scope(channel_thread,
					 AGS_SOUND_SCOPE_MIDI);

      g_object_set(channel_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_set_channel_thread(playback,
				      (AgsThread *) channel_thread,
				      AGS_SOUND_SCOPE_MIDI);

      /* set thread child */
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     AGS_SOUND_SCOPE_MIDI);

      ags_thread_add_child_extended((AgsThread *) audio_thread,
				    (AgsThread *) channel_thread,
				    TRUE, TRUE);
    }
  }

  /* unref */
  g_object_unref(audio);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  if(playback != NULL){
    g_object_unref(playback);
  }

  /* set flags */
  g_rec_mutex_lock(channel_mutex);

  channel->ability_flags |= ability_flags;
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_unset_ability_flags:
 * @channel: the #AgsChannel
 * @ability_flags: see enum AgsSoundAbilityFlags
 *
 * Disable an ability of AgsChannel.
 *
 * Since: 3.0.0
 */
void
ags_channel_unset_ability_flags(AgsChannel *channel, guint ability_flags)
{
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;

  guint channel_ability_flags;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get channel fields */
  g_rec_mutex_lock(channel_mutex);
  
  channel_ability_flags = channel->ability_flags;
  
  g_rec_mutex_unlock(channel_mutex);
  
  g_object_get(channel,
	       "playback", &playback,
	       NULL);
  
  /* get playback fields */
  g_object_get(playback,
	       "playback-domain", &playback_domain,
	       NULL);
  
  /* playback ability */
  if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_PLAYBACK & (channel_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    AgsChannelThread *channel_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_PLAYBACK);
    channel_thread = (AgsChannelThread *) ags_playback_get_channel_thread(playback,
									  AGS_SOUND_SCOPE_PLAYBACK);
    ags_thread_remove_child((AgsThread *) audio_thread,
			    (AgsThread *) channel_thread);    

    ags_playback_set_channel_thread(playback,
				    NULL,
				    AGS_SOUND_SCOPE_PLAYBACK);
  }

  /* notation ability */
  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_NOTATION & (channel_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    AgsChannelThread *channel_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_NOTATION);
    channel_thread = (AgsChannelThread *) ags_playback_get_channel_thread(playback,
									  AGS_SOUND_SCOPE_NOTATION);
    ags_thread_remove_child((AgsThread *) audio_thread,
			    (AgsThread *) channel_thread);    

    ags_playback_set_channel_thread(playback,
				    NULL,
				    AGS_SOUND_SCOPE_NOTATION);
  }

  /* sequencer ability */
  if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_SEQUENCER & (channel_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    AgsChannelThread *channel_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_SEQUENCER);
    channel_thread = (AgsChannelThread *) ags_playback_get_channel_thread(playback,
									  AGS_SOUND_SCOPE_SEQUENCER);
    ags_thread_remove_child((AgsThread *) audio_thread,
			    (AgsThread *) channel_thread);    

    ags_playback_set_channel_thread(playback,
				    NULL,
				    AGS_SOUND_SCOPE_SEQUENCER);
  }

  /* wave ability */
  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_WAVE & (channel_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    AgsChannelThread *channel_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_WAVE);
    channel_thread = (AgsChannelThread *) ags_playback_get_channel_thread(playback,
									  AGS_SOUND_SCOPE_WAVE);
    ags_thread_remove_child((AgsThread *) audio_thread,
			    (AgsThread *) channel_thread);    

    ags_playback_set_channel_thread(playback,
				    NULL,
				    AGS_SOUND_SCOPE_WAVE);
  }

  /* midi ability */
  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_MIDI & (channel_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    AgsChannelThread *channel_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_MIDI);
    channel_thread = (AgsChannelThread *) ags_playback_get_channel_thread(playback,
									  AGS_SOUND_SCOPE_MIDI);
    ags_thread_remove_child((AgsThread *) audio_thread,
			    (AgsThread *) channel_thread);    

    ags_playback_set_channel_thread(playback,
				    NULL,
				    AGS_SOUND_SCOPE_MIDI);
  }

  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  if(playback != NULL){
    g_object_unref(playback);
  }
  
  /* unset flags */
  g_rec_mutex_lock(channel_mutex);

  channel->ability_flags &= (~ability_flags);
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_test_behaviour_flags:
 * @channel: the #AgsChannel
 * @behaviour_flags: the behaviour flags
 *
 * Test @behaviour_flags to be set on @channel.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_channel_test_behaviour_flags(AgsChannel *channel, guint behaviour_flags)
{
  gboolean retval;  
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(FALSE);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test */
  g_rec_mutex_lock(channel_mutex);

  retval = (behaviour_flags & (channel->behaviour_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(channel_mutex);

  return(retval);
}

/**
 * ags_channel_set_behaviour_flags:
 * @channel: the #AgsChannel
 * @behaviour_flags: the behaviour flags
 * 
 * Set behaviour flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_set_behaviour_flags(AgsChannel *channel, guint behaviour_flags)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* set flags */
  g_rec_mutex_lock(channel_mutex);

  channel->behaviour_flags |= behaviour_flags;

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_unset_behaviour_flags:
 * @channel: the #AgsChannel
 * @behaviour_flags: the behaviour flags
 * 
 * Unset behaviour flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_unset_behaviour_flags(AgsChannel *channel, guint behaviour_flags)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* unset flags */
  g_rec_mutex_lock(channel_mutex);

  channel->behaviour_flags &= (~behaviour_flags);

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_test_staging_flags:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to test
 * @staging_flags: the staging flags
 *
 * Test @staging_flags to be set on @channel.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_channel_test_staging_flags(AgsChannel *channel, gint sound_scope,
			       guint staging_flags)
{
  gboolean retval;  
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(FALSE);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test */
  g_rec_mutex_lock(channel_mutex);

  retval = (staging_flags & (channel->staging_flags[sound_scope])) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(channel_mutex);

  return(retval);
}

/**
 * ags_channel_set_staging_flags:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Set staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_set_staging_flags(AgsChannel *channel, gint sound_scope,
			      guint staging_flags)
{
  guint i;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* set flags */
  g_rec_mutex_lock(channel_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      channel->staging_flags[i] |= staging_flags;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    channel->staging_flags[sound_scope] |= staging_flags;
  }

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_unset_staging_flags:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Unset staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_unset_staging_flags(AgsChannel *channel, gint sound_scope,
				guint staging_flags)
{
  guint i;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* unset flags */
  g_rec_mutex_lock(channel_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      channel->staging_flags[i] &= (~staging_flags);
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    channel->staging_flags[sound_scope] &= (~staging_flags);
  }

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_test_staging_completed:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to test
 *
 * Test @sound_scope to be completed on @channel.
 * 
 * Returns: %TRUE if completed, otherwise %FALSE
 *
 * Since: 3.3.0
 */
gboolean
ags_channel_test_staging_completed(AgsChannel *channel, gint sound_scope)
{
  guint i;
  gboolean success;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(FALSE);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test staging completed */
  success = FALSE;
  
  g_rec_mutex_lock(channel_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(!channel->staging_completed[i]){
	break;
      }
    }

    if(i == AGS_SOUND_SCOPE_LAST){
      success = TRUE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    success = channel->staging_completed[sound_scope];
  }

  g_rec_mutex_unlock(channel_mutex);

  return(success);
}

/**
 * ags_channel_set_staging_flags:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * 
 * Set @sound_scope to be completed.
 * 
 * Since: 3.3.0
 */
void
ags_channel_set_staging_completed(AgsChannel *channel, gint sound_scope)
{
  guint i;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test staging completed */  
  g_rec_mutex_lock(channel_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      channel->staging_completed[i] = TRUE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    channel->staging_completed[sound_scope] = TRUE;
  }

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_unset_staging_flags:
 * @channel: the #AgsChannel
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * 
 * Unset @sound_scope to be completed.
 * 
 * Since: 3.3.0
 */
void
ags_channel_unset_staging_completed(AgsChannel *channel, gint sound_scope)
{
  guint i;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* test staging completed */  
  g_rec_mutex_lock(channel_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      channel->staging_completed[i] = FALSE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    channel->staging_completed[sound_scope] = FALSE;
  }

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_get_audio:
 * @channel: the #AgsChannel
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
GObject*
ags_channel_get_audio(AgsChannel *channel)
{
  GObject *audio;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_channel_set_audio:
 * @channel: the #AgsChannel
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_audio(AgsChannel *channel, GObject *audio)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "audio", audio,
	       NULL);
}

/**
 * ags_channel_next:
 * @channel: the #AgsChannel
 * 
 * Iterate @channel.
 * 
 * Returns: (transfer full): the next of #AgsChannel if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_next(AgsChannel *channel)
{
  AgsChannel *next;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get next */
  g_rec_mutex_lock(channel_mutex);

  next = channel->next;

  if(next != NULL){
    g_object_ref(next);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  return(next);
}

/**
 * ags_channel_prev:
 * @channel: the #AgsChannel
 * 
 * Iterate @channel.
 * 
 * Returns: (transfer full): the prev of #AgsChannel if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_prev(AgsChannel *channel)
{
  AgsChannel *prev;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get prev */
  g_rec_mutex_lock(channel_mutex);

  prev = channel->prev;

  if(prev != NULL){
    g_object_ref(prev);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  return(prev);
}

/**
 * ags_channel_next_pad:
 * @channel: the #AgsChannel
 * 
 * Iterate @channel.
 * 
 * Returns: (transfer full): the next pad of #AgsChannel if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_next_pad(AgsChannel *channel)
{
  AgsChannel *next_pad;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get next pad */
  g_rec_mutex_lock(channel_mutex);

  next_pad = channel->next_pad;

  if(next_pad != NULL){
    g_object_ref(next_pad);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  return(next_pad);
}

/**
 * ags_channel_prev_pad:
 * @channel: the #AgsChannel
 * 
 * Iterate @channel.
 * 
 * Returns: (transfer full): the prev pad of #AgsChannel if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_prev_pad(AgsChannel *channel)
{
  AgsChannel *prev_pad;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get prev pad */
  g_rec_mutex_lock(channel_mutex);

  prev_pad = channel->prev_pad;

  if(prev_pad != NULL){
    g_object_ref(prev_pad);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  return(prev_pad);
}

/**
 * ags_channel_first:
 * @channel: an #AgsChannel
 *
 * Iterates until the first #AgsChannel was found.
 *
 * Returns: (transfer full): the first #AgsChannel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_first(AgsChannel *channel)
{
  AgsChannel *prev;
  
  gboolean has_prev;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* first */
  channel = ags_channel_pad_first(channel);
    
  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check prev */
    has_prev = TRUE;
    
    g_rec_mutex_lock(channel_mutex);

    prev = channel->prev;
    
    if(prev == NULL){
      has_prev = FALSE;
    }else{
      g_object_ref(prev);
    }
    
    g_rec_mutex_unlock(channel_mutex);

    if(!has_prev){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);    

    channel = prev;
  }
  
  return(channel);
}

/**
 * ags_channel_last:
 * @channel: an #AgsChannel
 *
 * Iterates until the last #AgsChannel was found.
 *
 * Returns: (transfer full): the last #AgsChannel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_last(AgsChannel *channel)
{
  AgsChannel *next;
  
  gboolean has_next;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
  /* last */
  channel = ags_channel_pad_last(channel);

  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check next */
    has_next = TRUE;
    
    g_rec_mutex_lock(channel_mutex);

    next = channel->next;
    
    if(next == NULL){
      has_next = FALSE;
    }else{
      g_object_ref(next);
    }

    g_rec_mutex_unlock(channel_mutex);

    if(!has_next){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);    

    channel = next;
  }

  return(channel);
}

/**
 * ags_channel_nth:
 * @channel: an #AgsChannel
 * @nth: the count to iterate
 *
 * Iterates @nth times forward.
 *
 * Returns: (transfer full): the nth #AgsChannel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_nth(AgsChannel *channel, guint nth)
{
  AgsChannel *next;
  
  guint i;

  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_ref(channel);
  
  for(i = 0; i < nth && channel != NULL; i++){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* prepare */
    g_rec_mutex_lock(channel_mutex);
    
    next = channel->next;

    if(next != NULL){
      g_object_ref(next);
    }
    
    g_rec_mutex_unlock(channel_mutex);

    /* iterate */
    g_object_unref(channel);

    channel = next;
  }

#ifdef AGS_DEBUG
  if((nth != 0 && i != nth) || channel == NULL){
    g_message("ags_channel_nth:\n  nth channel does not exist\n  `- stopped @: i = %u; nth = %u\n", i, nth);
  }
#endif
  
  return(channel);
}

/**
 * ags_channel_pad_first:
 * @channel: an #AgsChannel
 *
 * Iterates until the first pad has been reached.
 *
 * Returns: (transfer full): the first #AgsChannel with the same audio channel as @channel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_pad_first(AgsChannel *channel)
{
  AgsChannel *prev_pad;
  
  gboolean has_prev_pad;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* pad first */
  g_object_ref(channel);
  
  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has prev pad */
    has_prev_pad = TRUE;
    
    g_rec_mutex_lock(channel_mutex);

    prev_pad = channel->prev_pad;
    
    if(prev_pad == NULL){
      has_prev_pad = FALSE;
    }else{
      g_object_ref(prev_pad);
    }

    g_rec_mutex_unlock(channel_mutex);

    if(!has_prev_pad){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);
    
    channel = prev_pad;
  }

  return(channel);
}

/**
 * ags_channel_pad_last:
 * @channel: an #AgsChannel
 *
 * Iterates until the last pad has been reached.
 *
 * Returns: (transfer full): the last #AgsChannel with the same audio channel as @channel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_pad_last(AgsChannel *channel)
{
  AgsChannel *next_pad;
  
  gboolean has_next_pad;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_ref(channel);

  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has next pad */
    has_next_pad = TRUE;
    
    g_rec_mutex_lock(channel_mutex);

    next_pad = channel->next_pad;
    
    if(next_pad == NULL){
      has_next_pad = FALSE;
    }else{
      g_object_ref(next_pad);
    }

    g_rec_mutex_unlock(channel_mutex);

    if(!has_next_pad){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);
    
    channel = next_pad;
  }

  return(channel);
}

/**
 * ags_channel_pad_nth:
 * @channel: an #AgsChannel
 * @nth: the count of pads to step
 *
 * Iterates nth times.
 *
 * Returns: (transfer full): the nth pad
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_pad_nth(AgsChannel *channel, guint nth)
{
  AgsChannel *next_pad;
  
  guint i;

  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_ref(channel);
  
  for(i = 0; i < nth && channel != NULL; i++){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* prepare */
    g_rec_mutex_lock(channel_mutex);

    next_pad = channel->next_pad;

    if(next_pad != NULL){
      g_object_ref(next_pad);
    }
    
    g_rec_mutex_unlock(channel_mutex);
    
    /* iterate */
    g_object_unref(channel);
    
    channel = next_pad;
  }

#ifdef AGS_DEBUG
  if(channel == NULL){
    g_message("ags_channel_nth_pad:\n  nth pad does not exist\n  `- stopped @: i = %u; nth = %u", i, nth);
  }
#endif
  
  return(channel);
}

/**
 * ags_channel_first_with_recycling:
 * @channel: an #AgsChannel
 *
 * Find first recycling related to @channel.
 *
 * Returns: (transfer full): the first channel with an #AgsRecycling
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_first_with_recycling(AgsChannel *channel)
{
  AgsChannel *next_pad;
  
  gboolean has_recycling;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  channel = ags_channel_pad_first(channel);

  /* first with recycling */
  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has recycling */
    has_recycling = FALSE;
    
    g_rec_mutex_lock(channel_mutex);

    next_pad = channel->next_pad;
    
    if(channel->first_recycling != NULL){
      has_recycling = TRUE;
    }else{
      if(next_pad != NULL){
	g_object_ref(next_pad);
      }
    }

    g_rec_mutex_unlock(channel_mutex);

    if(has_recycling){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);
    
    channel = next_pad;
  }

  return(channel);
}

/**
 * ags_channel_last_with_recycling:
 * @channel: an #AgsChannel
 *
 * Find last recycling related to @channel.
 *
 * Returns: (transfer full): the last channel with an #AgsRecycling
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_last_with_recycling(AgsChannel *channel)
{
  AgsChannel *prev_pad;
  
  gboolean has_recycling;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  channel = ags_channel_pad_last(channel);

  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has recycling */
    has_recycling = FALSE;
    
    g_rec_mutex_lock(channel_mutex);

    prev_pad = channel->prev_pad;
    
    if(channel->first_recycling != NULL){
      has_recycling = TRUE;
    }else{
      if(prev_pad != NULL){
	g_object_ref(prev_pad);
      }
    }
    
    g_rec_mutex_unlock(channel_mutex);

    if(has_recycling){
      break;
    }
    
    /* iterate */
    g_object_unref(channel);
    
    channel = prev_pad;
  }

  return(channel);
}

/**
 * ags_channel_prev_with_recycling:
 * @channel: an #AgsChannel
 *
 * Find previous #AgsRecycling of @channel.
 *
 * Returns: (transfer full): the prev channel with an #AgsRecycling
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_prev_with_recycling(AgsChannel *channel)
{
  AgsChannel *current, *prev_pad;
  AgsRecycling *recycling;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* prev with recycling */
  g_rec_mutex_lock(channel_mutex);

  prev_pad = channel->prev_pad;

  if(prev_pad != NULL){
    g_object_ref(prev_pad);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  current = prev_pad;
  
  while(current != NULL && current != channel){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has recycling */
    g_rec_mutex_lock(channel_mutex);

    recycling = current->last_recycling;

    prev_pad = current->prev_pad;
    
    if(prev_pad != NULL){
      g_object_ref(prev_pad);
    }
    
    g_rec_mutex_unlock(channel_mutex);
    
    if(recycling != NULL){
      break;
    }

    /* iterate */
    g_object_unref(current);
    
    current = prev_pad;
  }
  
  return(current);
}
  
/**
 * ags_channel_next_with_recycling:
 * @channel: an #AgsChannel
 *
 * Find next #AgsRecycling of @channel.
 *
 * Returns: (transfer full): the next channel with an #AgsRecycling
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_next_with_recycling(AgsChannel *channel)
{
  AgsChannel *current, *next_pad;
  AgsRecycling *recycling;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* next with recycling */
  g_rec_mutex_lock(channel_mutex);

  next_pad = channel->next_pad;

  if(next_pad != NULL){
    g_object_ref(next_pad);
  }

  g_rec_mutex_unlock(channel_mutex);

  current = next_pad;

  while(current != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* check has recycling */
    g_rec_mutex_lock(channel_mutex);
    
    recycling = current->first_recycling;

    next_pad = current->next_pad;
    
    if(next_pad != NULL){
      g_object_ref(next_pad);
    }
    
    g_rec_mutex_unlock(channel_mutex);
    
    if(recycling != NULL){
      break;
    }

    /* iterate */
    g_object_unref(current);
    
    current = next_pad;
  }  

  return(current);
}

/**
 * ags_channel_get_link:
 * @channel: the #AgsChannel
 * 
 * Ascend/descend @channel.
 * 
 * Returns: (transfer full): the link of #AgsChannel if available, otherwise %NULL
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_get_link(AgsChannel *channel)
{
  AgsChannel *link;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get next pad */
  g_rec_mutex_lock(channel_mutex);

  link = channel->link;

  if(link != NULL){
    g_object_ref(link);
  }
  
  g_rec_mutex_unlock(channel_mutex);

  return(link);
}

/**
 * ags_channel_set_link:
 * @channel: an #AgsChannel to link
 * @link: an other #AgsChannel to link with
 * @error: you may retrieve a AGS_CHANNEL_ERROR_LOOP_IN_LINK error
 *
 * Change the linking of #AgsChannel objects. Sets link, calls ags_channel_reset_recycling().
 * Further it does loop detection and makes your machine running.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
		     GError **error)
{
  AgsChannel *old_channel_link, *old_link_link;
  AgsChannel *old_channel_level, *old_link_level;
  
  GError *this_error;

  GRecMutex *channel_mutex;
  GRecMutex *link_mutex;

#ifdef AGS_DEBUG
  g_message("set link %x %x", channel, link);
#endif

  if(channel == NULL &&
     link == NULL){
    return;
  }

  if(channel != NULL &&
     !AGS_IS_CHANNEL(channel)){
    g_warning("ags_channel_set_link() - unsupported type");
    
    return;
  }

  if(link != NULL &&
     !AGS_IS_CHANNEL(link)){
    g_warning("ags_channel_set_link() - unsupported type");
    
    return;
  }

  /* get channel and link mutex */
  channel_mutex = NULL;
  link_mutex = NULL;
  
  if(channel != NULL){
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  }

  if(link != NULL){
    link_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(link);
  }
  
  //FIXME:JK: use g_object_get() to retrieve fields
  /* get some fields */
  old_channel_link = NULL;
  old_link_link = NULL;
  
  if(channel != NULL){
    g_rec_mutex_lock(channel_mutex);
    
    old_channel_link = channel->link;

    g_rec_mutex_unlock(channel_mutex);

    /* check no change */
    if(old_channel_link == link){
      return;
    }
  }else{
    g_rec_mutex_lock(link_mutex);
    
    old_link_link = link->link;

    g_rec_mutex_unlock(link_mutex);

    /* check no change */
    if(old_link_link == NULL){
      return;
    }
  }
  
  /* get some fields */
  if(link != NULL){
    g_rec_mutex_lock(link_mutex);
    
    old_link_link = link->link;

    g_rec_mutex_unlock(link_mutex);
  }

  /* check for a loop */
  if(channel != NULL &&
     link != NULL){
    AgsAudio *audio, *current_audio;
    AgsChannel *start_output, *start_input;
    AgsChannel *current_channel, *current_link;

    GObject *soundcard;
    
    guint current_audio_channel, current_line;
    
    if(AGS_IS_OUTPUT(channel)){
      /* get audio */
      g_object_get(link,
		   "audio", &audio,
		   "line", &current_line,
		   "audio-channel", &current_audio_channel,
		   NULL);

      /* set current */
      current_channel = link;

      g_object_ref(current_channel);
    }else{
      /* get audio */
      g_object_get(channel,
		   "audio", &audio,
		   "line", &current_line,
		   "audio-channel", &current_audio_channel,
		   NULL);

      /* set current */
      current_channel = channel;

      g_object_ref(current_channel);
    }
    
    /* get some fields */
    g_object_get(audio,
		 "output", &start_output,
		 "input", &start_input,
		 NULL);

    /*  */
    if(ags_audio_test_flags(audio, AGS_AUDIO_ASYNC)){
      current_channel = ags_channel_nth(start_output,
					current_audio_channel);
    }else{
      current_channel = ags_channel_nth(start_output,
					current_line);
    }
    
    /* unref */
    if(start_output != NULL){
      g_object_unref(start_output);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }

    current_link = NULL;
    
    if(current_channel != NULL){
      /* ascend tree */
      current_link = ags_channel_get_link(current_channel);

      g_object_unref(current_channel);

      current_channel = current_link;
    }

    if(current_channel != NULL){
      /* get some fields */
      g_object_get(current_channel,
		   "audio", &current_audio,
		   NULL);

      while(current_channel != NULL){
	/* check audio */
	if(current_audio == audio){
	  if(error != NULL){
	    g_set_error(error,
			AGS_CHANNEL_ERROR,
			AGS_CHANNEL_ERROR_LOOP_IN_LINK,
			"failed to link channel");
	  }

	  return;
	}

	/* get some fields */
	g_object_get(current_audio,
		     "output", &start_output,
		     NULL);

	/* get some fields */
	g_object_get(current_channel,
		     "line", &current_line,
		     "audio-channel", &current_audio_channel,
		     NULL);

	if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
	  current_channel = ags_channel_nth(start_output,
					    current_audio_channel);
	}else{
	  current_channel = ags_channel_nth(start_output,
					    current_line);
	}
    
	/* unref */
	if(start_output != NULL){
	  g_object_unref(start_output);
	}

	if(current_channel != NULL){
	  /* ascend tree */
	  current_link = ags_channel_get_link(current_channel);

	  g_object_unref(current_channel);

	  current_channel = current_link;
	}	  

	g_object_unref(current_audio);
	
	if(current_channel != NULL){
	  /* get audio */
	  g_object_get(current_channel,
		       "audio", &current_audio,
		       NULL);
	}
      }
    }

    if(current_link != NULL){
      g_object_unref(current_link);
    }
  }

  /* set channel link */
  if(channel != NULL){
    g_rec_mutex_lock(channel_mutex);

    channel->link = link;

    g_rec_mutex_unlock(channel_mutex);
  }  

  /* set link link */
  if(link != NULL){
    g_rec_mutex_lock(link_mutex);

    link->link = channel;

    g_rec_mutex_unlock(link_mutex);
  }

  /* ref count */
  if(channel != NULL && link != NULL){
    g_object_ref(channel);
    g_object_ref(link);
  }

  if(channel != NULL &&
     old_channel_link != NULL){
    g_object_unref(channel);
    g_object_unref(old_channel_link);    
  }

  if(link != NULL &&
     old_link_link != NULL){
    g_object_unref(link);
    g_object_unref(old_link_link);    
  }
  
  /* notify link */
  if(channel != NULL &&
     link != NULL){
    ags_channel_set_link(link,
			 channel,
			 &this_error);
  }

  if(channel != NULL &&
     old_channel_link != NULL){
    GRecMutex *old_channel_link_mutex;

    /* get old channel link mutex */
    old_channel_link_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(old_channel_link);

    /* reset link */
    g_rec_mutex_lock(old_channel_link_mutex);
    
    old_channel_link->link = NULL;

    g_rec_mutex_unlock(old_channel_link_mutex);

    /* emit */
    this_error = NULL;

    ags_channel_set_link(old_channel_link,
			 NULL,
			 &this_error);
  }

  if(link != NULL &&
     old_link_link != NULL){
    GRecMutex *old_link_link_mutex;

    /* get old link link mutex */
    old_link_link_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(old_link_link);

    /* reset link */
    g_rec_mutex_lock(old_link_link_mutex);

    old_link_link->link = NULL;

    g_rec_mutex_unlock(old_link_link_mutex);

    /* emit */
    this_error = NULL;

    ags_channel_set_link(old_link_link,
			 NULL,
			 &this_error);
  }  

  /* reset recycling */
  if(AGS_IS_OUTPUT(channel)){
    if(link != NULL){
      AgsRecycling *first_recycling, *last_recycling;
      
      /* get some fields */
      g_rec_mutex_lock(channel_mutex);

      first_recycling = channel->first_recycling;
      last_recycling = channel->last_recycling;

      g_rec_mutex_unlock(channel_mutex);

      /* reset */
      ags_channel_reset_recycling(link,
				  first_recycling, last_recycling);
    }
  }else if(AGS_IS_OUTPUT(link)){
    if(channel != NULL){
      AgsRecycling *first_recycling, *last_recycling;
      
      /* get some fields */
      g_rec_mutex_lock(link_mutex);

      first_recycling = link->first_recycling;
      last_recycling = link->last_recycling;

      g_rec_mutex_unlock(link_mutex);

      /* reset */
      ags_channel_reset_recycling(channel,
				  first_recycling, last_recycling);
    }
  }

  if(AGS_IS_INPUT(channel)){
    if(link == NULL){
      AgsAudio *audio;

      guint audio_flags;

      GRecMutex *audio_mutex;
      
      /* get some fields */
      g_rec_mutex_lock(channel_mutex);

      audio = (AgsAudio *) channel->audio;
      
      g_rec_mutex_unlock(channel_mutex);      

      /* get audio mutex */
      audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
      
      /* get some fields */
      g_rec_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      g_rec_mutex_unlock(audio_mutex);

      /* instantiate new recycling */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	AgsRecycling *recycling;

	GObject *output_soundcard;
	GObject *input_soundcard;

	gint output_soundcard_channel;
	gint input_soundcard_channel;
	guint samplerate;
	guint buffer_size;
	guint format;
	
	/* get some fields */
	g_rec_mutex_lock(channel_mutex);

	output_soundcard = channel->output_soundcard;
	output_soundcard_channel = channel->output_soundcard_channel;

	input_soundcard = channel->input_soundcard;
	input_soundcard_channel = channel->input_soundcard_channel;

	samplerate = channel->samplerate;
	buffer_size = channel->buffer_size;
	format = channel->format;
	
	g_rec_mutex_unlock(channel_mutex);

	/* reset */
	recycling = g_object_new(AGS_TYPE_RECYCLING,
				 "output-soundcard", output_soundcard,
				 "output-soundcard-channel", output_soundcard_channel,
				 "input-soundcard", input_soundcard,
				 "input-soundcard-channel", input_soundcard_channel,
				 "samplerate", samplerate,
				 "buffer-size", buffer_size,
				 "format", format,
				 NULL);
	ags_channel_reset_recycling(channel,
				    recycling, recycling);
      }
    }
  }else if(AGS_IS_INPUT(link)){
    if(channel == NULL){
      AgsAudio *audio;

      guint audio_flags;

      GRecMutex *audio_mutex;

      /* get some fields */
      g_rec_mutex_lock(link_mutex);

      audio = (AgsAudio *) link->audio;
      
      g_rec_mutex_unlock(link_mutex);

      /* get audio mutex */
      audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
      
      /* get some fields */
      g_rec_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      g_rec_mutex_unlock(audio_mutex);

      /* instantiate new recycling */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	AgsRecycling *recycling;

	GObject *output_soundcard;
	GObject *input_soundcard;

	gint output_soundcard_channel;
	gint input_soundcard_channel;
	guint samplerate;
	guint buffer_size;
	guint format;
	
	/* get some fields */
	g_rec_mutex_lock(link_mutex);

	output_soundcard = link->output_soundcard;
	output_soundcard_channel = link->output_soundcard_channel;

	input_soundcard = link->input_soundcard;
	input_soundcard_channel = link->input_soundcard_channel;

	samplerate = link->samplerate;
	buffer_size = link->buffer_size;
	format = link->format;
	
	g_rec_mutex_unlock(link_mutex);

	/* reset */
	recycling = g_object_new(AGS_TYPE_RECYCLING,
				 "output-soundcard", output_soundcard,
				 "output-soundcard-channel", output_soundcard_channel,
				 "input-soundcard", input_soundcard,
				 "input-soundcard-channel", input_soundcard_channel,
				 "samplerate", samplerate,
				 "buffer-size", buffer_size,
				 "format", format,
				 NULL);
	ags_channel_reset_recycling(link,
				    recycling, recycling);
      }
    }
  }

  if(AGS_IS_INPUT(old_channel_link)){
    AgsAudio *audio;

    guint audio_flags;

    GRecMutex *audio_mutex;
    GRecMutex *old_channel_link_mutex;

    /* get old channel link mutex */
    old_channel_link_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(old_channel_link);

    /* get some fields */
    g_rec_mutex_lock(old_channel_link_mutex);

    audio = (AgsAudio *) old_channel_link->audio;
      
    g_rec_mutex_unlock(old_channel_link_mutex);

    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
    /* get some fields */
    g_rec_mutex_lock(audio_mutex);
    
    audio_flags = audio->flags;
      
    g_rec_mutex_unlock(audio_mutex);

    /* instantiate new recycling */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
      AgsRecycling *recycling;
      
      GObject *output_soundcard;
      GObject *input_soundcard;
      
      gint output_soundcard_channel;
      gint input_soundcard_channel;
      guint samplerate;
      guint buffer_size;
      guint format;
	
      /* get some fields */
      g_rec_mutex_lock(old_channel_link_mutex);

      output_soundcard = old_channel_link->output_soundcard;
      output_soundcard_channel = old_channel_link->output_soundcard_channel;

      input_soundcard = old_channel_link->input_soundcard;
      input_soundcard_channel = old_channel_link->input_soundcard_channel;

      samplerate = old_channel_link->samplerate;
      buffer_size = old_channel_link->buffer_size;
      format = old_channel_link->format;
	
      g_rec_mutex_unlock(old_channel_link_mutex);

      /* reset */
      recycling = g_object_new(AGS_TYPE_RECYCLING,
			       "output-soundcard", output_soundcard,
			       "output-soundcard-channel", output_soundcard_channel,
			       "input-soundcard", input_soundcard,
			       "input-soundcard-channel", input_soundcard_channel,
			       "samplerate", samplerate,
			       "buffer-size", buffer_size,
			       "format", format,
			       NULL);
      ags_channel_reset_recycling(old_channel_link,
				  recycling, recycling);
    }else{
      ags_channel_reset_recycling(old_channel_link,
				  NULL, NULL);
    }
  }

  if(AGS_IS_INPUT(old_link_link)){
    AgsAudio *audio;

    guint audio_flags;

    GRecMutex *audio_mutex;
    GRecMutex *old_link_link_mutex;

    /* get old link link mutex */
    old_link_link_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(old_link_link);

    /* get some fields */
    g_rec_mutex_lock(old_link_link_mutex);

    audio = (AgsAudio *) old_link_link->audio;
      
    g_rec_mutex_unlock(old_link_link_mutex);

    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
    /* get some fields */
    g_rec_mutex_lock(audio_mutex);
    
    audio_flags = audio->flags;
      
    g_rec_mutex_unlock(audio_mutex);

    /* instantiate new recycling */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
      AgsRecycling *recycling;
      
      GObject *output_soundcard;
      GObject *input_soundcard;
      
      gint output_soundcard_channel;
      gint input_soundcard_channel;
      guint samplerate;
      guint buffer_size;
      guint format;
	
      /* get some fields */
      g_rec_mutex_lock(old_link_link_mutex);

      output_soundcard = old_link_link->output_soundcard;
      output_soundcard_channel = old_link_link->output_soundcard_channel;

      input_soundcard = old_link_link->input_soundcard;
      input_soundcard_channel = old_link_link->input_soundcard_channel;

      samplerate = old_link_link->samplerate;
      buffer_size = old_link_link->buffer_size;
      format = old_link_link->format;
	
      g_rec_mutex_unlock(old_link_link_mutex);

      /* reset */
      recycling = g_object_new(AGS_TYPE_RECYCLING,
			       "output-soundcard", output_soundcard,
			       "output-soundcard-channel", output_soundcard_channel,
			       "input-soundcard", input_soundcard,
			       "input-soundcard-channel", input_soundcard_channel,
			       "samplerate", samplerate,
			       "buffer-size", buffer_size,
			       "format", format,
			       NULL);
      ags_channel_reset_recycling(old_link_link,
				  recycling, recycling);
    }else{
      ags_channel_reset_recycling(old_channel_link,
				  NULL, NULL);
    }
  }

  /* reset soundcard */
  if((AGS_IS_OUTPUT(channel) &&
      AGS_IS_INPUT(link)) ||
     (AGS_IS_INPUT(channel) &&
      AGS_IS_OUTPUT(link))){
    AgsChannel *current, *current_link;
    
    GObject *output_soundcard;
    GObject *input_soundcard;
      
    gchar **parameter_name;
    
    gint output_soundcard_channel;
    gint input_soundcard_channel;
    guint n_params;
    
    GValue *value;

    GRecMutex *mutex;
    
    if(AGS_IS_INPUT(channel)){
      current = channel;
      current_link = link;

      mutex = channel_mutex;
    }else{
      current = link;
      current_link = channel;

      mutex = link_mutex;
    }
    
    /* get some fields */
    g_rec_mutex_lock(mutex);

    output_soundcard = current->output_soundcard;
    output_soundcard_channel = current->output_soundcard_channel;

    input_soundcard = current->input_soundcard;
    input_soundcard_channel = current->input_soundcard_channel;

    g_rec_mutex_unlock(mutex);

    /* allocate parameter name and value */
    n_params = 4;
    
    parameter_name = (gchar **) malloc((n_params + 1) * sizeof(gchar *));
    value = g_new0(GValue,
		   n_params);

    /* output soundcard */
    parameter_name[0] = "output-soundcard";
    g_value_init(&(value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(value[0]),
		       output_soundcard);

    /* output soundcard channel */
    parameter_name[1] = "output-soundcard-channel";
    g_value_init(&(value[1]),
		 G_TYPE_INT);
    g_value_set_int(&(value[1]),
		    output_soundcard_channel);
    
    /* input soundcard */
    parameter_name[2] = "input-soundcard";
    g_value_init(&(value[2]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(value[2]),
		       input_soundcard);

    /* input soundcard channel */
    parameter_name[3] = "input-soundcard-channel";
    g_value_init(&(value[3]),
		 G_TYPE_INT);
    g_value_set_int(&(value[3]),
		    input_soundcard_channel);

    /* terminate string vector */
    parameter_name[4] = NULL;

    /* recursive set property */
    ags_channel_recursive_set_property(current_link,
				       n_params,
				       parameter_name, value);
  }

  /* unset recall id - old channel */
  if(AGS_IS_INPUT(old_channel_link)){
    old_channel_level = ags_channel_get_level(old_channel_link);

    if(old_channel_level == NULL){
      gint i;
    
      for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	GList *recall_id;

	recall_id = ags_channel_check_scope(old_channel_link, i);
      
	if(recall_id != NULL){
	  ags_channel_recursive_run_stage(old_channel_link,
					  i, (AGS_SOUND_STAGING_CANCEL |
					      AGS_SOUND_STAGING_DONE |
					      AGS_SOUND_STAGING_REMOVE));

	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }
    }else{
      g_object_unref(old_channel_level);
    }
  }
  
  /* unset recall id - old link */
  if(AGS_IS_INPUT(old_link_link)){
    old_link_level = ags_channel_get_level(old_link_link);

    if(old_link_level == NULL){
      gint i;
    
      for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	GList *recall_id;

	recall_id = ags_channel_check_scope(old_link_link, i);
      
	if(recall_id != NULL){
	  ags_channel_recursive_run_stage(old_link_link,
					  i, (AGS_SOUND_STAGING_CANCEL |
					      AGS_SOUND_STAGING_DONE |
					      AGS_SOUND_STAGING_REMOVE));

	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }
    }else{
      g_object_unref(old_link_level);
    }
  }
  
  /* reset recall id */
  if(channel != NULL){
    gint i;
    
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      GList *recall_id;

      recall_id = ags_channel_check_scope(channel, i);
      
      if(recall_id != NULL){
	ags_channel_recursive_run_stage(link,
					i, (AGS_SOUND_STAGING_CHECK_RT_DATA |
					    AGS_SOUND_STAGING_RUN_INIT_PRE |
					    AGS_SOUND_STAGING_RUN_INIT_INTER |
					    AGS_SOUND_STAGING_RUN_INIT_POST));

	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }
  }
  
  if(link != NULL){
    gint i;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      GList *recall_id;

      recall_id = ags_channel_check_scope(link, i);
      
      if(recall_id != NULL){
	ags_channel_recursive_run_stage(channel,
					i, (AGS_SOUND_STAGING_CHECK_RT_DATA |
					    AGS_SOUND_STAGING_RUN_INIT_PRE |
					    AGS_SOUND_STAGING_RUN_INIT_INTER |
					    AGS_SOUND_STAGING_RUN_INIT_POST));

	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }
  }
}

gboolean
ags_channel_reset_recycling_recursive_input(AgsChannel *input,
					    AgsAudio **found_next, AgsAudio **found_prev,
					    AgsChannel **next_channel, AgsChannel **prev_channel,
					    AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
					    guint *complete_level_first, guint *complete_level_last,
					    gboolean *find_next, gboolean *find_prev,
					    gboolean *replace_first, gboolean *replace_last)
{
  AgsAudio *audio;
  AgsChannel *nth_channel_prev, *nth_channel_next;

  guint audio_flags;

  GRecMutex *audio_mutex;
  GRecMutex *input_mutex;
  GRecMutex *next_channel_mutex;
  GRecMutex *prev_channel_mutex;

  if(input == NULL){
    return(TRUE);
  }
    
  /* get input mutex */
  input_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(input);

  /* check done */
  g_rec_mutex_lock(input_mutex);
    
  if((input->first_recycling == replace_with_first_recycling[0] &&
      input->last_recycling == replace_with_last_recycling[0])){
    g_rec_mutex_unlock(input_mutex);
      
    return(TRUE);
  }

  /* set recycling */
  if(replace_first[0]){
    input->first_recycling = replace_with_first_recycling[0];
  }

  if(replace_last[0]){
    input->last_recycling = replace_with_last_recycling[0];
  }

  /* search for neighboor recyclings */
  audio = (AgsAudio *) input->audio;

  g_rec_mutex_unlock(input_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* audio flags */
  g_rec_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  g_rec_mutex_unlock(audio_mutex);
    
  if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
    if(find_prev[0]){
      nth_channel_prev = ags_channel_prev_with_recycling(input);

      if(nth_channel_prev != NULL){
#ifdef AGS_DEBUG
	g_message("found prev");
#endif	  
	find_prev[0] = FALSE;
	replace_first[0] = FALSE;

	if(complete_level_first[0] == 0){
	  found_prev[0] = audio;
	  prev_channel[0] = nth_channel_prev;

	  complete_level_first[0] = 1;
	}

	g_object_unref(nth_channel_prev);
      }
    }else{
      nth_channel_prev = ags_channel_prev_with_recycling(input);

      if(nth_channel_prev != NULL){
	g_object_unref(nth_channel_prev);
      }
    }
      
    if(find_next[0]){
      nth_channel_next = ags_channel_next_with_recycling(input);
	
      if(nth_channel_next != NULL){
#ifdef AGS_DEBUG
	g_message("found next");
#endif	  

	find_next[0] = FALSE;
	replace_last[0] = FALSE;
	  
	if(complete_level_last[0] == 0){
	  found_next[0] = audio;
	  next_channel[0] = nth_channel_next;
	    
	  complete_level_last[0] = 1;
	}

	g_object_unref(nth_channel_next);
      }
    }else{
      nth_channel_next = ags_channel_next_with_recycling(input);

      if(nth_channel_next != NULL){
	g_object_unref(nth_channel_next);
      }
    }

    if(prev_channel[0] != NULL){
      if(next_channel[0] == NULL){
	if(replace_with_last_recycling[0] == NULL){
	  /* get prev channel mutex */
	  prev_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_channel[0]);

	  /* prev channel */
	  g_rec_mutex_lock(prev_channel_mutex);
	    
	  replace_with_last_recycling[0] = prev_channel[0]->last_recycling;

	  g_rec_mutex_unlock(prev_channel_mutex);
	    
	  /* continues flag */
	  find_next[0] = FALSE;
	}
      }else{
	/* get prev and next channel mutex */
	prev_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_channel[0]);
	next_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(next_channel[0]);
	  
	/* prev channel */
	g_rec_mutex_lock(prev_channel_mutex);
	  
	replace_with_last_recycling[0] = prev_channel[0]->last_recycling;

	g_rec_mutex_unlock(prev_channel_mutex);

	/* next channel */
	g_rec_mutex_lock(next_channel_mutex);

	replace_with_first_recycling[0] = next_channel[0]->first_recycling;

	g_rec_mutex_unlock(next_channel_mutex);
      }
    }else{
      if(next_channel[0] != NULL){
	if(replace_with_first_recycling[0] == NULL){
	  /* get prev and next channel mutex */
	  next_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(next_channel[0]);

	  /* next channel */
	  g_rec_mutex_lock(next_channel_mutex);
	    
	  replace_with_first_recycling[0] = next_channel[0]->first_recycling;

	  g_rec_mutex_unlock(next_channel_mutex);

	  /* continues flag */
	  find_prev[0] = FALSE;
	}
      }
    }
  }
    
  if(replace_first[0] || replace_last[0]){
    return(FALSE);
  }else{
    return(TRUE);
  }
}
  
void
ags_channel_reset_recycling_recursive_output(AgsChannel *output,
					     AgsAudio **found_next, AgsAudio **found_prev,
					     AgsChannel **next_channel, AgsChannel **prev_channel,
					     AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
					     guint *complete_level_first, guint *complete_level_last,
					     gboolean *find_next, gboolean *find_prev,
					     gboolean *replace_first, gboolean *replace_last)
{
  AgsAudio *audio;
  AgsChannel *input;
  AgsChannel *link;
  AgsChannel *nth_channel_prev, *nth_channel_next;
  AgsRecycling *first_recycling, *last_recycling;

  guint audio_flags;
  guint audio_channel;

  GRecMutex *audio_mutex;
  GRecMutex *output_mutex;
    
  if(output == NULL){
    return;
  }

  /* get output mutex */
  output_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(output);

  /* get audio and audio channel */
  g_rec_mutex_lock(output_mutex);

  audio = (AgsAudio *) output->audio;

  audio_channel = output->audio_channel;

  g_rec_mutex_unlock(output_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get input and flags */    
  g_rec_mutex_lock(audio_mutex);

  input = audio->input;
  audio_flags = audio->flags;

  g_rec_mutex_unlock(audio_mutex);

  /* replace */
  if(replace_last[0]){      
    /* do it so */
    g_rec_mutex_lock(output_mutex);
      
    output->last_recycling = replace_with_last_recycling[0];

    g_rec_mutex_unlock(output_mutex);
  }

  /* last recycling */
  if(replace_first[0]){
    /* do it so */
    g_rec_mutex_lock(output_mutex);
      
    output->first_recycling = replace_with_first_recycling[0];

    g_rec_mutex_unlock(output_mutex);
  }

  /* deeper level */
  g_rec_mutex_lock(output_mutex);

  link = output->link;

  g_rec_mutex_unlock(output_mutex);
    
  if(link != NULL){
    ags_channel_reset_recycling_recursive(link,
					  found_next, found_prev,
					  next_channel, prev_channel,
					  replace_with_first_recycling, replace_with_last_recycling,
					  complete_level_first, complete_level_last,
					  find_next, find_prev,
					  replace_first, replace_last);
  }
}

void
ags_channel_reset_recycling_recursive(AgsChannel *input,
				      AgsAudio **found_next, AgsAudio **found_prev,
				      AgsChannel **next_channel, AgsChannel **prev_channel,
				      AgsRecycling **replace_with_first_recycling, AgsRecycling **replace_with_last_recycling,
				      guint *complete_level_first, guint *complete_level_last,
				      gboolean *find_next, gboolean *find_prev,
				      gboolean *replace_first, gboolean *replace_last)
{
  AgsAudio *audio;
  AgsChannel *output;
  AgsChannel *nth_output;
    
  guint audio_channel, line;
  gboolean completed;

  if(input == NULL){
    return;
  }

  /* get audio and audio channel */
  g_object_get(input,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* get output */
  g_object_get(audio,
	       "output", &output,
	       NULL);

  /* AgsInput */
  completed = ags_channel_reset_recycling_recursive_input(input,
							  found_next, found_prev,
							  next_channel, prev_channel,
							  replace_with_first_recycling, replace_with_last_recycling,
							  complete_level_first, complete_level_last,
							  find_next, find_prev,
							  replace_first, replace_last);

  if(completed){
    if(audio != NULL){
      g_object_unref(audio);
    }

    if(output != NULL){
      g_object_unref(output);
    }
      
    return;
  }
    
  /* AgsOutput */
  if(!ags_audio_test_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    if(ags_audio_test_flags(audio, AGS_AUDIO_ASYNC)){
      nth_output = ags_channel_nth(output, audio_channel);

      if(output != NULL){
	g_object_unref(output);
      }

      output = nth_output;
    }else{
      nth_output = ags_channel_nth(output, line);

      if(output != NULL){
	g_object_unref(output);
      }

      output = nth_output;
    }
      
    ags_channel_reset_recycling_recursive_output(output,
						 found_next, found_prev,
						 next_channel, prev_channel,
						 replace_with_first_recycling, replace_with_last_recycling,
						 complete_level_first, complete_level_last,
						 find_next, find_prev,
						 replace_first, replace_last);

    if(audio != NULL){
      g_object_unref(audio);
    }

    if(output != NULL){
      g_object_unref(output);
    }
  }
}

void
ags_channel_reset_recycling_reset_recycling_context_up(AgsChannel *current)
{
  //NOTE:JK: actually handled by ags_channel_set_link()
#if 0
  gint i;

  if(!AGS_IS_CHANNEL(current)){
    return;
  }
    
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    GList *recall_id;

    recall_id = ags_channel_check_scope(current, i);
      
    if(recall_id != NULL){
      ags_channel_recursive_run_stage(current,
				      i, (AGS_SOUND_STAGING_CANCEL));

      g_list_free_full(recall_id,
		       g_object_unref);
    }
  }
#endif
}
  
void
ags_channel_reset_recycling_reset_recycling_context_down(AgsChannel *current_output,
							 AgsRecyclingContext *new_recycling_context, AgsRecyclingContext *old_recycling_context)
{
  AgsAudio *current_audio;

  GList *recall_id_start, *recall_id;

  guint current_audio_flags;
    
  GRecMutex *current_audio_mutex;
  GRecMutex *current_output_mutex;

  if(!AGS_IS_CHANNEL(current_output)){
    return;
  }
    
  /* get current output mutex */
  current_output_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(current_output);

  /* get current audio */
  g_rec_mutex_lock(current_output_mutex);

  current_audio = (AgsAudio *) current_output->audio;

  g_rec_mutex_unlock(current_output_mutex);
    
  /* get current audio mutex */
  current_audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(current_audio);

  /* get recall id - output */
  g_rec_mutex_lock(current_output_mutex);

  recall_id = 
    recall_id_start = g_list_copy(current_output->recall_id);
    
  g_rec_mutex_unlock(current_output_mutex);

  /* reset - output */
  while(recall_id != NULL){
    AgsRecallID *current_recall_id;
    AgsRecyclingContext *current_recycling_context;
      
    GRecMutex *recall_id_mutex;

    current_recall_id = recall_id->data;
      
    /* get recall id mutex */
    recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

    /* get some fields */
    g_rec_mutex_lock(recall_id_mutex);

    current_recycling_context = current_recall_id->recycling_context;
      
    g_rec_mutex_unlock(recall_id_mutex);
      
    if(current_recycling_context == old_recycling_context){
      g_object_set(current_recall_id,
		   "recycling-context", new_recycling_context,
		   NULL);
    }
      
    recall_id = recall_id->next;
  }

  g_list_free(recall_id_start);

  /* get recall id - audio */
  g_rec_mutex_lock(current_audio_mutex);

  current_audio_flags = current_audio->flags;
    
  recall_id = g_list_copy(current_audio->recall_id);
    
  g_rec_mutex_unlock(current_audio_mutex);

  /* reset - audio */
  while(recall_id != NULL){
    AgsRecallID *current_recall_id;
    AgsRecyclingContext *current_recycling_context;
      
    GRecMutex *recall_id_mutex;

    current_recall_id = recall_id->data;
      
    /* get recall id mutex */
    recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

    /* get some fields */
    g_rec_mutex_lock(recall_id_mutex);

    current_recycling_context = current_recall_id->recycling_context;
      
    g_rec_mutex_unlock(recall_id_mutex);
      
    if(current_recycling_context == old_recycling_context){
      g_object_set(current_recall_id,
		   "recycling-context", new_recycling_context,
		   NULL);
    }
      
    recall_id = recall_id->next;
  }

  g_list_free(recall_id_start);

  /* check completed */
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (current_audio_flags)) != 0){
    return;
  }
    
  /* traverse the tree */
  ags_channel_reset_recycling_reset_recycling_context_down_input(current_output,
								 new_recycling_context, old_recycling_context);
}
  
void
ags_channel_reset_recycling_reset_recycling_context_down_input(AgsChannel *current_output,
							       AgsRecyclingContext *new_recycling_context, AgsRecyclingContext *old_recycling_context)
{
  AgsAudio *current_audio;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;

  GList *start_recall_id, *recall_id;

  guint audio_flags;
  guint audio_channel, line;
    
  if(!AGS_IS_CHANNEL(current_output)){
    return;
  }

  /* get current audio */
  g_object_get(current_output,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* get some fields */
  g_object_get(current_audio,
	       "input", &current_input,
	       NULL);

  /* reset and traverse */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(current_input, audio_channel);

    g_object_unref(current_input);

    current_input = nth_input;
      
    while(current_input != NULL){
      /* get recall id - input */
      g_object_get(current_input,
		   "recall-id", &start_recall_id,
		   NULL);

      current_link = ags_channel_get_link(current_input);

      /* reset - input */
      recall_id = start_recall_id;
	
      while(recall_id != NULL){
	AgsRecallID *current_recall_id;
	AgsRecyclingContext *current_recycling_context;

	current_recall_id = recall_id->data;

	g_object_get(current_recall_id,
		     "recycling-context", &current_recycling_context,
		     NULL);
      
	if(current_recycling_context == old_recycling_context){
	  g_object_set(current_recall_id,
		       "recycling-context", new_recycling_context,
		       NULL);
	}

	if(current_recycling_context != NULL){
	  g_object_unref(current_recycling_context);
	}
	  
	recall_id = recall_id->next;
      }

      g_list_free_full(start_recall_id,
		       g_object_unref);

      /* traverse the tree */
      ags_channel_reset_recycling_reset_recycling_context_down_input(current_link,
								     new_recycling_context, old_recycling_context);

      /* unref */
      if(current_link != NULL){
	g_object_unref(current_link);
      }
	
      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }
  }else{
    nth_input = ags_channel_nth(current_input, line);

    g_object_unref(current_input);

    current_input = nth_input;
      
    /* get recall id - input */
    g_object_get(current_input,
		 "recall-id", &start_recall_id,
		 NULL);

    current_link = ags_channel_get_link(current_input);

    /* reset - input */
    recall_id = start_recall_id;
      
    while(recall_id != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context;
	
      current_recall_id = recall_id->data;
      
      g_object_get(current_recall_id,
		   "recycling-context", &current_recycling_context,
		   NULL);
      
      if(current_recycling_context == old_recycling_context){
	g_object_set(current_recall_id,
		     "recycling-context", new_recycling_context,
		     NULL);
      }
      
      if(current_recycling_context != NULL){
	g_object_unref(current_recycling_context);
      }
	
      recall_id = recall_id->next;
    }

    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* traverse the tree */
    ags_channel_reset_recycling_reset_recycling_context_down_input(current_link,
								   new_recycling_context, old_recycling_context);

    /* unref */
    if(current_link != NULL){
      g_object_unref(current_link);
    }
  }

  if(current_audio != NULL){
    g_object_unref(current_audio);
  }
}
  
void
ags_channel_reset_recycling_emit_changed_input(AgsChannel *start_channel, AgsChannel *input,
					       AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
					       AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
					       AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  AgsRecycling *input_first_recycling, *input_last_recycling;
    
  GRecMutex *input_mutex;

  if(input == NULL){
    return;
  }

  /* get input mutex */
  input_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(input);

  /* get current recycling */
  g_rec_mutex_lock(input_mutex);

  input_first_recycling = input->first_recycling;
  input_last_recycling = input->last_recycling;

  g_rec_mutex_unlock(input_mutex);
    
  /* emit changed */
  if(input != start_channel){
    ags_channel_recycling_changed(input,
				  changed_old_first_recycling, changed_old_last_recycling,
				  input_first_recycling, input_last_recycling,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);
  }
}

void
ags_channel_reset_recycling_emit_changed_output(AgsChannel *start_channel, AgsChannel *output,
						AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
						AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
						AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  AgsChannel *link;
  AgsRecycling *output_first_recycling, *output_last_recycling;
    
  GRecMutex *output_mutex;

  if(output == NULL){
    return;
  }

  /* get output mutex */
  output_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(output);

  /* get current recycling */
  g_rec_mutex_lock(output_mutex);

  link = output->link;

  output_first_recycling = output->first_recycling;
  output_last_recycling = output->last_recycling;

  g_rec_mutex_unlock(output_mutex);
      
  /* emit changed */
  if(output != start_channel){
    ags_channel_recycling_changed(output,
				  changed_old_first_recycling, changed_old_last_recycling,
				  output_first_recycling, output_last_recycling,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);
  }
    
  if(link != NULL){
    ags_channel_reset_recycling_emit_changed(start_channel, link,
					     changed_old_first_recycling, changed_old_last_recycling,
					     old_first_recycling, old_last_recycling,
					     first_recycling, last_recycling);
  }
}

void
ags_channel_reset_recycling_emit_changed(AgsChannel *start_channel, AgsChannel *input,
					 AgsRecycling *changed_old_first_recycling, AgsRecycling *changed_old_last_recycling,
					 AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
					 AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  AgsAudio *audio;
  AgsChannel *output, *nth_output;

  guint audio_channel, line;

  if(input == NULL){
    return;
  }

  /* get audio and audio channel */
  g_object_get(input,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* get output */
  g_object_get(audio,
	       "output", &output,
	       NULL);
    
  /* AgsInput */
  ags_channel_reset_recycling_emit_changed_input(start_channel, input,
						 changed_old_first_recycling, changed_old_last_recycling,
						 old_first_recycling, old_last_recycling,
						 first_recycling, last_recycling);

  /* higher level */
  if(!ags_audio_test_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    if(ags_audio_test_flags(audio, AGS_AUDIO_ASYNC)){
      nth_output = ags_channel_nth(output, audio_channel);

      if(output != NULL){
	g_object_unref(output);
      }

      output = nth_output;
    }else{
      nth_output = ags_channel_nth(output, line);

      if(output != NULL){
	g_object_unref(output);
      }

      output = nth_output;
    }
      
    ags_channel_reset_recycling_emit_changed_output(start_channel, output,
						    changed_old_first_recycling, changed_old_last_recycling,
						    old_first_recycling, old_last_recycling,
						    first_recycling, last_recycling);
  }

  if(output != NULL){
    g_object_unref(output);
  }
    
  return;
}

/**
 * ags_channel_reset_recycling:
 * @channel: the channel to reset
 * @first_recycling: the recycling to set for channel->first_recycling
 * @last_recycling: the recycling to set for channel->last_recycling
 *
 * Called by ags_channel_set_link() to handle outdated #AgsRecycling references.
 * Invoke only by a task.
 *
 * Since: 3.0.0
 */
void
ags_channel_reset_recycling(AgsChannel *channel,
			    AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  AgsAudio *audio;
  AgsAudio *found_prev, *found_next;
  AgsAudio *level_audio;
  AgsChannel *prev_channel, *next_channel, *current;
  AgsChannel *level_channel, *level_link;
  AgsRecycling *parent;
  AgsRecycling *old_first_recycling, *old_last_recycling;
  AgsRecycling *replace_with_first_recycling, *replace_with_last_recycling;
  AgsRecycling *changed_old_first_recycling, *changed_old_last_recycling;
  AgsRecycling *nth_recycling, *next_recycling, *stop_recycling;
  AgsRecycling *next_first_recycling, *prev_last_recycling;

  guint audio_flags;
  guint level_audio_flags;
  guint complete_level_first, complete_level_last;
  gboolean is_output;
  gboolean replace_first, replace_last;
  gboolean find_prev, find_next;
  gboolean change_old_last, change_old_first;

  GRecMutex *audio_mutex;
  GRecMutex *level_audio_mutex;
  GRecMutex *channel_mutex;
  GRecMutex *current_mutex;
  GRecMutex *nth_channel_mutex;
  GRecMutex *level_channel_mutex;
  GRecMutex *recycling_mutex;
  GRecMutex *nth_recycling_mutex;
  
  /* entry point */
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /*  */
  g_rec_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;
  
  g_rec_mutex_unlock(channel_mutex);
  
  /* fix first or last recycling if needed */
  if(first_recycling == NULL && last_recycling != NULL){
    first_recycling = last_recycling;
  }

  if(last_recycling == NULL && first_recycling != NULL){
    last_recycling = first_recycling;
  }

  /* set old recycling */
  old_first_recycling = channel->first_recycling;
  old_last_recycling = channel->last_recycling;

  /* initialising */
  found_prev = NULL;
  found_next = NULL;

  parent = NULL;

  prev_channel = NULL;
  next_channel = NULL;

  replace_with_first_recycling = first_recycling;
  replace_with_last_recycling = last_recycling;

  changed_old_first_recycling = NULL;
  changed_old_last_recycling = NULL;

  complete_level_first = 0;
  complete_level_last = 0;
  
  replace_first = TRUE;
  replace_last = TRUE;

  find_next = TRUE;
  find_prev = TRUE;

  change_old_first = TRUE;
  change_old_last = TRUE;

  if((old_first_recycling == first_recycling)){
    replace_first = FALSE;
  }

  if((old_last_recycling == last_recycling)){
    replace_last = FALSE;
  }

  /* set recycling - update AgsChannel */
  if(AGS_IS_INPUT(channel)){
    ags_channel_reset_recycling_recursive(channel,
					  &found_next, &found_prev,
					  &next_channel, &prev_channel,
					  &replace_with_first_recycling, &replace_with_last_recycling,
					  &complete_level_first, &complete_level_last,
					  &find_next, &find_prev,
					  &replace_first, &replace_last);
  }else{
    ags_channel_reset_recycling_recursive_output(channel,
						 &found_next, &found_prev,
						 &next_channel, &prev_channel,
						 &replace_with_first_recycling, &replace_with_last_recycling,
						 &complete_level_first, &complete_level_last,
						 &find_next, &find_prev,
						 &replace_first, &replace_last);
  }

  g_rec_mutex_lock(channel_mutex);

  channel->first_recycling = first_recycling;
  channel->last_recycling = last_recycling;

  g_rec_mutex_unlock(channel_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* audio flags */
  g_rec_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  g_rec_mutex_unlock(audio_mutex);

  /* join now the retrieved recyclings */
  next_first_recycling = NULL;
  prev_last_recycling = NULL;
  
  if(!(AGS_IS_INPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0)){
    if(first_recycling != NULL){
      if(prev_channel != NULL){
	/* get prev channel mutex */
	nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_channel);

	/* get prev channel last recycling */
	g_rec_mutex_lock(nth_channel_mutex);

	prev_last_recycling = prev_channel->last_recycling;

	g_rec_mutex_unlock(nth_channel_mutex);
	
	if(prev_last_recycling != NULL){
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(prev_last_recycling);

	  /* prev channel last recycling next */
	  g_object_set(prev_last_recycling,
		       "next", first_recycling,
		       NULL);

	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(first_recycling);

	  /* first recycling prev */
	  g_object_set(first_recycling,
		       "prev", prev_last_recycling,
		       NULL);
	}else{
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(first_recycling);

	  /* first recycling prev */	  
	  g_object_set(first_recycling,
		       "prev", NULL,
		       NULL);
	}
      }else{
	/* get nth recycling mutex */
	nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(first_recycling);

	/* first recycling prev */	  
	g_object_set(first_recycling,
		     "prev", NULL,
		     NULL);
      }
    
      if(next_channel != NULL){
	/* get next channel mutex */
	nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(next_channel);
	
	/* get prev channel last recycling */
	g_rec_mutex_lock(nth_channel_mutex);

	next_first_recycling = next_channel->first_recycling;

	g_rec_mutex_unlock(nth_channel_mutex);

	if(next_first_recycling != NULL){
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(next_first_recycling);
	  
	  /* next channel first recycling next */
	  g_object_set(next_first_recycling,
		       "prev", last_recycling,
		       NULL);
	  
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(last_recycling);

	  /* last recycling next */
	  g_object_set(last_recycling,
		       "next", next_first_recycling,
		       NULL);
	}else{
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(last_recycling);

	  /* last recycling next */
	  g_object_set(last_recycling,
		       "next", NULL,
		       NULL);
	}
      }else{
	/* get nth recycling mutex */
	nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(last_recycling);

	/* last recycling next */	  
	g_object_set(last_recycling,
		     "next", NULL,
		     NULL);
      }
    }else{
      gboolean link_next, link_prev;

      if(prev_channel != NULL){
	/* get prev channel mutex */
	nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_channel);

	/*  */
	g_rec_mutex_lock(nth_channel_mutex);

	prev_last_recycling = prev_channel->last_recycling;
	
	g_rec_mutex_unlock(nth_channel_mutex);

	if(prev_last_recycling != NULL){
	  link_next = TRUE;
	}else{
	  link_next = FALSE;
	}
      }else{
	link_next = FALSE;
      }
    
      if(next_channel != NULL){
	/* get prev channel mutex */
	nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(next_channel);

	/*  */
	g_rec_mutex_lock(nth_channel_mutex);

	next_first_recycling = next_channel->first_recycling;
	
	g_rec_mutex_unlock(nth_channel_mutex);
	
	if(next_first_recycling != NULL){
	  link_prev = TRUE;
	}else{
	  link_prev = FALSE;
	}
      }else{
	link_prev = FALSE;
      }
    
      if(link_next){
	if(link_prev){
	  AgsRecycling *first_recycling, *last_recycling;

	  /* get first recycling mutex */
	  nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(next_channel);

	  g_rec_mutex_lock(nth_channel_mutex);

	  first_recycling = next_channel->first_recycling;
	  
	  g_rec_mutex_unlock(nth_channel_mutex);
	  
	  /* get last recycling mutex */
	  nth_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_channel);
	  
	  g_rec_mutex_lock(nth_channel_mutex);

	  last_recycling = prev_channel->last_recycling;
	  
	  g_rec_mutex_unlock(nth_channel_mutex);

	  /*
	   * 
	   */
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(next_first_recycling);

	  /* next channel first recycling next */
	  g_object_set(next_first_recycling,
		       "prev", last_recycling,
		       NULL);

	  /*
	   * 
	   */
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(prev_last_recycling);

	  /* prev channel last recycling next */
	  g_object_set(prev_last_recycling,
		       "next", first_recycling,
		       NULL);
	}else{
	  /* get nth recycling mutex */
	  nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(prev_last_recycling);

	  /* prev channel last recycling next */
	  g_object_set(prev_last_recycling,
		       "next", NULL,
		       NULL);
	}
      }else if(link_prev){
	/* get nth recycling mutex */
	nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(next_first_recycling);

	/* next channel first recycling next */
	g_object_set(next_first_recycling,
		     "prev", NULL,
		     NULL);
      }
    }
  }

  /* find and set parent */
  if(first_recycling != NULL){
    /* find parent */
    parent = NULL;
    
    if(AGS_IS_OUTPUT(channel)){
      g_rec_mutex_lock(channel_mutex);
      
      current = channel->link;

      g_rec_mutex_unlock(channel_mutex);
    }else{
      current = channel;
    }
    
    while(current != NULL &&
	  parent == NULL){
      AgsAudio *audio;
      AgsChannel *output;

      guint audio_channel, line;
    
      GRecMutex *audio_mutex;
    
      guint audio_flags;
      
      /* get current mutex */
      current_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(current);
      
      /* get audio */
      g_rec_mutex_lock(current_mutex);
      
      audio = (AgsAudio *) current->audio;

      audio_channel = current->audio_channel;
      line = current->line;

      g_rec_mutex_unlock(current_mutex);

      /* get audio mutex */  
      audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

      /* get nth */
      g_rec_mutex_lock(audio_mutex);
      
      output = audio->output;

      audio_flags = audio->flags;
      
      g_rec_mutex_unlock(audio_mutex);

      /* get current */
      if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	current = ags_channel_nth(output,
				  audio_channel);

	if(current != NULL){
	  g_object_unref(current); //FIXME:JK: bad ref count
	}
      }else{
	current = ags_channel_nth(output,
				  line);

	if(current != NULL){
	  g_object_unref(current); //FIXME:JK: bad ref count
	}
      }

      if(current != NULL){
	/* get current mutex */
	current_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(current);

	/* check if parent found */
	g_rec_mutex_lock(current_mutex);
	
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0){
	  /* set parent */
	  parent = current->first_recycling;
	}

	current = current->link;

	g_rec_mutex_unlock(current_mutex);
      }else{
	break;
      }
    }
    
    /* apply parent */
    nth_recycling = first_recycling;

    /* get recycling mutex */
    recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(last_recycling);

    /*  */
    g_rec_mutex_lock(recycling_mutex);
      
    stop_recycling = last_recycling->next;

    g_rec_mutex_unlock(recycling_mutex);

    /* parent - do it so */
    while(nth_recycling != stop_recycling){
      /* get nth recycling mutex */
      nth_recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(nth_recycling);

      /* set parent and iterate */
      g_object_set(nth_recycling,
		   "parent", parent,
		   NULL);
      
      g_rec_mutex_lock(nth_recycling_mutex);

      nth_recycling = nth_recycling->next;

      g_rec_mutex_unlock(nth_recycling_mutex);
    }
  }

  /* reset recycling context */
  level_channel = ags_channel_get_level(channel);
  level_audio = NULL;
  
  if(level_channel != NULL){
    /* get level channel mutex */
    level_channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(level_channel);

    /* get level audio */
    g_rec_mutex_lock(level_channel_mutex);
    
    level_audio = (AgsAudio *) level_channel->audio;

    level_link = level_channel->link;
    
    g_rec_mutex_unlock(level_channel_mutex);

    /* get level audio mutex */
    level_audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(level_audio);

    /* get some fields */
    g_rec_mutex_lock(level_audio_mutex);
    
    level_audio_flags = level_audio->flags;
    
    g_rec_mutex_unlock(level_audio_mutex);

    if(AGS_IS_INPUT(level_channel)){
      gboolean reset_recycling_context;
      
      reset_recycling_context = FALSE;

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (level_audio_flags)) != 0){
	reset_recycling_context = TRUE;
      }

      if(reset_recycling_context){
	gint i;
	
	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  GList *recall_id_start, *recall_id;
	  
	  recall_id =
	    recall_id_start = ags_channel_check_scope(level_channel, i);

	  while(recall_id != NULL){
	    AgsRecallID *current_recall_id;
	    AgsRecyclingContext *recycling_context, *new_recycling_context;

	    GRecMutex *recall_id_mutex;

	    current_recall_id = AGS_RECALL_ID(recall_id->data);

	    /* get recall id mutex */
	    recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

	    /* get recycling context */
	    g_rec_mutex_lock(recall_id_mutex);
      
	    recycling_context = current_recall_id->recycling_context;

	    g_rec_mutex_unlock(recall_id_mutex);

	    /* reset recycling context */
	    new_recycling_context = ags_recycling_context_reset_recycling(recycling_context,
									  old_first_recycling, old_last_recycling,
									  first_recycling, last_recycling);

	    /* add/remove recycling context */
	    ags_audio_remove_recycling_context(level_audio,
					       (GObject *) recycling_context);

	    ags_audio_add_recycling_context(level_audio,
					    (GObject *) new_recycling_context);

	    /* traverse the tree */
	    ags_channel_reset_recycling_reset_recycling_context_down(level_link,
								     new_recycling_context, recycling_context);
	    
	    /* iterate */
	    recall_id = recall_id->next;
	  }

	  g_list_free_full(recall_id_start,
			   g_object_unref);
	}
      }
    }else{
#ifdef AGS_DEBUG
      g_message("unexpected result as retrieving level: !AGS_IS_INPUT(level)");
#endif
    }
    
    g_object_unref(level_channel);
  }else{
    if(first_recycling == NULL){
      /* reset recycling context */
      ags_channel_reset_recycling_reset_recycling_context_up(channel);
    }else{
#ifdef AGS_DEBUG
      g_message("unexpected result: level_channel == NULL && first_recycling != NULL");
#endif
    }
  }
  
  /* emit changed */
  changed_old_first_recycling = NULL;
  changed_old_last_recycling = NULL;
  
  is_output = AGS_IS_OUTPUT(channel);

  if(!is_output){
    if(old_first_recycling != NULL){
      changed_old_first_recycling = old_first_recycling;
      changed_old_last_recycling = old_last_recycling;
    }
    
    ags_channel_reset_recycling_emit_changed_input(channel, channel,
						   changed_old_first_recycling, changed_old_last_recycling,
						   old_first_recycling, old_last_recycling,
						   first_recycling, last_recycling);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) == 0){
      AgsChannel *output;

      g_rec_mutex_lock(audio_mutex);
      
      output = audio->output;

      g_rec_mutex_unlock(audio_mutex);

      /* get matching output */
      if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	guint audio_channel;
	
	g_rec_mutex_lock(channel_mutex);
	
	audio_channel = channel->audio_channel;

	g_rec_mutex_unlock(channel_mutex);
	
	output = ags_channel_nth(output, audio_channel);

	if(output != NULL){
	  g_object_unref(output); //FIXME:JK: bad ref count
	}
      }else{
	guint line;
 
	g_rec_mutex_lock(channel_mutex);

	line = channel->line;

	g_rec_mutex_unlock(channel_mutex);
	
	output = ags_channel_nth(output, line);

	if(output != NULL){
	  g_object_unref(output); //FIXME:JK: bad ref count
	}
      }

      /* emit */
      ags_channel_reset_recycling_emit_changed_output(channel, output,
						      changed_old_first_recycling, changed_old_last_recycling,
						      old_first_recycling, old_last_recycling,
						      first_recycling, last_recycling);
    }
  }else{
    ags_channel_reset_recycling_emit_changed_output(channel, channel,
						    changed_old_first_recycling, changed_old_last_recycling,
						    old_first_recycling, old_last_recycling,
						    first_recycling, last_recycling);
  }
}

/**
 * ags_channel_recycling_changed:
 * @channel: the object recycling changed
 * @old_start_region: first recycling
 * @old_end_region: last recycling
 * @new_start_region: new first recycling
 * @new_end_region: new last recycling
 * @old_start_changed_region: modified link recycling start
 * @old_end_changed_region: modified link recyclig end
 * @new_start_changed_region: replacing link recycling start
 * @new_end_changed_region: replacing link recycling end
 *
 * Modify recycling. Asynchronously only.
 *
 * Since: 3.0.0
 */
void
ags_channel_recycling_changed(AgsChannel *channel,
			      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));
  
  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RECYCLING_CHANGED], 0,
		old_start_region, old_end_region,
		new_start_region, new_end_region,
		old_start_changed_region, old_end_changed_region,
		new_start_changed_region, new_end_changed_region);
  g_object_unref(G_OBJECT(channel));
}

/**
 * ags_channel_get_output_soundcard:
 * @channel: the #AgsChannel
 *
 * Get the output soundcard object of @channel.
 *
 * Returns: (transfer full): the output soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_channel_get_output_soundcard(AgsChannel *channel)
{
  GObject *output_soundcard;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "output-soundcard", &output_soundcard,
	       NULL);

  return(output_soundcard);
}

void
ags_channel_real_set_output_soundcard(AgsChannel *channel,
				      GObject *output_soundcard)
{
  AgsRecycling *recycling;
  AgsPlayback *playback;
  
  AgsThread *channel_thread;

  GObject *old_soundcard;

  GList *list;
  
  guint samplerate;
  guint buffer_size;
  guint format;
  gint i;
  gboolean reset_recycling;
  
  GRecMutex *channel_mutex;
  GRecMutex *playback_mutex;
  GRecMutex *play_mutex, *recall_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }
  
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* old soundcard */
  g_rec_mutex_lock(channel_mutex);

  old_soundcard = channel->output_soundcard;

  g_rec_mutex_unlock(channel_mutex);
  
  if(old_soundcard == output_soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(output_soundcard != NULL){
    g_object_ref(output_soundcard);
  }

  g_rec_mutex_lock(channel_mutex);
  
  channel->output_soundcard = (GObject *) output_soundcard;

  g_rec_mutex_unlock(channel_mutex);

  if(output_soundcard != NULL){
    /* get presets */
    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			      NULL,
			      &samplerate,
			      &buffer_size,
			      &format);

    /* apply presets */
    g_object_set(channel,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);
  }

  /* AgsRecycling */
  reset_recycling = FALSE;

  g_rec_mutex_lock(channel_mutex);

  if((AGS_IS_OUTPUT(channel) ||
      channel->link == NULL) &&
     channel->first_recycling != NULL){
    reset_recycling = TRUE;
  }

  g_rec_mutex_unlock(channel_mutex);
  
  if(reset_recycling){
    g_rec_mutex_lock(channel_mutex);

    recycling = channel->first_recycling;

    g_rec_mutex_unlock(channel_mutex);

    g_object_set(G_OBJECT(recycling),
		 "output-soundcard", output_soundcard,
		 NULL); 
  }
  
  /* playback - channel thread */
  g_rec_mutex_lock(channel_mutex);

  playback = AGS_PLAYBACK(channel->playback);

  g_rec_mutex_unlock(channel_mutex);

  /* get playback domain mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* channel thread - output soundcard */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    g_rec_mutex_lock(playback_mutex);
    
    channel_thread = playback->channel_thread[i];

    g_rec_mutex_unlock(playback_mutex);
    
    if(channel_thread != NULL){
      /* set output soundcard */      
      g_object_set(channel_thread,
		   "default-output-soundcard", output_soundcard,
		   NULL);
    }
  }
  
  /* get play mutex */
  play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

  /* play context */
  g_rec_mutex_lock(play_mutex);
  
  list = channel->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "output-soundcard", output_soundcard,
		 NULL);
    
    list = list->next;
  }
  
  g_rec_mutex_unlock(play_mutex);

  /* get recall mutex */  
  recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

  /* recall context */
  g_rec_mutex_lock(recall_mutex);

  list = channel->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "output-soundcard", output_soundcard,
		 NULL);
    
    list = list->next;
  } 

  g_rec_mutex_unlock(recall_mutex);

  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_channel_set_output_soundcard:
 * @channel: an #AgsChannel
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the output soundcard object of @channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_output_soundcard(AgsChannel *channel, GObject *output_soundcard)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "output-soundcard", output_soundcard,
	       NULL);
}

/**
 * ags_channel_get_input_soundcard:
 * @channel: the #AgsChannel
 *
 * Get the input soundcard object of @channel.
 *
 * Returns: (transfer full): the input soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_channel_get_input_soundcard(AgsChannel *channel)
{
  GObject *input_soundcard;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "input-soundcard", &input_soundcard,
	       NULL);

  return(input_soundcard);
}

void
ags_channel_real_set_input_soundcard(AgsChannel *channel,
				     GObject *input_soundcard)
{
  AgsRecycling *recycling;

  GObject *old_soundcard;

  GList *list;
  
  gboolean reset_recycling;

  GRecMutex *channel_mutex;
  GRecMutex *play_mutex, *recall_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */  
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* old soundcard */
  g_rec_mutex_lock(channel_mutex);

  old_soundcard = channel->input_soundcard;

  g_rec_mutex_unlock(channel_mutex);
  
  if(old_soundcard == input_soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(input_soundcard != NULL){
    g_object_ref(input_soundcard);
  }
  
  g_rec_mutex_lock(channel_mutex);
  
  channel->input_soundcard = (GObject *) input_soundcard;

  g_rec_mutex_unlock(channel_mutex);  

  /* AgsRecycling */
  reset_recycling = FALSE;

  g_rec_mutex_lock(channel_mutex);

  if((AGS_IS_OUTPUT(channel) ||
      channel->link == NULL) &&
     channel->first_recycling != NULL){
    reset_recycling = TRUE;
  }

  g_rec_mutex_unlock(channel_mutex);
  
  if(reset_recycling){
    g_rec_mutex_lock(channel_mutex);

    recycling = channel->first_recycling;

    g_rec_mutex_unlock(channel_mutex);

    g_object_set(G_OBJECT(recycling),
		 "input-soundcard", input_soundcard,
		 NULL); 
  }

  /* get play mutex */
  play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

  /* play context */
  g_rec_mutex_lock(play_mutex);
  
  list = channel->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "input-soundcard", input_soundcard,
		 NULL);
    
    list = list->next;
  }
  
  g_rec_mutex_unlock(play_mutex);

  /* get recall mutex */  
  recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

  /* recall context */
  g_rec_mutex_lock(recall_mutex);

  list = channel->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "input-soundcard", input_soundcard,
		 NULL);
    
    list = list->next;
  } 

  g_rec_mutex_unlock(recall_mutex);
  
  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_channel_set_input_soundcard:
 * @channel: an #AgsChannel
 * @input_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the input soundcard object of @channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_input_soundcard(AgsChannel *channel, GObject *input_soundcard)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "input-soundcard", input_soundcard,
	       NULL);
}

/**
 * ags_channel_get_samplerate:
 * @channel: the #AgsChannel
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_samplerate(AgsChannel *channel)
{
  guint samplerate;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

void
ags_channel_real_set_samplerate(AgsChannel *channel, guint samplerate)
{
  AgsChannel *link;
  AgsRecycling *recycling;
  AgsPlayback *playback;
  
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;

  gdouble frequency;
  guint old_samplerate;
  gint i;
  
  GRecMutex *channel_mutex;
  GRecMutex *playback_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* set samplerate */
  g_rec_mutex_lock(channel_mutex);

  old_samplerate = channel->samplerate;

  channel->samplerate = samplerate;

  link = channel->link;
  recycling = channel->first_recycling;
  
  playback = (AgsPlayback *) channel->playback;
  
  frequency = ceil((gdouble) channel->samplerate / (gdouble) channel->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_rec_mutex_unlock(channel_mutex);

  if(playback != NULL){
    /* get playback domain mutex */
    playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

    /* channel thread - frequency */
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      g_rec_mutex_lock(playback_mutex);
    
      channel_thread = playback->channel_thread[i];

      g_rec_mutex_unlock(playback_mutex);
    
      if(channel_thread != NULL){
	/* apply new frequency */
	g_object_set(channel_thread,
		     "frequency", frequency,
		     NULL);
      }
    }
  }
  
  if(link == NULL &&
     recycling != NULL){
    g_object_set(recycling,
		 "samplerate", samplerate,
		 NULL);
  }
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::set-samplerate");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* samplerate */
    message->parameter_name[0] = "samplerate";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     samplerate);

    /* old samplerate */
    message->parameter_name[1] = "old-samplerate";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_samplerate);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_channel_set_samplerate:
 * @channel: the #AgsChannel
 * @samplerate: the samplerate
 *
 * Set samplerate.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_samplerate(AgsChannel *channel, guint samplerate)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "samplerate", samplerate,
	       NULL);
}

/**
 * ags_channel_get_buffer_size:
 * @channel: the #AgsChannel
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_buffer_size(AgsChannel *channel)
{
  guint buffer_size;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

void
ags_channel_real_set_buffer_size(AgsChannel *channel, guint buffer_size)
{
  AgsChannel *link;
  AgsRecycling *recycling;
  AgsPlayback *playback;
  
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;

  gdouble frequency;
  guint old_buffer_size;
  gint i;
  
  GRecMutex *channel_mutex;
  GRecMutex *playback_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* set buffer size */
  g_rec_mutex_lock(channel_mutex);

  old_buffer_size = channel->buffer_size;

  channel->buffer_size = buffer_size;

  link = channel->link;
  recycling = channel->first_recycling;
  
  playback = (AgsPlayback *) channel->playback;

  frequency = ceil((gdouble) channel->samplerate / (gdouble) channel->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  
  g_rec_mutex_unlock(channel_mutex);

  if(playback != NULL){
    /* get playback domain mutex */
    playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

    /* channel thread - frequency */
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      g_rec_mutex_lock(playback_mutex);
    
      channel_thread = playback->channel_thread[i];

      g_rec_mutex_unlock(playback_mutex);
    
      if(channel_thread != NULL){
	/* apply new frequency */
	g_object_set(channel_thread,
		     "frequency", frequency,
		     NULL);
      }
    }
  }
  
  if(link == NULL &&
     recycling != NULL){
    g_object_set(recycling,
		 "buffer-size", buffer_size,
		 NULL);
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::set-buffer-size");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* buffer_size */
    message->parameter_name[0] = "buffer-size";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     buffer_size);

    /* old buffer_size */
    message->parameter_name[1] = "old-buffer-size";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_buffer_size);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_channel_set_buffer_size:
 * @channel: the #AgsChannel
 * @buffer_size: the buffer_size
 *
 * Set buffer-size.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_buffer_size(AgsChannel *channel, guint buffer_size)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "buffer-size", buffer_size,
	       NULL);
}

/**
 * ags_channel_get_format:
 * @channel: the #AgsChannel
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_format(AgsChannel *channel)
{
  guint format;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "format", &format,
	       NULL);

  return(format);
}

void
ags_channel_real_set_format(AgsChannel *channel, guint format)
{
  AgsChannel *link;
  AgsRecycling *recycling;

  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;

  guint old_format;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* set buffer size */
  g_rec_mutex_lock(channel_mutex);

  old_format = channel->format;

  channel->format = format;

  link = channel->link;
  recycling = channel->first_recycling;
    
  g_rec_mutex_unlock(channel_mutex);
  
  if(link == NULL &&
     recycling != NULL){
    g_object_set(recycling,
		 "format", format,
		 NULL);
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::set-format");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* format */
    message->parameter_name[0] = "format";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     format);

    /* old format */
    message->parameter_name[1] = "old-format";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_format);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_channel_set_format:
 * @channel: the #AgsChannel
 * @format: the format
 *
 * Set format.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_format(AgsChannel *channel, guint format)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "format", format,
	       NULL);
}

/**
 * ags_channel_get_pad:
 * @channel: the #AgsChannel
 *
 * Gets pad.
 * 
 * Returns: the pad
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_pad(AgsChannel *channel)
{
  guint pad;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "pad", &pad,
	       NULL);

  return(pad);
}

/**
 * ags_channel_set_pad:
 * @channel: the #AgsChannel
 * @pad: the pad
 *
 * Set pad.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_pad(AgsChannel *channel, guint pad)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }


  g_object_set(channel,
	       "pad", pad,
	       NULL);
}

/**
 * ags_channel_get_audio_channel:
 * @channel: the #AgsChannel
 *
 * Gets audio_channel.
 * 
 * Returns: the audio_channel
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_audio_channel(AgsChannel *channel)
{
  guint audio_channel;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "audio-channel", &audio_channel,
	       NULL);

  return(audio_channel);
}

/**
 * ags_channel_set_audio_channel:
 * @channel: the #AgsChannel
 * @audio_channel: the audio_channel
 *
 * Set audio_channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_audio_channel(AgsChannel *channel, guint audio_channel)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }


  g_object_set(channel,
	       "audio-channel", audio_channel,
	       NULL);
}

/**
 * ags_channel_get_line:
 * @channel: the #AgsChannel
 *
 * Gets line.
 * 
 * Returns: the line
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_line(AgsChannel *channel)
{
  guint line;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "line", &line,
	       NULL);

  return(line);
}

/**
 * ags_channel_set_line:
 * @channel: the #AgsChannel
 * @line: the line
 *
 * Set line.
 *
 * Since: 3.0.0
 */
void
ags_channel_set_line(AgsChannel *channel, guint line)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }


  g_object_set(channel,
	       "line", line,
	       NULL);
}

/**
 * ags_channel_get_octave:
 * @channel: the #AgsChannel
 *
 * Gets octave.
 * 
 * Returns: the octave
 * 
 * Since: 3.1.0
 */
gint
ags_channel_get_octave(AgsChannel *channel)
{
  gint octave;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "octave", &octave,
	       NULL);

  return(octave);
}

/**
 * ags_channel_set_octave:
 * @channel: the #AgsChannel
 * @octave: the octave
 *
 * Sets octave.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_octave(AgsChannel *channel, gint octave)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "octave", octave,
	       NULL);
}

/**
 * ags_channel_get_key:
 * @channel: the #AgsChannel
 *
 * Gets key.
 * 
 * Returns: the key
 * 
 * Since: 3.1.0
 */
guint
ags_channel_get_key(AgsChannel *channel)
{
  guint key;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "key", &key,
	       NULL);

  return(key);
}

/**
 * ags_channel_set_key:
 * @channel: the #AgsChannel
 * @key: the key
 *
 * Sets key.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_key(AgsChannel *channel, guint key)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "key", key,
	       NULL);
}

/**
 * ags_channel_get_absolute_key:
 * @channel: the #AgsChannel
 *
 * Gets absolute key.
 * 
 * Returns: the absolute key
 * 
 * Since: 3.1.0
 */
gint
ags_channel_get_absolute_key(AgsChannel *channel)
{
  gint absolute_key;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(0);
  }

  g_object_get(channel,
	       "absolute-key", &absolute_key,
	       NULL);

  return(absolute_key);
}

/**
 * ags_channel_set_absolute_key:
 * @channel: the #AgsChannel
 * @absolute_key: the absolute key
 *
 * Sets absolute key.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_absolute_key(AgsChannel *channel, gint absolute_key)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "absolute-key", absolute_key,
	       NULL);
}

/**
 * ags_channel_get_pattern:
 * @channel: the #AgsChannel
 * 
 * Get pattern.
 * 
 * Returns: (element-type AgsAudio.Pattern) (transfer full): the #GList-struct containig #AgsPattern
 * 
 * Since: 3.1.0
 */
GList*
ags_channel_get_pattern(AgsChannel *channel)
{
  GList *pattern;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "pattern", &pattern,
	       NULL);

  return(pattern);
}

/**
 * ags_channel_set_pattern:
 * @channel: the #AgsChannel
 * @pattern: (element-type AgsAudio.Pattern) (transfer full): the #GList-struct containing #AgsPattern
 * 
 * Set pattern by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_pattern(AgsChannel *channel, GList *pattern)
{
  GList *start_pattern;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
    
  g_rec_mutex_lock(channel_mutex);

  start_pattern = channel->pattern;
  channel->pattern = pattern;
  
  g_rec_mutex_unlock(channel_mutex);

  g_list_free_full(start_pattern,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_channel_add_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 3.0.0
 */
void
ags_channel_add_pattern(AgsChannel *channel, GObject *pattern)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /*  add pattern */
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->pattern, pattern) == NULL){
    g_object_ref(pattern);
    channel->pattern = g_list_prepend(channel->pattern,
				      pattern);
  }
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_remove_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 3.0.0
 */
void
ags_channel_remove_pattern(AgsChannel *channel, GObject *pattern)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* remove pattern */
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->pattern, pattern) != NULL){
    channel->pattern = g_list_remove(channel->pattern,
				     pattern);
    g_object_unref(G_OBJECT(pattern));
  }

  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_get_playback:
 * @channel: the #AgsChannel
 * 
 * Get playback.
 * 
 * Returns: (transfer full): the #AgsPlayback
 * 
 * Since: 3.1.0
 */
GObject*
ags_channel_get_playback(AgsChannel *channel)
{
  GObject *playback;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "playback", &playback,
	       NULL);

  return(playback);
}

/**
 * ags_channel_set_playback:
 * @channel: the #AgsChannel
 * @playback: the #AgsPlayback
 * 
 * Set playback.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_playback(AgsChannel *channel, GObject *playback)
{
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  g_object_set(channel,
	       "playback", playback,
	       NULL);
}

/**
 * ags_channel_get_recall_id:
 * @channel: the #AgsChannel
 * 
 * Get recall id.
 * 
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containig #AgsRecallID
 * 
 * Since: 3.1.0
 */
GList*
ags_channel_get_recall_id(AgsChannel *channel)
{
  GList *recall_id;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "recall_id", &recall_id,
	       NULL);

  return(recall_id);
}

/**
 * ags_channel_set_recall_id:
 * @channel: the #AgsChannel
 * @recall_id: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID
 * 
 * Set recall id by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_recall_id(AgsChannel *channel, GList *recall_id)
{
  GList *start_recall_id;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
    
  g_rec_mutex_lock(channel_mutex);

  start_recall_id = channel->recall_id;
  channel->recall_id = recall_id;
  
  g_rec_mutex_unlock(channel_mutex);

  g_list_free_full(start_recall_id,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_channel_add_recall_id:
 * @channel: an #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 3.0.0
 */
void
ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* add recall id */    
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->recall_id, recall_id) == NULL){
    g_object_ref(G_OBJECT(recall_id));
    channel->recall_id = g_list_prepend(channel->recall_id,
					recall_id);
  }
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_remove_recall_id:
 * @channel: an #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 3.0.0
 */
void
ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* remove recall id */    
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->recall_id, recall_id) != NULL){
    channel->recall_id = g_list_remove(channel->recall_id,
				       recall_id);
    g_object_unref(G_OBJECT(recall_id));
  }
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_get_recall_container:
 * @channel: the #AgsChannel
 * 
 * Get recall_container.
 * 
 * Returns: (element-type AgsAudio.RecallContainer) (transfer full): the #GList-struct containig #AgsRecallContainer
 * 
 * Since: 3.1.0
 */
GList*
ags_channel_get_recall_container(AgsChannel *channel)
{
  GList *recall_container;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "recall-container", &recall_container,
	       NULL);

  return(recall_container);
}

/**
 * ags_channel_set_recall_container:
 * @channel: the #AgsChannel
 * @recall_container: (element-type AgsAudio.RecallContainer) (transfer full): the #GList-struct containing #AgsRecallContainer
 * 
 * Set recall_container by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_recall_container(AgsChannel *channel, GList *recall_container)
{
  GList *start_recall_container;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
    
  g_rec_mutex_lock(channel_mutex);

  start_recall_container = channel->recall_container;
  channel->recall_container = recall_container;
  
  g_rec_mutex_unlock(channel_mutex);

  g_list_free_full(start_recall_container,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_channel_add_recall_container:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 3.0.0
 */
void
ags_channel_add_recall_container(AgsChannel *channel, GObject *recall_container)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* add recall container */    
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->recall_container, recall_container) == NULL){
    g_object_ref(G_OBJECT(recall_container));
    channel->recall_container = g_list_prepend(channel->recall_container,
					       recall_container);
  }
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_remove_recall_container:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 3.0.0
 */
void
ags_channel_remove_recall_container(AgsChannel *channel, GObject *recall_container)
{
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* remove recall container */    
  g_rec_mutex_lock(channel_mutex);

  if(g_list_find(channel->recall_container, recall_container) != NULL){
    channel->recall_container = g_list_remove(channel->recall_container,
					      recall_container);
    g_object_unref(G_OBJECT(recall_container));
  }
  
  g_rec_mutex_unlock(channel_mutex);
}

/**
 * ags_channel_get_play:
 * @channel: the #AgsChannel
 * 
 * Get play.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containig #AgsRecall
 * 
 * Since: 3.1.0
 */
GList*
ags_channel_get_play(AgsChannel *channel)
{
  GList *play;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "play", &play,
	       NULL);

  return(play);
}

/**
 * ags_channel_set_play:
 * @channel: the #AgsChannel
 * @play: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Set play by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_play(AgsChannel *channel, GList *play)
{
  GList *start_play;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
    
  g_rec_mutex_lock(channel_mutex);

  start_play = channel->play;
  channel->play = play;
  
  g_rec_mutex_unlock(channel_mutex);

  g_list_free_full(start_play,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_channel_get_recall:
 * @channel: the #AgsChannel
 * 
 * Get recall.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containig #AgsRecall
 * 
 * Since: 3.1.0
 */
GList*
ags_channel_get_recall(AgsChannel *channel)
{
  GList *recall;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  g_object_get(channel,
	       "recall", &recall,
	       NULL);

  return(recall);
}

/**
 * ags_channel_set_recall:
 * @channel: the #AgsChannel
 * @recall: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Set recall by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_channel_set_recall(AgsChannel *channel, GList *recall)
{
  GList *start_recall;
  
  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
    
  g_rec_mutex_lock(channel_mutex);

  start_recall = channel->recall;
  channel->recall = recall;
  
  g_rec_mutex_unlock(channel_mutex);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_channel_add_recall:
 * @channel: an #AgsChannel
 * @recall: the #AgsRecall
 * @play_context: %TRUE if play context, else if %FALSE recall context
 *
 * Adds a recall.
 *
 * Since: 3.0.0
 */
void
ags_channel_add_recall(AgsChannel *channel, GObject *recall,
		       gboolean play_context)
{
  GObject *output_soundcard, *input_soundcard;

  gint output_soundcard_channel, input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  gboolean success;

  GRecMutex *channel_mutex;
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  output_soundcard = channel->output_soundcard;
  output_soundcard_channel = channel->output_soundcard_channel;

  input_soundcard = channel->input_soundcard;
  input_soundcard_channel = channel->input_soundcard_channel;

  samplerate = channel->samplerate;
  buffer_size = channel->buffer_size;
  format = channel->format; 
  
  g_rec_mutex_unlock(channel_mutex);
  
  success = FALSE;
  g_object_set(recall,
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* add recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(channel->play, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      channel->play = g_list_prepend(channel->play,
				     recall);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* add recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(channel->recall, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      channel->recall = g_list_prepend(channel->recall,
				       recall);
    }

    g_rec_mutex_unlock(recall_mutex);
  }

  if(success){
    if(AGS_IS_RECALL_CHANNEL(recall) ||
       AGS_IS_RECALL_CHANNEL_RUN(recall)){
      g_object_set(recall,
		   "source", channel,
		   NULL);
    }
  }
}

/**
 * ags_channel_insert_recall:
 * @channel: an #AgsChannel
 * @recall: the #AgsRecall
 * @play_context: %TRUE if play context, else if %FALSE recall context
 * @position: the position
 *
 * Insert @recall at @position in @channel's @play_context.
 *
 * Since: 3.3.0
 */
void
ags_channel_insert_recall(AgsChannel *channel, GObject *recall,
			  gboolean play_context,
			  gint position)
{
  GObject *output_soundcard, *input_soundcard;

  gint output_soundcard_channel, input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  gboolean success;

  GRecMutex *channel_mutex;
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  output_soundcard = channel->output_soundcard;
  output_soundcard_channel = channel->output_soundcard_channel;

  input_soundcard = channel->input_soundcard;
  input_soundcard_channel = channel->input_soundcard_channel;

  samplerate = channel->samplerate;
  buffer_size = channel->buffer_size;
  format = channel->format; 
  
  g_rec_mutex_unlock(channel_mutex);
  
  success = FALSE;
  g_object_set(recall,
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* insert recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(channel->play, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      channel->play = g_list_insert(channel->play,
				    recall,
				    position);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* insert recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(channel->recall, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      channel->recall = g_list_insert(channel->recall,
				      recall,
				      position);
    }

    g_rec_mutex_unlock(recall_mutex);
  }

  if(success){
    if(AGS_IS_RECALL_CHANNEL(recall) ||
       AGS_IS_RECALL_CHANNEL_RUN(recall)){
      g_object_set(recall,
		   "source", channel,
		   NULL);
    }
  }
}

/**
 * ags_channel_remove_recall:
 * @channel: an #AgsChannel
 * @recall: the #AgsRecall
 * @play_context: %TRUE if play context, else if %FALSE recall context
 *
 * Removes a recall.
 *
 * Since: 3.0.0
 */
void
ags_channel_remove_recall(AgsChannel *channel, GObject *recall,
			  gboolean play_context)
{
  gboolean success;
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  success = FALSE;
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* remove recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(channel->play, recall) != NULL){
      success = TRUE;

      channel->play = g_list_remove(channel->play,
				    recall);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* remove recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(channel->recall, recall) != NULL){
      success = TRUE;
      
      channel->recall = g_list_remove(channel->recall,
				      recall);
    }

    g_rec_mutex_unlock(recall_mutex);
  }
  
  if(success){
#if 0 
    if(AGS_IS_RECALL_CHANNEL(recall) ||
       AGS_IS_RECALL_CHANNEL_RUN(recall)){
      g_object_set(recall,
		   "source", NULL,
		   "destination", NULL,
		   NULL);
    }
#endif

    g_object_unref(G_OBJECT(recall));
  }
}

GList*
ags_channel_add_ladspa_effect(AgsChannel *channel,
			      gchar *filename,
			      gchar *effect)
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallLadspa *recall_ladspa;

  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  
  GObject *output_soundcard, *input_soundcard;

  GList *recall_list;
  
  gint output_soundcard_channel, input_soundcard_channel;
  
  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  unsigned long effect_index;

  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;

  output_soundcard = channel->output_soundcard;
  output_soundcard_channel = channel->output_soundcard_channel;
  
  input_soundcard = channel->input_soundcard;
  input_soundcard_channel = channel->input_soundcard_channel;
  
  g_rec_mutex_unlock(channel_mutex);

  /* initialize return value */
  recall_list = NULL;
  
  /* load plugin */
  ladspa_manager = ags_ladspa_manager_get_instance();
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
							filename, effect);

  effect_index = AGS_BASE_PLUGIN(ladspa_plugin)->effect_index;

  /* ladspa play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);  
  AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_ladspa),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_ladspa,
			 TRUE);

  recall_list = g_list_prepend(recall_list,
			       recall_ladspa);
  
  /* load */
  ags_recall_ladspa_load(recall_ladspa);
  ags_recall_ladspa_load_ports(recall_ladspa);  
  
  /* generic */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_LADSPA_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 TRUE);

  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);
  
  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;
    
    GRecMutex *recall_id_mutex;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){
	  gint sound_scope;
	  guint staging_flags;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 TRUE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }

  /* ladspa recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);
  AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_ladspa),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_ladspa,
			 FALSE);
  
  recall_list = g_list_prepend(recall_list,
			       recall_ladspa);

  /* load */
  ags_recall_ladspa_load(recall_ladspa);
  ags_recall_ladspa_load_ports(recall_ladspa);
  
  /* generic recall */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_LADSPA_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 FALSE);  
  
  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);

  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){

	  gint sound_scope;
	  guint staging_flags;
	  
	  GRecMutex *recall_id_mutex;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);
	  

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 FALSE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }

  return(recall_list);
}

GList*
ags_channel_add_dssi_effect(AgsChannel *channel,
			    gchar *filename,
			    gchar *effect)
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallDssi *recall_dssi;

  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  GObject *output_soundcard, *input_soundcard;
  
  GList *recall_list;

  gint output_soundcard_channel, input_soundcard_channel;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  unsigned long effect_index;

  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;

  output_soundcard = channel->output_soundcard;
  output_soundcard_channel = channel->output_soundcard_channel;
  
  input_soundcard = channel->input_soundcard;
  input_soundcard_channel = channel->input_soundcard_channel;
  
  g_rec_mutex_unlock(channel_mutex);

  recall_list = NULL;
  
  /* load plugin */
  dssi_manager = ags_dssi_manager_get_instance();
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
						  filename, effect);

  effect_index = AGS_BASE_PLUGIN(dssi_plugin)->effect_index;

  /* dssi play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  AGS_RECALL(recall_dssi)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_dssi),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_dssi,
			 TRUE);

  recall_list = g_list_prepend(recall_list,
			       recall_dssi);
  
  /* load */
  ags_recall_dssi_load(recall_dssi);
  ags_recall_dssi_load_ports(recall_dssi);  
  
  /* generic */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_DSSI_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 TRUE);
  
  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);

  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;
    
    GRecMutex *recall_id_mutex;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){
	  gint sound_scope;
	  guint staging_flags;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 TRUE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }
  
  /* dssi recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  AGS_RECALL(recall_dssi)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_dssi),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_dssi,
			 FALSE);
  
  recall_list = g_list_prepend(recall_list,
			       recall_dssi);

  /* load */
  ags_recall_dssi_load(recall_dssi);
  ags_recall_dssi_load_ports(recall_dssi);
  
  /* generic */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_DSSI_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 FALSE);  
  
  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);

  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){

	  gint sound_scope;
	  guint staging_flags;
	  
	  GRecMutex *recall_id_mutex;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);
	  

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 FALSE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }
  
  return(recall_list);
}

GList*
ags_channel_add_lv2_effect(AgsChannel *channel,
			   gchar *filename,
			   gchar *effect)
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallLv2 *recall_lv2;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GObject *output_soundcard, *input_soundcard;

  GList *uri_node;
  GList *recall_list;

  gchar *uri;
  gchar *str;
  
  gint output_soundcard_channel, input_soundcard_channel;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;
  uint32_t effect_index;

  GRecMutex *channel_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;

  output_soundcard = channel->output_soundcard;
  output_soundcard_channel = channel->output_soundcard_channel;
  
  input_soundcard = channel->input_soundcard;
  input_soundcard_channel = channel->input_soundcard_channel;
  
  g_rec_mutex_unlock(channel_mutex);

  recall_list = NULL;
  
  /* load plugin */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
					       filename, effect);

  effect_index = AGS_BASE_PLUGIN(lv2_plugin)->effect_index;
  uri = lv2_plugin->uri;
  
  /* lv2 play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_lv2),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_lv2,
			 TRUE);

  recall_list = g_list_prepend(recall_list,
			       recall_lv2);

  /* load */
  ags_recall_lv2_load(recall_lv2);
  ags_recall_lv2_load_ports(recall_lv2);
    
  /* generic */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_LV2_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 TRUE);
  
  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);

  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;
    
    GRecMutex *recall_id_mutex;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){
	  gint sound_scope;
	  guint staging_flags;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 TRUE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }

  /* lv2 recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_lv2),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_lv2,
			 FALSE);

  recall_list = g_list_prepend(recall_list,
			       recall_lv2);
  
  /* load */
  ags_recall_lv2_load(recall_lv2);
  ags_recall_lv2_load_ports(recall_lv2);
  
  /* generic */
  generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
								  AGS_TYPE_GENERIC_RECALL_RECYCLING,
								  AGS_TYPE_RECALL_LV2_RUN);
  AGS_RECALL(generic_recall_channel_run)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(generic_recall_channel_run),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "recall-container", recall_container,
	       "recall-channel", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) generic_recall_channel_run,
			 FALSE);  
  
  recall_list = g_list_prepend(recall_list,
			       generic_recall_channel_run);

  /* check if connected or running */
  if(ags_connectable_is_connected(AGS_CONNECTABLE(channel))){
    AgsRecall *current;

    GList *recall_id_start, *recall_id;
    
    GRecMutex *recall_id_mutex;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

    /* get recall id */
    g_rec_mutex_lock(channel_mutex);
    
    recall_id = 
      recall_id_start = g_list_copy(channel->recall_id);

    g_rec_mutex_unlock(channel_mutex);
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
	 AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	if(ags_recall_id_check_staging_flags(recall_id->data,
					     (AGS_SOUND_STATE_IS_WAITING |
					      AGS_SOUND_STATE_IS_ACTIVE))){
	  gint sound_scope;
	  guint staging_flags;
	  
	  current = ags_recall_duplicate((AgsRecall *) generic_recall_channel_run,
					 (AgsRecallID *) recall_id->data,
					 NULL, NULL, NULL);

	  /* get recall id mutex */
	  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id->data);
	  
	  /* set appropriate scope */
	  g_rec_mutex_lock(recall_id_mutex);

	  sound_scope = AGS_RECALL_ID(recall_id->data)->sound_scope;
	  staging_flags = AGS_RECALL_ID(recall_id->data)->staging_flags;

	  g_rec_mutex_unlock(recall_id_mutex);
	  
	  ags_recall_set_sound_scope(current,
				     sound_scope);

	  /* append to AgsChannel */
	  ags_channel_add_recall(channel,
				 (GObject *) current,
				 FALSE);
	  
	  /* connect */
	  ags_connectable_connect(AGS_CONNECTABLE(current));
	  
	  /* notify run and resolve dependencies */
	  ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	  ags_recall_resolve_dependency(current);

	  /* set staging flags */	  
	  ags_recall_set_staging_flags(current,
				       staging_flags);
	}
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }

    /* free GList */
    g_list_free(recall_id_start);
  }

  return(recall_list);
}

GList*
ags_channel_real_add_effect(AgsChannel *channel,
			    gchar *filename,
			    gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;
  AgsDssiPlugin *dssi_plugin;
  AgsLv2Plugin *lv2_plugin;

  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  GList *recall_list;
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  recall_list = NULL;
  
  if(ladspa_plugin != NULL){
    recall_list = ags_channel_add_ladspa_effect(channel,
						filename,
						effect);
  }

  if(ladspa_plugin == NULL){
    dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						    filename, effect);
    
    if(dssi_plugin != NULL){
      recall_list = ags_channel_add_dssi_effect(channel,
						filename,
						effect);
    }
  }
  
  if(ladspa_plugin == NULL &&
     dssi_plugin == NULL){
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);
    
    if(lv2_plugin != NULL){
      recall_list = ags_channel_add_lv2_effect(channel,
					       filename,
					       effect);
    }
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::add-effect");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* filename */
    message->parameter_name[0] = "filename";
    g_value_init(&(message->value[0]),
		 G_TYPE_STRING);
    g_value_set_string(&(message->value[0]),
		       filename);

    /* effect */
    message->parameter_name[1] = "effect";
    g_value_init(&(message->value[1]),
		 G_TYPE_STRING);
    g_value_set_string(&(message->value[1]),
		       effect);

    /* terminate string vector */
    message->parameter_name[2] = NULL;

    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
  
  return(recall_list);
}

/**
 * ags_channel_add_effect:
 * @channel: the #AgsChannel
 * @filename: the filename
 * @effect: the effect
 * 
 * Add specified effect to @channel.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Since: 3.0.0
 */
GList*
ags_channel_add_effect(AgsChannel *channel,
		       gchar *filename,
		       gchar *effect)
{
  GList *recall_list;

  g_return_val_if_fail(AGS_IS_CHANNEL(channel), NULL);

  recall_list = NULL;

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[ADD_EFFECT], 0,
		filename,
		effect,
		&recall_list);
  g_object_unref((GObject *) channel);

  return(recall_list);
}

void
ags_channel_real_remove_effect(AgsChannel *channel,
			       guint nth)
{
  AgsAudio *audio;
  AgsRecall *recall_channel, *recall_channel_run;
  AgsRecallContainer *recall_container;

  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  GList *automation_start, *automation;
  GList *port;
  GList *list_start, *list;
  
  GList *play_start, *play;
  GList *recall_start, *recall;
  GList *task;

  gchar *specifier;
  
  guint nth_effect;

  GRecMutex *audio_mutex;
  GRecMutex *channel_mutex;
  GRecMutex *play_mutex, *recall_mutex;

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);
  recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

  /* get some fields */
  g_rec_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;
    
  g_rec_mutex_unlock(channel_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* play */
  nth_effect = 0;

  g_rec_mutex_lock(play_mutex);
  
  play =
    play_start = g_list_copy(channel->play);

  while((play = ags_recall_template_find_all_type(play,
						  AGS_TYPE_RECALL_LADSPA,
						  AGS_TYPE_RECALL_LV2,
						  G_TYPE_NONE)) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play->data)->flags)) != 0){
      nth_effect++;
    }

    if(nth_effect == nth + 1){
      break;
    }
    
    play = play->next;
  }

  g_rec_mutex_unlock(play_mutex);
  
  /* recall */
  nth_effect = 0;

  g_rec_mutex_lock(recall_mutex);
  
  recall =
    recall_start = g_list_copy(channel->recall);

  while((recall = ags_recall_template_find_all_type(recall,
						    AGS_TYPE_RECALL_LADSPA,
						    AGS_TYPE_RECALL_LV2,
						    G_TYPE_NONE)) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall->data)->flags)) != 0){
      nth_effect++;
    }

    if(nth_effect == nth + 1){
      break;
    }
    
    recall = recall->next;
  }
    
  g_rec_mutex_unlock(recall_mutex);

  /* play context */
  /* automation */
  port = NULL;

  if(play != NULL){
    g_rec_mutex_lock(play_mutex);

    port = AGS_RECALL(play->data)->port;

    g_rec_mutex_unlock(play_mutex);
  }
  
  while(port != NULL){
    specifier = AGS_PORT(port->data)->specifier;
    
    /* get automation */
    g_rec_mutex_lock(audio_mutex);

    automation =
      automation_start = g_list_copy(audio->automation);

    g_rec_mutex_unlock(audio_mutex);
    
    while((automation = ags_automation_find_specifier(automation,
						      specifier)) != NULL){
      /* remove automation  */
      ags_audio_remove_automation(audio,
				  automation->data);

      /* run dispose and unref */
      g_object_run_dispose(automation->data);
      g_object_unref(automation->data);

      /* iterate */
      automation = automation->next;
    }

    g_list_free(automation_start);
    
    /* iterate */
    port = port->next;
  }  

  /* remove - recall channel run */
  recall_container = (AgsRecallContainer *) AGS_RECALL(play->data)->recall_container;
  recall_channel = (AgsRecall *) play->data;
  recall_channel_run = (AgsRecall *) ags_recall_find_template(recall_container->recall_channel_run)->data;
  
  list =
    list_start = g_list_copy(recall_container->recall_channel_run);

  while(list != NULL){
    ags_channel_remove_recall(channel,
			      (GObject *) list->data,
			      TRUE);

    /* iterate */
    list = list->next;
  }
  
  g_list_free(list_start);

  /* remove - recall channel */
  ags_channel_remove_recall(channel,
			    (GObject *) recall_channel,
			    TRUE);

  /* remove recal container */
  ags_channel_remove_recall_container(channel,
				      (GObject *) recall_container);

  /* recall context */
  /* automation */
  port = NULL;

  if(recall != NULL){
    g_rec_mutex_lock(recall_mutex);
  
    port = AGS_RECALL(recall->data)->port;

    g_rec_mutex_unlock(recall_mutex);
  }
  
  while(port != NULL){
    specifier = AGS_PORT(port->data)->specifier;

    /* get automation */
    g_rec_mutex_lock(audio_mutex);

    automation =
      automation_start = g_list_copy(audio->automation);

    g_rec_mutex_unlock(audio_mutex);
    
    while((automation = ags_automation_find_specifier(automation,
						      specifier)) != NULL){
      /* remove automation  */
      ags_audio_remove_automation(audio,
				  automation->data);      

      /* run dispose and unref */
      g_object_run_dispose(automation->data);
      g_object_unref(automation->data);

      /* iterate */
      automation = automation->next;
    }
    
    g_list_free(automation_start);

    /* iterate */
    port = port->next;
  }

  /* remove - recall channel */
  recall_container = (AgsRecallContainer *) AGS_RECALL(recall->data)->recall_container;
  recall_channel = (AgsRecall *) recall->data;
  recall_channel_run = (AgsRecall *) ags_recall_find_template(recall_container->recall_channel_run)->data;
  
  list =
    list_start = g_list_copy(recall_container->recall_channel_run);

  while(list != NULL){
    ags_channel_remove_recall(channel,
			      (GObject *) list->data,
			      FALSE);
    
    /* iterate */
    list = list->next;
  }

  g_list_free(list_start);

  /* remove - recall channel */
  ags_channel_remove_recall(channel,
			    (GObject *) recall_channel,
			    FALSE);

  /* remove recal container */  
  ags_channel_remove_recall_container(channel,
				      (GObject *) recall_container);


  /* free lists */
  g_list_free(play_start);
  g_list_free(recall_start);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::remove-effect");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 1;
    
    message->parameter_name = (gchar **) malloc(2 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    1);

    /* nth */
    message->parameter_name[0] = "nth";
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     nth);

    /* terminate string vector */
    message->parameter_name[1] = NULL;

    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_channel_remove_effect:
 * @channel: the #AgsChannel
 * @nth: nth effect
 * 
 * Remove specified effect of @channel.
 * 
 * Since: 3.0.0
 */
void
ags_channel_remove_effect(AgsChannel *channel,
			  guint nth)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_duplicate_recall(AgsChannel *channel,
				  AgsRecallID *recall_id)
{
  AgsRecall *recall, *copy_recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  gboolean play_context;
  
  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  //  g_message("dup");
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only duplicate for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);
  
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }
  
  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  /* get the appropriate list */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list_start = g_list_copy_deep(channel->play,
				  (GCopyFunc) g_object_ref,
				  NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    play_context = FALSE;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list_start = g_list_copy_deep(channel->recall,
				  (GCopyFunc) g_object_ref,
				  NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* notify run */  
  //  ags_recall_notify_dependency(AGS_RECALL(list->data), AGS_RECALL_NOTIFY_RUN, 1);
  
  /* return if already played */
  g_rec_mutex_lock(recall_id_mutex);

  if(ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_WAITING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_ACTIVE) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_PROCESSING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_TERMINATING)){
    g_list_free_full(list_start,
		     g_object_unref);

    if(parent_recycling_context != NULL){
      g_object_unref(parent_recycling_context);
    }
  
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }
    
    g_rec_mutex_unlock(recall_id_mutex);
    
    return;
  }

  ags_recall_id_set_state_flags(recall_id,
				AGS_SOUND_STATE_IS_WAITING);

  g_rec_mutex_unlock(recall_id_mutex);
  
  /* duplicate */
  while(list != NULL){
    GRecMutex *current_recall_mutex;
    
    recall = AGS_RECALL(list->data);

    /* get current recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

    /* some checks */
    g_rec_mutex_lock(current_recall_mutex);
    
    if(AGS_IS_RECALL_CHANNEL(recall) ||
       (AGS_RECALL_TEMPLATE & (recall->flags)) == 0 ||
       recall->recall_id != NULL ||
       !ags_recall_match_ability_flags_to_scope(recall,
						sound_scope)){
      list = list->next;

      g_rec_mutex_unlock(current_recall_mutex);
      
      continue;
    }  

    g_rec_mutex_unlock(current_recall_mutex);

    /* duplicate the recall */
    copy_recall = ags_recall_duplicate(recall,
				       recall_id,
				       NULL, NULL, NULL);
      
    if(copy_recall == NULL){
      /* iterate */    
      list = list->next;

      continue;
    }
    
#ifdef AGS_DEBUG
    g_message("recall duplicated: %s %s", G_OBJECT_TYPE_NAME(channel), G_OBJECT_TYPE_NAME(copy_recall));
#endif

    /* set appropriate sound scope */
    ags_recall_set_sound_scope(copy_recall, sound_scope);
      
    /* append to AgsChannel */
    ags_channel_add_recall(channel,
			   (GObject *) copy_recall,
			   play_context);
    g_signal_connect(copy_recall, "done",
		     G_CALLBACK(ags_channel_recall_done_callback), channel);
    
    /* connect */
    ags_connectable_connect(AGS_CONNECTABLE(copy_recall));

    /* notify run */
    ags_recall_notify_dependency(copy_recall, AGS_RECALL_NOTIFY_RUN, 1);

    /* iterate */
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_duplicate_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Duplicate #AgsRecall template and assign @recall_id to it.
 *
 * Since: 3.0.0
 */
void
ags_channel_duplicate_recall(AgsChannel *channel,
			     AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[DUPLICATE_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_resolve_recall(AgsChannel *channel,
				AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  
  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only resolve for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }
    
    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* resolve */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    ags_recall_resolve_dependency(recall);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_resolve_recall:
 * @channel: an #AgsChannel
 * @recall_id: appropriate #AgsRecallID
 * 
 * Resolve step of initialization.
 *
 * Since: 3.0.0
 */
void
ags_channel_resolve_recall(AgsChannel *channel,
			   AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RESOLVE_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_init_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, guint staging_flags)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only init for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }
    
    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }
  
  /* init  */
  staging_flags = staging_mask & staging_flags;
  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* run init stages */
    ags_recall_set_state_flags(recall,
			       AGS_SOUND_STATE_IS_ACTIVE);

    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }
  
  g_list_free_full(list_start,
		   g_object_unref);

#if 0
  ags_channel_set_staging_flags(channel, sound_scope,
				staging_flags);
#endif

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_init_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 * @staging_flags: the stages to invoke
 *
 * Prepare #AgsRecall objects and invoke #AgsRecall::run-init-pre, #AgsRecall::run-init-inter or
 * #AgsRecall::run-init-post as specified by @staging_flags.
 *
 * Since: 3.0.0
 */
void
ags_channel_init_recall(AgsChannel *channel,
			AgsRecallID *recall_id, guint staging_flags)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[INIT_RECALL], 0,
		recall_id, staging_flags);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_play_recall(AgsChannel *channel,
			     AgsRecallID *recall_id, guint staging_flags)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_mask = (AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST |
				     AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
				     AGS_SOUND_STAGING_AUTOMATE |
				     AGS_SOUND_STAGING_RUN_PRE |
				     AGS_SOUND_STAGING_RUN_INTER |
				     AGS_SOUND_STAGING_RUN_POST |
				     AGS_SOUND_STAGING_DO_FEEDBACK |
				     AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE |
				     AGS_SOUND_STAGING_FINI);

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;
  
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

//  g_message("recall ID 0x%x", recall_id);
  
  if(ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_TERMINATING)){
//    g_message("`⁻ term");
    
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

#if 0
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    return;
  }
#endif
  
  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list_start = g_list_copy_deep(channel->play,
				  (GCopyFunc) g_object_ref,
				  NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list_start = g_list_copy_deep(channel->recall,
				  (GCopyFunc) g_object_ref,
				  NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }
  
  /* automate and play  */
  if((AGS_SOUND_STAGING_FX & (staging_flags)) == 0){
    staging_flags = staging_flags & staging_mask;

    if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0){
      while(list != NULL){
	recall = AGS_RECALL(list->data);
    
	/* play stages */
	if(AGS_IS_RECALL_CHANNEL(recall)){
	  ags_recall_set_staging_flags(recall,
				       AGS_SOUND_STAGING_AUTOMATE);
	  ags_recall_unset_staging_flags(recall,
					 AGS_SOUND_STAGING_AUTOMATE);
	}
      
	list = list->next;
      }
    }

    staging_flags = staging_flags & (~AGS_SOUND_STAGING_AUTOMATE);
  }
  
  list = list_start;
  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* play stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);
    ags_recall_unset_staging_flags(recall,
				   staging_flags);

    list = list->next;
  }
  
  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
  
  //FIXME:JK: uncomment
  //  ags_channel_set_staging_flags(channel, sound_scope,
  //				staging_flags);
}

/**
 * ags_channel_play_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 * @staging_flags: the stages to invoke
 * 
 * Run the specified steps by @recall_id of @channel.
 *
 * Since: 3.0.0
 */
void
ags_channel_play_recall(AgsChannel *channel,
			AgsRecallID *recall_id, guint staging_flags)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[PLAY_RECALL], 0,
		recall_id, staging_flags);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_done_recall(AgsChannel *channel,
			     AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_flags = (AGS_SOUND_STAGING_DONE);  

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);  

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* done  */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* done stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }
  
  g_list_free_full(list_start,
		   g_object_unref);

  ags_channel_set_staging_flags(channel, sound_scope,
				staging_flags);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_done_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Done processing specified by @recall_id.
 *
 * Since: 3.0.0
 */
void
ags_channel_done_recall(AgsChannel *channel,
			AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[DONE_RECALL], 0,
		recall_id);
  g_object_unref(G_OBJECT(channel));
}

void
ags_channel_real_cancel_recall(AgsChannel *channel,
			       AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL);

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* cancel  */  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* cancel stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    ags_recall_unset_state_flags(recall,
				 AGS_SOUND_STATE_IS_ACTIVE);
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  ags_channel_set_staging_flags(channel, sound_scope,
				staging_flags);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_cancel_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Cancel processing specified by @recall_id.
 *
 * Since: 3.0.0
 */
void
ags_channel_cancel_recall(AgsChannel *channel,
			  AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[CANCEL_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) channel);
}

void
ags_channel_real_cleanup_recall(AgsChannel *channel,
				AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;
  GList *match_start, *match;
  
  gint sound_scope;
  guint current_staging_flags;
  gboolean play_context;

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  /* get staging flags */
  g_rec_mutex_lock(channel_mutex);

  current_staging_flags = channel->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(channel_mutex);

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    play_context = FALSE;
    
    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* cleanup  */
  match_start = NULL;
  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);

    /* remove recall */
    ags_channel_remove_recall(channel, (GObject *) recall, play_context);
    match_start = g_list_prepend(match_start,
				 recall);
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* destroy  */
  match = match_start;
  
  while(match != NULL){
    recall = AGS_RECALL(match->data);
    
    /* destroy */
    ags_connectable_disconnect(AGS_CONNECTABLE(recall));
    g_object_run_dispose((GObject *) recall);
    
    match = match->next;
  }

  g_list_free_full(match_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_channel_cleanup_recall:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Cleanup processing specified by @recall_id.
 *
 * Since: 3.0.0
 */
void
ags_channel_cleanup_recall(AgsChannel *channel,
			   AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel) && AGS_IS_RECALL_ID(recall_id));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[CLEANUP_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) channel);
}

void
ags_channel_recall_done_callback(AgsRecall *recall,
				 AgsChannel *channel)
{
  AgsCancelChannel *cancel_channel;
  
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;

  gint sound_scope;

  if(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING) &&
     (AGS_IS_PLAY_CHANNEL_RUN(recall) ||
      AGS_IS_FX_PLAYBACK_CHANNEL_PROCESSOR(recall))){
    sound_scope = ags_recall_get_sound_scope(recall);

    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
      application_context = ags_application_context_get_instance();

      task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

      cancel_channel = ags_cancel_channel_new(channel,
					      sound_scope);

      ags_task_launcher_add_task(task_launcher,
				 cancel_channel);
    }
  }
}

GList*
ags_channel_real_start(AgsChannel *channel,
		       gint sound_scope)
{
  AgsAudio *audio;
  AgsRecycling *first_recycling;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsRecallID *audio_recall_id;
  AgsRecallID *channel_recall_id;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsThread *audio_loop;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  AgsApplicationContext *application_context;  

  GList *start_message_queue;
  GList *start_recall_id;
  GList *start_wait_thread, *wait_thread;

  gint64 start_wait_timeout;
  gint i;

  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);

  if(!AGS_IS_INPUT(channel) ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  /* test input has recycling */
  if(!ags_audio_test_flags(audio, AGS_AUDIO_INPUT_HAS_RECYCLING)){
    g_object_unref(audio);
    
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* add channel to AgsAudioLoop */
  ags_audio_loop_add_channel(AGS_AUDIO_LOOP(audio_loop),
			     (GObject *) channel);
  
  ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL);

  /* get playback domain */
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);
  
  /* get recycling and playback */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "playback", &playback,
	       NULL);

  /* run stage */
  start_recall_id = NULL;

  if(sound_scope >= 0){
    current_recall_id = ags_playback_get_recall_id(playback,
						   sound_scope);

    if(current_recall_id == NULL){
      /* recycling context */
      recycling_context = ags_recycling_context_new(1);
      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);

      /* set recycling */
      ags_recycling_context_replace(recycling_context,
				    first_recycling,
				    0);

      /* create audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context", recycling_context,
				     NULL);
      ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
      ags_audio_add_recall_id(audio,
			      (GObject *) audio_recall_id);

      g_object_set(recycling_context,
		   "recall-id", audio_recall_id,
		   NULL);

      /* create channel recall id */
      channel_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
      ags_channel_add_recall_id(channel,
				(GObject *) channel_recall_id);

      /* prepend recall id */
      start_recall_id = g_list_prepend(start_recall_id,
				       channel_recall_id);
    
      /* set playback's recall id */
      if(AGS_SOUND_SCOPE_PLAYBACK == sound_scope){
//	ags_recall_id_set_state_flags(channel_recall_id, AGS_SOUND_STATE_IS_WAITING);
      }
      
      ags_playback_set_recall_id(playback,
				 channel_recall_id,
				 sound_scope);
    }else{
      start_recall_id = g_list_prepend(start_recall_id,
				       current_recall_id);
    }

    /* run stage */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, staging_flags);

    /* add to start queue */
    audio_thread = NULL;
    channel_thread = NULL;

    start_wait_thread = NULL;
    
    if(AGS_SOUND_SCOPE_PLAYBACK != sound_scope){    
      if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
	audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							    sound_scope);

	if(audio_thread != NULL){
	  start_wait_thread = g_list_prepend(start_wait_thread,
					     audio_thread);
	
#if 1
	  ags_thread_add_start_queue(audio_loop,
				     audio_thread);
#else
	  ags_thread_start(audio_thread);
#endif
	}
      }
    
      if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	channel_thread = ags_playback_get_channel_thread(playback,
							 sound_scope);

	if(channel_thread != NULL){
	  start_wait_thread = g_list_prepend(start_wait_thread,
					     channel_thread);
	
#if 1
	  ags_thread_add_start_queue(audio_loop,
				     channel_thread);
#else
	  ags_thread_start(channel_thread);
#endif
	}
      }
    }
      
    /* unref */
#if 0
    wait_thread = start_wait_thread;

    start_wait_timeout = g_get_monotonic_time() + 5 * G_USEC_PER_SEC;
    
    while(wait_thread != NULL){
      /* wait thread */
      g_mutex_lock(AGS_THREAD_GET_START_MUTEX(wait_thread->data));

      if(!ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_DONE)){
	ags_thread_set_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_WAIT);
      
	while(ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_WAIT) &&
	      !ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_DONE) &&
	      g_get_monotonic_time() < start_wait_timeout){
	  g_cond_wait_until(AGS_THREAD_GET_START_COND(wait_thread->data),
			    AGS_THREAD_GET_START_MUTEX(wait_thread->data),
			    start_wait_timeout);
	}
      }
	
      g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(wait_thread->data));

      if(g_get_monotonic_time() > start_wait_timeout){
	g_critical("sync timeout");

	goto ags_channel_real_start_ONE_SCOPE_TIMEOUT;
      }

      wait_thread = wait_thread->next;
    }
#endif
    
  ags_channel_real_start_ONE_SCOPE_TIMEOUT:
    g_list_free_full(start_wait_thread,
		     g_object_unref);

    start_wait_thread = NULL;
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      current_recall_id = ags_playback_get_recall_id(playback,
						     i);

      if(current_recall_id == NULL){
	/* recycling context */
	recycling_context = ags_recycling_context_new(1);
	ags_audio_add_recycling_context(audio,
					(GObject *) recycling_context);

	/* set recycling */
	ags_recycling_context_replace(recycling_context,
				      first_recycling,
				      0);

	/* create audio recall id */
	audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
	ags_recall_id_set_sound_scope(audio_recall_id, i);
	ags_audio_add_recall_id(audio,
				(GObject *) audio_recall_id);

	g_object_set(recycling_context,
		     "recall-id", audio_recall_id,
		     NULL);
      
	/* create channel recall id */
	channel_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling-context", recycling_context,
					 NULL);
	ags_recall_id_set_sound_scope(channel_recall_id, i);
	ags_channel_add_recall_id(channel,
				  (GObject *) channel_recall_id);
      
	/* prepend recall id */
	start_recall_id = g_list_prepend(start_recall_id,
					 channel_recall_id);
    
	/* set playback's recall id */
	if(AGS_SOUND_SCOPE_PLAYBACK == i){
//	  ags_recall_id_set_state_flags(channel_recall_id, AGS_SOUND_STATE_IS_WAITING);
	}
	
	ags_playback_set_recall_id(playback,
				   channel_recall_id,
				   i);
      }else{
	start_recall_id = g_list_prepend(start_recall_id,
					 current_recall_id);
      }
      
      /* run stage */
      ags_channel_recursive_run_stage(channel,
				      i, staging_flags);

      /* add to start queue */
      audio_thread = NULL;
      channel_thread = NULL;

      start_wait_thread = NULL;

      if(AGS_SOUND_SCOPE_PLAYBACK != i){
	if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      i);

	  if(audio_thread != NULL){
	    start_wait_thread = g_list_prepend(start_wait_thread,
					       audio_thread);
	  
#if 1
	    ags_thread_add_start_queue(audio_loop,
				       audio_thread);
#else
	    ags_thread_start(audio_thread);
#endif
	  }
	}
      
	if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	  channel_thread = ags_playback_get_channel_thread(playback,
							   i);

	  if(channel_thread != NULL){
	    start_wait_thread = g_list_prepend(start_wait_thread,
					       channel_thread);
      
#if 1
	    ags_thread_add_start_queue(audio_loop,
				       channel_thread);
#else
	    ags_thread_start(channel_thread);
#endif
	  }
	}
      }
      
      /* unref */
#if 0
      wait_thread = start_wait_thread;

      start_wait_timeout = g_get_monotonic_time() + 5 * G_USEC_PER_SEC;

      while(wait_thread != NULL){
	/* wait thread */
	g_mutex_lock(AGS_THREAD_GET_START_MUTEX(wait_thread->data));

	if(!ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_DONE)){
	  ags_thread_set_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_WAIT);
	
	  while(ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_WAIT) &&
		!ags_thread_test_status_flags(wait_thread->data, AGS_THREAD_STATUS_START_DONE) &&
		g_get_monotonic_time() < start_wait_timeout){
	    g_cond_wait_until(AGS_THREAD_GET_START_COND(wait_thread->data),
			      AGS_THREAD_GET_START_MUTEX(wait_thread->data),
			      start_wait_timeout);
	  }
	}
	  
	g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(wait_thread->data));

	if(g_get_monotonic_time() > start_wait_timeout){
	  g_critical("sync timeout");

	  goto ags_channel_real_start_ALL_SCOPE_TIMEOUT;
	}

	wait_thread = wait_thread->next;
      }
#endif
      
    ags_channel_real_start_ALL_SCOPE_TIMEOUT:
      g_list_free_full(start_wait_thread,
		       g_object_unref);

      start_wait_thread = NULL;
    }
  }

  g_object_unref(audio);

  g_object_unref(playback_domain);

  g_object_unref(first_recycling);
  g_object_unref(playback);
  
  start_recall_id = g_list_reverse(start_recall_id);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::start");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* sound scope */
    message->parameter_name[0] = "sound-scope";
    g_value_init(&(message->value[0]),
		 G_TYPE_INT);
    g_value_set_int(&(message->value[0]),
		    sound_scope);

    /* recall id */
    message->parameter_name[1] = "recall-id";
    g_value_init(&(message->value[1]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[1]),
			g_list_copy_deep(start_recall_id,
					 (GCopyFunc) g_object_ref,
					 NULL));
    
    /* terminate string vector */
    message->parameter_name[2] = NULL;

    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }

  return(start_recall_id);
}

/**
 * ags_channel_start:
 * @channel: the #AgsChannel
 * @sound_scope: the sound scope
 * 
 * Start @channel's @sound_scope to do playback.
 * 
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID
 * 
 * Since: 3.0.0
 */
GList*
ags_channel_start(AgsChannel *channel,
		  gint sound_scope)
{
  GList *recall_id;
  
  g_return_val_if_fail(AGS_IS_CHANNEL(channel), NULL);

  recall_id = NULL;
  
  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[START], 0,
		sound_scope,
		&recall_id);
  g_object_unref((GObject *) channel);

  return(recall_id);
}

void
ags_channel_real_stop(AgsChannel *channel,
		      GList *recall_id, gint sound_scope)
{
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  
  AgsThread *audio_loop;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  AgsApplicationContext *application_context;

  GList *list;
  GList *start_message_queue;

  gint i;

  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL |
				      AGS_SOUND_STAGING_REMOVE);

  if(recall_id == NULL ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }

  list = recall_id;

  while(list != NULL){
    ags_recall_id_set_state_flags(list->data, AGS_SOUND_STATE_IS_TERMINATING);
    
    list = list->next;
  }
  
  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get some fields */
  g_object_get(channel,
	       "playback", &playback,
	       NULL);

  g_object_get(playback,
	       "playback-domain", &playback_domain,
	       NULL);

  if(sound_scope >= 0){
    /* stop thread */
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							sound_scope);

    channel_thread = ags_playback_get_channel_thread(playback,
						     sound_scope);

    if(audio_thread != NULL){
      ags_thread_stop(audio_thread);

      g_object_unref(audio_thread);
    }

    if(channel_thread != NULL){
      ags_thread_stop(channel_thread);

      g_object_unref(channel_thread);
    }

    /* cancel */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, staging_flags);

    /* clean - fini */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, AGS_SOUND_STAGING_FINI);

    ags_playback_set_recall_id(playback,
			       NULL,
			       sound_scope);
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      /* stop thread */
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);

      channel_thread = ags_playback_get_channel_thread(playback,
						       i);

      if(audio_thread != NULL){
	ags_thread_stop(audio_thread);

	g_object_unref(audio_thread);
      }

      if(channel_thread != NULL){
	ags_thread_stop(channel_thread);

	g_object_unref(channel_thread);
      }
    }
    
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      /* cancel */
      ags_channel_recursive_run_stage(channel,
				      i, staging_flags);

      /* clean - fini */
      ags_channel_recursive_run_stage(channel,
				      i, AGS_SOUND_STAGING_FINI);

      ags_playback_set_recall_id(playback,
				 NULL,
				 i);
    }
  }
    
  /* remove channel from AgsAudioLoop */
  ags_audio_loop_remove_channel(audio_loop,
				(GObject *) channel);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsChannel::stop");

    /* add message */
    message = ags_message_envelope_new((GObject *) channel,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);
    
    /* recall id */
    message->parameter_name[0] = "recall-id";
    g_value_init(&(message->value[0]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[0]),
			recall_id);
    
    /* sound scope */
    message->parameter_name[1] = "sound-scope";
    g_value_init(&(message->value[1]),
		 G_TYPE_INT);
    g_value_set_int(&(message->value[1]),
		    sound_scope);
    
    /* terminate string vector */
    message->parameter_name[2] = NULL;

    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_channel_stop:
 * @channel: the #AgsChannel
 * @recall_id: (element-type AgsAudio.RecallID) (transfer none): the #GList-struct containing #AgsRecallID
 * @sound_scope: the sound scope
 * 
 * Stop @channel's @sound_scope playback specified by @recall_id.
 * 
 * Since: 3.0.0
 */
void
ags_channel_stop(AgsChannel *channel,
		 GList *recall_id, gint sound_scope)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[STOP], 0,
		recall_id, sound_scope);
  g_object_unref((GObject *) channel);
}

GList*
ags_channel_real_check_scope(AgsChannel *channel, gint sound_scope)
{
  GList *list_start, *list;
  GList *recall_id;

  gint i;

  /* get recall id */
  g_object_get(channel,
	       "recall-id", &list_start,
	       NULL);
  
  /* iterate recall id */
  list = list_start;

  recall_id = NULL;

  if(sound_scope >= 0){
    while(list != NULL){
      /* check sound scope */
      if(ags_recall_id_check_sound_scope(list->data, sound_scope)){
	recall_id = g_list_prepend(recall_id,
				   list->data);
      }

      /* iterate */
      list = list->next;
    }
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      list = list_start;

      while(list != NULL){
	/* check sound scope */
	if(ags_recall_id_check_sound_scope(list->data, i)){
	  recall_id = g_list_prepend(recall_id,
				     list->data);
	}

	/* iterate */
	list = list->next;
      }
    }
  }

  /* reverse recall id */
  recall_id = g_list_reverse(recall_id);
  
  g_list_foreach(recall_id,
		 (GFunc) g_object_ref,
		 NULL);

  /* unref */
  g_list_free_full(list_start,
		   g_object_unref);
  
  return(recall_id);
}

/**
 * ags_channel_check_scope:
 * @channel: the #AgsChannel
 * @sound_scope: the sound scope
 * 
 * Check @channel's @sound_scope.
 * 
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID or %NULL if not playing
 * 
 * Since: 3.0.0
 */
GList*
ags_channel_check_scope(AgsChannel *channel, gint sound_scope)
{
  GList *recall_id;
  
  g_return_val_if_fail(AGS_IS_CHANNEL(channel), NULL);

  recall_id = NULL;
  
  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[CHECK_SCOPE], 0,
		sound_scope,
		&recall_id);
  g_object_unref((GObject *) channel);

  return(recall_id);
}

/**
 * ags_channel_collect_all_channel_ports:
 * @channel: the #AgsChannel
 *
 * Retrieve all ports of #AgsChannel.
 *
 * Returns: (element-type AgsAudio.Port) (transfer full): a new #GList containing #AgsPort
 *
 * Since: 3.0.0
 */
GList*
ags_channel_collect_all_channel_ports(AgsChannel *channel)
{
  GList *recall_start, *recall;
  GList *list;

  GRecMutex *recall_mutex, *play_mutex;
  GRecMutex *mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  list = NULL;
 
  /* get play mutex */  
  play_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

  /* collect port of playing recall */
  g_rec_mutex_lock(play_mutex);

  recall =
    recall_start = g_list_copy_deep(channel->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(play_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* concat port */
    g_rec_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy_deep(current->port,
				(GCopyFunc) g_object_ref,
				NULL);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy_deep(current->port,
						(GCopyFunc) g_object_ref,
						NULL));
	}
      }
    }

    g_rec_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(recall_start,
		   g_object_unref);
  
  /* get recall mutex */  
  recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

  /* the same for true recall */
  g_rec_mutex_lock(recall_mutex);

  recall =
    recall_start = g_list_copy_deep(channel->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(recall_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* concat port */
    g_rec_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy_deep(current->port,
				(GCopyFunc) g_object_ref,
				NULL);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy_deep(current->port,
						(GCopyFunc) g_object_ref,
						NULL));
	}
      }
    }

    g_rec_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }
   
  g_list_free_full(recall_start,
		   g_object_unref);

  /*  */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_channel_collect_all_channel_ports_by_specifier_and_context:
 * @channel: an #AgsChannel
 * @specifier: the port's name
 * @play_context: either %TRUE for play or %FALSE for recall
 *
 * Retrieve specified port of #AgsChannel
 *
 * Returns: (element-type AgsAudio.Port) (transfer full): a #GList-struct of #AgsPort if found, otherwise %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_channel_collect_all_channel_ports_by_specifier_and_context(AgsChannel *channel,
							       gchar *specifier,
							       gboolean play_context)
{
  GList *recall_start, *recall;
  GList *port_start, *port;
  GList *list;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
  if(play_context){
    /* get play mutex */
    recall_mutex = AGS_CHANNEL_GET_PLAY_MUTEX(channel);

    /* get recall */
    g_rec_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy_deep(channel->play,
				      (GCopyFunc) g_object_ref,
				      NULL);
    
    g_rec_mutex_unlock(recall_mutex);
  }else{
    /* get recall mutex */
    recall_mutex = AGS_CHANNEL_GET_RECALL_MUTEX(channel);

    /* get recall */
    g_rec_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy_deep(channel->recall,
				      (GCopyFunc) g_object_ref,
				      NULL);
    
    g_rec_mutex_unlock(recall_mutex);
  }
  
  /* collect port of playing recall */
  list = NULL;

  while(recall != NULL){
    AgsRecall *current;

    GRecMutex *mutex;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* get port */
    g_rec_mutex_lock(mutex);
    
    port =
      port_start = g_list_copy_deep(current->port,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(mutex);

    /* check specifier */
    while((port = ags_port_find_specifier(port, specifier)) != NULL){
      AgsPort *current;

      current = AGS_PORT(port->data);

      g_object_ref(current);
      list = g_list_prepend(list,
			    current);

      /* iterate - port */
      port = port->next;
    }

    g_list_free_full(port_start,
		     g_object_unref);

    /* iterate - recall */
    recall = recall->next;
  }

  g_list_free_full(recall_start,
		   g_object_unref);

  /* reverse result */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_channel_get_level:
 * @channel: the #AgsChannel
 * 
 * Get level.
 * 
 * Returns: (transfer full): the level of @channel as #AgsChannel or %NULL if no parent recycling.
 * 
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_get_level(AgsChannel *channel)
{
  AgsAudio *audio;
  AgsChannel *start_output, *nth_output;
  AgsChannel *level, *next_level;

  guint audio_channel;
  guint input_line;
	
  /* check above recycling */
  level = channel;

  if(level != NULL){
    g_object_ref(level);
  }

  audio = NULL;
  
  if(AGS_IS_OUTPUT(channel)){
    goto ags_channel_get_level_ITERATE;
  }
	
  while(level != NULL){
    /* get some fields */
    g_object_get(level,
		 "audio", &audio,
		 "audio-channel", &audio_channel,
		 "line", &input_line,
		 NULL);

    if(ags_audio_test_flags(audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      if(audio != NULL){
	g_object_unref(audio);
      }
      
      break;
    }

    /* get some fields */
    g_object_get(audio,
		 "output", &start_output,
		 NULL);
	  
    if(ags_audio_test_flags(audio, AGS_AUDIO_ASYNC)){
      nth_output = ags_channel_nth(start_output,
				   audio_channel);

      if(level != NULL){
	g_object_unref(level);
      }      

      level = nth_output;
    }else{
      nth_output = ags_channel_nth(start_output,
				   input_line);

      if(level != NULL){
	g_object_unref(level);
      }

      level = nth_output;
    }

    if(audio != NULL){
      g_object_unref(audio);
    }

    if(level == NULL){
      break;
    }

  ags_channel_get_level_ITERATE:

    /* iterate */
    next_level = ags_channel_get_link(level);

    g_object_unref(level);

    level = next_level;
  }

  return(level);
}

void
ags_channel_recursive_set_property_setv(AgsChannel *channel,
					gint n_params,
					gchar **parameter_name, GValue *value)
{
#if HAVE_GLIB_2_54    
  g_object_setv((GObject *) channel,
		n_params,
		parameter_name, value);
#else
  guint i;
  
  for(i = 0; i < n_params; i++){
    g_object_set_property((GObject *) channel,
			  parameter_name[i], &(value[i]));
  }
#endif
}
  
void
ags_channel_recursive_set_property_down(AgsChannel *channel,
					gint n_params,
					     gchar **parameter_name, GValue *value){
  if(channel == NULL){
    return;
  }

  ags_channel_recursive_set_property_setv(channel,
					  n_params,
					  parameter_name, value);
    
  ags_channel_recursive_set_property_down_input(channel,
						n_params,
						parameter_name, value);
}
    
void
ags_channel_recursive_set_property_down_input(AgsChannel *channel,
					      gint n_params,
					      gchar **parameter_name, GValue *value)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *next_pad, *nth_input;
  AgsChannel *link;

  guint audio_channel, line;
    
  if(channel == NULL){
    return;
  }
    
  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  if(audio == NULL){
    return;
  }

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);
    
  /* sync/async */
  if(ags_audio_test_flags(audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    input = nth_input;

    next_pad = NULL;
      
    while(input != NULL){
      /* get some fields */
      link = ags_channel_get_link(input);
      
      /* set property */
      ags_channel_recursive_set_property_setv(input,
					      n_params,
					      parameter_name, value);
      
      ags_channel_recursive_set_property_down(link,
					      n_params,
					      parameter_name, value);

      /* unref */
      if(link != NULL){
	g_object_unref(link);
      }
	
      /* iterate */
      next_pad = ags_channel_next_pad(input);

      g_object_unref(input);

      input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);

    input = nth_input;

    /* get some fields */
    link = ags_channel_get_link(input);
      
    /* set property */
    ags_channel_recursive_set_property_setv(input,
					    n_params,
					    parameter_name, value);
      
    ags_channel_recursive_set_property_down(link,
					    n_params,
					    parameter_name, value);

    /* unref */
    if(link != NULL){
      g_object_unref(link);
    }

    if(input != NULL){
      g_object_unref(input);
    }
  }
}

/**
 * ags_channel_recursive_set_property:
 * @channel: the #AgsChannel
 * @n_params: the count of paramter name and value pairs
 * @parameter_name: a string vector containing parameter names
 * @value: the value array
 *
 * Recursive set property for #AgsChannel.
 *
 * Since: 3.0.0
 */
void
ags_channel_recursive_set_property(AgsChannel *channel,
				   gint n_params,
				   gchar **parameter_name, GValue *value)
{
  AgsChannel *link;  
  
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  /* get some fields */
  link = ags_channel_get_link(channel);
    
  if(AGS_IS_INPUT(channel)){
    ags_channel_recursive_set_property_setv(channel,
					    n_params,
					    parameter_name, value);
    
    ags_channel_recursive_set_property_down(link,
					    n_params,
					    parameter_name, value);
  }else{
    ags_channel_recursive_set_property_down(channel,
					    n_params,
					    parameter_name, value);
  }

  if(link != NULL){
    g_object_unref(link);
  }
}

void
ags_channel_recursive_setup_run_stage_up(AgsChannel *channel,
					 AgsRecyclingContext *recycling_context,
					 gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel, *nth_channel;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint pad, audio_channel;
  guint line;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  current_channel = channel;

  if(current_channel != NULL){
    g_object_ref(current_channel);
  }

  current_link = NULL;
    
  if(AGS_IS_OUTPUT(channel)){
    g_object_get(channel,
		 "audio", &current_audio,
		 NULL);
      
    goto ags_channel_recursive_prepare_run_stage_up_OUTPUT;
  }

  while(current_channel != NULL){      
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    if(current_recall_id == NULL){
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

      ags_channel_add_recall_id(current_channel,
				current_recall_id);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* get current audio */
    g_object_get(current_channel,
		 "audio", &current_audio,
		 NULL);

    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);

    if(current_recall_id == NULL){
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

      ags_audio_add_recall_id(current_audio,
			      (GObject *) current_recall_id);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* get some fields */
    g_object_get(current_channel,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);

    /* move up */
    if(current_channel != NULL){
      g_object_unref(current_channel);
    }

    g_object_get(current_audio,
		 "output", &current_channel,
		 NULL);
      
    if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      /* unref current audio */
      g_object_unref(current_audio);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }
	
      break;
    }
      
    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_channel = ags_channel_nth(current_channel,
				    audio_channel);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }else{
      nth_channel = ags_channel_nth(current_channel,
				    line);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }
      
  ags_channel_recursive_prepare_run_stage_up_OUTPUT:

    /* check scope - output */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);

    if(current_recall_id == NULL){
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

      ags_channel_add_recall_id(current_channel,
				current_recall_id);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* unref current audio */
    g_object_unref(current_audio);
      
    /* iterate */
    current_link = ags_channel_get_link(current_channel);

    g_object_unref(current_channel);

    current_channel = current_link;
  }

  if(current_link != NULL){
    g_object_unref(current_link);
  }
}
  
void
ags_channel_recursive_setup_run_stage_down(AgsChannel *channel,
					   AgsRecyclingContext *recycling_context,
					   gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *next_channel, *nth_input;
  AgsRecallID *current_recall_id, *next_recall_id;
  AgsRecyclingContext *next_recycling_context;
    
  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
  gboolean play_context;
    
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* setup */
  next_recycling_context = recycling_context;
    
  /* check scope - output */
  recall_id =
    start_recall_id = ags_channel_check_scope(channel, sound_scope);
      
  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);

  if(current_recall_id == NULL){
    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context", recycling_context,
				     NULL);
    ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

    ags_channel_add_recall_id(channel,
			      current_recall_id);
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get current audio */
  g_object_get(channel,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* check scope - audio */
  recall_id =
    start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
    
  if(current_recall_id == NULL){
    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context", recycling_context,
				     NULL);
    ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

    ags_audio_add_recall_id(current_audio,
			    (GObject *) current_recall_id);
  }
    
  g_list_free_full(start_recall_id,
		   g_object_unref);
    
  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);

  /* check next recycling context */
  current_input = NULL;
    
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    AgsRecycling *first_recycling;
    AgsRecycling *recycling;

    gint position;
      
    next_recycling_context = NULL;

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      AgsChannel *first_with_recycling;
	  
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;
	
      first_with_recycling = ags_channel_first_with_recycling(current_input);
	
      g_object_get(first_with_recycling,
		   "first-recycling", &first_recycling,
		   NULL);

      g_object_unref(first_with_recycling);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;
	
      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   NULL);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    if(first_recycling != NULL){
      position = ags_recycling_context_find_child(recycling_context,
						  first_recycling);

      if(position >= 0){
	GList *child_start;

	g_object_get(recycling_context,
		     "child", &child_start,
		     NULL);

	next_recycling_context = g_list_nth_data(child_start, position);
	g_list_free_full(child_start,
			 g_object_unref);
      }

      g_object_unref(first_recycling);
    }
  }

  if(next_recycling_context == NULL){
    AgsRecycling *first_recycling, *last_recycling;
    AgsRecycling *end_region;
    AgsRecycling *recycling, *next_recycling;

    guint64 length;
    guint64 i;

    current_input = NULL;	

    length = 0;
      
    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;

      if(current_input != NULL){
	g_object_ref(current_input);
      }
	
      next_pad = NULL;
	
      while(current_input != NULL){
	gint position;
	  
	g_object_get(current_input,
		     "first-recycling", &first_recycling,
		     NULL);
	g_object_get(current_input,
		     "last-recycling", &last_recycling,
		     NULL);

	end_region = ags_recycling_next(last_recycling);
	  
	position = ags_recycling_position(first_recycling, end_region,
					  last_recycling);
	length += (position + 1);

	/* unref */
	if(first_recycling != NULL){
	  g_object_unref(first_recycling);
	  g_object_unref(last_recycling);
	}

	if(end_region != NULL){
	  g_object_unref(end_region);
	}

	/* iterate */
	next_pad = ags_channel_next_pad(current_input);

	g_object_unref(current_input);

	current_input = next_pad;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;

      if(current_input != NULL){
	g_object_ref(current_input);
      }
	
      next_pad = NULL;

      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   "last-recycling", &last_recycling,
		   NULL);

      if(first_recycling != NULL){
	end_region = ags_recycling_next(last_recycling);
	  
	length = ags_recycling_position(first_recycling, end_region,
					last_recycling);
	length++;

	g_object_unref(first_recycling);
	g_object_unref(last_recycling);
	  
	if(end_region != NULL){
	  g_object_unref(end_region);
	}
      }else{
	length = 0;
      }

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    /* instantiate next recycling context */
    if(length > 0){
      next_recycling_context = ags_recycling_context_new(length);
      g_object_set(recycling_context,
		   "child", next_recycling_context,
		   NULL);

      ags_audio_add_recycling_context(current_audio,
				      (GObject *) next_recycling_context);

      current_input = nth_input;
	
      for(i = 0; i < length;){
	g_object_get(current_input,
		     "first-recycling", &first_recycling,
		     NULL);

	recycling = first_recycling;
	g_object_ref(recycling);
	  
	next_recycling = NULL;
	  
	for(; i < length && recycling != NULL; i++){
	  /* set recycling */
	  ags_recycling_context_replace(next_recycling_context,
					recycling,
					i);
	  
	  /* iterate */
	  i++;

	  next_recycling = ags_recycling_next(recycling);

	  g_object_unref(recycling);

	  recycling = next_recycling;
	}

	/* unref */
	if(first_recycling != NULL){
	  g_object_unref(first_recycling);
	}

	if(next_recycling != NULL){
	  g_object_unref(next_recycling);
	}
	  
	/* iterate */
	next_pad = ags_channel_next_pad(current_input);

	g_object_unref(current_input);

	current_input = next_pad;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }
  }
    
  if(next_recycling_context != NULL &&
     next_recycling_context != recycling_context){
    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     next_recycling_context);

    if(current_recall_id == NULL){
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", next_recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

      ags_audio_add_recall_id(current_audio,
			      (GObject *) current_recall_id);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
  }
        
  ags_channel_recursive_setup_run_stage_down_input(channel,
						   next_recycling_context,
						   sound_scope, local_staging_flags);

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
  
void
ags_channel_recursive_setup_run_stage_down_input(AgsChannel *channel,
						 AgsRecyclingContext *recycling_context,
						 gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;
    
  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
    
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }
    
  /* get some fields */
  g_object_get(channel,
	       "audio", &current_audio,
	       "line", &line,
	       "audio-channel", &audio_channel,
	       NULL);

  if(current_audio == NULL){
    return;
  }

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);
    
  /* sync/async */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    current_input = nth_input;

    next_pad = NULL;
      
    while(current_input != NULL){
      /* get some fields */
      current_link = ags_channel_get_link(current_input);

      /* check scope - input */
      recall_id =
	start_recall_id = ags_channel_check_scope(current_input, sound_scope);

      current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							       recycling_context);
	
      if(current_recall_id == NULL){
	current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling-context", recycling_context,
					 NULL);
	ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

	ags_channel_add_recall_id(current_input,
				  current_recall_id);
      }
      	
      /* free recall id */
      g_list_free_full(start_recall_id,
		       g_object_unref);

      /* traverse the tree */
      ags_channel_recursive_setup_run_stage_down(current_link,
						 recycling_context,
						 sound_scope, local_staging_flags);

      if(current_link != NULL){
	g_object_unref(current_link);
      }
	
      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);
      
    current_input = nth_input;

    /* get some fields */
    current_link = ags_channel_get_link(current_input);
      
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_input, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    if(current_recall_id == NULL){
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
      ags_recall_id_set_sound_scope(current_recall_id, sound_scope);

      ags_channel_add_recall_id(current_input,
				current_recall_id);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* traverse the tree */
    ags_channel_recursive_setup_run_stage_down(current_link,
					       recycling_context,
					       sound_scope, local_staging_flags);

    if(current_link != NULL){
      g_object_unref(current_link);
    }

    if(current_input != NULL){
      g_object_unref(current_input);
    }
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
  
void
ags_channel_recursive_prepare_run_stage_up(AgsChannel *channel,
					   AgsRecyclingContext *recycling_context,
					   gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel, *nth_channel;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint pad, audio_channel;
  guint line;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  current_channel = channel;

  if(current_channel != NULL){
    g_object_ref(current_channel);
  }

  current_link = NULL;

  if(AGS_IS_OUTPUT(channel)){
    g_object_get(channel,
		 "audio", &current_audio,
		 NULL);
      
    goto ags_channel_recursive_prepare_run_stage_up_OUTPUT;
  }

  while(current_channel != NULL){      
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* duplicate */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_duplicate_recall(current_channel,
				     current_recall_id);
      }
    }

    /* resolve */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_resolve_recall(current_channel,
				   current_recall_id);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* get current audio */
    g_object_get(current_channel,
		 "audio", &current_audio,
		 NULL);

    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);

    /* duplicate */
    g_object_get(current_channel,
		 "pad", &pad,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);
      
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_duplicate_recall(current_audio,
				   current_recall_id,
				   pad, audio_channel,
				   line);
      }	  
    }

    /* resolve */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_resolve_recall(current_audio,
				 current_recall_id);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* get some fields */
    g_object_get(current_channel,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);

    /* move up */
    if(current_channel != NULL){
      g_object_unref(current_channel);
    }

    g_object_get(current_audio,
		 "output", &current_channel,
		 NULL);

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      /* unref current audio */
      g_object_unref(current_audio);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }

      break;
    }
      
    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_channel = ags_channel_nth(current_channel,
				    audio_channel);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }else{
      nth_channel = ags_channel_nth(current_channel,
				    line);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }
      
  ags_channel_recursive_prepare_run_stage_up_OUTPUT:
      
    /* check scope - output */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* duplicate */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_duplicate_recall(current_channel,
				     current_recall_id);
      }
    }

    /* resolve */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_resolve_recall(current_channel,
				   current_recall_id);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* unref current audio */
    g_object_unref(current_audio);

    /* iterate */
    current_link = ags_channel_get_link(current_channel);

    g_object_unref(current_channel);

    current_channel = current_link;
  }

  if(current_link != NULL){
    g_object_unref(current_link);
  }
}

void
ags_channel_recursive_prepare_run_stage_down(AgsChannel *channel,
					     AgsRecyclingContext *recycling_context,
					     gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *next_channel, *nth_input;
  AgsRecallID *current_recall_id, *next_recall_id;
  AgsRecyclingContext *next_recycling_context;
    
  GList *start_recall_id, *recall_id;

  guint pad, audio_channel;
  guint line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* prepare */
  next_recycling_context = recycling_context;
    
  /* check scope - output */
  recall_id =
    start_recall_id = ags_channel_check_scope(channel, sound_scope);
      
  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);

  /* duplicate */
  if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_channel_duplicate_recall(channel,
				   current_recall_id);
    }
  }

  /* resolve */
  if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_channel_resolve_recall(channel,
				 current_recall_id);
    }
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get current audio */
  g_object_get(channel,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* check scope - audio */
  recall_id =
    start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);

  /* duplicate - audio */
  g_object_get(channel,
	       "pad", &pad,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){	
    if(current_recall_id != NULL){
      ags_audio_duplicate_recall(current_audio,
				 current_recall_id,
				 pad, audio_channel,
				 line);
    }
  }

  /* resolve - audio */
  if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_audio_resolve_recall(current_audio,
			       current_recall_id);
    }
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);

  /* check next recycling context */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    AgsRecycling *first_recycling;

    gint position;
      
    next_recycling_context = NULL;

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      AgsChannel *first_with_recycling;
	  
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;
	
      first_with_recycling = ags_channel_first_with_recycling(current_input);
	
      g_object_get(first_with_recycling,
		   "first-recycling", &first_recycling,
		   NULL);

      g_object_unref(first_with_recycling);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;

      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   NULL);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    if(first_recycling != NULL){
      position = ags_recycling_context_find_child(recycling_context,
						  first_recycling);

      if(position >= 0){
	GList *child_start;

	g_object_get(recycling_context,
		     "child", &child_start,
		     NULL);

	next_recycling_context = g_list_nth_data(child_start, position);
	g_list_free_full(child_start,
			 g_object_unref);
      }

      g_object_unref(first_recycling);
    }
  }

  if(next_recycling_context != NULL &&
     next_recycling_context != recycling_context){
    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     next_recycling_context);

    /* duplicate - audio */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_duplicate_recall(current_audio,
				   current_recall_id,
				   pad, audio_channel,
				   line);
      }
    }

    /* resolve - audio */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_audio_resolve_recall(current_audio,
				 current_recall_id);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
    
  /* traverse the tree */
  ags_channel_recursive_prepare_run_stage_down_input(channel,
						     next_recycling_context,
						     sound_scope, local_staging_flags);

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }
}

void
ags_channel_recursive_prepare_run_stage_down_input(AgsChannel *channel,
						   AgsRecyclingContext *recycling_context,
						   gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }
    
  /* get some fields */
  g_object_get(channel,
	       "audio", &current_audio,
	       "line", &line,
	       "audio-channel", &audio_channel,
	       NULL);

  if(current_audio == NULL){
    return;
  }

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);
    
  /* sync/async */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    current_input = nth_input;

    next_pad = NULL;
      
    while(current_input != NULL){
      /* get some fields */
      current_link = ags_channel_get_link(current_input);
      
      /* check scope - input */
      recall_id =
	start_recall_id = ags_channel_check_scope(current_input, sound_scope);

      current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							       recycling_context);
	
      /* duplicate */
      if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){	
	if(current_recall_id != NULL){
	  ags_channel_duplicate_recall(current_input,
				       current_recall_id);
	}
      }

      /* resolve */
      if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){
	
	if(current_recall_id){
	  ags_channel_resolve_recall(current_input,
				     current_recall_id);
	}
      }

      /* free recall id */
      g_list_free_full(start_recall_id,
		       g_object_unref);
	
      /* traverse the tree */
      ags_channel_recursive_prepare_run_stage_down(current_link,
						   recycling_context,
						   sound_scope, local_staging_flags);

      if(current_link != NULL){
	g_object_unref(current_link);
      }
	
      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);

    current_input = nth_input;
      
    /* get some fields */
    current_link = ags_channel_get_link(current_input);
      
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_input, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* duplicate */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_duplicate_recall(current_input,
				     current_recall_id);
      }
    }

    /* resolve */
    if((AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_resolve_recall(current_input,
				   current_recall_id);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* traverse the tree */
    ags_channel_recursive_prepare_run_stage_down(current_link,
						 recycling_context,
						 sound_scope, local_staging_flags);

    if(current_link != NULL){
      g_object_unref(current_link);
    }

    if(current_input != NULL){
      g_object_unref(current_input);
    }
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_channel_recursive_do_run_stage_up(AgsChannel *channel,
				      AgsRecyclingContext *recycling_context,
				      gint sound_scope, guint staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel, *nth_channel;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint audio_channel;
  guint line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  current_channel = channel;

  if(current_channel != NULL){
    g_object_ref(current_channel);
  }

  current_link = NULL;

  if(AGS_IS_OUTPUT(channel)){
    g_object_get(channel,
		 "audio", &current_audio,
		 NULL);
      
    goto ags_channel_recursive_do_run_stage_up_OUTPUT;
  }

  while(current_channel != NULL){
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* init recall */
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_init_recall(current_channel,
				current_recall_id, staging_flags);
      }
    }      

    /* play recall */
    if(current_recall_id != NULL){
      ags_channel_play_recall(current_channel,
			      current_recall_id, staging_flags);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* get current audio */
    g_object_get(current_channel,
		 "audio", &current_audio,
		 NULL);

    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);

    /* init recall */
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_audio_init_recall(current_audio,
			      current_recall_id, staging_flags);
      }
    }
      
    /* play recall */	
    if(current_recall_id != NULL){
      ags_audio_play_recall(current_audio,
			    current_recall_id, staging_flags);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* get some fields */
    g_object_get(current_channel,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);
      
    /* move up */
    if(current_channel != NULL){
      g_object_unref(current_channel);
    }

    g_object_get(current_audio,
		 "output", &current_channel,
		 NULL);

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      /* unref current audio */
      g_object_unref(current_audio);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }

      break;
    }

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_channel = ags_channel_nth(current_channel,
				    audio_channel);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }else{
      nth_channel = ags_channel_nth(current_channel,
				    line);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }
      
  ags_channel_recursive_do_run_stage_up_OUTPUT:

    /* check scope - output */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* init recall */
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_init_recall(current_channel,
				current_recall_id, staging_flags);
      }
    }
      
    /* play recall */	
    if(current_recall_id != NULL){
      ags_channel_play_recall(current_channel,
			      current_recall_id, staging_flags);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* unref current audio */
    g_object_unref(current_audio);

    /* iterate */
    current_link = ags_channel_get_link(current_channel);

    g_object_unref(current_channel);

    current_channel = current_link;
  }

  if(current_link != NULL){
    g_object_unref(current_link);
  }
}

void
ags_channel_recursive_do_run_stage_down(AgsChannel *channel,
					AgsRecyclingContext *recycling_context,
					gint sound_scope, guint staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *next_channel, *nth_input;
  AgsRecallID *current_recall_id, *next_recall_id;
  AgsRecyclingContext *next_recycling_context;
    
  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* do */
  next_recycling_context = recycling_context;
    
  /* check scope - output */
  recall_id =
    start_recall_id = ags_channel_check_scope(channel, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
      
  /* init recall */
  if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){	
    if(current_recall_id != NULL){
      ags_channel_init_recall(channel,
			      current_recall_id, staging_flags);
    }
  }      

  /* play recall */
  if(current_recall_id != NULL){
    ags_channel_play_recall(channel,
			    current_recall_id, staging_flags);
  }
    
  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get current audio */
  g_object_get(channel,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* check scope - audio */
  recall_id =
    start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
      
  /* init recall */
  if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_audio_init_recall(current_audio,
			    current_recall_id, staging_flags);
    }
  }
      
  /* play recall */	
  if(current_recall_id != NULL){
    ags_audio_play_recall(current_audio,
			  current_recall_id, staging_flags);
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);

  /* check next recycling context */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    AgsRecycling *first_recycling;

    gint position;
      
    next_recycling_context = NULL;

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      AgsChannel *first_with_recycling;
	  
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;
	
      first_with_recycling = ags_channel_first_with_recycling(current_input);
	  
      g_object_get(first_with_recycling,
		   "first-recycling", &first_recycling,
		   NULL);

      g_object_unref(first_with_recycling);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;

      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   NULL);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    if(first_recycling != NULL){
      position = ags_recycling_context_find_child(recycling_context,
						  first_recycling);

      if(position >= 0){
	GList *child_start;

	g_object_get(recycling_context,
		     "child", &child_start,
		     NULL);

	next_recycling_context = g_list_nth_data(child_start, position);
	g_list_free_full(child_start,
			 g_object_unref);
      }

      g_object_unref(first_recycling);
    }
  }

  if(next_recycling_context != NULL &&
     next_recycling_context != recycling_context){
    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     next_recycling_context);

    /* init recall */
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_init_recall(current_audio,
			      current_recall_id, staging_flags);
      }
    }
      
    /* play recall */	
    if(current_recall_id != NULL){
      ags_audio_play_recall(current_audio,
			    current_recall_id, staging_flags);
    }
    
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
    
  /* traverse the tree */
  ags_channel_recursive_do_run_stage_down_input(channel,
						next_recycling_context,
						sound_scope, staging_flags);

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }
}
  
void
ags_channel_recursive_do_run_stage_down_input(AgsChannel *channel,
					      AgsRecyclingContext *recycling_context,
					      gint sound_scope, guint staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;
    
  guint audio_channel, line;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }
    
  /* get some fields */
  g_object_get(channel,
	       "audio", &current_audio,
	       "line", &line,
	       "audio-channel", &audio_channel,
	       NULL);

  if(current_audio == NULL){
    return;
  }

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);
    
  /* sync/async */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    current_input = nth_input;

    next_pad = NULL;
      
    while(current_input != NULL){
      /* get some fields */
      current_link = ags_channel_get_link(current_input);
      
      /* check scope - input */
      recall_id =
	start_recall_id = ags_channel_check_scope(current_input, sound_scope);

      current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							       recycling_context);

      /* init recall */
      if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
	 (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
	 (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
	 (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){
	if(current_recall_id != NULL){
	  ags_channel_init_recall(current_input,
				  current_recall_id, staging_flags);
	}
      }      

      /* play recall */	
      if(current_recall_id != NULL){
	ags_channel_play_recall(current_input,
				current_recall_id, staging_flags);
      }

      /* free recall id */
      g_list_free_full(start_recall_id,
		       g_object_unref);

      /* traverse the tree */
      ags_channel_recursive_do_run_stage_down(current_link,
					      recycling_context,
					      sound_scope, staging_flags);

      if(current_link != NULL){
	g_object_unref(current_link);
      }

      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);

    current_input = nth_input;
      
    /* get some fields */
    current_link = ags_channel_get_link(current_input);
      
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_input, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* init recall */
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 ||
       (AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_init_recall(current_input,
				current_recall_id, staging_flags);
      }
    }      

    /* play recall */	
    if(current_recall_id != NULL){
      ags_channel_play_recall(current_input,
			      current_recall_id, staging_flags);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* traverse the tree */
    ags_channel_recursive_do_run_stage_down(current_link,
					    recycling_context,
					    sound_scope, staging_flags);

    if(current_link != NULL){
      g_object_unref(current_link);
    }

    if(current_input != NULL){
      g_object_unref(current_input);
    }
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_channel_recursive_cleanup_run_stage_up(AgsChannel *channel,
					   AgsRecyclingContext *recycling_context,
					   gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel, *nth_channel;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint audio_channel;
  guint line;
    
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  current_channel = channel;

  if(current_channel != NULL){
    g_object_ref(current_channel);
  }

  current_link = NULL;
    
  if(AGS_IS_OUTPUT(channel)){
    g_object_get(channel,
		 "audio", &current_audio,
		 NULL);
      
    goto ags_channel_recursive_cleanup_run_stage_up_OUTPUT;
  }

  while(current_channel != NULL){
    /* check scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* cancel */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_cancel_recall(current_channel,
				  current_recall_id);
      }
    }

    /* remove */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_cleanup_recall(current_channel,
				   current_recall_id);
      }
    }

    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_remove_recall_id(current_channel,
				     current_recall_id);

	ags_channel_unset_staging_flags(current_channel, sound_scope,
					staging_mask);
      }
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* get current audio */
    g_object_get(current_channel,
		 "audio", &current_audio,
		 NULL);
      
    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* cancel */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_cancel_recall(current_audio,
				current_recall_id);
      }
    }

    /* remove */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id){
	ags_audio_cleanup_recall(current_audio,
				 current_recall_id);
      }
    }

    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_audio_remove_recall_id(current_audio,
				   (GObject *) current_recall_id);

	ags_audio_unset_staging_flags(current_audio, sound_scope,
				      staging_mask);
      }
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* get some fields */
    g_object_get(current_channel,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);

    /* move up */
    if(current_channel != NULL){
      g_object_unref(current_channel);
    }

    g_object_get(current_audio,
		 "output", &current_channel,
		 NULL);

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      /* fini - clean */
      if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){
	ags_audio_remove_recycling_context(current_audio, (GObject *) recycling_context);
      }

      /* unref current audio */
      g_object_unref(current_audio);
	
      break;
    }

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_channel = ags_channel_nth(current_channel,
				    audio_channel);

      g_object_unref(current_channel);
	  
      current_channel = nth_channel;
    }else{
      nth_channel = ags_channel_nth(current_channel,
				    line);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }
      
  ags_channel_recursive_cleanup_run_stage_up_OUTPUT:
      
    /* check scope - output */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);

    /* cancel */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_cancel_recall(current_channel,
				  current_recall_id);
      }
    }

    /* remove */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_channel_cleanup_recall(current_channel,
				   current_recall_id);
      }
    }

    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){
      if(current_recall_id){
	ags_channel_remove_recall_id(current_channel,
				     current_recall_id);

	ags_channel_unset_staging_flags(current_channel, sound_scope,
					staging_mask);
      }
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){
      ags_audio_remove_recycling_context(current_audio, (GObject *) recycling_context);
    }
      
    /* unref current audio */
    g_object_unref(current_audio);
      
    /* iterate */
    current_link = ags_channel_get_link(current_channel);

    g_object_unref(current_channel);

    current_channel = current_link;
  }

  if(current_link != NULL){
    g_object_unref(current_link);
  }
}

void
ags_channel_recursive_cleanup_run_stage_down(AgsChannel *channel,
					     AgsRecyclingContext *recycling_context,
					     gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *next_channel, *nth_input;
  AgsRecallID *current_recall_id, *next_recall_id;
  AgsRecyclingContext *next_recycling_context;
    
  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
  gboolean play_context;
    
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* cleanup */
  next_recycling_context = recycling_context;
    
  /* check scope - output */
  recall_id =
    start_recall_id = ags_channel_check_scope(channel, sound_scope);
      
  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);

  /* cancel */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_channel_cancel_recall(channel,
				current_recall_id);
    }
  }

  /* remove */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){	
    if(current_recall_id != NULL){
      ags_channel_cleanup_recall(channel,
				 current_recall_id);
    }
  }
    
  /* fini - clean */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
    if(current_recall_id != NULL){
      ags_channel_remove_recall_id(channel,
				   current_recall_id);

      ags_channel_unset_staging_flags(channel, sound_scope,
				      staging_mask);
    }
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get current audio */
  g_object_get(channel,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);
      
  /* check scope - audio */
  recall_id =
    start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
      
  /* cancel */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
    if(current_recall_id != NULL){
      ags_audio_cancel_recall(current_audio,
			      current_recall_id);
    }
  }

  /* remove */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){
    if(current_recall_id){
      ags_audio_cleanup_recall(current_audio,
			       current_recall_id);
    }
  }

  /* fini - clean */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
    if(current_recall_id != NULL){
      ags_audio_remove_recall_id(current_audio,
				 (GObject *) current_recall_id);

      ags_audio_unset_staging_flags(current_audio, sound_scope,
				    staging_mask);
    }
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);

  /* check next recycling context */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    AgsRecycling *first_recycling;

    gint position;
      
    next_recycling_context = NULL;
    first_recycling = NULL;

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      AgsChannel *first_with_recycling;
	  
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;
	
      first_with_recycling = ags_channel_first_with_recycling(current_input);
	  
      g_object_get(first_with_recycling,
		   "first-recycling", &first_recycling,
		   NULL);

      g_object_unref(first_with_recycling);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;

      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   NULL);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    if(first_recycling != NULL){
      position = ags_recycling_context_find_child(recycling_context,
						  first_recycling);

      if(position >= 0){
	GList *child_start;

	g_object_get(recycling_context,
		     "child", &child_start,
		     NULL);

	next_recycling_context = g_list_nth_data(child_start, position);
	g_list_free_full(child_start,
			 g_object_unref);
      }

      g_object_unref(first_recycling);
    }
  }

  if(next_recycling_context != NULL &&
     next_recycling_context != recycling_context){
    /* check scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     next_recycling_context);

    /* cancel */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){
      if(current_recall_id != NULL){
	ags_audio_cancel_recall(current_audio,
				current_recall_id);
      }
    }

    /* remove */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_audio_cleanup_recall(current_audio,
				 current_recall_id);
      }
    }

    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_audio_remove_recall_id(current_audio,
				   (GObject *) current_recall_id);

	ags_audio_unset_staging_flags(current_audio, sound_scope,
				      staging_mask);
      }
    }      

    g_list_free_full(start_recall_id,
		     g_object_unref);
  }

  /* free recall id */
  ags_channel_recursive_cleanup_run_stage_down_input(channel,
						     next_recycling_context,
						     sound_scope, local_staging_flags);
    
  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
    
  /* fini - clean */
  if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){
    ags_audio_remove_recycling_context(current_audio, (GObject *) recycling_context);
    ags_audio_remove_recycling_context(current_audio, (GObject *) next_recycling_context);
  }

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }
}

void
ags_channel_recursive_cleanup_run_stage_down_input(AgsChannel *channel,
						   AgsRecyclingContext *recycling_context,
						   gint sound_scope, guint local_staging_flags)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
    
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }
    
  /* get some fields */
  g_object_get(channel,
	       "audio", &current_audio,
	       "line", &line,
	       "audio-channel", &audio_channel,
	       NULL);

  if(current_audio == NULL){
    return;
  }

  /* get some fields */
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);    
    
  /* sync/async */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    current_input = nth_input;

    next_pad = NULL;
      
    while(current_input != NULL){
      /* get some fields */
      current_link = ags_channel_get_link(current_input);

      /* check scope - input */
      recall_id =
	start_recall_id = ags_channel_check_scope(current_input, sound_scope);

      current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							       recycling_context);
      
      /* cancel */
      if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){	
	if(current_recall_id != NULL){
	  ags_channel_cancel_recall(current_input,
				    current_recall_id);
	}
      }

      /* remove */
      if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){
	if(current_recall_id != NULL){
	  ags_channel_cleanup_recall(current_input,
				     current_recall_id);
	}
      }
	
      /* fini - clean */
      if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
	if(current_recall_id != NULL){
	  ags_channel_remove_recall_id(current_input,
				       current_recall_id);

	  ags_channel_unset_staging_flags(current_input, sound_scope,
					  staging_mask);
	}
      }
	
      /* free recall id */
      g_list_free_full(start_recall_id,
		       g_object_unref);

      /* traverse the tree */
      ags_channel_recursive_cleanup_run_stage_down(current_link,
						   recycling_context,
						   sound_scope, local_staging_flags);

      if(current_link != NULL){
	g_object_unref(current_link);
      }

      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);

    current_input = nth_input;

    /* get some fields */
    current_link = ags_channel_get_link(current_input);
      
    /* check scope - input */
    start_recall_id = ags_channel_check_scope(current_input, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* cancel */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_cancel_recall(current_input,
				  current_recall_id);
      }
    }

    /* remove */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_cleanup_recall(current_input,
				   current_recall_id);
      }
    }

    /* fini - clean */
    if((AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE & (local_staging_flags)) != 0){	
      if(current_recall_id != NULL){
	ags_channel_remove_recall_id(current_input,
				     current_recall_id);

	ags_channel_unset_staging_flags(current_input, sound_scope,
					staging_mask);
      }
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* traverse the tree */
    ags_channel_recursive_cleanup_run_stage_down(current_link,
						 recycling_context,
						 sound_scope, local_staging_flags);

    if(current_link != NULL){
      g_object_unref(current_link);
    }

    if(current_input != NULL){
      g_object_unref(current_input);
    }
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_channel_real_recursive_run_stage(AgsChannel *channel,
				     gint sound_scope, guint staging_flags)
{
  AgsChannel *link;
  AgsRecyclingContext *recycling_context;

  GList *recall_id, *recall_id_iter;

  guint pad;
  gint i;

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  
  if(sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }
  
  pad = 0;
  
  /* check scope - input */
  recall_id = ags_channel_check_scope(channel, sound_scope);

  recycling_context = NULL;
    
  if(recall_id != NULL){
    AgsRecycling *recycling;
    
    GList *iter;

    g_object_get(channel,
		 "first-recycling", &recycling,
		 NULL);
    
    iter = recall_id;

    while(iter != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context;
      
      current_recall_id = iter->data;

      /* get recall id mutex */
      recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);
      
      /* get recycling context */
      g_rec_mutex_lock(recall_id_mutex);
      
      current_recycling_context = current_recall_id->recycling_context;
      
      g_rec_mutex_unlock(recall_id_mutex);

      if(ags_recycling_context_find(current_recycling_context, recycling) >= 0){
	recycling_context = current_recycling_context;
	
	break;
      }
      
      /* iterate */
      iter = iter->next;
    }

    g_object_unref(recycling);

    g_list_free_full(recall_id,
		     g_object_unref);
  }
    
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* get link and pad */
  g_rec_mutex_lock(channel_mutex);
      
  link = channel->link;

  pad = channel->pad;
  
  g_rec_mutex_unlock(channel_mutex);
  
  if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0){
    if(AGS_IS_OUTPUT(channel)){
      if(pad == 0){
	/* add recall id */
	ags_channel_recursive_setup_run_stage_down(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID);
      
	ags_channel_recursive_setup_run_stage_up(link,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID);
	
	/* duplicate */
	ags_channel_recursive_prepare_run_stage_down(channel,
						     recycling_context,
						     sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE);
      
	ags_channel_recursive_prepare_run_stage_up(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE);

	/* resolve */
	ags_channel_recursive_prepare_run_stage_down(channel,
						     recycling_context,
						     sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE);
      
	ags_channel_recursive_prepare_run_stage_up(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE);
      }else{
	/* add recall id */
	ags_channel_recursive_setup_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID);

	/* duplicate */
	ags_channel_recursive_prepare_run_stage_up(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE);

	/* resolve */
	ags_channel_recursive_prepare_run_stage_up(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE);
      }
    }else{
      /* add recall id */
      ags_channel_recursive_prepare_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID);
      ags_channel_recursive_prepare_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_ADD_RECALL_ID);

      /* duplicate */
      ags_channel_recursive_prepare_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE);
      ags_channel_recursive_prepare_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_DUPLICATE);

      /* resolve */
      ags_channel_recursive_prepare_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE);
      ags_channel_recursive_prepare_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_PREPARE_STAGING_RESOLVE);
    }
  }

  if(AGS_IS_OUTPUT(channel)){
    /* do run stage - init/play recall */
    if(pad == 0){
      AgsAudio *audio;
      AgsChannel *start_output;
      AgsChannel *output, *next;
      
      ags_channel_recursive_do_run_stage_down(channel,
					      recycling_context,
					      sound_scope, staging_flags);

      audio = NULL;
    
      g_object_get(channel,
		   "audio", &audio,
		   NULL);

      start_output = NULL;

      g_object_get(audio,
		   "output", &start_output,
		   NULL);
      
      if(start_output != NULL){
	output = start_output;
	g_object_ref(output);

	while(output != NULL){
	  ags_channel_set_staging_completed(output, sound_scope);

	  /* iterate */
	  next = ags_channel_next(output);

	  g_object_unref(output);
	
	  output = next;
	}
      }
          
      ags_channel_recursive_do_run_stage_up(link,
					    recycling_context,
					    sound_scope, staging_flags);

      if(audio != NULL){
	g_object_unref(audio);
      }
    
      if(output != NULL){
	g_object_unref(output);
      }
    }else{
      while(!ags_channel_test_staging_completed(channel, sound_scope));

      ags_channel_recursive_do_run_stage_up(channel,
					    recycling_context,
					    sound_scope, staging_flags);      
    }
  }else{
    /* do run stage - init/play recall */
    ags_channel_recursive_do_run_stage_down(link,
					    recycling_context,
					    sound_scope, staging_flags);
    ags_channel_recursive_do_run_stage_up(channel,
					  recycling_context,
					  sound_scope, staging_flags);
  }
    
  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0){
    if(AGS_IS_OUTPUT(channel)){
      /* cancel */
      if(pad == 0){
	ags_channel_recursive_cleanup_run_stage_down(channel,
						     recycling_context,
						     sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL);

	ags_channel_recursive_cleanup_run_stage_up(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL);
      }else{
	ags_channel_recursive_cleanup_run_stage_up(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL);
      }      
    }else{
      /* cancel */
      ags_channel_recursive_cleanup_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL);
      ags_channel_recursive_cleanup_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_CANCEL_RECALL);
    }
  }
    
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0){
    if(AGS_IS_OUTPUT(channel)){
      /* remove */
      if(pad == 0){
	ags_channel_recursive_cleanup_run_stage_down(channel,
						     recycling_context,
						     sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL);

	ags_channel_recursive_cleanup_run_stage_up(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL);
      }else{
	ags_channel_recursive_cleanup_run_stage_up(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL);
      }
    }else{
      /* remove */
      ags_channel_recursive_cleanup_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL);
      ags_channel_recursive_cleanup_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_STAGING_REMOVE_RECALL);
    }
  }

  if((AGS_SOUND_STAGING_FINI & (staging_flags)) != 0){
    if(AGS_IS_OUTPUT(channel)){
      if(pad == 0){
	ags_channel_recursive_cleanup_run_stage_down(channel,
						     recycling_context,
						     sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE);

	ags_channel_recursive_cleanup_run_stage_up(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE);
      }else{
	ags_channel_recursive_cleanup_run_stage_up(channel,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE);
      }
    }else{
      ags_channel_recursive_cleanup_run_stage_down(link,
						   recycling_context,
						   sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE);
      ags_channel_recursive_cleanup_run_stage_up(channel,
						 recycling_context,
						 sound_scope, AGS_CHANNEL_RECURSIVE_CLEANUP_SCOPE);
    }
  }
}

/**
 * ags_channel_recursive_run_stage:
 * @channel: the #AgsChannel
 * @sound_scope: the sound scope
 * @staging_flags: the staging flags
 * 
 * Recursive run stage specified by @staging_flags for matching @sound_scope.
 * 
 * Since: 3.0.0
 */
void
ags_channel_recursive_run_stage(AgsChannel *channel,
				gint sound_scope, guint staging_flags)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));
  
  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RECURSIVE_RUN_STAGE], 0,
		sound_scope, staging_flags);
  g_object_unref(G_OBJECT(channel));
}

/**
 * ags_channel_new:
 * @audio: the #AgsAudio
 *
 * Creates a new instance of #AgsChannel, linking tree to @audio.
 *
 * Returns: a new #AgsChannel
 *
 * Since: 3.0.0
 */
AgsChannel*
ags_channel_new(GObject *audio)
{
  AgsChannel *channel;

  channel = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
					"audio", audio,
					NULL);

  return(channel);
}
