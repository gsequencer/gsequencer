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

#ifndef __AGS_EMBEDDED_AUDIO_H__
#define __AGS_EMBEDDED_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_EMBEDDED_AUDIO                (ags_embedded_audio_get_type())
#define AGS_EMBEDDED_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EMBEDDED_AUDIO, AgsEmbeddedAudio))
#define AGS_EMBEDDED_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_EMBEDDED_AUDIO, AgsEmbeddedAudio))
#define AGS_IS_EMBEDDED_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EMBEDDED_AUDIO))
#define AGS_IS_EMBEDDED_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EMBEDDED_AUDIO))
#define AGS_EMBEDDED_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_EMBEDDED_AUDIO, AgsEmbeddedAudioClass))

typedef struct _AgsEmbeddedAudio AgsEmbeddedAudio;
typedef struct _AgsEmbeddedAudioClass AgsEmbeddedAudioClass;

struct _AgsEmbeddedAudio
{
  GObject object;

  gpointer *data;
};

struct _AgsEmbeddedAudioClass
{
  GObjectClass object;
};

GType ags_embedded_audio_get_type();

AgsEmbeddedAudio* ags_embedded_audio_new();

#endif /*__AGS_EMBEDDED_AUDIO_H__*/
