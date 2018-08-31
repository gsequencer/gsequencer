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

#ifndef __AGS_AUDIO_CONTAINER_H__
#define __AGS_AUDIO_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_AUDIO_CONTAINER                (ags_audio_container_get_type())
#define AGS_AUDIO_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_CONTAINER, AgsAudioContainer))
#define AGS_AUDIO_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_CONTAINER, AgsAudioContainerClass))
#define AGS_IS_AUDIO_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_CONTAINER))
#define AGS_IS_AUDIO_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_CONTAINER))
#define AGS_AUDIO_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_CONTAINER, AgsAudioContainerClass))

typedef struct _AgsAudioContainer AgsAudioContainer;
typedef struct _AgsAudioContainerClass AgsAudioContainerClass;

struct _AgsAudioContainer
{
  GObject object;

  GObject *soundcard;

  gchar *filename;
  gchar *preset;
  gchar *instrument;
  gchar *sample;

  guint file_audio_channels;
  guint file_samplerate;
  guint file_frame_count;

  guint samplerate;
  guint buffer_size;
  guint format;

  gint audio_channel;

  GObject *sound_container;

  GList *audio_signal;
  GList *wave;
};

struct _AgsAudioContainerClass
{
  GObjectClass object;
};

GType ags_audio_container_get_type();

gboolean ags_audio_container_check_suffix(gchar *filename);

/* fields */
void ags_audio_container_add_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal);
void ags_audio_container_remove_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal);

void ags_audio_container_add_wave(AgsAudioContainer *audio_container, GObject *wave);
void ags_audio_container_remove_wave(AgsAudioContainer *audio_container, GObject *wave);

/* IO functions */
gboolean ags_audio_container_open(AgsAudioContainer *audio_container);
gboolean ags_audio_container_open_from_data(AgsAudioContainer *audio_container, gchar *data);
gboolean ags_audio_container_rw_open(AgsAudioContainer *audio_container,
				     gboolean create);
void ags_audio_container_close(AgsAudioContainer *audio_container);

void* ags_audio_container_read(AgsAudioContainer *audio_container,
			       guint audio_channel,
			       guint format,
			       GError **error);
GList* ags_audio_container_read_audio_signal(AgsAudioContainer *audio_container);
GList* ags_audio_container_read_wave(AgsAudioContainer *audio_container,
				     guint64 x_offset,
				     gdouble delay, guint attack);
void ags_audio_container_seek(AgsAudioContainer *audio_container, guint frames, gint whence);
void ags_audio_container_write(AgsAudioContainer *audio_container,
			       void *buffer, guint buffer_size,
			       guint format);
void ags_audio_container_flush(AgsAudioContainer *audio_container);

/* instantiate */
AgsAudioContainer* ags_audio_container_new(gchar *filename,
					   gchar *preset,
					   gchar *instrument,
					   gchar *sample,
					   GObject *soundcard,
					   gint audio_channel);

#endif /*__AGS_AUDIO_CONTAINER_H__*/
