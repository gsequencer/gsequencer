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

#include <ags/file/ags_file_sound.h>

void
ags_file_read_thread(AgsFile *file, xmlNode *node, AgsThread **thread)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_thread(AgsFile *file, xmlNode *parent, AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_file_read_thread_list(AgsFile *file, xmlNode *node, GList **thread)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_thread_list(AgsFile *file, xmlNode *parent, GList *thread)
{
  //TODO:JK: implement me
}

void
ags_file_read_thread_pool(AgsFile *file, xmlNode *node, AgsThreadPool **thread_pool)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_thread_pool(AgsFile *file, xmlNode *parent, AgsThreadPool *thread_pool)
{
  //TODO:JK: implement me
}

void
ags_file_read_audio_loop(AgsFile *file, xmlNode *node, AgsAudioLoop **audio_loop)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_audio_loop(AgsFile *file, xmlNode *parent, AgsAudioLoop *audio_loop)
{
  //TODO:JK: implement me
}

