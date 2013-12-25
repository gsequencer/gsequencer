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

#ifndef __AGS_FILE_THREAD_H__
#define __AGS_FILE_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/file/ags_file.h>

#include <ags/thread/ags_thread.h>
#include <ags/thread/ags_audio_loop.h>

void ags_file_read_thread(AgsFile *file, xmlNode *node, AgsThread **thread);
xmlNode* ags_file_write_thread(AgsFile *file, xmlNode *parent, AgsThread *thread);

void ags_file_read_thread_list(AgsFile *file, xmlNode *node, GList **thread);
xmlNode* ags_file_write_thread_list(AgsFile *file, xmlNode *parent, GList *thread);

void ags_file_read_thread_pool(AgsFile *file, xmlNode *node, AgsThreadPool **thread_pool);
xmlNode* ags_file_write_thread_pool(AgsFile *file, xmlNode *parent, AgsThreadPool *thread_pool);

void ags_file_read_audio_loop(AgsFile *file, xmlNode *node, AgsAudioLoop *audio_loop);
xmlNode* ags_file_write_audio_loop(AgsFile *file, xmlNode *parent, AgsAudioLoop *audio_loop);

#endif /*__AGS_FILE_THREAD_H__*/
