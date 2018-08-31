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

#ifndef __AGS_AUDIO_FILE_H__
#define __AGS_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

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

  GObject *soundcard;

  gchar *filename;

  guint file_audio_channels;
  guint file_samplerate;
  guint file_frame_count;
  
  guint samplerate;
  guint buffer_size;
  guint format;

  gint audio_channel;

  GObject *sound_resource;

  GList *audio_signal;
  GList *wave;
};

struct _AgsAudioFileClass
{
  GObjectClass object;
};

GType ags_audio_file_get_type();

gboolean ags_audio_file_check_suffix(gchar *filename);

/* fields */
void ags_audio_file_add_audio_signal(AgsAudioFile *audio_file, GObject *audio_signal);
void ags_audio_file_remove_audio_signal(AgsAudioFile *audio_file, GObject *audio_signal);

void ags_audio_file_add_wave(AgsAudioFile *audio_file, GObject *wave);
void ags_audio_file_remove_wave(AgsAudioFile *audio_file, GObject *wave);

/* IO functions */
gboolean ags_audio_file_open(AgsAudioFile *audio_file);
gboolean ags_audio_file_open_from_data(AgsAudioFile *audio_file, gchar *data);
gboolean ags_audio_file_rw_open(AgsAudioFile *audio_file,
				gboolean create);
void ags_audio_file_close(AgsAudioFile *audio_file);

void* ags_audio_file_read(AgsAudioFile *audio_file,
			  guint audio_channel,
			  guint format,
			  GError **error);
void ags_audio_file_read_audio_signal(AgsAudioFile *audio_file);
void ags_audio_file_read_wave(AgsAudioFile *audio_file,
			      guint64 x_offset,
			      gdouble delay, guint attack);
void ags_audio_file_seek(AgsAudioFile *audio_file, guint frames, gint whence);
void ags_audio_file_write(AgsAudioFile *audio_file,
			  void *buffer, guint buffer_size,
			  guint format);
void ags_audio_file_flush(AgsAudioFile *audio_file);

/* instantiate */
AgsAudioFile* ags_audio_file_new(gchar *filename,
				 GObject *soundcard,
				 gint audio_channel);

#endif /*__AGS_AUDIO_FILE_H__*/
