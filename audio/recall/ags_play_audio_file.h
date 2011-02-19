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

#ifndef __AGS_PLAY_AUDIO_FILE_H__
#define __AGS_PLAY_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_PLAY_AUDIO_FILE                (ags_play_audio_file_get_type())
#define AGS_PLAY_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_AUDIO_FILE, AgsPlayAudioFile))
#define AGS_PLAY_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AgsPlayAudioFileClass))
#define AGS_IS_PLAY_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_AUDIO_FILE))
#define AGS_IS_PLAY_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_AUDIO_FILE))
#define AGS_PLAY_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_AUDIO_FILE, AgsPlayAudioFileClass))

typedef struct _AgsPlayAudioFile AgsPlayAudioFile;
typedef struct _AgsPlayAudioFileClass AgsPlayAudioFileClass;

struct _AgsPlayAudioFile
{
  AgsRecall recall;

  AgsAudioFile *audio_file;
  guint current_frame;

  AgsDevout *devout;
};

struct _AgsPlayAudioFileClass
{
  AgsRecallClass recall;
};

GType ags_play_audio_file_get_type();

AgsPlayAudioFile* ags_play_audio_file_new(AgsAudioFile *audio_file,
					  AgsDevout *devout);

#endif /*__AGS_PLAY_AUDIO_FILE_H__*/
