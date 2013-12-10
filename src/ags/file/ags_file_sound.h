/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_FILE_SOUND_H__
#define __AGS_FILE_SOUND_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_task.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_timestamp.h>

/* AgsDevout */
void ags_file_read_devout(AgsFile *file, xmlNode *node, AgsDevout **devout);
xmlNode* ags_file_write_devout(AgsFile *file, xmlNode *parent, AgsDevout *devout);

void ags_file_read_devout_list(AgsFile *file, xmlNode *node, GList **devout);
xmlNode* ags_file_write_devout_list(AgsFile *file, xmlNode *parent, GList *devout);

void ags_file_read_devout_play(AgsFile *file, xmlNode *node, AgsDevoutPlay **play);
xmlNode* ags_file_write_devout_play(AgsFile *file, xmlNode *parent, AgsDevoutPlay *play);

void ags_file_read_devout_play_list(AgsFile *file, xmlNode *node, GList **play);
xmlNode* ags_file_write_devout_play_list(AgsFile *file, xmlNode *parent, GList *play);

/* AgsAudio */
void ags_file_read_audio(AgsFile *file, xmlNode *node, AgsAudio **audio);
xmlNode* ags_file_write_audio(AgsFile *file, xmlNode *parent, AgsAudio *audio);

void ags_file_read_audio_list(AgsFile *file, xmlNode *node, GList **audio);
xmlNode* ags_file_write_audio_list(AgsFile *file, xmlNode *parent, GList *audio);

/* AgsChannel */
void ags_file_read_channel(AgsFile *file, xmlNode *node, AgsChannel **channel);
xmlNode* ags_file_write_channel(AgsFile *file, xmlNode *parent, AgsChannel *channel);

void ags_file_read_channel_list(AgsFile *file, xmlNode *node, GList **channel);
xmlNode* ags_file_write_channel_list(AgsFile *file, xmlNode *parent, GList *channel);

void ags_file_read_input(AgsFile *file, xmlNode *node, AgsChannel *input);
xmlNode* ags_file_write_input(AgsFile *file, xmlNode *parent, AgsChannel *input);

void ags_file_read_output(AgsFile *file, xmlNode *node, AgsChannel *output);
xmlNode* ags_file_write_output(AgsFile *file, xmlNode *parent, AgsChannel *output);

/* AgsFileLink */
void ags_file_read_file_link(AgsFile *file, xmlNode *node, AgsFileLink *file_link);
xmlNode* ags_file_write_file_link(AgsFile *file, xmlNode *parent, AgsFileLink *file_link);

/* AgsRecall */
void ags_file_read_recall(AgsFile *file, xmlNode *node, AgsRecall **recall);
xmlNode* ags_file_write_recall(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_list(AgsFile *file, xmlNode *node, GList **recall);
xmlNode* ags_file_write_recall_list(AgsFile *file, xmlNode *parent, GList *recall);

void ags_file_read_recall_audio(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_audio(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_audio_run(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_audio_run(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_channel(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_channel(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_channel_run(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_channel_run(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_recycling(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_recycling(AgsFile *file, xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_audio_signal(AgsFile *file, xmlNode *node, AgsRecall *recall);
xmlNode* ags_file_write_recall_audio_signal(AgsFile *file, xmlNode *parent, AgsRecall *recall);

/* AgsPort */
void ags_file_read_port(AgsFile *file, xmlNode *node, AgsPort **port);
xmlNode* ags_file_write_port(AgsFile *file, xmlNode *parent, AgsPort *port);

void ags_file_read_port_list(AgsFile *file, xmlNode *node, GList **port);
xmlNode* ags_file_write_port_list(AgsFile *file, xmlNode *parent, GList *port);

/* AgsRecycling */
void ags_file_read_recycling(AgsFile *file, xmlNode *node, AgsRecycling **recycling);
xmlNode* ags_file_write_recycling(AgsFile *file, xmlNode *parent, AgsRecycling *recycling);

void ags_file_read_recycling_list(AgsFile *file, xmlNode *node, GList **recycling);
xmlNode* ags_file_write_recycling_list(AgsFile *file, xmlNode *parent, GList *recycling);

/* AgsAudioSignal */
void ags_file_read_audio_signal(AgsFile *file, xmlNode *node, AgsAudioSignal **audio_signal);
xmlNode* ags_file_write_audio_signal(AgsFile *file, xmlNode *parent, AgsAudioSignal *audio_signal);

void ags_file_read_audio_signal_list(AgsFile *file, xmlNode *node, GList **audio_signal);
xmlNode* ags_file_write_audio_signal_list(AgsFile *file, xmlNode *parent, GList *audio_signal);

/* AgsStream */
void ags_file_read_stream(AgsFile *file, xmlNode *node,
			  GList **stream, guint *index,
			  guint buffer_size);
xmlNode* ags_file_write_stream(AgsFile *file, xmlNode *parent,
			       GList *stream, guint index,
			       guint buffer_size);

void ags_file_read_stream_list(AgsFile *file, xmlNode *node,
			       GList **stream,
			       guint buffer_size);
xmlNode* ags_file_write_stream_list(AgsFile *file, xmlNode *parent,
				    GList *stream,
				    guint buffer_size);

/* AgsPattern */
void ags_file_read_pattern(AgsFile *file, xmlNode *node, AgsPattern **pattern);
xmlNode* ags_file_write_pattern(AgsFile *file, xmlNode *parent, AgsPattern *pattern);

void ags_file_read_pattern_list(AgsFile *file, xmlNode *node, GList **pattern);
xmlNode* ags_file_write_pattern_list(AgsFile *file, xmlNode *parent, GList *pattern);

void ags_file_read_pattern_data(AgsFile *file, xmlNode *node,
				AgsPattern *pattern, guint *i, guint *j,
				guint length);
xmlNode* ags_file_write_pattern_data(AgsFile *file, xmlNode *parent,
				     AgsPattern *pattern, guint i, guint j,
				     guint length);

void ags_file_read_pattern_data_list(AgsFile *file, xmlNode *node,
				     AgsPattern *pattern,
				     guint length);
xmlNode* ags_file_write_pattern_data_list(AgsFile *file, xmlNode *parent,
					  AgsPattern *pattern,
					  guint length);

/* AgsNotation */
void ags_file_read_notation(AgsFile *file, xmlNode *node, AgsNotation **notation);
xmlNode* ags_file_write_notation(AgsFile *file, xmlNode *parent, AgsNotation *notation);

void ags_file_read_notation_list(AgsFile *file, xmlNode *node, GList **notation);
xmlNode* ags_file_write_notation_list(AgsFile *file, xmlNode *parent, GList *notation);

void ags_file_read_note(AgsFile *file, xmlNode *node, AgsNote **note);
xmlNode* ags_file_write_note(AgsFile *file, xmlNode *parent, AgsNote *note);

void ags_file_read_note_list(AgsFile *file, xmlNode *node, GList **note);
xmlNode* ags_file_write_note_list(AgsFile *file, xmlNode *parent, GList *note);

/* AgsTask */
void ags_file_read_task(AgsFile *file, xmlNode *node, AgsTask **task);
xmlNode* ags_file_write_task(AgsFile *file, xmlNode *parent, AgsTask *task);

void ags_file_read_task_list(AgsFile *file, xmlNode *node, GList **task);
xmlNode* ags_file_write_task_list(AgsFile *file, xmlNode *parent, GList *task);

/* AgsTimestamp */
void ags_file_read_timestamp(AgsFile *file, xmlNode *node, AgsTimestamp **timestamp);
xmlNode* ags_file_write_timestamp(AgsFile *file, xmlNode *parent, AgsTimestamp *timestamp);

void ags_file_read_timestamp_list(AgsFile *file, xmlNode *node, GList **timestamp);
xmlNode* ags_file_write_timestamp_list(AgsFile *file, xmlNode *parent, GList *timestamp);

#endif /*__AGS_FILE_SOUND_H__*/
