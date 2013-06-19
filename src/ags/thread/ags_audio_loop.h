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

#ifndef __AGS_AUDIO_LOOP_H__
#define __AGS_AUDIO_LOOP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

#define AGS_TYPE_AUDIO_LOOP                (ags_audio_loop_get_type())
#define AGS_AUDIO_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_LOOP, AgsAudioLoop))
#define AGS_AUDIO_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_LOOP, AgsAudioLoop))
#define AGS_IS_AUDIO_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_LOOP))
#define AGS_IS_AUDIO_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_LOOP))
#define AGS_AUDIO_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_LOOP, AgsAudioLoopClass))

typedef struct _AgsAudioLoop AgsAudioLoop;
typedef struct _AgsAudioLoopClass AgsAudioLoopClass;

struct _AgsAudioLoop
{
  AgsThread thread;

  guint play_recall_ref;
  GList *play_recall; // play AgsRecall

  guint play_channel_ref;
  GList *play_channel; // play AgsChannel

  guint play_audio_ref;
  GList *play_audio; // play AgsAudio
};

struct _AgsAudioLoopClass
{
  AgsThreadClass thread;
};

GType ags_audio_loop_get_type();

AgsAudioLoop* ags_audio_loop_new();

#endif /*__AGS_AUDIO_LOOP_H__*/
