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

#ifndef __AGS_AUDIO_FILE_H__
#define __AGS_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#ifndef AGS_USE_AO
#include <ags/audio/ags_devout.h>
#else
#include <ags/audio/ags_libao.h>
#endif


#define AGS_TYPE_AUDIO_FILE                (ags_audio_file_get_type())
#define AGS_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFile))
#define AGS_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))
#define AGS_IS_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_FILE))
#define AGS_IS_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_FILE))
#define AGS_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

struct _AgsAudioFile
{
  GObject object;

  AgsDevout *devout;

  gchar *name;
  guint frames;
  guint channels;

  guint start_channel;
  guint audio_channels;

  GList *audio_signal;

  GObject *file;
};

struct _AgsAudioFileClass
{
  GObjectClass object;
};

GType ags_audio_file_get_type();

gboolean ags_audio_file_open(AgsAudioFile *audio_file);
gboolean ags_audio_file_open_from_data(AgsAudioFile *audio_file, gchar *data);
void ags_audio_file_close(AgsAudioFile *audio_file);

void ags_audio_file_read_audio_signal(AgsAudioFile *audio_file);
void ags_audio_file_seek(AgsAudioFile *audio_file, guint frames, gint whence);
void ags_audio_file_write(AgsAudioFile *audio_file,
			  signed short *buffer, guint buffer_size);
void ags_audio_file_flush(AgsAudioFile *audio_file);

AgsAudioFile* ags_audio_file_new(gchar *name,
				 AgsDevout *devout,
				 guint start_channel, guint audio_channels);

#endif /*__AGS_AUDIO_FILE_H__*/
