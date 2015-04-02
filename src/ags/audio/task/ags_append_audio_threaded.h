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

#ifndef __AGS_APPEND_AUDIO_THREADED_H__
#define __AGS_APPEND_AUDIO_THREADED_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_APPEND_AUDIO_THREADED                (ags_append_audio_threaded_get_type())
#define AGS_APPEND_AUDIO_THREADED(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPEND_AUDIO_THREADED, AgsAppendAudioThreaded))
#define AGS_APPEND_AUDIO_THREADED_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPEND_AUDIO_THREADED, AgsAppendAudioThreadedClass))
#define AGS_IS_APPEND_AUDIO_THREADED(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPEND_AUDIO_THREADED))
#define AGS_IS_APPEND_AUDIO_THREADED_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPEND_AUDIO_THREADED))
#define AGS_APPEND_AUDIO_THREADED_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPEND_AUDIO_THREADED, AgsAppendAudioThreadedClass))

typedef struct _AgsAppendAudioThreaded AgsAppendAudioThreaded;
typedef struct _AgsAppendAudioThreadedClass AgsAppendAudioThreadedClass;

struct _AgsAppendAudioThreaded
{
  AgsTask task;

  GObject *audio_loop;
  AgsDevoutPlay *devout_play;
};

struct _AgsAppendAudioThreadedClass
{
  AgsTaskClass task;
};

GType ags_append_audio_threaded_get_type();

AgsAppendAudioThreaded* ags_append_audio_threaded_new(GObject *audio_loop,
						      AgsDevoutPlay *play);

#endif /*__AGS_APPEND_AUDIO_THREADED_H__*/
