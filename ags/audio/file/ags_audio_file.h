/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>

#define AGS_TYPE_AUDIO_FILE                (ags_audio_file_get_type())
#define AGS_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFile))
#define AGS_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))
#define AGS_IS_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_FILE))
#define AGS_IS_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_FILE))
#define AGS_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))

#define AGS_AUDIO_FILE_GET_OBJ_MUTEX(obj) (((AgsAudioFile *) obj)->obj_mutex)

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

/**
 * AgsAudioFileFlags:
 * @AGS_AUDIO_FILE_ADDED_TO_REGISTRY: the audio file was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_AUDIO_FILE_CONNECTED: indicates the audio file was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioFile by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_FILE_ADDED_TO_REGISTRY    = 1,
  AGS_AUDIO_FILE_CONNECTED            = 1 <<  1,
}AgsAudioFileFlags;

struct _AgsAudioFile
{
  GObject object;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsUUID *uuid;

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

pthread_mutex_t* ags_audio_file_get_class_mutex();

gboolean ags_audio_file_test_flags(AgsAudioFile *audio_file, guint flags);
void ags_audio_file_set_flags(AgsAudioFile *audio_file, guint flags);
void ags_audio_file_unset_flags(AgsAudioFile *audio_file, guint flags);

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
