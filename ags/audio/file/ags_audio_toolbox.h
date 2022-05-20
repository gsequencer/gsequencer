/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_AUDIO_TOOLBOX_H__
#define __AGS_AUDIO_TOOLBOX_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <AudioToolbox/AudioToolbox.h>

#define AGS_TYPE_AUDIO_TOOLBOX                (ags_audio_toolbox_get_type())
#define AGS_AUDIO_TOOLBOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_TOOLBOX, AgsAudioToolbox))
#define AGS_AUDIO_TOOLBOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_TOOLBOX, AgsAudioToolboxClass))
#define AGS_IS_AUDIO_TOOLBOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_TOOLBOX))
#define AGS_IS_AUDIO_TOOLBOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_TOOLBOX))
#define AGS_AUDIO_TOOLBOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_TOOLBOX, AgsAudioToolboxClass))

#define AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(obj) (&(((AgsAudioToolbox *) obj)->obj_mutex))

typedef struct _AgsAudioToolbox AgsAudioToolbox;
typedef struct _AgsAudioToolboxClass AgsAudioToolboxClass;

/**
 * AgsAudioToolboxFlags:
 * @AGS_AUDIO_TOOLBOX_FILL_CACHE: fill cache
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioToolbox by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_TOOLBOX_FILL_CACHE           = 1,
}AgsAudioToolboxFlags;

struct _AgsAudioToolbox
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint audio_channels;
  gint64 *audio_channel_written;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint64 offset;
  guint64 buffer_offset;

  void *full_buffer;
  void *buffer;

  guchar *pointer;
  guchar *current;
  gsize length;

  ExtAudioFileRef audio_file;
  AudioBufferList *audio_buffer_list;
  AudioStreamBasicDescription *stream;
  AudioStreamBasicDescription *client_stream;
};

struct _AgsAudioToolboxClass
{
  GObjectClass gobject;
};

GType ags_audio_toolbox_get_type();

gboolean ags_audio_toolbox_test_flags(AgsAudioToolbox *audio_toolbox, guint flags);
void ags_audio_toolbox_set_flags(AgsAudioToolbox *audio_toolbox, guint flags);
void ags_audio_toolbox_unset_flags(AgsAudioToolbox *audio_toolbox, guint flags);

gboolean ags_audio_toolbox_check_suffix(gchar *filename);

AgsAudioToolbox* ags_audio_toolbox_new();

G_END_DECLS

#endif /*__AGS_AUDIO_TOOLBOX_H__*/
