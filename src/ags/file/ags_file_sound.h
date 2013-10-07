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

/* AgsDevout */
void ags_file_read_devout(xmlNode *node, AgsDevout **devout);
void ags_file_write_devout(xmlNode *parent, AgsDevout *devout);

void ags_file_read_devout_list(xmlNode *node, GList **devout);
void ags_file_write_devout_list(xmlNode *parent, GList *devout);

void ags_file_read_devout_play(xmlNode *node, AgsDevoutPlay **play);
void ags_file_write_devout_play(xmlNode *parent, AgsDevoutPlay *play);

void ags_file_read_devout_play_list(xmlNode *node, GList **play);
void ags_file_write_devout_play_list(xmlNode *parent, GList *play);

/* AgsAudio */
void ags_file_read_audio(xmlNode *node, AgsAudio **audio);
void ags_file_write_audio(xmlNode *parent, AgsAudio *audio);

void ags_file_read_audio_list(xmlNode *node, GList **audio);
void ags_file_write_audio_list(xmlNode *parent, GList *audio);

/* AgsChannel */
void ags_file_read_channel(xmlNode *node, AgsChannel **channel);
void ags_file_write_channel(xmlNode *parent, AgsChannel *channel);

void ags_file_read_channel_list(xmlNode *node, GList **channel);
void ags_file_write_channel_list(xmlNode *parent, GList *channel);

void ags_file_read_input(xmlNode *node, AgsChannel *input);
void ags_file_write_input(xmlNode *parent, AgsChannel *input);

void ags_file_read_output(xmlNode *node, AgsChannel *output);
void ags_file_write_output(xmlNode *parent, AgsChannel *output);

/* AgsRecall */
void ags_file_read_recall(xmlNode *node, AgsRecall **recall);
void ags_file_write_recall(xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_list(xmlNode *node, GList **recall);
void ags_file_write_recall_list(xmlNode *parent, GList *recall);

void ags_file_read_recall_audio(xmlNode *node, AgsRecall **recall);
void ags_file_write_recall_audio(xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_audio_run(xmlNode *node, AgsRecall **recall);
void ags_file_write_recall_audio_run(xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_channel(xmlNode *node, AgsRecall **recall);
void ags_file_write_recall_channel(xmlNode *parent, AgsRecall *recall);

void ags_file_read_recall_channel_run(xmlNode *node, AgsRecall **recall);
void ags_file_write_recall_channel_run(xmlNode *parent, AgsRecall *recall);

/* AgsRecycling */
void ags_file_read_recycling(xmlNode *node, AgsRecycling **recycling);
void ags_file_write_recycling(xmlNode *parent, AgsRecycling *recycling);

void ags_file_read_recycling_list(xmlNode *node, GList **recycling);
void ags_file_write_recycling_list(xmlNode *parent, GList *recycling);

/* AgsAudioSignal */
void ags_file_read_audio_signal(xmlNode *node, AgsAudioSignal **audio_signal);
void ags_file_write_audio_signal(xmlNode *parent, AgsAudioSignal *audio_signal);

void ags_file_read_audio_signal_list(xmlNode *node, GList **audio_signal);
void ags_file_write_audio_signal_list(xmlNode *parent, GList *audio_signal);

/* AgsStream */
void ags_file_read_stream(xmlNode *node, AgsStream **stream);
void ags_file_write_stream(xmlNode *parent, AgsStream *stream);

void ags_file_read_stream_list(xmlNode *node, GList **stream);
void ags_file_write_stream_list(xmlNode *parent, GList *stream);

/* AgsPattern */
void ags_file_read_pattern(xmlNode *node, AgsPattern **pattern);
void ags_file_write_pattern(xmlNode *parent, AgsPattern *pattern);

void ags_file_read_pattern_list(xmlNode *node, GList **pattern);
void ags_file_write_pattern_list(xmlNode *parent, GList *pattern);

void ags_file_read_pattern_data(xmlNode *node, AgsPattern *pattern, guint i, guint j);
void ags_file_write_pattern_data(xmlNode *parent, AgsPattern *pattern, guint i, guint j);

void ags_file_read_pattern_data_list(xmlNode *node, AgsPattern *pattern);
void ags_file_write_pattern_data_list(xmlNode *parent, AgsPattern *pattern);

/* AgsNotation */
void ags_file_read_notation(xmlNode *node, AgsNotation **notation);
void ags_file_write_notation(xmlNode *parent, AgsNotation *notation);

void ags_file_read_notation_list(xmlNode *node, GList **notation);
void ags_file_write_notation_list(xmlNode *parent, GList *notation);

void ags_file_read_note(xmlNode *node, AgsNote **note);
void ags_file_write_note(xmlNode *parent, AgsNote *note);

void ags_file_read_note_list(xmlNode *node, GList **note);
void ags_file_write_note_list(xmlNode *parent, GList *note);

/* AgsTask */
void ags_file_read_task(xmlNode *node, AgsTask **task);
void ags_file_write_task(xmlNode *parent, AgsTask *task);

void ags_file_read_task_list(xmlNode *node, GList **task);
void ags_file_write_task_list(xmlNode *parent, GList *task);

#endif /*__AGS_FILE_SOUND_H__*/
